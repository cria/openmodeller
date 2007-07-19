

#ifndef _SCALEDMODEL_HH
#define _SCALEDMODEL_HH

#include <Model.hh>
#include <openmodeller/Normalizer.hh>

class ScaledModelImpl : public ModelImpl
{
  
public:
  
  ScaledModelImpl( Normalizer * normalizerPtr );
  
  virtual ~ScaledModelImpl();
  
  /** Normalize the given environment.
   * @param samp Sampler to normalize.
   */
  virtual void setNormalization( const SamplerPtr& samp ) const ;
  
  /** Normalize the given environment.
   * @param env Environment to normalize.
   */
  virtual void setNormalization( const EnvironmentPtr& env ) const;
  
  /** Compute a value in the Model
   * @param x Environment vector.
   */
  virtual Scalar getValue( const Sample& x ) const = 0;
  
protected:
  Normalizer * _normalizerPtr;
  
private:
  ScaledModelImpl();
  
};


#endif
