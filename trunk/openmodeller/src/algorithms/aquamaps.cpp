/**
 * Declaration of AquaMaps Algorithm.
 * 
 * @file
 * @author Renato De Giovanni <renato [at] cria dot org dot br>
 * @date 2006-08-07
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2006 by CRIA -
 * Centro de Referência em Informação Ambiental
 *
 * http://www.cria.org.br
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details:
 * 
 * http://www.gnu.org/copyleft/gpl.html
 */

#include "aquamaps.hh"

#include <openmodeller/Configuration.hh>

#include <openmodeller/Exceptions.hh>

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <sqlite3.h>

// Algorithm is included for std::min and std::max.
#include <algorithm>

using namespace std;


/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 0

/*************************************/
/*** Algorithm parameters metadata ***/

static AlgParamMetadata parameters[NUM_PARAM];


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "AquaMaps",  // Id.
  "AquaMaps",  // Name.
  "0.1",       // Version.

  // Overview
  "Environmental envelope modelling algorithm for marine organisms. \
Needs 7 predefined layers (see detailed description) to calculate the \
preferred and accepted envelopes, and also needs a scientific name (genus \
plus species) labeling each occurrence group." ,

  // Description.
  "AquaMaps is a niche modelling tool developed as part of the Incofish \
project and particularly tailored towards modelling marine organisms distribution.\n \
The basic idea follows the environmental envelope type modelling approach, \
where each variable has an associated preferred range and a broader accepted \
range. Within the preferred range the probabilty of presence is 1, between the \
preferred range and the acceptable range the probability varies from 1 to 0 \
(linear decay), and outside the accepted range the probability is 0. The \
overall probability is calculated by multiplying all individual probabilities.\n\
This algorithm differs from other traditional ones since it requires a specific \
set of layers to work, which should also be in this order: maximum depth in meters, \
minimum depth in meters, mean annual sea ice concentration, mean annual distance to \
land in kilometers, mean annual primary production (chlorophyll A, measured in \
mgC per square meter per day), mean annual salinity in psu, mean annual sea surface \
temperature in celsius. These layers can be downloaded from: \n\
http://openmodeller.cria.org.br/download/marine.zip \n\
AquaMaps also makes use of expert information for the lower and upper limits of \
each variable. Depth ranges are taken from a database provided by FishBase. To find \
this information in the database, occurrences must be identified by the scientific \
name (only genus and species). The database contains depth ranges for about 30k \
different marine species. \n\
For the other variables, preferred ranges are initially calculated based on \
percentiles 10th and 90th. They are further adjusted using interquartile values and \
ensuring a minimum envelope size function.",
  
  "K. Kaschner, J. Ready, S. Kullander, R. Froese",  // Authors

  // Bibliography.
  "",

  "Renato De Giovanni",              // Code author.
  "renato [at] cria dot org dot br", // Code author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new AquaMaps();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/****************************************************************/
/**************************** AquaMaps **************************/

/*******************/
/*** constructor ***/

AquaMaps::AquaMaps() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _lower_limit( 7, LOWER_LIMIT ),
  _upper_limit( 7, UPPER_LIMIT ),
  _inner_size( 7, INNER_SIZE ),
  _minimum(),
  _maximum(),
  _pref_minimum(),
  _pref_maximum(),
  _pelagic(-1)
{ }

/******************/
/*** destructor ***/

AquaMaps::~AquaMaps()
{
}

/******************/
/*** initialize ***/
int
AquaMaps::initialize()
{
  // Number of independent variables.
  int dim = _samp->numIndependent();
  g_log.info( "Reading %d-dimensional occurrence points.\n", dim );

  // Check the number of layers.
  if ( dim != 7 ) {
  
    g_log.error( 1, "AquaMaps needs precisely 7 layers to work, and they should be in this order: maximum depth in meters, minimum depth in meters, mean annual sea ice concentration, mean annual distance to land in Kilometers, mean annual primary production (chlorophyll A), mean annual salinity in psu, mean annual sea surface temperature in Celsius.\n" ); 
    // g_log.error() does a ::exit(rc).
  }

  // Remove duplicates accross geography
  _samp->spatiallyUnique();

  // Check the number of sampled points.
  int npnt = _samp->numPresence();

  if (  npnt < 5 ) {

    g_log.error( 1, "AquaMaps needs at least 5 points inside the mask!\n" ); 
    // g_log.error() does a ::exit(rc).
  }

  g_log.info( "Using %d points to find AquaMaps envelopes.\n", npnt );

  calculateEnvelopes( _samp->getPresences() );

  _done = true;

  return 1;
}


/***************/
/*** iterate ***/
int
AquaMaps::iterate()
{
  return 1;
}


/***********************/
/*** get Convergence ***/
int
AquaMaps::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}


/************/
/*** done ***/
int
AquaMaps::done() const
{
  // This is not an iterative algorithm.
  return _done;
}


/***************************/
/*** calculate envelopes ***/
void
AquaMaps::calculateEnvelopes( const OccurrencesPtr& occs )
{

  g_log.debug("Species is: %s\n", occs->name());
  g_log.debug("Layers are:\n");
  g_log.debug("0 = Maximum depth\n");
  g_log.debug("1 = Minimum depth\n");
  g_log.debug("2 = Ice concentration\n");
  g_log.debug("3 = Distance to land\n");
  g_log.debug("4 = Primary production (chlorophyll A)\n");
  g_log.debug("5 = Salinity\n");
  g_log.debug("6 = Surface temperature\n");

  // Compute min, pref_min, pref_max and max
  OccurrencesImpl::const_iterator oc = occs->begin();
  OccurrencesImpl::const_iterator ocEnd = occs->end();

  // Intialize _minimum, _maximum, _pref_minimum, and _pref_maximum
  // to the values of the first point, and increment
  // to get it out of the next loop.
  Sample const & sample = (*oc)->environment();
  _minimum = sample;
  _pref_minimum = sample;
  _pref_maximum = sample;
  _maximum = sample;
  ++oc;

  // Load default min/max
  while ( oc != ocEnd ) {
      
    Sample const& sample = (*oc)->environment();

    // Default min/max will be the actual min/max values
    _minimum &= sample;
    _maximum |= sample;

    ++oc;
  }

  // Default values for depth ranges
  _minimum[0] = _minimum[1] = _pref_minimum[0] = _pref_minimum[1] = 0;
  _maximum[0] = _maximum[1] = _pref_maximum[0] = _pref_maximum[1] = 9999;

  // Try to get expert information about depth range from database
  readDepthData( occs->name() );

  // Get matrix data structure so that we can sort values for each layer
  std::vector<ScalarVector> matrix = occs->getEnvironmentMatrix();

  // For each layer (except min/max depth), calculate the percentiles and set 
  // default values for the prefered min/max
  ScalarVector::iterator lStart, lEnd;

  for ( unsigned int j = 2; j < matrix.size(); j++ ) {

    g_log.debug("--------------------------------\n", j);
    g_log.debug("Calculating envelope for layer %d\n", j);
    g_log.debug("--------------------------------\n", j);

    // 0 = Maximum depth
    // 1 = Minimum depth
    // 2 = Ice concentration
    // 3 = Distance to land
    // 4 = Primary production (chlorophyll A)
    // 5 = Salinity
    // 6 = Surface temperature

    lStart = matrix[j].begin();
    lEnd = matrix[j].end();
    sort( lStart, lEnd );

    int numOccurrences = occs->numOccurrences();

    // Just for debugging
    ostringstream debug;
    debug << "row -->";
    for ( int w = 0; w < numOccurrences; w++ ) {

	debug << " [" << j << "," << w << "]=" << matrix[j][w];
    }
    debug << "\n";
    g_log.debug( debug.str().c_str() );

    // Calculate percentiles
    Scalar v10, v25, v75, v90;

    percentile( &v10, numOccurrences, 0.10, &matrix, j );
    percentile( &v25, numOccurrences, 0.25, &matrix, j );
    percentile( &v75, numOccurrences, 0.75, &matrix, j );
    percentile( &v90, numOccurrences, 0.90, &matrix, j );

    g_log.debug("10th percentile: %f\n", v10);
    g_log.debug("25th percentile: %f\n", v25);
    g_log.debug("75th percentile: %f\n", v75);
    g_log.debug("90th percentile: %f\n", v90);

    Scalar interquartile = fabs( v25 - v75 );

    g_log.debug("Interquartile: %f\n", interquartile);

    Scalar adjmin = v25 - ( 1.5 * interquartile );
    Scalar adjmax = v75 + ( 1.5 * interquartile );

    g_log.debug("Adjmin: %f\n", adjmin);
    g_log.debug("Adjmax: %f\n", adjmax);

    _pref_minimum[j] = v10;
    _pref_maximum[j] = v90;

    g_log.debug("_Before adjustments_\n");
    g_log.debug("min: %f\n", _minimum[j]);
    g_log.debug("prefmin: %f\n", _pref_minimum[j]);
    g_log.debug("prefmax: %f\n", _pref_maximum[j]);
    g_log.debug("max: %f\n", _maximum[j]);

    // Make the interquartile adjusting and ensure the envelope sizes
    // for all variables except ice concentration
    if ( j > 1 ) {

      adjustInterquartile( j, adjmin, adjmax );
      ensureEnvelopeSize( j );
    }

    g_log.debug("_After adjustments_\n");
    g_log.debug("min: %f\n", _minimum[j]);
    g_log.debug("prefmin: %f\n", _pref_minimum[j]);
    g_log.debug("prefmax: %f\n", _pref_maximum[j]);
    g_log.debug("max: %f\n", _maximum[j]);
  }
}


/******************/
/*** percentile ***/
void 
AquaMaps::percentile( Scalar *result, int n, double percent, std::vector<ScalarVector> *matrix, int layerIndex )
{
  int iPrev, iNext;

  Scalar k = n * percent;

  iPrev = (int) floor( k );
  iNext = (int) ceil( k );

  // Subtract 1 from the result since arrays begin with position 0
  --iPrev;
  --iNext;

  if ( iPrev == iNext ) {

    // If k is an integer, use the mean between the two positions
    *result = ( (*matrix)[layerIndex][iPrev] + (*matrix)[layerIndex][iNext] ) / 2;
  }
  else {

    // If k is not an integer, use the next as a reference
    *result = (*matrix)[layerIndex][iNext];
  }
}


/***********************/
/*** read depth data ***/
void 
AquaMaps::readDepthData( const char *species )
{
  sqlite3 *db;

#ifndef WIN32
  string dbname( OMDATAPATH ); 
  dbname.append( "/" );
  dbname.append( "data" );
  dbname.append( "/" );
  dbname.append( "aquamaps.db" );
#else
  string dbname("data\\aquamaps.db");
#endif
  int rc = sqlite3_open( dbname.c_str(), &db);

  if ( rc ) {

    // This will likely never happen since on open, sqlite creates the
    // database if it does not exist.
    g_log.warn( "Could not open database with depth range data: %s\n", sqlite3_errmsg( db ) );
    sqlite3_close(db);
    return;
  }

  // Prepare the sql statement
  const char *pzTail;

  sqlite3_stmt *ppStmt;

  char* sql = sqlite3_mprintf( "select pelagic, min, prefmin, prefmax, max from depthinfo where species = '%q'", species );

  rc = sqlite3_prepare( db, sql, strlen( sql ), &ppStmt, &pzTail );

  if ( rc == SQLITE_OK ) {

    // Fecth data
    if ( sqlite3_step( ppStmt ) == SQLITE_ROW ) {

      int    pelagic = sqlite3_column_int( ppStmt, 0 );
      double min     = sqlite3_column_double( ppStmt, 1 );
      double prefmin = sqlite3_column_double( ppStmt, 2 );
      double prefmax = sqlite3_column_double( ppStmt, 3 );
      double max     = sqlite3_column_double( ppStmt, 4 );

      _pelagic         = pelagic;

      // Repeat the information for both min and max depth for symmetry
      _minimum[0]      = min;
      _minimum[1]      = min;
      _pref_minimum[0] = prefmin;
      _pref_minimum[1] = prefmin;
      _pref_maximum[0] = prefmax;
      _pref_maximum[1] = prefmax;
      _maximum[0]      = max;
      _maximum[1]      = max;

      g_log.debug("Depth values from database:\n");
      g_log.debug("pelagic: %i\n", pelagic);
      g_log.debug("min: %f\n", min);
      g_log.debug("prefmin: %f\n", prefmin);
      g_log.debug("prefmax: %f\n", prefmax);
      g_log.debug("max: %f\n", max);
    }
    else {

      g_log.warn( "Could not fetch data from depth range database: %s\n", 
                  sqlite3_errmsg( db ) );
    }
  }
  else {

    g_log.warn( "Could not prepare SQL statement to query depth range database: %s\n", 
                sqlite3_errmsg( db ) );
  }

  // free memory
  sqlite3_free( sql );

  // close the statement
  sqlite3_finalize(ppStmt);

  // close the database
  sqlite3_close(db);
}


/****************************/
/*** ensure envelope size ***/
void
AquaMaps::adjustInterquartile( int layerIndex, Scalar adjmin, Scalar adjmax )
{
  if ( adjmin > _lower_limit[layerIndex] && adjmin < _minimum[layerIndex] ) {

    _minimum[layerIndex] = adjmin;
  }

  if ( adjmax < _upper_limit[layerIndex] && adjmax > _maximum[layerIndex] ) {

    _maximum[layerIndex] = adjmax;
  }
}


/****************************/
/*** ensure envelope size ***/
void
AquaMaps::ensureEnvelopeSize( int layerIndex )
{
  Scalar halfSize = _inner_size[layerIndex]/2;

  // Try to ensure that the prefered envelope has the specified size
  // (but only change if the new values are still within the accepted envelope)
  if ( _pref_maximum[layerIndex] - _pref_minimum[layerIndex] < _inner_size[layerIndex] ) {

    Scalar center = (_pref_minimum[layerIndex] + _pref_maximum[layerIndex]) / 2;

    Scalar new_pref_min = center - halfSize;

    if ( new_pref_min >= _minimum[layerIndex] ) {

      _pref_minimum[layerIndex] = new_pref_min;
    }

    Scalar new_pref_max = center + halfSize;

    if ( new_pref_max <= _maximum[layerIndex] ) {

      _pref_maximum[layerIndex] = new_pref_max;
    }
  }

  // Ensure that the distance between the envelopes has half the specified size
  // (but changes should never exceed the allowed limits!)
  if ( _pref_minimum[layerIndex] - _minimum[layerIndex] < halfSize ) {

    Scalar new_min = _pref_minimum[layerIndex] - halfSize;

    if ( new_min > _lower_limit[layerIndex] ) {

      _minimum[layerIndex] = new_min;
    }
    else {

      _minimum[layerIndex] = _lower_limit[layerIndex];
    }
  }

  if ( _maximum[layerIndex] - _pref_maximum[layerIndex] < halfSize ) {

    Scalar new_max = _pref_maximum[layerIndex] + halfSize;

    if ( new_max < _upper_limit[layerIndex] ) {

      _maximum[layerIndex] = new_max;
    }
    else {

      _maximum[layerIndex] = _upper_limit[layerIndex];
    }
  }
}


/*****************/
/*** get Value ***/
Scalar
AquaMaps::getValue( const Sample& x ) const
{
  // Final probability is the multiplication of all
  // individual probabilities
  Scalar prob = 1.0;

  // Depth probability

  if ( _maximum[0] != 9999 ) {  // If there is a depth range

    // Probability of occurrence is zero if depth at this point is less
    // than the minimum depth for the species.
    // note: using maximum depth layer here
    if ( x[0] < _minimum[0] ) {

        return 0.0;
    }

    // Point is sufficiently deep, but still not enough for the prefered range
    // note: using maximum depth layer here
    if (  x[0] >= _minimum[0] && x[0] < _pref_minimum[0] ) {

      // Linear decay
      prob *= (x[0] - _minimum[0]) / (_pref_minimum[0] - _minimum[0]);
    }
    // Point is sufficiently deep for the prefered range
    // Note: pelagic means "belonging to the upper layers of the open sea". Or in other 
    //       words, no need to feed at the bottom of the sea.
    else if (  _pelagic == 1 ) {

      // Just keep prob as 1
    }
    // Species needs to feed at the bottom of the sea (or there's no data about this) 
    // and point is within the prefered range
    // note: the inner envelope logic makes use of both minimum and maximum depth layers.
    else if (  x[0] >= _pref_minimum[0] && x[1] <= _pref_maximum[1] ) {

      // Just keep prob as 1
    }
    // Species needs to feed at the bottom of the sea (or there's no data about this) and 
    // point is deeper than the prefered maximum depth but not so deep as the maximum
    // note: using minimum depth layer here
    else if (  x[1] >= _pref_maximum[1] && x[1] < _maximum[1] ) {

      // Linear decay
      prob *= (_maximum[1] - x[1]) / (_maximum[1] - _pref_maximum[1]);
    }
    // Species needs to feed at the bottom of the sea (or there's no data about this) 
    // but depth at the point is greater then the maximum accepted depth
    else {

      return 0.0;
    }
  }
  else { // If there is no depth range

    // Just keep prob as 1
  }

  // For all layers except min/max depths

  int numLayers = x.size();

  for ( int i = 2; i < numLayers; i++ ) {

    // Probability zero for points outside the envelope
    if ( x[i] < _minimum[i] || x[i] > _maximum[i] ) {

      return 0.0;
    }

    // Linear decay for points outside the prefered envelope
    if ( x[i] < _pref_minimum[i] ) {

      prob *= (x[i] - _minimum[i]) / (_pref_minimum[i] - _minimum[i]);
    }
    // Linear decay for points outside the prefered envelope
    else if ( x[i] > _pref_maximum[i] ) {

      prob *= (_maximum[i] - x[i]) / (_maximum[i] - _pref_maximum[i]);
    }
    else {

      // No need to check inside preferred envelope - probability is 1 there, so
      // it would not change the final value
    }
  }

  // There used to be an option to use the geometric mean
  //return pow( prob, (Scalar)1/numLayers );

  // Return probability product
  return prob;
}


/****************************************************************/
/****************** configuration *******************************/
void
AquaMaps::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( ! _done ) {

    return;
  }

  ConfigurationPtr model_config( new ConfigurationImpl( "AquaMaps" ) );
  config->addSubsection( model_config );

  model_config->addNameValue( "Pelagic", _pelagic );
  model_config->addNameValue( "Minimum", _minimum );
  model_config->addNameValue( "PreferedMinimum", _pref_minimum );
  model_config->addNameValue( "PreferedMaximum", _pref_maximum );
  model_config->addNameValue( "Maximum", _maximum );
}


void
AquaMaps::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "AquaMaps", false );

  if ( ! model_config ) {

    return;
  }
  _done = true;

  _pelagic      = model_config->getAttributeAsInt( "Pelagic", -1 );
  _minimum      = model_config->getAttributeAsSample( "Minimum" );
  _pref_minimum = model_config->getAttributeAsSample( "PreferedMinimum" );
  _pref_maximum = model_config->getAttributeAsSample( "PreferedMaximum" );
  _maximum      = model_config->getAttributeAsSample( "Maximum" );

  return;
}


/********************/
/*** log Envelope ***/
void
AquaMaps::logEnvelope()
{
  g_log( "Envelope with %d dimensions (variables).\n\n", _minimum.size() );

  for ( unsigned  int i = 0; i < _minimum.size(); i++ )
    {
      g_log( "Variable %02d:", i );
      g_log( " Minimum         : %f\n", _minimum[i] );
      g_log( " Prefered Minimum: %f\n", _pref_minimum[i] );
      g_log( " Prefered Maximum: %f\n", _pref_maximum[i] );
      g_log( " Maximum         : %f\n", _maximum[i] );
      g_log( "\n" );
    }
}
