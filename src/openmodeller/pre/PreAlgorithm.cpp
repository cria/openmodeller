#include "PreAlgorithm.hh"

#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>

PreAlgorithm::PreAlgorithm()
{
}

PreAlgorithm::PreAlgorithm( const PreAlgorithm& )
{
};


PreAlgorithm::~PreAlgorithm()
{
}


bool PreAlgorithm::reset( const PreParameters& params )
{
  if( checkParameters( params ) ) {

    params_.clear();
    params_ = params;

    return true;
  } 
  else 
  {
	 std::string msg = "PreAlgorithm::Reset: Invalid supplied parameters.\n";
     Log::instance()->error( msg.c_str() );
	 throw InvalidParameterException( msg );
  }
}


bool PreAlgorithm::apply()
{
  bool return_value = false;
  
  if( checkInternalParameters() ) {
    return_value = runImplementation();
  } 
  else 
  {
	 std::string msg = "TePreAlgorithm::apply: Invalid supplied parameters.\n";
     Log::instance()->error( msg.c_str() );
	 throw InvalidParameterException( msg );
  }
  
  return return_value;
}


const PreParameters& PreAlgorithm::getParameters() const
{
  return params_;
}


bool PreAlgorithm::checkInternalParameters() const
{
  return checkParameters( params_ );
}


const PreAlgorithm& PreAlgorithm::operator=( 
  const PreAlgorithm& external )
{
  std::string msg = "PreAlgorithm::operator=: Algorithms cannot be copied.\n";
  Log::instance()->error( msg.c_str() );
  throw InvalidParameterException( msg );
  
  return external;
}


