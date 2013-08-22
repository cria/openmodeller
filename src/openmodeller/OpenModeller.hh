/**
 * Declaration of OpenModeller class (former ControlInterface class).
 * 
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

#include <openmodeller/om_defs.hh>
#include <openmodeller/CallbackWrapper.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/AlgMetadata.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/ConfusionMatrix.hh>
#include <openmodeller/RocCurve.hh>

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
class RocCurve;
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
  std::string getVersion();

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
  Model getModel() const { return _alg->getModel(); }

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
  int setAlgorithm( std::string const id, int nparam, AlgParameter const *param );

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

  /** Sets a callback function to be called during model creation and
   * map projection to check if the job should be aborted.
   * @param func Pointer to the callback function.
   * @param param User parameter to be passed to the callback function.
   */
  void setAbortionCallback( AbortionCallback func, void *param=0 );

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
  void setMapCallback( ModelProjectionCallback func, void *param=0 );

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
  void setModelCallback( ModelCreationCallback func, void *param=0 );

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
   * IMPORTANT: you should delete the pointer because it returns
   * a copy of the internal object!
   */
  AreaStats * getActualAreaStats();

  /**
   * Returns a pointer to the model AreaStats object which 
   * contains statistics about areas on the map generated by OM.
   * This one uses only a random sample of the data points
   * to estimate prediction areas. 
   * IMPORTANT: you should NOT delete the returned pointer!
   * @param proportionAreaToSample Proportion of the area of 
   *  interest (mask or intersection of all layers extents) to
   *  use as sample size.
   */
  AreaStats * getEstimatedAreaStats(double proportionAreaToSample = 0.01);
  AreaStats * getEstimatedAreaStats(const ConstEnvironmentPtr& env, 
				    double proportionAreaToSample = 0.01);

  /**
   * Returns a pointer to the internal object representing the confusion 
   * matrix that was calculated from the last generated model. 
   * IMPORTANT: you should NOT delete the returned pointer!
   */
  const ConfusionMatrix * const getConfusionMatrix();

  /**
   * Returns a pointer to the internal object representing the ROC 
   * curve that was calculated from the last generated model. 
   * IMPORTANT: you should NOT delete the returned pointer!
   */
  RocCurve * const getRocCurve();

  /*****************************************************************************
   *
   * Serialization & Deserialization
   *
   ****************************************************************************/

  ConfigurationPtr getModelConfiguration() const;

  void setModelConfiguration( const ConstConfigurationPtr & );

  void setProjectionConfiguration( const ConstConfigurationPtr & );

  void calculateModelStatistics( const ConstConfigurationPtr & );

private:

  bool hasEnvironment();

  // Sampler object
  SamplerPtr _samp;

  // Algorithm object
  AlgorithmPtr _alg;

  // Presence occurrences points
  OccurrencesPtr _presence;

  // Absence occurrences points
  OccurrencesPtr _absence;

  // Original environmental layers
  EnvironmentPtr _env;

  // Wrapper object for callbacks
  CallbackWrapper _callback_wrapper;

  // Output format
  MapFormat _format;

  // Environmental layers for projection
  EnvironmentPtr _projEnv;

  // Model statistics: helper objects
  AreaStats * _actualAreaStats;
  AreaStats * _estimatedAreaStats;

  // Confusion matrix
  ConfusionMatrix _confusion_matrix;

  // ROC Curve
  RocCurve _roc_curve;

  char _error[256];
};


#endif
