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
class OmAlgParameter;
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
  ~ControlInterface();


  /** Returns the openModeller client interface version in the
   * format "n.m"
   */
  char *getVersion();


  /** Finds the system available algorithms' metadata.
   *
   * The pointer returned are copied from an internal storage of
   * algorithms. So they can not be deallocated.
   * Another point is that the AlgMetadata will be
   * reallocated the next time this method is called.
   *
   * @return a null terminated list of available algorithms.
   */
  AlgMetadata **availableAlgorithms();

  /** Returns an specific algorithm metadata
   * @param algorithm_id Identifier of the algorithm.
   * @return Algorithm's metadata or zero if there algorithm
   *  was not found.
   */
  AlgMetadata *algorithmMetadata( char *algorithm_id );

  /** Number of available algorithms.
   * If the algorithms are not already searched in the system,
   * searchs them first.
   *
   * @return Number of available algorithms.
   */
  int numAvailableAlgorithms();

  /** Define algorithm that will be used to generate the
   *  distribution map.
   * @param id Algorithm's identifier. Must match the
   *  Algorithm::getID() returned string.
   * @param nparam Number of parameters.
   * @param param Vector with all parameters. The address 'param'
   *  points to must exists when the method "run()" is called.
   */
  void setAlgorithm( char *id, int nparam, OmAlgParameter *param );


  /** Defines environmental layers and the mask.
   * @param num_categ Number of categorical map layers.
   * @param categ_map File names of categorical map layers.
   * @param num_continuos Number of continuos map layers.
   * @param continuos_map File names of continuos map layers.
   * @param mask File name of the mask map layer.
   */
  void setEnvironment( int num_categ,     char **categ_map,
		       int num_continuos, char **continuous_map,
		       char *mask=0 );

  // Define output map.
  //

  /** Set the output distribution map file format and its map
   *  properties.
   * @param file File name.
   * @param hdr Georeferencing header with the map properties.
   * @param mult Value that the probabilities will be multiplied
   *  to.
   */
  void setOutputMap( char *file, Header *hdr, Scalar mult );

  /** Set the output distribution map file format and its map
   *  properties.
   * @param file File name.
   * @param map_file Georeferenced map file whose header will be
   *  used in the output.
   * @param mult Value that the probabilities will be multiplied
   *  to.
   */
  void setOutputMap( char *file, char *map_file, Scalar mult );


  /**
   * Define occurrence points to be used.
   * 
   * @param presence Occurrence points which the abundance
   *  attribute is not zero.
   * @param absence Occurrence points which the abundance
   *  attribute is zero.
   */
  void setOccurrences( Occurrences *presence,
		       Occurrences *absence=0 );

  int run();

  char *error()  { return f_error; }


private:

  /** Reallocate *dst and copy content from *src.*/
  void stringCopy( char **dst, char *src );

  /** Check if all necessary parameters have been defined.
   *  If not, an error message is returned.*/
  char *basicCheck();


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

  OmAlgParameter *_alg_param;  ///< Algorithm parameters.
  char *_alg_id;     ///< Algorithm's ID and parameters.
  int   _alg_nparam; ///< Number of algorithm parameters.
  int   _ncycle;     ///< Max algorithm cicles.

  Occurrences *_presence; ///< Presence occurrences points.
  Occurrences *_absence;  ///< Absence occurrences points.

  char f_error[256];
};


#endif
