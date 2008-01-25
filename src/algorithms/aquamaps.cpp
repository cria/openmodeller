/**
 * Definition of AquaMaps Algorithm.
 * 
 * @author Renato De Giovanni <renato [at] cria dot org dot br>
 * @date 2006-08-07
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2006 by CRIA -
 * Centro de Refer�ncia em Informa��o Ambiental
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

#define NUM_PARAM 6

/*************************************/
/*** Algorithm parameters metadata ***/

#define PARAM_USE_DEPTH_RANGE        "UseDepthRange"
#define PARAM_USE_ICE_CONCENTRATION  "UseIceConcentration"
#define PARAM_USE_DISTANCE_TO_LAND   "UseDistanceToLand"
#define PARAM_USE_PRIMARY_PRODUCTION "UsePrimaryProduction"
#define PARAM_USE_SALINITY           "UseSalinity"
#define PARAM_USE_TEMPERATURE        "UseTemperature"

static AlgParamMetadata parameters[NUM_PARAM] = { // Parameters
   {
      PARAM_USE_DEPTH_RANGE,        // Id
      "Use depth range",            // Name
      "Integer",                    // Type
      "Use depth range when calculating probabilities", // Overview
      "Use depth range provided by experts (if available) when calculating probabilities", // Description
      1,    // Not zero if the parameter has lower limit
      0,    // Parameter's lower limit
      1,    // Not zero if the parameter has upper limit
      1,    // Parameter's upper limit
      "1"   // Parameter's typical (default) value
   },
   {
      PARAM_USE_ICE_CONCENTRATION,  // Id
      "Use ice concentration",      // Name
      "Integer",                    // Type
      "Use ice concentration envelope when calculating probabilities", // Overview
      "Use ice concentration when calculating probabilities", // Description
      1,    // Not zero if the parameter has lower limit
      0,    // Parameter's lower limit
      1,    // Not zero if the parameter has upper limit
      1,    // Parameter's upper limit
      "1"   // Parameter's typical (default) value
   },
   {
      PARAM_USE_DISTANCE_TO_LAND,   // Id
      "Use distance to land",       // Name
      "Integer",                    // Type
      "Use distance to land envelope when calculating probabilities", // Overview
      "Use distance to land envelope when calculating probabilities", // Description
      1,    // Not zero if the parameter has lower limit
      0,    // Parameter's lower limit
      1,    // Not zero if the parameter has upper limit
      1,    // Parameter's upper limit
      "0"   // Parameter's typical (default) value
   },
   {
      PARAM_USE_PRIMARY_PRODUCTION, // Id
      "Use primary production",     // Name
      "Integer",                    // Type
      "Use primary production envelope when calculating probabilities", // Overview
      "Use primary production envelope when calculating probabilities", // Description
      1,    // Not zero if the parameter has lower limit
      0,    // Parameter's lower limit
      1,    // Not zero if the parameter has upper limit
      1,    // Parameter's upper limit
      "1"   // Parameter's typical (default) value
   },
   {
      PARAM_USE_SALINITY,           // Id
      "Use salinity",               // Name
      "Integer",                    // Type
      "Use salinity envelope when calculating probabilities", // Overview
      "Use salinity envelope when calculating probabilities", // Description
      1,    // Not zero if the parameter has lower limit
      0,    // Parameter's lower limit
      1,    // Not zero if the parameter has upper limit
      1,    // Parameter's upper limit
      "1"   // Parameter's typical (default) value
   },
   {
      PARAM_USE_TEMPERATURE,        // Id
      "Use temperature",            // Name
      "Integer",                    // Type
      "Use temperature envelope when calculating probabilities", // Overview
      "Use temperature envelope when calculating probabilities", // Description
      1,    // Not zero if the parameter has lower limit
      0,    // Parameter's lower limit
      1,    // Not zero if the parameter has upper limit
      1,    // Parameter's upper limit
      "1"   // Parameter's typical (default) value
   },
};


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "AquaMaps",  // Id.
  "AquaMaps",  // Name.
  "0.2",       // Version.

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
mgC per square meter per day), mean annual bottom salinity in psu, mean annual \
surface salinity in psu, mean annual bottom temperature in celsius, mean annual \
surface temperature in celsius. These layers can be downloaded from: \n\
http://openmodeller.cria.org.br/download/marine2.zip \n\
AquaMaps also makes use of expert information for the lower and upper limits of \
each variable. Depth ranges are taken from a database provided by FishBase. To find \
this information in the database, occurrences must be identified by the scientific \
name (only genus and species). The database contains depth ranges for about 9k \
different marine species. \n\
For the other variables, preferred ranges are initially calculated based on \
percentiles 10th and 90th. They are further adjusted using interquartile values but \
also ensuring a minimum envelope size based on pre-defined values.",
  
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
  _use_layer( NULL ),
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
  delete _use_layer;
}

/******************/
/*** initialize ***/
int
AquaMaps::initialize()
{
  _use_layer = new int[7];

  // Parameter UseDepthRange
  if ( ! getAndCheckParameter( PARAM_USE_DEPTH_RANGE, &_use_layer[MAXDEPTH] ) ) {

    return 0;
  }
  else {

    _use_layer[MINDEPTH] = _use_layer[MAXDEPTH]; // just repeat the value
  }

  // Parameter UseIceConcentration
  if ( ! getAndCheckParameter( PARAM_USE_ICE_CONCENTRATION, &_use_layer[ICE_CONCENTRATION] ) ) {

    return 0;
  }

  // Parameter UseDistanceToLand
  if ( ! getAndCheckParameter( PARAM_USE_DISTANCE_TO_LAND, &_use_layer[DISTANCE_TO_LAND] ) ) {

    return 0;
  }

  // Parameter UsePrimaryProduction
  if ( ! getAndCheckParameter( PARAM_USE_PRIMARY_PRODUCTION, &_use_layer[PRIMARY_PRODUCTION] ) ) {

    return 0;
  }

  // Parameter UseSalinity
  if ( ! getAndCheckParameter( PARAM_USE_SALINITY, &_use_layer[SALINITY] ) ) {

    return 0;
  }

  // Parameter UseTemperature
  if ( ! getAndCheckParameter( PARAM_USE_TEMPERATURE, &_use_layer[TEMPERATURE] ) ) {

    return 0;
  }

  // Number of independent variables.
  int dim = _samp->numIndependent();
  Log::instance()->info( "Reading %d-dimensional occurrence points.\n", dim );

  // Check the number of layers.
  if ( dim != 7 ) {
  
    Log::instance()->error( 1, "AquaMaps needs precisely 7 layers to work, and they should be in this order: maximum depth in meters, minimum depth in meters, mean annual sea ice concentration, mean annual distance to land in Kilometers, mean annual primary production (chlorophyll A), mean annual salinity in psu, mean annual sea surface temperature in Celsius.\n" ); 
    // Log::instance()->error() does a ::exit(rc).
  }

  // Remove duplicates accross geography
  _samp->spatiallyUnique();

  // Check the number of sampled points.
  int npnt = _samp->numPresence();

  if (  npnt < 5 ) {

    Log::instance()->error( 1, "AquaMaps needs at least 5 points inside the mask!\n" ); 
    // Log::instance()->error() does a ::exit(rc).
  }

  Log::instance()->info( "Using %d points to find AquaMaps envelopes.\n", npnt );

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


/*******************************/
/*** get and check parameter ***/
int 
AquaMaps::getAndCheckParameter( std::string const &name, int * value )
{
  // Parameter UseSalinity
  if ( ! getParameter( name, value ) ) {

    Log::instance()->error( 1, "Parameter '%s' not passed.\n", name.c_str() );
    return 0;
  }
  // Check if value is 0 or 1
  if ( *value != 0 && *value != 1 ) {

    Log::instance()->error( 1, "Parameter '%s' not set properly. It must be 0 or 1.\n", name.c_str() );
    return 0;
  }

  return 1;
}


/***************************/
/*** calculate envelopes ***/
void
AquaMaps::calculateEnvelopes( const OccurrencesPtr& occs )
{

  Log::instance()->debug("Species is: %s\n", occs->name());
  Log::instance()->debug("Layers are:\n");
  Log::instance()->debug("0 = Maximum depth\n");
  Log::instance()->debug("1 = Minimum depth\n");
  Log::instance()->debug("2 = Ice concentration\n");
  Log::instance()->debug("3 = Distance to land\n");
  Log::instance()->debug("4 = Primary production (chlorophyll A)\n");
  Log::instance()->debug("5 = Salinity\n");
  Log::instance()->debug("6 = Surface temperature\n");

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
  _minimum[MAXDEPTH] = _minimum[MINDEPTH] = _pref_minimum[MAXDEPTH] = _pref_minimum[MINDEPTH] = 0;
  _maximum[MAXDEPTH] = _maximum[MINDEPTH] = _pref_maximum[MAXDEPTH] = _pref_maximum[MINDEPTH] = 9999;

  // Try to get expert information about depth range from database
  readDepthData( occs->name() );

  // Get matrix data structure so that we can sort values for each layer
  std::vector<ScalarVector> matrix = occs->getEnvironmentMatrix();

  // For each layer (except min/max depth), calculate the percentiles and set 
  // default values for the prefered min/max
  ScalarVector::iterator lStart, lEnd;

  for ( unsigned int j = 2; j < matrix.size(); j++ ) {

    Log::instance()->debug("--------------------------------\n", j);
    Log::instance()->debug("Calculating envelope for layer %d\n", j);
    Log::instance()->debug("--------------------------------\n", j);

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
    Log::instance()->debug( debug.str().c_str() );

    // Calculate percentiles
    Scalar v10, v25, v75, v90;

    percentile( &v10, numOccurrences, 0.10, &matrix, j );
    percentile( &v25, numOccurrences, 0.25, &matrix, j );
    percentile( &v75, numOccurrences, 0.75, &matrix, j );
    percentile( &v90, numOccurrences, 0.90, &matrix, j );

    Log::instance()->debug("10th percentile: %f\n", v10);
    Log::instance()->debug("25th percentile: %f\n", v25);
    Log::instance()->debug("75th percentile: %f\n", v75);
    Log::instance()->debug("90th percentile: %f\n", v90);

    Scalar interquartile = fabs( v25 - v75 );

    Log::instance()->debug("Interquartile: %f\n", interquartile);

    Scalar adjmin = v25 - ( 1.5 * interquartile );
    Scalar adjmax = v75 + ( 1.5 * interquartile );

    Log::instance()->debug("Adjmin: %f\n", adjmin);
    Log::instance()->debug("Adjmax: %f\n", adjmax);

    _pref_minimum[j] = v10;
    _pref_maximum[j] = v90;

    Log::instance()->debug("_Before adjustments_\n");
    Log::instance()->debug("min: %f\n", _minimum[j]);
    Log::instance()->debug("prefmin: %f\n", _pref_minimum[j]);
    Log::instance()->debug("prefmax: %f\n", _pref_maximum[j]);
    Log::instance()->debug("max: %f\n", _maximum[j]);

    // Make the interquartile adjusting and ensure the envelope sizes
    // for all variables except ice concentration
    if ( j > 1 ) {

      adjustInterquartile( j, adjmin, adjmax );
      ensureEnvelopeSize( j );
    }

    Log::instance()->debug("_After adjustments_\n");
    Log::instance()->debug("min: %f\n", _minimum[j]);
    Log::instance()->debug("prefmin: %f\n", _pref_minimum[j]);
    Log::instance()->debug("prefmax: %f\n", _pref_maximum[j]);
    Log::instance()->debug("max: %f\n", _maximum[j]);
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
    Log::instance()->warn( "Could not open database with depth range data: %s\n", sqlite3_errmsg( db ) );
    sqlite3_close(db);
    return;
  }

  // Prepare the sql statement
  const char *pzTail;

  sqlite3_stmt *ppStmt;

  char* sql = sqlite3_mprintf( "select pelagic, min, prefmin, prefmax, max from spinfo where species = '%q'", species );

  rc = sqlite3_prepare( db, sql, strlen( sql ), &ppStmt, &pzTail );

  if ( rc == SQLITE_OK ) {

    // Fecth data
    if ( sqlite3_step( ppStmt ) == SQLITE_ROW ) {

      int    pelagic = sqlite3_column_int( ppStmt, 0 );
      double min     = sqlite3_column_double( ppStmt, 1 );
      double prefmin = sqlite3_column_double( ppStmt, 2 );
      double prefmax = sqlite3_column_double( ppStmt, 3 );
      double max     = sqlite3_column_double( ppStmt, 4 );

      _pelagic = pelagic;

      // Repeat the information for both min and max depth for symmetry
      _minimum[MAXDEPTH]      = min;
      _minimum[MINDEPTH]      = min;
      _pref_minimum[MAXDEPTH] = prefmin;
      _pref_minimum[MINDEPTH] = prefmin;
      _pref_maximum[MAXDEPTH] = prefmax;
      _pref_maximum[MINDEPTH] = prefmax;
      _maximum[MAXDEPTH]      = max;
      _maximum[MINDEPTH]      = max;

      Log::instance()->debug("Depth values from database:\n");
      Log::instance()->debug("pelagic: %i\n", pelagic);
      Log::instance()->debug("min: %f\n", min);
      Log::instance()->debug("prefmin: %f\n", prefmin);
      Log::instance()->debug("prefmax: %f\n", prefmax);
      Log::instance()->debug("max: %f\n", max);
    }
    else {

      Log::instance()->warn( "Could not fetch data from depth range database: %s\n", 
                  sqlite3_errmsg( db ) );
    }
  }
  else {

    Log::instance()->warn( "Could not prepare SQL statement to query depth range database: %s\n", 
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

  if ( _use_layer[MAXDEPTH] ) {

    if ( _maximum[MAXDEPTH] != 9999 ) {  // If there is a depth range

      // Probability of occurrence is zero if depth at this point is less
      // than the minimum depth for the species.
      // note: using maximum depth layer here
      if ( x[MAXDEPTH] < _minimum[MAXDEPTH] ) {

          return 0.0;
      }

      // Point is sufficiently deep, but still not enough for the prefered range
      // note: using maximum depth layer here
      if ( x[MAXDEPTH] >= _minimum[MAXDEPTH] && x[MAXDEPTH] < _pref_minimum[MAXDEPTH] ) {

        // Linear decay
        prob *= (x[MAXDEPTH] - _minimum[MAXDEPTH]) / (_pref_minimum[MAXDEPTH] - _minimum[MAXDEPTH]);
      }
      // Point is sufficiently deep for the prefered range
      // Note: pelagic means "belonging to the upper layers of the open sea". Or in other 
      //       words, no need to feed at the bottom of the sea.
      else if ( _pelagic == 1 ) {

        // Just keep prob as 1
      }
      // Species needs to feed at the bottom of the sea (or there's no data about this) 
      // and point is within the prefered range
      // note: the inner envelope logic makes use of both minimum and maximum depth layers.
      else if ( x[MAXDEPTH] >= _pref_minimum[MAXDEPTH] && x[MINDEPTH] <= _pref_maximum[MINDEPTH] ) {

        // Just keep prob as 1
      }
      // Species needs to feed at the bottom of the sea (or there's no data about this) and 
      // point is deeper than the prefered maximum depth but not so deep as the maximum
      // note: using minimum depth layer here
      else if ( x[MINDEPTH] >= _pref_maximum[MINDEPTH] && x[MINDEPTH] < _maximum[MINDEPTH] ) {

        // Linear decay
        prob *= (_maximum[MINDEPTH] - x[MINDEPTH]) / (_maximum[MINDEPTH] - _pref_maximum[MINDEPTH]);
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
  }
  else { // If user doesn't want to use depth ranges

    // Just keep prob as 1
  }

  // For all layers except min/max depths

  int numLayers = x.size();

  for ( int i = 2; i < numLayers; i++ ) {

    if ( ! _use_layer[i] ) {

      // Ignore layer if user doesn't want to use it
      continue;
    }

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

  model_config->addNameValue( "UseLayer", _use_layer, 7 );
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

  try {

    int size;

    model_config->getAttributeAsIntArray( "UseLayer", &_use_layer, &size );
  }
  catch ( AttributeNotFound& exception ) {

    Log::instance()->error( 1, "Could not find UseLayer attribute in serialized model. You may be trying to load a model that was generated by an older version (< 0.2) of this algorithm.\n" );

    return;
  }

  _pelagic      = model_config->getAttributeAsInt( "Pelagic", -1 );
  _minimum      = model_config->getAttributeAsSample( "Minimum" );
  _pref_minimum = model_config->getAttributeAsSample( "PreferedMinimum" );
  _pref_maximum = model_config->getAttributeAsSample( "PreferedMaximum" );
  _maximum      = model_config->getAttributeAsSample( "Maximum" );

  _done = true;

  return;
}


/********************/
/*** log Envelope ***/
void
AquaMaps::logEnvelope()
{
  Log::instance()->info( "Envelope with %d dimensions (variables).\n\n", _minimum.size() );

  for ( unsigned  int i = 0; i < _minimum.size(); i++ )
    {
      Log::instance()->info( "Variable %02d:", i );
      Log::instance()->info( " Minimum         : %f\n", _minimum[i] );
      Log::instance()->info( " Prefered Minimum: %f\n", _pref_minimum[i] );
      Log::instance()->info( " Prefered Maximum: %f\n", _pref_maximum[i] );
      Log::instance()->info( " Maximum         : %f\n", _maximum[i] );
      Log::instance()->info( "\n" );
    }
}