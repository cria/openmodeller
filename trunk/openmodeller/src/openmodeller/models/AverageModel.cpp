
#include <models/AverageModel.hh>
#include <Model.hh>

#include <vector>
using std::vector;

AverageModelImpl::AverageModelImpl() :
  ModelImpl(),
  _models(),
  _size( Scalar(0.0) )
{ }

AverageModelImpl::~AverageModelImpl()
{ }

void
AverageModelImpl::setNormalization( const SamplerPtr& samp ) const
{
  if ( _models.size() > 0 ) {
    _models[0]->setNormalization( samp );
  }
}

void
AverageModelImpl::setNormalization( const EnvironmentPtr& env ) const
{
  if ( _models.size() > 0 ) {
    _models[0]->setNormalization( env );
  }
}

Scalar
AverageModelImpl::getValue( const Sample& x ) const
{
  if ( _size == Scalar(0.0) )
    return Scalar(0.0);

  Scalar total = 0.0;

  vector<Model>::const_iterator amodel;

  for ( amodel = _models.begin();
	amodel != _models.end();
	++amodel ) {
    total += (*amodel)->getValue( x );
  }

  return total / _size;

}

void
AverageModelImpl::addModel( Model model )
{
  _models.push_back( model );
  _size += Scalar( 1.0 );
}
