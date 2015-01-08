
#ifndef _AVERAGEMODEL_HH
#define _AVERAGEMODEL_HH

#include <openmodeller/om_defs.hh>
#include <openmodeller/Model.hh>

#include <vector>

class AverageModelImpl;

typedef ReferenceCountedPointer<AverageModelImpl> AverageModelPtr;

class dllexp AverageModelImpl : public ModelImpl
{

public:
  AverageModelImpl();
  
  virtual ~AverageModelImpl();
  
  /** Normalize the given environment.
   * @param samp Sampler to normalize.
   */
  virtual void setNormalization( const SamplerPtr& samp ) const ;
  
  /** Normalize the given environment.
   * @param env Environment to normalize.
   */
  virtual void setNormalization( const EnvironmentPtr& env ) const;

  virtual Scalar getValue( const Sample& x ) const;
  
  virtual void addModel( Model model );
  
private:
  
  std::vector<Model> _models;
  Scalar _size;
};

#endif
