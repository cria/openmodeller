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
#include <openmodeller/os_specific.hh>

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <fstream>
#include <sqlite3.h>

// Algorithm is included for std::min and std::max.
#include <algorithm>

using namespace std;


/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 7

/*************************************/
/*** Algorithm parameters metadata ***/

#define PARAM_USE_SURFACE_LAYERS     "UseSurfaceLayers"
#define PARAM_USE_DEPTH_RANGE        "UseDepthRange"
#define PARAM_USE_ICE_CONCENTRATION  "UseIceConcentration"
#define PARAM_USE_DISTANCE_TO_LAND   "UseDistanceToLand"
#define PARAM_USE_PRIMARY_PRODUCTION "UsePrimaryProduction"
#define PARAM_USE_SALINITY           "UseSalinity"
#define PARAM_USE_TEMPERATURE        "UseTemperature"

static AlgParamMetadata parameters[NUM_PARAM] = { // Parameters
   {
      PARAM_USE_SURFACE_LAYERS,       // Id
      "Use surface layers (only for temperature and salinity)", // Name
      Integer,                      // Type
      "Use surface layers (1=yes, 0=no, -1=let the algorithm decide)", // Overview
      "Use surface layers (1=yes, 0=no, -1=let the algorithm decide). By default (-1), aquamaps will try to find the species' depth range in its internal database. If the minimum depth is equals or less than 200m, then aquamaps will use sea surface layers for temperature and salinity. Otherwise it will use bottom layers. This parameter can be used to force aquamaps to use surface or bottom layers.", // Description
      1,    // Not zero if the parameter has lower limit
      -1,   // Parameter's lower limit
      1,    // Not zero if the parameter has upper limit
      1,    // Parameter's upper limit
      "-1"  // Parameter's typical (default) value
   },
   {
      PARAM_USE_DEPTH_RANGE,        // Id
      "Use depth range",            // Name
      Integer,                      // Type
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
      Integer,                      // Type
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
      Integer,                      // Type
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
      Integer,                      // Type
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
      Integer,                      // Type
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
      Integer,                      // Type
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

  "AQUAMAPS", // Id.
  "AquaMaps (beta version)", // Name.
  "0.3", // Version.

  // Overview
  "Environmental envelope modelling algorithm for marine organisms. \
Needs 9 predefined layers (see detailed description) to calculate the \
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
Preferred ranges are usually calculated based on percentiles 10th and 90th. \
They are further adjusted using interquartile values but also ensuring a minimum \
envelope size based on pre-defined values. Under certain circumstances, AquaMaps can \
make use of expert information to define the envelopes. This can happen for mammals \
(any envelope) or for fishes (depth range envelope). Expert information comes from \
FishBase and is stored in a local SQLite database (aquamaps.db) accessed by the \
algorithm. To find information in the database, all occurrences provided to \
openModeller must be labeled with the scientific name (only genus and species). \
Matches are exact, using case-sensitive equals operation. In this version, the \
internal database contains information for more then 7000 marine species.",
  
  "Kaschner, K., J. S. Ready, E. Agbayani, J. Rius, K. Kesner-Reyes, P. D. Eastwood, A. B. South, S. O. Kullander, T. Rees, C. H. Close, R. Watson, D. Pauly, and R. Froese",  // Authors

  // Bibliography.
  "Kaschner, K., J. S. Ready, E. Agbayani, J. Rius, K. Kesner-Reyes, P. D. Eastwood, A. B. South, S. O. Kullander, T. Rees, C. H. Close, R. Watson, D. Pauly, and R. Froese. 2007 AquaMaps: Predicted range maps for aquatic species. World wide web electronic publication, www.aquamaps.org, Version 12/2007.",

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
  _use_layer( NULL ),
  _lower_limit( 7, SURFACE_LOWER_LIMIT ),
  _upper_limit( 7, SURFACE_UPPER_LIMIT ),
  _inner_size( 7, INNER_SIZE ),
  _minimum(),
  _maximum(),
  _pref_minimum(),
  _pref_maximum(),
  _pelagic(-1),
  _use_surface_layers(-1),
  _has_expert_range( NULL ),
  _progress( 0.0 )
{
}

/******************/
/*** destructor ***/

AquaMaps::~AquaMaps()
{
  if ( _use_layer ) {

    delete[] _use_layer;
  }

  if ( _has_expert_range ) {

    delete[] _has_expert_range;
  }
}

/******************/
/*** initialize ***/
int
AquaMaps::initialize()
{
  _has_expert_range = new bool[7];

  // Initialize array
  for ( int i = 0; i < 7; ++i ) {

    _has_expert_range[i] = false;
  }

  // Parameter UseSurfaceLayers
  if ( ! getParameter( PARAM_USE_SURFACE_LAYERS, &_use_surface_layers ) ) {

    _use_surface_layers = -1;
  }

  _use_layer = new int[7];

  // Parameter UseDepthRange
  if ( ! _getAndCheckParameter( PARAM_USE_DEPTH_RANGE, &_use_layer[MAXDEPTH] ) ) {

    return 0;
  }
  else {

    _use_layer[MINDEPTH] = _use_layer[MAXDEPTH]; // just repeat the value
  }

  // Parameter UseIceConcentration
  if ( ! _getAndCheckParameter( PARAM_USE_ICE_CONCENTRATION, &_use_layer[ICE_CONCENTRATION] ) ) {

    return 0;
  }

  // Parameter UseDistanceToLand
  if ( ! _getAndCheckParameter( PARAM_USE_DISTANCE_TO_LAND, &_use_layer[DISTANCE_TO_LAND] ) ) {

    return 0;
  }

  // Parameter UsePrimaryProduction
  if ( ! _getAndCheckParameter( PARAM_USE_PRIMARY_PRODUCTION, &_use_layer[PRIMARY_PRODUCTION] ) ) {

    return 0;
  }

  // Parameter UseSalinity
  if ( ! _getAndCheckParameter( PARAM_USE_SALINITY, &_use_layer[SALINITY] ) ) {

    return 0;
  }

  // Parameter UseTemperature
  if ( ! _getAndCheckParameter( PARAM_USE_TEMPERATURE, &_use_layer[TEMPERATURE] ) ) {

    return 0;
  }

  // Number of independent variables.
  int dim = _samp->numIndependent();
  Log::instance()->info( "Reading %d-dimensional occurrence points.\n", dim );

  // Check the number of layers.
  if ( dim != 9 ) {
  
    Log::instance()->error( "AquaMaps needs precisely 9 layers to work, and they should be in this order: maximum depth in meters, minimum depth in meters, mean annual sea ice concentration, mean annual distance to land in Kilometers, mean annual primary production (chlorophyll A), mean annual sea bottom salinity in psu,, mean annual sea surface salinity in psu, mean annual sea bottom temperature in Celsius, mean annual sea surface temperature in Celsius.\n" ); 
    return 0;
  }

  // Remove duplicates accross geography
  _samp->spatiallyUnique();

  // Check the number of sampled points.
  int npnt = _samp->numPresence();

  if (  npnt < 5 ) {

    Log::instance()->error( "AquaMaps needs at least 5 points inside the mask!\n" );
    return 0;
  }

  return 1;
}


/***************/
/*** iterate ***/
int
AquaMaps::iterate()
{
  Log::instance()->info( "Using %d points to find AquaMaps envelopes.\n", _samp->numPresence() );

  _calculateEnvelopes( _samp->getPresences() );

  return 1;
}


/***********************/
/*** get Convergence ***/
int
AquaMaps::getConvergence( Scalar * const val ) const
{
  *val = 1.0;
  return 1;
}


/********************/
/*** get Progress ***/
float
AquaMaps::getProgress( )
{
  return _progress;
}


/************/
/*** done ***/
int
AquaMaps::done() const
{
  // This is not an iterative algorithm.
  return ( _progress > 0.99 ) ? 1 : 0;
}


/*******************************/
/*** get and check parameter ***/
int 
AquaMaps::_getAndCheckParameter( std::string const &name, int * value )
{
  // Parameter UseSalinity
  if ( ! getParameter( name, value ) ) {

    Log::instance()->error( "Parameter '%s' not passed.\n", name.c_str() );
    return 0;
  }
  // Check if value is 0 or 1
  if ( *value != 0 && *value != 1 ) {

    Log::instance()->error( "Parameter '%s' not set properly. It must be 0 or 1.\n", name.c_str() );
    return 0;
  }

  return 1;
}


/***************************/
/*** calculate envelopes ***/
void
AquaMaps::_calculateEnvelopes( const OccurrencesPtr& occs )
{
  Log::instance()->debug("Species is: %s\n", occs->name());
  Log::instance()->debug("Layers are:\n");
  Log::instance()->debug("0 = Maximum depth\n");
  Log::instance()->debug("1 = Minimum depth\n");
  Log::instance()->debug("2 = Ice concentration\n");
  Log::instance()->debug("3 = Distance to land\n");
  Log::instance()->debug("4 = Primary production (chlorophyll A)\n");
  Log::instance()->debug("5 = Salinity bottom\n");
  Log::instance()->debug("6 = Salinity surface\n");
  Log::instance()->debug("7 = Temperature bottom\n");
  Log::instance()->debug("8 = Temperature surface\n");
  
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
  Sample mean = sample;
  ++oc;

  // Load default min/max
  while ( oc != ocEnd ) {
      
    Sample const& sample = (*oc)->environment();

    // Default min/max will be the actual min/max values
    _minimum &= sample;
    _maximum |= sample;

    mean += sample;

    ++oc;
  }

  mean /= occs->numOccurrences();

  int num_layers = _minimum.size();

  _progress = 1.0f/num_layers; // this is arbitrary

  // Default values for depth ranges
  _minimum[MAXDEPTH] = _minimum[MINDEPTH] = _pref_minimum[MAXDEPTH] = _pref_minimum[MINDEPTH] = 0.0;
  _maximum[MAXDEPTH] = _maximum[MINDEPTH] = _pref_maximum[MAXDEPTH] = _pref_maximum[MINDEPTH] = 9999.0;

  // Try to get expert information about depth range from database
  _readSpeciesData( occs->name() );

  if ( _use_surface_layers == -1 ) {

    _use_surface_layers = ( _minimum[MINDEPTH] <= DEPTH_LIMIT ) ? 0 : 1;
  }

  if ( ! _use_surface_layers ) {

    Log::instance()->info("Using bottom layers.\n");

    for ( int i = 0; i < 7; ++i ) {

      _lower_limit[i] = BOTTOM_LOWER_LIMIT[i];
      _upper_limit[i] = BOTTOM_UPPER_LIMIT[i];
    }
  }
  else {

    Log::instance()->info("Using surface layers.\n");
  }

  _progress = 2.0f/num_layers; // this is arbitrary

  // Get matrix data structure so that we can sort values for each layer
  std::vector<ScalarVector> matrix = occs->getEnvironmentMatrix();

  Scalar nodata = -1.0;
  
  // For each layer (except min/max depth), calculate the percentiles and set 
  // default values for the prefered min/max
  ScalarVector::iterator lStart, lEnd;

  for ( int j = 2; j < num_layers; j++ ) {

    if ( _use_surface_layers ) {

      // Ignore bottom layers
      if ( j == 5 || j == 7 ) {

        _progress = (float)(j+1)/num_layers;

        _minimum[j] = _minimum[j] = _pref_minimum[j] = _pref_minimum[j] = nodata;
      
        continue;
      }
    }
    else {

      // Ignore surface layers
      if ( j == 6 || j == 8 ) {

        _progress = (float)(j+1)/num_layers;

        _minimum[j] = _minimum[j] = _pref_minimum[j] = _pref_minimum[j] = nodata;
      
        continue;
      }
    }

    if ( ( j == 5 || j == 6 ) && _has_expert_range[SALINITY] ) {

      _progress = (float)(j+1)/num_layers;

      continue;
    }

    if ( ( j == 7 || j == 8 ) && _has_expert_range[TEMPERATURE] ) {

      _progress = (float)(j+1)/num_layers;

      continue;
    }

    Log::instance()->debug("--------------------------------\n", j);
    Log::instance()->debug("Calculating envelope for layer %d\n", j);
    Log::instance()->debug("--------------------------------\n", j);

    // 0 = Maximum depth
    // 1 = Minimum depth
    // 2 = Ice concentration
    // 3 = Distance to land
    // 4 = Primary production (chlorophyll A)
    // 5 = Salinity bottom
    // 6 = Salinity surface
    // 7 = Temperature bottom
    // 8 = Temperature surface

    lStart = matrix[j].begin();
    lEnd = matrix[j].end();
    sort( lStart, lEnd );

    int numOccurrences = occs->numOccurrences();

    // Just for debugging
    //ostringstream debug;
    //debug << "row -->";
    //for ( int w = 0; w < numOccurrences; w++ ) {

    //debug << " [" << j << "," << w << "]=" << matrix[j][w];
    //}
    //debug << "\n";
    //Log::instance()->debug( debug.str().c_str() );

    // Calculate percentiles
    Scalar v10, v25, v75, v90;

    _percentile( &v10, numOccurrences, 0.10, &matrix, j );
    _percentile( &v25, numOccurrences, 0.25, &matrix, j );
    _percentile( &v75, numOccurrences, 0.75, &matrix, j );
    _percentile( &v90, numOccurrences, 0.90, &matrix, j );

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
    if ( j > 2 ) {

      _adjustInterquartile( j, adjmin, adjmax );
      _ensureEnvelopeSize( j );
    }
    else {

      if ( _minimum[j] == 0.0) {

        _minimum[j] = mean[j] - 1.0; // black magic copied from original vb code
      }
    }

    Log::instance()->debug("_After adjustments_\n");
    Log::instance()->debug("min: %f\n", _minimum[j]);
    Log::instance()->debug("prefmin: %f\n", _pref_minimum[j]);
    Log::instance()->debug("prefmax: %f\n", _pref_maximum[j]);
    Log::instance()->debug("max: %f\n", _maximum[j]);

    _progress = (float)(j+1)/num_layers;
  }
}


/******************/
/*** percentile ***/
void 
AquaMaps::_percentile( Scalar *result, int n, double percent, std::vector<ScalarVector> *matrix, int layerIndex )
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
AquaMaps::_readSpeciesData( const char *species )
{
  string dbfullname = omDataPath();

#ifndef WIN32
  dbfullname.append( "/" );
#else
  dbfullname.append( "\\" );
#endif

  dbfullname.append( "aquamaps.db" );

  Log::instance()->debug( "Trying database: %s\n", dbfullname.c_str() );

  ifstream dbfile( dbfullname.c_str(), ios::in );

  if ( ! dbfile ) {

    Log::instance()->warn( "Could not find internal database with species data.\n" );
    return;
  }

  Log::instance()->info( "Using internal database with species data.\n" );
  
  sqlite3 *db;
  
  int rc = sqlite3_open( dbfullname.c_str(), &db);

  if ( rc ) {

    // This will likely never happen since on open, sqlite creates the
    // database if it does not exist.
    Log::instance()->warn( "Could not open internal database: %s\n", sqlite3_errmsg( db ) );
    sqlite3_close( db );
    return;
  }

  // Prepare the sql statement
  const char *pzTail;

  sqlite3_stmt *ppStmt;

  char* sql = sqlite3_mprintf( "select pelagic, provider, depthmin, depthmax, depthprefmin, depthprefmax, iceconmin, iceconmax, iceconprefmin, iceconprefmax, landdistmin, landdistmax, landdistprefmin, landdistprefmax, primprodmin, primprodmax, primprodprefmin, primprodprefmax, salinitymin, salinitymax, salinityprefmin, salinityprefmax, tempmin, tempmax, tempprefmin, tempprefmax from spinfo where species = '%q'", species );

  rc = sqlite3_prepare( db, sql, strlen( sql ), &ppStmt, &pzTail );

  if ( rc == SQLITE_OK ) {

    // Fecth data
    int result_code = sqlite3_step( ppStmt );

    if ( result_code == SQLITE_ROW ) {

      int pelagic           = sqlite3_column_int( ppStmt, 0 );
      const char * provider = (char *)sqlite3_column_text( ppStmt, 1 );
      double depthmin       = sqlite3_column_double( ppStmt, 2 );
      double depthmax       = sqlite3_column_double( ppStmt, 3 );
      double depthprefmin   = sqlite3_column_double( ppStmt, 4 );
      double depthprefmax   = sqlite3_column_double( ppStmt, 5 );

      _pelagic = pelagic;

      // Repeat the information for both min and max depth for symmetry
      _minimum[MAXDEPTH]      = depthmin;
      _minimum[MINDEPTH]      = depthmin;
      _maximum[MAXDEPTH]      = depthmax;
      _maximum[MINDEPTH]      = depthmax;
      _pref_minimum[MAXDEPTH] = depthprefmin;
      _pref_minimum[MINDEPTH] = depthprefmin;
      _pref_maximum[MAXDEPTH] = depthprefmax;
      _pref_maximum[MINDEPTH] = depthprefmax;

      Log::instance()->debug("Values from expert database:\n");
      Log::instance()->debug("provider: %s\n", provider);
      Log::instance()->debug("pelagic: %i\n", pelagic);
      Log::instance()->debug("depthmin: %f\n", depthmin);
      Log::instance()->debug("depthmax: %f\n", depthmax);
      Log::instance()->debug("depthprefmin: %f\n", depthprefmin);
      Log::instance()->debug("depthprefmax: %f\n", depthprefmax);

      if ( sqlite3_column_bytes( ppStmt, 1 ) ) {

        string prov_code( provider );

        if ( prov_code == "MM" ) {

          Log::instance()->info( "Looks like a mammal\n" );

          for ( int i = ICE_CONCENTRATION; i < TEMPERATURE + 1; ++i ) {

            _has_expert_range[i] = _hasExpertRange( ppStmt, i );

            if ( _has_expert_range[i] ) {

               Log::instance()->info( "Found expert range for variable %s\n", NAME[i].c_str() );

               Log::instance()->debug("min: %f\n", _minimum[i]);
               Log::instance()->debug("max: %f\n", _maximum[i]);
               Log::instance()->debug("prefmin: %f\n", _pref_minimum[i]);
               Log::instance()->debug("prefmax: %f\n", _pref_maximum[i]);
            }
          }
	}
      }
    }
    else if ( result_code == SQLITE_DONE ) {

      Log::instance()->warn( "'%s' not found in species database\n", species );
    }
    else {

      Log::instance()->warn( "Could not fetch data from species database: %s\n", sqlite3_errmsg( db ) );
    }
  }
  else {

    Log::instance()->warn( "Could not prepare SQL statement to query species database: %s\n", sqlite3_errmsg( db ) );
  }

  // free memory
  sqlite3_free( sql );

  // close the statement
  sqlite3_finalize( ppStmt );

  // close the database
  sqlite3_close( db );
}


/****************************/
/*** ensure envelope size ***/
bool
AquaMaps::_hasExpertRange( sqlite3_stmt * stmt, int varIndex )
{
  int index;

  if ( varIndex == ICE_CONCENTRATION ) {

    index = 6;
  }
  else if ( varIndex == DISTANCE_TO_LAND ) {

    index = 10;
  }
  else if ( varIndex == PRIMARY_PRODUCTION ) {

    index = 14;
  }
  else if ( varIndex == SALINITY ) {

    index = 18;
  }
  else if ( varIndex == TEMPERATURE ) {

    index = 22;
  }
  else {

    return false;
  }

  if ( ! sqlite3_column_bytes( stmt, index ) ) {

    return false;
  }

  double value = sqlite3_column_double( stmt, index );

  _minimum[varIndex] = value;

  ++index;

  if ( ! sqlite3_column_bytes( stmt, index ) ) {

    return false;
  }

  value = sqlite3_column_double( stmt, index );

  _maximum[varIndex] = value;

  ++index;

  if ( ! sqlite3_column_bytes( stmt, index ) ) {

    return false;
  }

  value = sqlite3_column_double( stmt, index );

  _pref_minimum[varIndex] = value;

  ++index;

  if ( ! sqlite3_column_bytes( stmt, index ) ) {

    return false;
  }

  value = sqlite3_column_double( stmt, index );

  _pref_maximum[varIndex] = value;

  return true;
}


/****************************/
/*** ensure envelope size ***/
void
AquaMaps::_adjustInterquartile( int layerIndex, Scalar adjmin, Scalar adjmax )
{
  if ( adjmin > _lower_limit[_getRelatedIndex(layerIndex)] && adjmin < _minimum[layerIndex] ) {

    _minimum[layerIndex] = adjmin;
  }

  if ( adjmax < _upper_limit[_getRelatedIndex(layerIndex)] && adjmax > _maximum[layerIndex] ) {

    _maximum[layerIndex] = adjmax;
  }
}


/****************************/
/*** ensure envelope size ***/
void
AquaMaps::_ensureEnvelopeSize( int layerIndex )
{
  Scalar halfSize = _inner_size[_getRelatedIndex(layerIndex)]/2;

  // Try to ensure that the prefered envelope has the specified size
  // (but only change if the new values are still within the accepted envelope)
  if ( _pref_maximum[layerIndex] - _pref_minimum[layerIndex] < _inner_size[_getRelatedIndex(layerIndex)] ) {

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

    if ( new_min > _lower_limit[_getRelatedIndex(layerIndex)] ) {

      _minimum[layerIndex] = new_min;
    }
    else {

      _minimum[layerIndex] = _lower_limit[_getRelatedIndex(layerIndex)];
    }
  }

  if ( _maximum[layerIndex] - _pref_maximum[layerIndex] < halfSize ) {

    Scalar new_max = _pref_maximum[layerIndex] + halfSize;

    if ( new_max < _upper_limit[_getRelatedIndex(layerIndex)] ) {

      _maximum[layerIndex] = new_max;
    }
    else {

      _maximum[layerIndex] = _upper_limit[_getRelatedIndex(layerIndex)];
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

  int num_variables_used = 0;

  // Depth probability

  if ( _use_layer[MAXDEPTH] ) { // If user wants to use depth range

    ++num_variables_used;

    if ( _maximum[MAXDEPTH] != 9999 ) {  // If there is a depth range in the internal database

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

    // Ignore layer if user doesn't want to use it
    if ( i < 5 ) {

      if ( ! _use_layer[i] ) {

        continue;
      }
    }
    else if ( i < 7 ) {

      if ( ! _use_layer[SALINITY] ) {

        continue;
      }
    }
    else {

      if ( ! _use_layer[TEMPERATURE] ) {

        continue;
      }
    }
    
    if ( _use_surface_layers ) {

      // Ignore bottom layers
      if ( i == 5 || i == 7 ) {

        continue;
      }
    }
    else 
    {
      // Ignore surface layers
      if ( i == 6 || i == 8 ) {

        continue;
      }
    }

    ++num_variables_used;

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

  // Return probability product
  //return prob;

  // Return geometric mean
  return pow( prob, (Scalar)1/num_variables_used );
}


/****************************************************************/
/****************** configuration *******************************/
void
AquaMaps::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( ! done() ) {

    return;
  }

  ConfigurationPtr model_config( new ConfigurationImpl( "AquaMaps" ) );
  config->addSubsection( model_config );

  model_config->addNameValue( "UseLayer", _use_layer, 7 );
  model_config->addNameValue( "UseSurfaceLayers", _use_surface_layers );
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

    _use_surface_layers = model_config->getAttributeAsInt( "UseSurfaceLayers", -1 );
  }
  catch ( AttributeNotFound& exception ) {

    Log::instance()->error( "Could not find attribute '%s' in serialized model. You may be trying to load a model that was generated with an older version (< 0.2) of this algorithm.\n", exception.getName().c_str() );

    return;
  }

  _pelagic      = model_config->getAttributeAsInt( "Pelagic", -1 );
  _minimum      = model_config->getAttributeAsSample( "Minimum" );
  _pref_minimum = model_config->getAttributeAsSample( "PreferedMinimum" );
  _pref_maximum = model_config->getAttributeAsSample( "PreferedMaximum" );
  _maximum      = model_config->getAttributeAsSample( "Maximum" );

  _progress = 1.0;

  return;
}


/********************/
/*** log Envelope ***/
void
AquaMaps::_logEnvelope()
{
  Log::instance()->info( "Envelope with %d dimensions (variables).\n\n", _minimum.size() );

  for ( unsigned  int i = 0; i < _minimum.size(); i++ ) {

    Log::instance()->info( "Variable %02d:", i );
    Log::instance()->info( " Minimum         : %f\n", _minimum[i] );
    Log::instance()->info( " Prefered Minimum: %f\n", _pref_minimum[i] );
    Log::instance()->info( " Prefered Maximum: %f\n", _pref_maximum[i] );
    Log::instance()->info( " Maximum         : %f\n", _maximum[i] );
    Log::instance()->info( "\n" );
  }
}

int 
AquaMaps::_getRelatedIndex( int index )
{
  if ( index == 8 ) {

    return index - 2;
  }
  else if ( index > 5 ) {

    return index - 1;
  }

  return index;
}
