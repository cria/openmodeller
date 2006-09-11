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

#ifndef _AQUAMAPSHH_
#define _AQUAMAPSHH_

#include <openmodeller/om.hh>

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
 * SELECT s.genus + ' ' + s.species AS name, 
 *        h.depthmin, h.depthprefmin, h.depthprefmax, h.depthmax, h.pelagic
 * FROM hspen AS h, speciesoccursum AS s
 * WHERE h.speciesid=s.speciesid And h.depthmin Is Not Null;
 *
 * The result of this query must be exported to a csv file without header and separated by TABs.
 *
 * To create and import data to a SQLite database, use:
 *
 * sqlite3 aquamaps.db
 * sqlite> CREATE TABLE depthinfo (species TEXT, min REAL, prefmin REAL, prefmax REAL, max REAL, pelagic INTEGER);
 * sqlite> .separator \t
 * sqlite> .import location_of_csv_file depthinfo
 * sqlite> .q
 * 
 */

/****************************************/
/*********** Expert's values ************/

const Scalar MINIMUM_MINDEPTH = 0.0;
const Scalar MAXIMUM_MINDEPTH = 9999.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_MINDEPTH = 0.0; // not used, just defined

const Scalar MINIMUM_MAXDEPTH = 0.0;
const Scalar MAXIMUM_MAXDEPTH = 9999.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_MAXDEPTH = 0.0; // not used, just defined

const Scalar MINIMUM_ICE_CONCENTRATION = 0.0;
const Scalar MAXIMUM_ICE_CONCENTRATION = 8000.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_ICE_CONCENTRATION = 2.0;

const Scalar MINIMUM_DISTANCE_TO_LAND = 0.0;
const Scalar MAXIMUM_DISTANCE_TO_LAND = 4200.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_DISTANCE_TO_LAND = 2.0;

const Scalar MINIMUM_PRIMARY_PRODUCTION = 2.5765575E-09;
const Scalar MAXIMUM_PRIMARY_PRODUCTION = 3.9697;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_PRIMARY_PRODUCTION = 0.2;

const Scalar MINIMUM_SALINITY = 3.56;
const Scalar MAXIMUM_SALINITY = 40.2;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_SALINITY = 1.0;

const Scalar MINIMUM_SURFACE_TEMPERATURE = -2.0;
const Scalar MAXIMUM_SURFACE_TEMPERATURE = 30.0;
const Scalar MINIMUM_ENVELOPE_SIZE_FOR_SURFACE_TEMPERATURE = 1.0;

const Scalar LOWER_LIMIT [7] = { MINIMUM_MINDEPTH,
                                 MINIMUM_MAXDEPTH,
                                 MINIMUM_ICE_CONCENTRATION,
                                 MINIMUM_DISTANCE_TO_LAND,
                                 MINIMUM_PRIMARY_PRODUCTION,
                                 MINIMUM_SALINITY,
                                 MINIMUM_SURFACE_TEMPERATURE };

const Scalar UPPER_LIMIT [7] = { MAXIMUM_MINDEPTH,
                                 MAXIMUM_MAXDEPTH,
                                 MAXIMUM_ICE_CONCENTRATION,
                                 MAXIMUM_DISTANCE_TO_LAND,
                                 MAXIMUM_PRIMARY_PRODUCTION,
                                 MAXIMUM_SALINITY,
                                 MAXIMUM_SURFACE_TEMPERATURE };

const Scalar INNER_SIZE [7] = { MINIMUM_ENVELOPE_SIZE_FOR_MINDEPTH,
                                MINIMUM_ENVELOPE_SIZE_FOR_MAXDEPTH,
                                MINIMUM_ENVELOPE_SIZE_FOR_ICE_CONCENTRATION,
                                MINIMUM_ENVELOPE_SIZE_FOR_DISTANCE_TO_LAND,
                                MINIMUM_ENVELOPE_SIZE_FOR_PRIMARY_PRODUCTION,
                                MINIMUM_ENVELOPE_SIZE_FOR_SALINITY,
                                MINIMUM_ENVELOPE_SIZE_FOR_SURFACE_TEMPERATURE };

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
  int getConvergence( Scalar *val );
  int done() const;
  Scalar getValue( const Sample& x ) const;

protected:

  // Model Serialization
  virtual void _getConfiguration( ConfigurationPtr& ) const;

  // Model Deserialization
  virtual void _setConfiguration( const ConstConfigurationPtr & );

private:

  /** Calculate the envelopes */
  void calculateEnvelopes( const OccurrencesPtr& );

  /** Calculate a percentile */
  void percentile( Scalar *result, int n, double percent, std::vector<ScalarVector> *matrix, int layerIndex );

  /** Read depth data from expert database */
  void readDepthData( const char *species );

  /** Make the interquartile adjusting */
  void adjustInterquartile( int layerIndex, Scalar adjmin, Scalar adjmax );

  /** Ensure that the envelope has a minimum size */
  void ensureEnvelopeSize( int layerIndex );

  /** Log the bioclimatic envelopes information. */
  void logEnvelope();

  /** Indicates if the algorithm is finished. */
  bool _done;

  /** Lower limit for each variable */
  Sample _lower_limit;

  /** Upper limit for each variable */
  Sample _upper_limit;

  /** Recommended values for the inner envelope sizes */
  Sample _inner_size;

  /** Minimum value for each variable. */
  Sample _minimum;

  /** Prefered minimum for each variable. */
  Sample _pref_minimum;

  /** Prefered maximum for each variable. */
  Sample _pref_maximum;

  /** Maximum value for each variable. */
  Sample _maximum;

  /** Indicates if the species is pelagic or not (-1 indicates no data).
   *  Note: pelagic means "belonging to the upper layers of the open sea" (poetic, huh?).
   */
  int _pelagic;
};


#endif

