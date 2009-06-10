/**
 * Declaration of AquaMaps Algorithm.
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

#ifndef _AQUAMAPSHH_
#define _AQUAMAPSHH_

#include <openmodeller/om.hh>

#include <string.h>
#include <sqlite3.h>

/********************************************************/
/*********************** AquaMaps ***********************/

/** Notes
 * 
 * This algorithm requires access to a SQLite database (aquamaps.db)
 * which contains depth range information about marine species.
 *
 * The original database comes from the Incofish projet (http://www.incofish.org/).
 * It is an Access database with the main tables: HCAF (env data), HSPEC (probabilities), 
 * HSPEN (species data). 
 *
 * To export data to the SQLite database it is necessary to create a query in the Access
 * database using this SQL statement:
 * 
 * SELECT HSPEN.SpeciesID, SpeciesOccursum.genus+' '+SpeciesOccursum.species AS name, hspen.pelagic, Speciesaddinfo.Provider, hspen.depthmin, hspen.depthprefmin, hspen.depthprefmax, hspen.depthmax, hspen.tempmin, hspen.tempmax, hspen.tempprefmin, hspen.tempprefmax, hspen.salinitymin, hspen.salinitymax, hspen.salinityprefmin, hspen.salinityprefmax, hspen.primprodmin, hspen.primprodmax, hspen.primprodprefmin, hspen.primprodprefmax,  hspen.iceconmin, hspen.iceconmax, hspen.iceconprefmin, hspen.iceconprefmax, hspen.landdistmin, hspen.landdistmax, hspen.landdistprefmin, hspen.landdistprefmax
FROM (hspen INNER JOIN SpeciesOccursum ON HSPEN.SpeciesID=SpeciesOccursum.SPECIESID) LEFT JOIN Speciesaddinfo ON HSPEN.SpeciesID=Speciesaddinfo.SpeciesID
WHERE hspen.depthmin Is Not Null
And Speciesaddinfo.Provider = "MM"
UNION SELECT HSPEN.SpeciesID, SpeciesOccursum.genus+' '+SpeciesOccursum.species AS name, hspen.pelagic, Speciesaddinfo.Provider, hspen.depthmin, hspen.depthprefmin, hspen.depthprefmax, hspen.depthmax, Null AS tempmin, Null AS tempmax, Null AS tempprefmin, Null AS tempprefmax, Null AS salinitymin, Null AS salinitymax, Null AS salinityprefmin, Null AS salinityprefmax, Null AS primprodmin,  Null AS primprodmax, Null AS primprodprefmin,  Null AS primprodprefmax,  Null AS iceconmin,  Null AS iceconmax,  Null AS iceconprefmin,  Null AS iceconprefmax,  Null AS landdistmin,  Null AS landdistmax,  Null AS landdistprefmin,  Null AS landdistprefmax
FROM (hspen INNER JOIN SpeciesOccursum ON HSPEN.SpeciesID=SpeciesOccursum.SPECIESID) LEFT JOIN Speciesaddinfo ON HSPEN.SpeciesID=Speciesaddinfo.SpeciesID
WHERE hspen.depthmin Is Not Null
And Speciesaddinfo.Provider <> "MM";
 *
 * The result of this query must be exported to a csv file in US-ASCII, without header, 
 * separated by TABs, without text qualifier and using "." as the decimal separator.
 *
 * To create and import data to a SQLite database, use:
 *
 * sqlite3 aquamaps.db
 * sqlite> CREATE TABLE spinfo (id TEXT, species TEXT, pelagic INTEGER, provider TEXT, depthmin REAL, depthprefmin REAL, depthprefmax REAL, depthmax REAL, tempmin REAL, tempmax REAL, tempprefmin REAL, tempprefmax REAL, salinitymin REAL, salinitymax REAL, salinityprefmin REAL, salinityprefmax REAL, primprodmin REAL, primprodmax REAL, primprodprefmin REAL, primprodprefmax REAL, iceconmin REAL, iceconmax REAL, iceconprefmin REAL, iceconprefmax REAL, landdistmin REAL, landdistmax REAL, landdistprefmin REAL, landdistprefmax REAL);
 * sqlite> .separator \t
 * sqlite> .import location_of_csv_file spinfo
 * sqlite> CREATE INDEX idx_species ON spinfo(species);
 * sqlite> .q
 * 
 *
 * In case you are wondering why this algorithm requires all 9 layers to be specified
 * (instead of letting the user specify a smaller set only with bottom layers or 
 * surface layers) the reason is that users may not know whether to choose bottom or
 * surface layers. In this case, if the species has depth ranges in the local database,
 * then the algorithm will be able to decide what subset of layers to use.
 */

/****************************************/
/*********** Expert's values ************/

const Scalar DEPTH_LIMIT = 200.0; // used to choose between surface or bottom layers (temp/sal)

const Scalar MINIMUM_MAXDEPTH = 0.0;
const Scalar MAXIMUM_MAXDEPTH = 9999.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_MAXDEPTH = 0.0; // not used, just defined

const Scalar MINIMUM_MINDEPTH = 0.0;
const Scalar MAXIMUM_MINDEPTH = 9999.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_MINDEPTH = 0.0; // not used, just defined

const Scalar MINIMUM_ICE_CONCENTRATION = 0.0;
const Scalar MAXIMUM_ICE_CONCENTRATION = 1.5;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_ICE_CONCENTRATION = 0.0; // not used, just defined

const Scalar MINIMUM_DISTANCE_TO_LAND = 0.0;
const Scalar MAXIMUM_DISTANCE_TO_LAND = 4200.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_DISTANCE_TO_LAND = 2.0;

const Scalar MINIMUM_PRIMARY_PRODUCTION = 0.0;
const Scalar MAXIMUM_PRIMARY_PRODUCTION = 6000.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_PRIMARY_PRODUCTION = 2.0;

const Scalar MINIMUM_SURFACE_SALINITY = 3.56;
const Scalar MAXIMUM_SURFACE_SALINITY = 40.2;

const Scalar MINIMUM_BOTTOM_SALINITY = 3.5;
const Scalar MAXIMUM_BOTTOM_SALINITY = 40.9;

const Scalar MINIMUM_ENVELOPE_SIZE_FOR_SALINITY = 1.0;

const Scalar MINIMUM_SURFACE_TEMPERATURE = -2.0;
const Scalar MAXIMUM_SURFACE_TEMPERATURE = 30.0;

const Scalar MINIMUM_BOTTOM_TEMPERATURE = -2.0;
const Scalar MAXIMUM_BOTTOM_TEMPERATURE = 30.0;

const Scalar MINIMUM_ENVELOPE_SIZE_FOR_TEMPERATURE = 1.0;

const Scalar SURFACE_LOWER_LIMIT [7] = { MINIMUM_MAXDEPTH,
                                         MINIMUM_MINDEPTH,
                                         MINIMUM_ICE_CONCENTRATION,
                                         MINIMUM_DISTANCE_TO_LAND,
                                         MINIMUM_PRIMARY_PRODUCTION,
                                         MINIMUM_SURFACE_SALINITY,
                                         MINIMUM_SURFACE_TEMPERATURE };

const Scalar BOTTOM_LOWER_LIMIT [7] = { MINIMUM_MAXDEPTH,
                                        MINIMUM_MINDEPTH,
                                        MINIMUM_ICE_CONCENTRATION,
                                        MINIMUM_DISTANCE_TO_LAND,
                                        MINIMUM_PRIMARY_PRODUCTION,
                                        MINIMUM_BOTTOM_SALINITY,
                                        MINIMUM_BOTTOM_TEMPERATURE };

const Scalar SURFACE_UPPER_LIMIT [7] = { MAXIMUM_MAXDEPTH,
                                         MAXIMUM_MINDEPTH,
                                         MAXIMUM_ICE_CONCENTRATION,
                                         MAXIMUM_DISTANCE_TO_LAND,
                                         MAXIMUM_PRIMARY_PRODUCTION,
                                         MAXIMUM_SURFACE_SALINITY,
                                         MAXIMUM_SURFACE_TEMPERATURE };

const Scalar BOTTOM_UPPER_LIMIT [7] = { MAXIMUM_MAXDEPTH,
                                        MAXIMUM_MINDEPTH,
                                        MAXIMUM_ICE_CONCENTRATION,
                                        MAXIMUM_DISTANCE_TO_LAND,
                                        MAXIMUM_PRIMARY_PRODUCTION,
                                        MAXIMUM_BOTTOM_SALINITY,
                                        MAXIMUM_BOTTOM_TEMPERATURE };

const Scalar INNER_SIZE [7] = { MINIMUM_ENVELOPE_SIZE_FOR_MAXDEPTH,
                                MINIMUM_ENVELOPE_SIZE_FOR_MINDEPTH,
                                MINIMUM_ENVELOPE_SIZE_FOR_ICE_CONCENTRATION,
                                MINIMUM_ENVELOPE_SIZE_FOR_DISTANCE_TO_LAND,
                                MINIMUM_ENVELOPE_SIZE_FOR_PRIMARY_PRODUCTION,
                                MINIMUM_ENVELOPE_SIZE_FOR_SALINITY,
                                MINIMUM_ENVELOPE_SIZE_FOR_TEMPERATURE };

const std::string NAME [7] = { "maximum depth",
                               "minimum depth",
                               "ice concentration",
                               "distance to land",
                               "primary production",
                               "salinity",
                               "temperature" };

// Variable position in sample or arrays

const int MAXDEPTH = 0;
const int MINDEPTH = 1;
const int ICE_CONCENTRATION = 2;
const int DISTANCE_TO_LAND = 3;
const int PRIMARY_PRODUCTION = 4;
const int SALINITY = 5;
const int TEMPERATURE = 6;

/** 
 * AquaMaps Algorithm
 *
 */
class AquaMaps : public AlgorithmImpl
{
public:

  AquaMaps();
  ~AquaMaps();

  // Basic methods inherited from Algorithm class

  int initialize();
  int iterate();
  int getConvergence( Scalar * const val ) const;
  float getProgress();
  int done() const;
  Scalar getValue( const Sample& x ) const;

protected:

  // Model Serialization
  virtual void _getConfiguration( ConfigurationPtr& ) const;

  // Model Deserialization
  virtual void _setConfiguration( const ConstConfigurationPtr & );

private:

  /** Get a Boolean parameter and check its value.
   *   
   *  @param name Parameter name.
   *  @param value Parameter value.
   *  @return Zero if the parameter does not exists or the
   *   parameters were not set yet.
   **/
  int _getAndCheckParameter( std::string const &name, int * value );

  /** Calculate the envelopes */
  void _calculateEnvelopes( const OccurrencesPtr& );

  /** Calculate a percentile */
  void _percentile( Scalar *result, int n, double percent, std::vector<ScalarVector> *matrix, int layerIndex );

  /** Read species data from expert database */
  void _readSpeciesData( const char *species );

  /** Fetch expert range data for a specific variable */
  bool _hasExpertRange( sqlite3_stmt * stmt, int varIndex );

  /** Make the interquartile adjusting */
  void _adjustInterquartile( int layerIndex, Scalar adjmin, Scalar adjmax );

  /** Ensure that the envelope has a minimum size */
  void _ensureEnvelopeSize( int layerIndex );

  /** Log the bioclimatic envelopes information. */
  void _logEnvelope();

  /** Return the 0-6 index from the constants defined in this header related to the 0-8 index from the complete set of layers. */
  int _getRelatedIndex( int index );

  /** Indicates if the respective layer should be used or not when calculating probabilities */
  int * _use_layer;

  /** Lower limit for each variable */
  Sample _lower_limit;

  /** Upper limit for each variable */
  Sample _upper_limit;

  /** Recommended values for the inner envelope sizes */
  Sample _inner_size;

  /** Minimum value for each variable. */
  Sample _minimum;

  /** Maximum value for each variable. */
  Sample _maximum;

  /** Prefered minimum for each variable. */
  Sample _pref_minimum;

  /** Prefered maximum for each variable. */
  Sample _pref_maximum;

  /** Indicates if the species is pelagic or not (-1 indicates no data).
   *  Note: pelagic means "belonging to the upper layers of the open sea" (poetic, huh?).
   */
  int _pelagic;

  /** Indicates if surface layers should be used for temperature and salinity (-1 means that
   *  aquamaps should try to find out by using depth ranges specified in its internal database).
   */
  int _use_surface_layers;

  /** Indicates if the variable has an expert range in the local database. */
  bool * _has_expert_range;

  /** Indication of progress during model creation. */
  float _progress;
};


#endif

