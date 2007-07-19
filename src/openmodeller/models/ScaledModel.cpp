
#include <openmodeller/models/ScaledModel.hh>

#include <openmodeller/Sampler.hh>
#include <openmodeller/Environment.hh>

/*********************************************************************************
 *
 * ScaledModelImpl - Abstract base class for derived/stand-alone models
 *
 *********************************************************************************/

ScaledModelImpl::ScaledModelImpl() :
  ModelImpl(),
  _normalizerPtr(0)
{}

ScaledModelImpl::ScaledModelImpl( Normalizer * normalizerPtr ) : 
  ModelImpl(),
  _normalizerPtr( normalizerPtr )
{ }

ScaledModelImpl::~ScaledModelImpl()
{ }

void
ScaledModelImpl::setNormalization( const SamplerPtr& sampl ) const 
{
  sampl->normalize( _normalizerPtr );
}

void
ScaledModelImpl::setNormalization( const EnvironmentPtr& env) const
{
  env->normalize( _normalizerPtr );
}

