#include <openmodeller/om.hh>
#include <iostream>
class MyLog : public Log::LogCallback
{
void operator()( Log::Level l, const std::string& msg )
{
std::cout << msg;
}
};

int main( int argc, char **argv )
{
try {
Log::instance()->setLevel( Log::Debug );
Log::instance()->setCallback( new MyLog() );
Log::instance()->debug( "sample debug message\n" );
Log::instance()->info( "sample info message\n" );
Log::instance()->warn( "sample warn message.\n");
//Log::instance()->error(0, "sample error message.\n");
}
catch (...)
{
return 1; //fail
}
return 0; //pass
}
