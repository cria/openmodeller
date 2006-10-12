/**
 * Declaration of OpenModeller class (former ControlInterface class).
 * 
 * @author Mauro E S Mu�oz <mauro@cria.org.br>
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

#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/AlgMetadata.hh>
#include <openmodeller/Configurable.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Algorithm.hh>

class AlgParameter;

#include <openmodeller/Occurrences.hh>
#include <openmodeller/Environment.hh>

// Since MapCommand's defn/decl have been moved to om_projector.hh,
// we need to actually include the entire header.
#include <openmodeller/Projector.hh>

#include <openmodeller/Algorithm.hh>

#include <openmodeller/MapFormat.hh>

class MapFormat;
class ConfusionMatrix;
class AreaStats;

#include <string>
#include <vector>


/****************************************************************/
/************************* Open Modeller ************************/

/**
 * Defines and implements all commands to interface with the model
 * generator.
 * 
 */
class dllexp OpenModeller
{
public:

  /** Model callback function.
   * @param progress A number between 0.0 and 1.0 reflecting the
   *  avance of the map creating task. 0.0 is the begin and
   *  1.0 is finished. Note that in model generation it is not 
   * always possible to know how many steps the algorithm will need.
   * @param extra_param A parameter set by user when
   *  setModelCallback() is called.
   */
  typedef void (*ModelCallback)( float progress, void *extra_param );

  /** Map callback function.
   * @param progress A number between 0.0 and 1.0 reflecting the
   *  avance of the map creating task. 0.0 is the begin and
   *  1.0 is finished.
   * @param extra_param A parameter set by user when
   *  setMapCallback() is called.
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

  /*****************************************************************************
   *
   * Algorithms related methods
   *
   ****************************************************************************/

  /** Finds the system available algorithms' metadata.
   *
   * The pointer returned are copied from an internal storage of
   * algorithms. So they can not be deallocated.
   * Another point is that the AlgMetadata will be
   * reallocated the next time this method is called.
   *
   * @return a null terminated list of available algorithms.
   */
  AlgMetadata const **availableAlgorithms();

  /** Returns a specific algorithm metadata
   * @param algorithm_id Identifier of the algorithm.
   * @return Algorithm's metadata or zero if the algorithm
   *  was not found.
   */
  AlgMetadata const *algorithmMetadata( char const *algorithm_id );

  /** Number of available algorithms.
   * If the algorithms are not already searched in the system,
   * searchs them first.
   *
   * @return Number of available algorithms.
   */
  int numAvailableAlgorithms();

  /*****************************************************************************
   *
   * Accessor methods
   *
   ****************************************************************************/

  /**
   * Returns the current environment related to model creation.
   * @return Pointer to environment related to model creation.
   */
  EnvironmentPtr getEnvironment() { return _env; }

  /**
   * Returns the current algorithm setting.
   * @return Pointer to algorithm.
   */
  AlgorithmPtr getAlgorithm() { return _alg; }

  /**
   * Returns model created by the algorithm.
   * @return Model object.
   */
  Model getModel() { return _alg->getModel(); }

  /**
   * Returns current sampler setting.
   * @return Pointer to sampler.
   */
  const SamplerPtr& getSampler() const { return _samp; }

  /*****************************************************************************
   *
   * Parameters setting methods
   *
   ****************************************************************************/

  /**
   * Define occurrence points to be used.
   * 
   * @param presence Occurrence points which the abundance
   *  attribute is not zero.
   * @param absence Occurrence points which the abundance
   *  attribute is zero.
   */
  int setOccurrences( const OccurrencesPtr& presence,
                      const OccurrencesPtr& absence=OccurrencesPtr() );

  /** Define algorithm that will be used to generate the model.
   * @param id Algorithm's identifier. Must match the
   *  Algorithm::getID() returned string.
   * @param nparam Number of parameters.
   * @param param Vector with all parameters. The address 'param'
   *  points to must exist when the method "run()" is called.
   * @return zero if something goes wrong like the algorithm ID
   *  does not exist, use different number of parameters, etc.
   */
  int setAlgorithm( char const *id, int nparam, AlgParameter const *param );

  /** Defines environmental layers and the mask using STL arguments.
   * Also creates the Environment object used for native range projection.
   * @param categ_map Vector of strings containing the file names 
   *  of categorical map layers.
   * @param continuous_map Vector of strings containing the file names 
   *  of continuous map layers.
   * @param mask File name of the mask map layer.
   */
  void setEnvironment( std::vector<std::string> categ_map,
		       std::vector<std::string> continuous_map,
		       const std::string& mask );
    
  /** Defines sampler to be used for modeling.
   * @param sampler Sampler object to be used for modeling
   */
  void setSampler(const SamplerPtr& sampler);

  /*****************************************************************************
   *
   * Projection / Map Generation Methods
   *
   ****************************************************************************/

  /** Sets a callback function to be called after each map
   * distribution line generation.
   * @param func Pointer to the callback function.
   * @param param User parameter to be passed to the callback
   *  function.
   */
  void setMapCallback( MapCallback func, void *param=0 );

  /** Sets a callback function to be called after each map
   * distribution line generation.
   * @param func Pointer to the callback function.
   */
  void setMapCommand( Projector::MapCommand *func );

  /** Create and save distribution map to disk using the specified
   * projection environment and output format.
   * @param env Pointer to Environment object with the layers 
   *  to project the model onto.
   * @param output_file Output file name.
   * @param format Georeferenced map file which will define
   *  cell size, extent, WKT projection, no data value, and file 
   *  type for the output map.
   */
  int createMap( const EnvironmentPtr & env, char const *output_file, MapFormat& format );

  /** Create and save distribution map to disk using the specified
   * projection environment. Output format defaults to the
   * output mask format.
   * @param env Pointer to Environment object with the layers 
   *  to project the model onto.
   * @param output_file Output file name.
   */
  int createMap( const EnvironmentPtr & env, char const *output_file );
  
  /** Create and save distribution map to disk using the specified
   * output format. Projection environment defaults to the same 
   * environment used during model creation and previously set 
   * by calling the setEnvironment() method.
   * @param output_file Output file name.
   * @param format Georeferenced map file which will define
   *  cell size, extent, WKT projection, no data value, and file 
   *  type for the output map.
   */
  int createMap( char const *output_file, MapFormat& format );

  /** Create and save distribution map to disk. Projection 
   * environment defaults to the same environment used during 
   * model creation and previously set by calling the 
   * setEnvironment() method. Output format defaults to the
   * output mask format.
   * @param output_file Output file name.
   */
  int createMap( char const *output_file );

  /*****************************************************************************
   *
   * Model generation procedures
   *
   ****************************************************************************/

  /** Sets a callback function to be called after each iteration
   * of the model creation.
   * @param func Pointer to the callback function.
   * @param param User parameter to be passed to the callback
   *  function.
   */
  void setModelCallback( ModelCallback func, void *param=0 );

  /** Sets a callback function to be called after each iteration
   * of the model creation.
   * @param func Pointer to the callback function.
   */
  void setModelCommand( Algorithm::ModelCommand *func );

  //
  // Model and distribution map related methods.
  //

  /** Run the algorithm to create the model.
   */
  int createModel();

  /** Compatibility with old oM client versions.
   */
  int run()  { return createModel(); }

  /** Get prediction at a given point.
   * @param env  Pointer to Environment class with the layers 
   *  to get environmental values from.
   * @param x X coordinate of point being queried
   * @param y Y coordinate of point being queried
   * @return    Prediction value at the specified point. Valid 
   *   values range from 0.0 to 1.0. Value -1.0 means there is
   *   no prediction for that point (masked or not predicted)
   */
  Scalar getValue(const ConstEnvironmentPtr& env, Coord x, Coord y);

  /** Get prediction at a given point.
   * @param environment_values Vector with environment values.
   * @return Prediction value at the specified point. Valid 
   *   values range from 0.0 to 1.0. Value -1.0 means there is
   *   no prediction for that point (masked or not predicted)
   */
  Scalar getValue( Scalar const *environment_values );

  char *error()  { return _error; }

  /*****************************************************************************
   *
   * Statistics
   *
   ****************************************************************************/

  /**
   * Returns a pointer to the model AreaStats object which 
   * contains statistics about areas on the map generated by OM.
   */
  AreaStats * getActualAreaStats();

  /**
   * Returns a pointer to the model AreaStats object which 
   * contains statistics about areas on the map generated by OM.
   * This one uses only a random sample of the data points
   * to estimate prediction areas.
   * @param proportionAreaToSample Proportion of the area of 
   *  interest (mask or intersection of all layers extents) to
   *  use as sample size.
   */
  AreaStats * getEstimatedAreaStats(double proportionAreaToSample = 0.01);
  AreaStats * getEstimatedAreaStats(const ConstEnvironmentPtr& env, 
				    double proportionAreaToSample = 0.01);

  ConfusionMatrix *getConfusionMatrix();

  /*****************************************************************************
   *
   * Serialization & Deserialization
   *
   ****************************************************************************/

  ConfigurationPtr getModelConfiguration() const;

  void setModelConfiguration( const ConstConfigurationPtr & );

  void setProjectionConfiguration( const ConstConfigurationPtr & );

private:

  /** Check if all necessary parameters to create the model
   *  have been defined. If not, an error message is returned.
   */
  char *parameterModelCheck();

  bool hasEnvironment();

  SamplerPtr _samp;           ///< Sampler object
  AlgorithmPtr _alg;          ///< Algorithm object

  OccurrencesPtr _presence; ///< Presence occurrences points.
  OccurrencesPtr _absence;  ///< Absence occurrences points.

  EnvironmentPtr _env;   ///< Original environmental layers

  Projector::MapCommand *_map_command;    ///< map call back pointer.

  // Command functions and user parameters.
  Algorithm::ModelCommand *_model_command;

  // Output format
  MapFormat _format;

  EnvironmentPtr _projEnv;   ///< Environmental layers for projection

  // model statistics: helper objects
  AreaStats * _actualAreaStats;
  AreaStats * _estimatedAreaStats;

  char _error[256];
};


#endif
