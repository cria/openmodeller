
#include <openmodeller/pre/ChiSquare.hh>
#include <openmodeller/pre/PreParameters.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>

#include <pre_test_chisquare.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/om.hh>
#include <sstream>

void main ()
{
	std::cout << std::endl;
	std::cout << "Testing chi-square..." << std::endl;
	try 
	{
		Log::instance()->setLevel( Log::Debug );
		Log::instance()->setCallback( new MyLog() );

		std::ostringstream myOutputStream ;
		AlgorithmFactory::searchDefaultDirs();
		OpenModeller om;

		std::string myInFileName("/tmp/model_request.xml");
		ConfigurationPtr c1 = Configuration::readXml( myInFileName.c_str() );
		om.setModelConfiguration(c1);

		PreParameters params;
		params.store( "Sampler", om.getSampler() );

		ChiSquare chi;
		if ( !chi.Reset( params ) )
		{
		   std::string msg = "chi.Reset: Invalid Parameters.\n";
           Log::instance()->error( msg.c_str() );
		   throw InvalidParameterException( msg );
		}
		if ( !chi.Apply() )
		{
		   std::string msg = "chi.Apply: Apply error.\n";
           Log::instance()->error( msg.c_str() );
		   throw InvalidParameterException( msg );
		}
		chi.showResult();

		return ;
	}
	catch( std::exception& e ) 
	{
		std::string myError("Exception caught!\n");
		std::cout << "Exception caught!" << std::endl;
		std::cout << e.what() << std::endl;
		myError.insert(myError.length(),e.what());
		return ;
	}
}


