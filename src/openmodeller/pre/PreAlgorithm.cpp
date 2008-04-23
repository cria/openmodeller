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


bool PreAlgorithm::Reset( const PreParameters& params )
{
  if( CheckParameters( params ) ) {

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


bool PreAlgorithm::Apply()
{
  bool return_value = false;
  
  if( CheckInternalParameters() ) {
    return_value = RunImplementation();
  } 
  else 
  {
	 std::string msg = "TePreAlgorithm::Apply: Invalid supplied parameters.\n";
     Log::instance()->error( msg.c_str() );
	 throw InvalidParameterException( msg );
  }
  
  return return_value;
}


const PreParameters& PreAlgorithm::GetParameters() const
{
  return params_;
}


bool PreAlgorithm::CheckInternalParameters() const
{
  return CheckParameters( params_ );
}


const PreAlgorithm& PreAlgorithm::operator=( 
  const PreAlgorithm& external )
{
  std::string msg = "PreAlgorithm::operator=: Algorithms cannot be copied.\n";
  Log::instance()->error( msg.c_str() );
  throw InvalidParameterException( msg );
  
  return external;
}


