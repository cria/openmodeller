/**
 * Declaration of OpenModeller class (former ControlInterface class).
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
class AlgParameter;
class AlgorithmFactory;
class Environment;
class Sampler;
class Occurrences;
class RasterFile;
class Map;
class MapFormat;
class Header;


/****************************************************************/
/************************* OpenModeller ********************/

/**
 * Defines and implements all commands to interface with the model
 * generator.
 * 
 */
class OpenModeller
{
public:

  OpenModeller();
  ~OpenModeller();


  /** Returns the openModeller client interface version in the
   * format "n.m"
   */
  char *getVersion();

  /** Returns openModeller plugin path (path where to look for algorithms)
   */
  char *getPluginPath();

  /** Load the system available algorithms. If there are
   * algorithm already loaded they are unloaded.
   * 
   * Warning:
   * 
   * The pointers to old algorithms are invalid after a call to
   * this method.
   * 
   * @return Number of loaded algorithms.
   */
  int loadAlgorithms();

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
   * @return zero if something goes wrong like the algorithm ID
   *  does not exist, use different number of parameters, etc.
   */
  int setAlgorithm( char *id, int nparam, AlgParameter *param );


  /** Defines environmental layers and the mask. Also creates
   *  the Environment object used for native range projection.
   * @param num_categ Number of categorical map layers.
   * @param categ_map File names of categorical map layers.
   * @param num_continuos Number of continuos map layers.
   * @param continuos_map File names of continuos map layers.
   * @param mask File name of the mask map layer.
   */
  int setEnvironment( int num_categ,     char **categ_map,
                      int num_continuos, char **continuous_map,
                      char *mask=0 );

  /** Defines environmental layers and the mask using an 
   *  existing Environment object. 
   * @param env Environment object to be used when building model.
   */
  int setEnvironment( Environment * env);

  // Define output map.
  //

  /** Set the output distribution map file format and its map
   *  properties and save distribution map to disk.
   * @param env  Pointer to Environment class with the layers 
   *  to project the model onto.
   * @param file File name.
   * @param mult Value that the probabilities will be multiplied
   *  to.
   * @param mask Georeferenced map file which will define the
   *  valid pixels on the output map.
   * @param map_file Georeferenced map file whose header will be
   *  used in the output.
   */
  int createMap( Environment *env, char *file, Scalar mult, 
		 char * mask, char *map_file );

  /** Set the output distribution map file format and its map
   *  properties and save distribution map to disk.
   * @param env  Pointer to Environment class with the layers 
   *  to project the model onto.
   * @param file File name.
   * @param mult Value that the probabilities will be multiplied
   *  to.
   * @param mask Georeferenced map file which will define the
   *  valid pixels on the output map.
   * @param format Pointer to MapFormat object defining the
   *  parameters of the output map.
   */
  int createMap( Environment *env, char *file, Scalar mult, 
		 char * mask, MapFormat * format );

  /**
   * Define occurrence points to be used.
   * 
   * @param presence Occurrence points which the abundance
   *  attribute is not zero.
   * @param absence Occurrence points which the abundance
   *  attribute is zero.
   */
  int setOccurrences( Occurrences *presence,
                      Occurrences *absence=0 );

  Environment * getEnvironment() { return _env; }

  /**
   * Run the algorithm.
   * 
   */
  int run();

  char *error()  { return f_error; }


private:

  /** Set the output distribution map file format and its map
   *  properties and save distribution map to disk.
   * @param env  Pointer to Environment class with the layers 
   *  to project the model onto.
   * @param file Output file name for the distribution map.
   * @param mult Multiplier for the prediction probabilities.
   * @param mask Georeferenced map file which will define the
   *  valid pixels on the output map.
   * @param hdr  Georeferencing header with the map properties.
   */
  int createMap( Environment *env, char *file, Scalar mult, 
		 char * mask, Header *hdr );

  /** Reallocate *dst and copy content from *src.*/
  void stringCopy( char **dst, char *src );

  /** Check if all necessary parameters have been defined.
   *  If not, an error message is returned.*/
  char *basicCheck();


  /** Build the model based on 'samp' and on algorithm.*/
  int createModel( Algorithm *alg, Sampler *samp);


  AlgorithmFactory *_factory;

  Sampler * _samp;           ///< Sampler object
  Algorithm * _alg;          ///< Algorithm object
  AlgParameter *_alg_param;  ///< Algorithm parameters.
  char *_alg_id;     ///< Algorithm's ID and parameters.
  int   _alg_nparam; ///< Number of algorithm parameters.

  Occurrences *_presence; ///< Presence occurrences points.
  Occurrences *_absence;  ///< Absence occurrences points.

  Environment * _env;      ///< Original environmental layers
  bool _internal;          ///< Flag that indicates whether _env was created internally or provided by the client.

  char f_error[256];
};


#endif
