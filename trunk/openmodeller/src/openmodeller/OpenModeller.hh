/**
 * Declaration of ControlInterface class.
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-25
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2003 by CRIA -
 * Centro de Referencia em Informacao Ambiental
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

#ifndef _OM_CONTROLHH_
#define _OM_CONTROLHH_

#include <om_defs.hh>
#include <om_algorithm_metadata.hh>

class Algorithm;
class AlgorithmFactory;
class Environment;
class Sampler;
class Occurrences;
class RasterFile;
class Map;
class Header;


/****************************************************************/
/************************* Control Interface ********************/

/**
 * Defines and implements all commands to interface with the model
 * generator.
 * 
 */
class ControlInterface
{
public:

  ControlInterface();

  /**
   * @param cs     Common coordinate system.
   * @param ncateg Number of categorical layers (they all need to
   *  be in the initial portion of "layers");
   * @param nlayer Total number of environmental layers;
   * @param layer  Names of the environmental layers;
   */
  ControlInterface( int ncateg, int nlayer, char **layers,
		    char *mask=0 );

  ~ControlInterface();


  /** Returns the openModeller client interface version in the
   * format "n.m"
   */
  char *getVersion();


  /** Finds the system available algorithms.
   *
   * The returned algorithms can not run because they are not
   * initialized with "Sampler" and parameters!
   * 
   * The pointer returned are copied from an internal storage of
   * algorithms. So they can not be deallocated.
   * Another point is that the Algorithms will be reallocated
   * the next time this method is called.
   *
   * @return a null terminated list of available algorithms.
   */
  Algorithm **availableAlgorithms();

  /** Number of available algorithms.
   * If the algorithms are not already searched in the system,
   * searchs them first.
   *
   * @return Number of available algorithms.
   */
  int numAvailableAlgorithms();


  // Define environmental layers and other basic settings.
  void setEnvironment( int ncateg, int nlayer, char **layers,
		       char *mask=0 );

  // Define output map.
  void setOutputMap( char *file, Header *hdr, Scalar mult );
  void setOutputMap( char *file, char *map_file, Scalar mult );


  /** Define algorithm that will be used to generate the
   *  distribution map.
   * @param alg_id Algorithm's identifier. Must match
   *  Algorithm::getID() method.
   * @param param String with algorithm's parameters separated by
   *  space and/or TABs.
   */
  void setAlgorithm( char *alg_id, char *param=0 );

  /**
   * Define occurrence points to be used.
   * 
   * @param file File name with localities (coordinates) from
   *             the occurrences;
   * @param cs   Coordinate system of all localities in the file.
   * @param oc   Name of the species of interest. If == 0, then
   *             use the first species found in file.
   */
  void setOccurrences( char *file, char *cs, char *oc=0 );

  int run();

  char *error()  { return f_error; }


private:

  /** Reallocate *dst and copy content from *src.*/
  void stringCopy( char **dst, char *src );

  /** Check if all necessary parameters have been defined.
   *  If not, an error message is returned.*/
  char *basicCheck();

  /** Read occurences of species 'name', provided in the coordinate
   *  system 'cs', from the file 'file'.*/
  Occurrences *readOccurrences( char *file, char *cs, char *name=0 );

  /** Return the object that implements the algorithm to be used.*/
  /*
  Algorithm *algorithmFactory( Sampler *samp, char *name,
			       char *params );
  */

  /** Build the model based on 'samp' and on algorithm.*/
  int createModel( Algorithm *alg, Sampler *samp, int max_cicles);

  /** Generate output map defined by '_file' and '_hdr'.*/
  int createMap( Environment *env, Algorithm *alg );


  AlgorithmFactory *_factory;

  int    _ncateg;
  int    _nlayers;
  char **_layers;
  char  *_mask;

  char   *_file;  ///< Output map.
  Header *_hdr;
  Scalar  _mult;  ///< Output multiplier.

  char *_alg_id;    ///< Algorithm's ID and parameters.
  char *_alg_param;
  int   _ncycle;    ///< Max algorithm cicles.

  char *_oc_file; ///< Occurrences file name.
  char *_oc_cs;   ///< Occurrences Coordinate System.
  char *_oc_name; ///< Occurrences ID (eg species name).

  char f_error[256];
};


#endif
