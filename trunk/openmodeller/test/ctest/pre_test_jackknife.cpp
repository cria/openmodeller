#include <openmodeller/pre/jackknife.hh>
#include <openmodeller/pre/PreParameters.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>

#include <pre_test_jackknife.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/om.hh>
#include <sstream>

void main ()
{
  std::cout << std::endl;
  std::cout << "Testing jackknife..." << std::endl;
  try 
  {
	 //
	 // Go on to do the test now...
	 //
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
	 params.store( "Algorithm", om.getAlgorithm() );

     Jackknife jackknife;
	 if ( !jackknife.Reset( params ) )
	 {
		std::string msg = "jackknife.Reset: Invalid Parameters.\n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
	 }
	 if ( !jackknife.Apply() )
	 {
		std::string msg = "jackknife.Apply: Apply error.\n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
	 }
	 jackknife.ResetState( params);

	 double out_param=0; // <------ output 1
     std::multimap<double, int> out_params; // <------ output 2
     double mean=0;                 // <------ output 3
     double variance=0;             // <------ output 4
     double std_deviation=0;        // <------ output 5
     double jackknife_estimate=0;   // <------ output 6
     double jackknife_bias=0;       // <------ output 7

	 if( !params.retrive( "out_param", out_param ) )
     {
		std::string msg = "Missing parameter: out_param. \n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
     }
	 if( !params.retrive( "out_params", out_params ) )
     {
		std::string msg = "Missing parameter: out_params. \n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
     } 
	 if( !params.retrive( "out_Mean", mean ) )
     {
		std::string msg = "Missing parameter: mean. \n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
     }
	 if( !params.retrive( "out_Variance", variance ) )
     {
 		std::string msg = "Missing parameter: variance. \n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
     }
	 if( !params.retrive( "out_Deviation", std_deviation ) )
     {
 		std::string msg = "Missing parameter: std_deviation. \n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
     }
	 if( !params.retrive( "out_Estimate", jackknife_estimate ) )
     {
 		std::string msg = "Missing parameter: jackknife_estimate. \n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
     } 
	 if( !params.retrive( "out_Bias", jackknife_bias ) )
     {
		std::string msg = "Missing parameter: jackknife_bias. \n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
     } 

     Log::instance()->debug( "With all layers: %f\n", out_param );

	 SamplerPtr samplerPtr = om.getSampler();
     EnvironmentPtr environment_ptr = samplerPtr->getEnvironment();
	 std::multimap<double, int>::const_iterator it = out_params.begin();
     std::multimap<double, int>::const_iterator end = out_params.end();
     for ( ; it != end; ++it ) 
	 {
       Log::instance()->debug( "Without layer %d: %f (%s)\n", (*it).second, (*it).first, (environment_ptr->getLayerPath( (*it).second )).c_str() );
     }

	 Log::instance()->debug( "Mean = %f\n", mean );
     Log::instance()->debug( "Variance = %f\n", variance );
     Log::instance()->debug( "Standard deviation = %f\n", std_deviation );
	 Log::instance()->debug( "Jackknife estimate = %f\n", jackknife_estimate );
     Log::instance()->debug( "Jackknife bias = %f\n", jackknife_bias );

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


