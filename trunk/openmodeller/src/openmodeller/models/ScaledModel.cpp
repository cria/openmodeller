
#include <models/ScaledModel.hh>

#include <om_sampler.hh>
#include <environment.hh>

/*********************************************************************************
 *
 * ScaledModelImpl - Abstract base class for derived/stand-alone models
 *
 *********************************************************************************/

ScaledModelImpl::ScaledModelImpl() :
  ModelImpl(),
  _norm_offsets(),
  _norm_scales(),
  _has_norm_params(false)
{}

ScaledModelImpl::ScaledModelImpl(  bool has_params, const Sample& offsets, const Sample& scales ) : 
  ModelImpl(),
  _norm_offsets( offsets ),
  _norm_scales( scales ),
  _has_norm_params( has_params )
{ }

ScaledModelImpl::~ScaledModelImpl()
{ }

void
ScaledModelImpl::setNormalization( const SamplerPtr& sampl ) const 
{
  sampl->normalize( _has_norm_params, _norm_offsets, _norm_scales );
}

void
ScaledModelImpl::setNormalization( const EnvironmentPtr& env) const
{
    env->normalize( _has_norm_params, _norm_offsets, _norm_scales );
}

