

#ifndef _SCALEDMODEL_HH
#define _SCALEDMODEL_HH

#include <Model.hh>

class ScaledModelImpl : public ModelImpl
{
  
public:
  
  ScaledModelImpl( bool has_params, const Sample& offsets, const Sample& scales );
  
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
  Sample _norm_offsets;
  Sample _norm_scales;
  bool _has_norm_params;
  
private:
  ScaledModelImpl();
  
};


#endif
