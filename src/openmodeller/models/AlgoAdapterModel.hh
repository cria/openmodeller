
#ifndef _ALGOADAPTERMODEL_HH
#define _ALGOADAPTERMODEL_HH

#include <Model.hh>

#include <om_algorithm.hh>

class AlgoAdapterModelImpl : public ModelImpl
{
public:
  AlgoAdapterModelImpl( const ConstAlgorithmPtr & );
  
  virtual ~AlgoAdapterModelImpl();
  
  /** Normalize the given environment.
   * @param samp Sampler to normalize.
   */
  virtual void setNormalization( const SamplerPtr& samp ) const ;
  
  /** Normalize the given environment.
   * @param env Environment to normalize.
   */
  virtual void setNormalization( const EnvironmentPtr& env ) const;
  
  virtual Scalar getValue( const Sample& x ) const;
  
private:
  
  ConstAlgorithmPtr _algo;
    
};

#endif
