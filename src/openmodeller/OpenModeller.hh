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
#include <om_log.hh>
#include <om_algorithm_metadata.hh>
#include <om_serializable.hh>

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
/************************* Open Modeller ************************/

/**
 * Defines and implements all commands to interface with the model
 * generator.
 * 
 */
class dllexp OpenModeller : public Serializable
{
public:

  /** Model callback function.
   * @param step Number of the iteration already done. Note that
   *  in model generation it is not always possible to know how
   *  many steps the algorithm will need.
   * @param extra_param A parameter set by user when
   *  setModelCallback() is called.
   */
  typedef void (*ModelCallback)( int step, void *extra_param );

  /** Model callback function.
   * @param progress A number between 0.0 and 1.0 reflecting the
   *  avance of the map creating task. 0.0 is the begin and
   *  1.0 is finished.
   * @param extra_param A parameter set by user when
   *  setModelCallback() is called.
   */
  typedef void (*MapCallback)( float progress, void *extra_param );


public:

  OpenModeller();
  ~OpenModeller();

  /** Sets the log level on the global Log g_log object.
   *
   *  Note: We need to add control over the log file and prefix
   *   as well.
   */
  void setLogLevel(Log::Level level);


  /** Returns the openModeller client interface version in the
   * format "n.m"
   */
  char *getVersion();

  /** Returns configuration file name used by openModeller 
   */
  char *getConfigFileName();

  /** Returns openModeller plugin path (path where to look for
   * algorithms)
   */
  char *getPluginPath();

  /** Sets new plugin path for openModeller
   * @param search_dirs Null terminated list of directory 
   *        paths to search for algorithms.
   */
  void setPluginPath(char ** search_dirs);

  /** Reset plugin path to default (from config file or from
   *  hardcoded PLUGINPATH)
   */
  void resetPluginPath();

  //
  // Algorithms related methods.
  //

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

  //
  // Attributes reading methods
  //

  Environment *getEnvironment() { return _env; }


  //
  // Parameters setting methods
  //

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

  /** Set the output distribution map file format and its map
   *  properties.
   * @param mult Value that the probabilities will be multiplied
   *  to.
   * @param output_file Output file name.
   * @param mask Georeferenced map file which will define the
   *  valid pixels on the output map.
   * @param file_with_format Georeferenced map file whose
   *  header will be used in the output.
   */
  int setOutputMap( Scalar mult, char *output_file,
                    char *output_mask, char *file_with_format );

  /** Set the output distribution map file format and its map
   *  properties.
   * @param mult Value that the probabilities will be multiplied
   *  to.
   * @param output_file Output file name.
   * @param mask Georeferenced map file which will define the
   *  valid pixels on the output map.
   * @param format Pointer to MapFormat object defining the
   *  parameters of the output map.
   */
  int setOutputMap( Scalar mult, char *output_file,
                    char *output_mask, MapFormat *format );

  //
  // Callback related methods.
  //

  /** Sets a callback function to be called after each iteration
   * of the model creation.
   * @param func Pointer to the callback function.
   * @param param User parameter to be passed to the callback
   *  function.
   */
  void setModelCallback( ModelCallback func, void *param=0 )
  { _model_callback = func; _model_callback_param = param; }

  /** Sets a callback function to be called after each map
   * distribution line generation.
   * @param func Pointer to the callback function.
   * @param param User parameter to be passed to the callback
   *  function.
   */
  void setMapCallback( MapCallback func, void *param=0 )
  { _map_callback = func; _map_callback_param = param; }


  //
  // Model and distribution map related methods.
  //

  /** Run the algorithm to create the model.
   */
  int createModel();

  /** Compatibility with old oM client versions.
   */
  int run()  { return createModel(); }

  /** Save distribution map to disk.
   * @param env  Pointer to Environment class with the layers 
   *  to project the model onto. Defaults to environment set
   *  with setEnvironment().
   * @param output_file Output file name. Defaults to file set
   *  with setOutputMap().
   * @param output_mask Georeferenced map file which will define
   *  the valid pixels on the output map. Defaults to mask set
   *  with setOutputMap().
   */
  int createMap( Environment *env=0, char *output_file=0,
                 char *output_mask=0 );

  char *error()  { return _error; }

  //
  // Serialization methods
  //
  int serialize(Serializer * serializer);
  int deserialize(Deserializer * deserializer);


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

  /** Check if all necessary parameters to create the model
   *  have been defined. If not, an error message is returned.
   */
  char *parameterModelCheck();

  /** Filter occurrence points in Occurrences object that 
   *  fall outside the mask (cells with NODATA in mask)
   */
  void filterMaskedOccurrences(Occurrences * occur);

  /** Delete every string in array of strings and then delete
   *  array itself.
   */
  void deleteStringArray(char ** array);

  AlgorithmFactory *_factory;

  Sampler * _samp;           ///< Sampler object
  Algorithm * _alg;          ///< Algorithm object
  AlgParameter *_alg_param;  ///< Algorithm parameters.
  char *_alg_id;     ///< Algorithm's ID and parameters.
  int   _alg_nparam; ///< Number of algorithm parameters.

  Occurrences *_presence; ///< Presence occurrences points.
  Occurrences *_absence;  ///< Absence occurrences points.

  Environment * _env;      ///< Original environmental layers

  // Output map default data.
  Scalar _output_mult;    ///< Output multiplier factor.
  char   *_output_file;   ///< Output file name.
  char   *_output_mask;   ///< Output mask.
  Header *_output_header; ///< Output associated metadata.

  // Callback functions and user parameters.
  ModelCallback _model_callback;
  void         *_model_callback_param;
  MapCallback   _map_callback;
  void         *_map_callback_param;

  // plugin path
  char ** _plugin_path;

  char _error[256];
};


#endif
