
#include <models/AlgoAdapterModel.hh>

#include <om_algorithm.hh>

/*********************************************************************************
 *
 * AlgoAdapterModelImpl - Adapt and Algorithm into a Model Impl.
 *
 *********************************************************************************/

AlgoAdapterModelImpl::AlgoAdapterModelImpl( const ConstAlgorithmPtr & algo ) :
  ModelImpl(),
  _algo( algo )
{ }

AlgoAdapterModelImpl::~AlgoAdapterModelImpl()
{
}

void
AlgoAdapterModelImpl::setNormalization( const SamplerPtr& sampl ) const 
{
  _algo->setNormalization( sampl );
}

void
AlgoAdapterModelImpl::setNormalization( const EnvironmentPtr& env) const
{
  _algo->setNormalization( env );
}

Scalar
AlgoAdapterModelImpl::getValue( const Sample& x ) const
{
  return _algo->getValue( x );
}
