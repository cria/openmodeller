

#include <icstring.hh>
#include <CppUnitLite/TestHarness.h>

#include <om_log.hh>

#include <string>

using std::string;

TEST( Compare1, icstring )
{
  g_log("icstring Compare1:\n");
  icstring a("AAAA");
  icstring A("aaaa");
  CHECK( a == A );
}

TEST( Compare2, icstring )
{
  g_log( "icstring Compare2:\n" );
  icstring a("AaAa");
  icstring b("aAaA");

  CHECK( a == b );
}

TEST( Compare3, icstring )
{
  g_log( "icstring Compare2:\n" );
  icstring a("AaAa");
  icstring b("AaAa");

  CHECK( a == b );
}

string stringfunc( const string& arg )
{
  string ret(arg);
  
  ret += "aAaA";

  return ret;
}

TEST( Convert1, icstring )
{
  g_log( "icstring Convert1:\n" );
  icstring a("bbBB");

  string result = stringfunc( a );

  CHECK( result == "bbBBaAaA" );
  CHECK( result != "BBBBAAAA" );
}

TEST( Convert2, icstring )
{
  g_log( "icstring Convert2:\n" );
  icstring a("bbBB");

  icstring result = stringfunc( a );

  CHECK( result == "bbBBaAaA" );
  CHECK( result != "BBBBAAAACCC" );
}
