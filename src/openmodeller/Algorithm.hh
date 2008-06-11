/**
 * Declaration of Algorithm class.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-05-26
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


#ifndef _ALGORITHMHH_
#define _ALGORITHMHH_

#include <map>
#include <string>

#include <openmodeller/om_defs.hh>
#include <openmodeller/CallbackWrapper.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/AlgMetadata.hh>
#include <openmodeller/ignorecase_traits.hh>

#include <openmodeller/Configurable.hh>
#include <openmodeller/Configuration.hh>

#include <openmodeller/refcount.hh>

#include <openmodeller/Model.hh>

#include <openmodeller/Normalizer.hh>

class AlgParameter;

// Forward Decl.
class AlgorithmImpl;

typedef ReferenceCountedPointer< AlgorithmImpl > AlgorithmPtr;
typedef ReferenceCountedPointer< const AlgorithmImpl > ConstAlgorithmPtr;

/**
 * Declares the function and function pointer to be used for the
 * algorithms Dynamic Linked to instantiate an object of its own
 * class. It is in C to avoid C++ name signatures.
 */
typedef AlgorithmImpl *(*TAlgFactory)();
typedef AlgMetadata *(*TAlgMetadata)();
extern "C"
{
dllexp AlgorithmImpl *algorithmFactory();
dllexp AlgMetadata const *algorithmMetadata();
}

/** 
 * Base class for all distribution modeling algorithms. Provide 
 * methods to all derived classes so that they can do data sampling
 * and access the environmental layers to build the model.
 */
class dllexp AlgorithmImpl : public Configurable, private ReferenceCountedObject
{

  friend class ReferenceCountedPointer<AlgorithmImpl>;
  friend class ReferenceCountedPointer<const AlgorithmImpl>;

public:

  typedef std::map< icstring, std::string > ParamSetType;

  // This constructor should only be used by default constructors
  // of derived classes.
  explicit AlgorithmImpl( AlgMetadata const *metadata );

  virtual ~AlgorithmImpl();
  
  /** Configure the algorithm's parameters.
   * @param nparam Number of parameters.
   * @param param Vector with all parameters. The address 'param'
   *  points to must exists while this object is used.
   */
  void setParameters( int nparam, AlgParameter const *param );
  void setParameters( const ParamSetType& );
  
  std::string const getID() const { return _metadata ? _metadata->id : 0; }
  
  AlgMetadata const *getMetadata() const { return _metadata; }

  /** Returns a copy of this algorithm in the same state as it was
   *  before being initialized (ie., no model, same parameters). 
   */
  AlgorithmPtr getFreshCopy();
  
  /** If algorithm returns 0 then it does not support projection
   *  of models to other environmental data layers. By default
   *  all algorithms support that feature. 
   */
  virtual int supportsModelProjection() const
  { return 1; }
  
  /*
   * Training Methods
   */
  Model createModel( const SamplerPtr& samp, CallbackWrapper *func = 0 );

  /** Set the sampler object. Need to be called before start
   *  the algorithm initialization or iteration.
   */
  void setSampler( const SamplerPtr& samp );
  
  /** Initiate a new training.
   */
  virtual int initialize() = 0;
  
  /** One step further on the training.
   * Return 0 if something wrong happened.
   */
  virtual int iterate() { return 1; };
  
  /** Called after the training phase has finished,
   * ie after done() returned not zero.
   */
  virtual int finalize() { return 1; }
  
  /** Return != 0 if algorithm finished. */
  virtual int done() const { return 1; }
  
  /** Returns the algorithm's convergence value at the moment */
  virtual int getConvergence( Scalar * const val ) const { return 0; }
  
  /** Returns progress so far (between 0.0 and 1.0) */
  virtual float getProgress() const { return (float) done(); }
  
  /*
   * Model Implementation.
   */
  
  /** The algorithm should return != 0 if it needs normalization
   *  of environmental variables (non categorical ones).
   */
  virtual int needNormalization()
  { return ( _normalizerPtr == 0 ) ? 0 : 1; }

  /** Returns the normalizer used by the algorithm. This method was 
   *  created for super algorithms like "best subsets", so that the
   *  super algorithm can retrieve and use the same normalizer needed
   *  by the sub algorithm.
   */
  Normalizer * getNormalizer() const; 

  /** Normalize the given environment.
   * @param samp Sampler to normalize.
   */
  void setNormalization( const SamplerPtr& samp ) const;
  
  /** Normalize the given environment.
   * @param env Environment to normalize.
   */
  void setNormalization( const EnvironmentPtr& env ) const;
  
  /** The algorithm must return the occurrence probability at
   * the given environment conditions.
   *
   * @param x Environmental conditions.
   * 
   * @return The occurrence probability in the range [0,1].
   *
   */
  virtual Scalar getValue( const Sample& x ) const = 0;
  
  /*
   * Extract the Model from the Algorithm
   */
  virtual Model getModel() const;
  
  /*
   * Extract and set configuration
   */
  
  ConfigurationPtr getConfiguration() const;
  
  void setConfiguration( const ConstConfigurationPtr & );
  
protected:

  virtual void _getConfiguration( ConfigurationPtr& ) const { return; }
  virtual void _setConfiguration( const ConstConfigurationPtr & ) { return; }
  
  /**
   *  All protected methods should not be called before
   *  initialized(). This is because the sampler object _samp
   *  is not initialized in the constructor and the algorithm's
   *  parameters were not set yet.
   */
  
  /** Dimension of problem domain: number of independent 
   *  variables (climatic + soil) added to the number of
   *  dependent variables (occurrence prediction). */
  int dimDomain()  { return _samp ? _samp->numIndependent() : 0; }
  
  /** Returns an algorithm parameter of string type.
   *   
   *  @param name Parameters name.
   *  @param value Filled with parameter's value.
   *  @return Zero if the parameter does not exists or the
   *   parameters were not set yet.
   **/
  int getParameter( std::string const &name, std::string* value );
  
  /** Returns an algorithm parameter of floating point type.
   *   
   *  @param name Parameters name.
   *  @param value Filled with parameter's value.
   *  @return Zero if the parameter does not exists or the
   *   parameters were not set yet.
   **/
  int getParameter( std::string const &name, double *value );
  int getParameter( std::string const &name, float  *value );
  
  /** Returns an algorithm parameter of int point type.
   *   
   *  @param name Parameters name.
   *  @param value Filled with parameter's value.
   *  @return Zero if the parameter does not exists or the
   *   parameters were not set yet.
   **/
  int getParameter( std::string const &name, int *value );
  

protected:
  
  SamplerPtr _samp;

  Normalizer * _normalizerPtr;

  ParamSetType _param;

private:

  AlgMetadata const *_metadata;

private:
  
  typedef ParamSetType::value_type ParamSetValueType;

};

#endif
