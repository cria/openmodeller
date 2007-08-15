

#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sample.hh>

#include <CppUnitLite/TestHarness.h>

#include <openmodeller/Log.hh>

#include <sstream>
using namespace std;

ConfigurationPtr A;

TEST( DefaultPtrConstructor, ConfigurationImpl )
{
  g_log("Default Constructor:\n");
  A = new ConfigurationImpl();
  CHECK( A->getName().empty() );
  CHECK( A->getValue().empty() );
  Configuration::subsection_list subs = A->getAllSubsections();
  CHECK( subs.empty() );
  Configuration::attribute_list attrs = A->getAllAttributes();
  CHECK( attrs.empty() );
}

TEST( SetName, ConfigurationImpl )
{
  g_log("SetName\n");
  string newname = "Freddy_the_Freeloader";
  A->setName( newname );
  CHECK( A->getName() == newname );
}

TEST( SetValue1, ConfiguratonImpl )
{
  g_log("SetValue (no whitesapce)\n");
  string val = "The Value";
  A->setValue( val );
  CHECK( A->getValue() == val );
}

TEST( SetValue2, ConfigurationImpl )
{
  g_log("SetValue with whitespace trim\n");
  string correctval = "The Value";
  string whitespace = " \t" + correctval + "\n\r   \t";
  A->setValue( whitespace );
  CHECK( A->getValue() == correctval );
}

TEST( GetAttrDefault, ConfigurationImpl )
{
  g_log("GetAttribute with default value\n" );
  string defaultvalue = "defaultvalue";
  CHECK( A->getAttribute("BadAttr",defaultvalue) == defaultvalue );
}

TEST( GetAttrThrow0, ConfigurationImpl )
{
  g_log("GetAttribute throw if not found (empty attr list)\n");
  string name = "BadAttr";
  try {
    string aa = A->getAttribute(name);
    FAIL("No Exception Thrown");
  }
  catch( AttributeNotFound& e ) {
    CHECK( name == e.getName() );
  }
  catch( ... ) {
    FAIL( "Incorrect Exception Thrown" );
  }
}

TEST( AddAttrString, ConfigurationImpl )
{
  g_log("AddAttribute string\n");
  string val = "String Value";
  A->addNameValue("StringAttr", val );
  CHECK( A->getAttribute("StringAttr") == val );
}

TEST( GetAttrThrow1, ConfigurationImpl )
{
  g_log("GetAttribute throw if not found (1 attr in list)\n");
  string name = "BadAttr";
  try {
    string aa = A->getAttribute(name);
    FAIL("No Exception Thrown");
  }
  catch( AttributeNotFound& e ) {
    CHECK( name == e.getName() );
  }
  catch( ... ) {
    FAIL( "Incorrect Exception Thrown" );
  }
}

TEST( AddAttrInt, ConfigurationImpl )
{
  g_log("AddAttribute int\n");
  int val = 42;
  A->addNameValue( "IntAttr", val );
  int newval = A->getAttributeAsInt( "IntAttr", 0 );
  CHECK( newval == val );
}

TEST( GetAttrThrow2, ConfigurationImpl )
{
  g_log("GetAttribute throw if not found (2 attr in list)\n");
  string name = "BadAttr";
  try {
    string aa = A->getAttribute(name);
    FAIL("No Exception Thrown");
  }
  catch( AttributeNotFound& e ) {
    CHECK( name == e.getName() );
  }
  catch( ... ) {
    FAIL( "Incorrect Exception Thrown" );
  }
}

TEST( AddAttrDouble, ConfigurationImpl )
{
  g_log("AddAttribute double\n");
  double val = 3.1415;
  A->addNameValue( "DoubleAttr", val );
  double newval = A->getAttributeAsDouble( "DoubleAttr", -1.0 );
  CHECK( newval = val );
}

TEST( AddAttrSample, ConfigurationImpl )
{
  g_log("AddAttribute Sample\n");
  Sample val(5);
  for( int i=0; i<val.size(); i++ ) {
    val[i] = -2 + i;
  }
  A->addNameValue( "SampleAttr", val );

  CHECK( val == A->getAttributeAsSample( "SampleAttr" ) );
}

TEST( XmlCheck1, ConfigurationImpl )
{
  g_log("Simple Xml Serialization check\n");

  stringstream sscheck( ios::out );
  Configuration::writeXml( A, sscheck );

  string expectedVal =
    "<Freddy_the_Freeloader\n StringAttr='String Value'\n IntAttr='42'\n DoubleAttr='3.1415000000000002'\n SampleAttr='-2 -1 0 1 2'\n>\nThe Value\n</Freddy_the_Freeloader>\n";

  CHECK( expectedVal == sscheck.str() );

}

TEST( SubsectionThrow0a, ConfigurationImpl )
{
  g_log( "getSubsection throw if not found (empty subsection list )\n");
  string name = "NoSubsection";
  try {
    ConfigurationPtr B = A->getSubsection( name );
    FAIL("No Exception Thrown");
  }
  catch( SubsectionNotFound& e ) {
    CHECK( name == e.getName() );
  }
  catch( ... ) {
    FAIL("Incorrect Exception Thrown" );
  }
}

TEST( SubsectionThrow0b, ConfigurationImpl )
{
  g_log( "getSubsection nothrow if not found (empty subsection list )\n");
  string name = "NoSubsection";
  try {
    ConfigurationPtr B = A->getSubsection( name, false );
    CHECK( !B );
  }
  catch( ... ) {
    FAIL("Exception Thrown" );
  }
}

TEST( SubsectionAdd, ConfigurationImpl )
{
  g_log( "Add subsection\n" );

  // Whip up a simple subsection:
  ConfigurationPtr B( new ConfigurationImpl( "Point" ) );
  B->addNameValue( "X", 1 );
  B->addNameValue( "Y", 2 );

  A->addSubsection( B );

  CHECK( A->getAllSubsections().size() == 1 );

}

TEST( SubsectionThrow1a, ConfigurationImpl )
{
  g_log( "getSubsection throw if not found (1 subsection in list )\n");
  string name = "NoSubsection";
  try {
    ConfigurationPtr B = A->getSubsection( name );
    FAIL("No Exception Thrown");
  }
  catch( SubsectionNotFound& e ) {
    CHECK( name == e.getName() );
  }
  catch( ... ) {
    FAIL("Incorrect Exception Thrown" );
  }
}

TEST( SubsectionThrow1b, ConfigurationImpl )
{
  g_log( "getSubsection nothrow if not found (1 subsection in list )\n");
  string name = "NoSubsection";
  try {
    ConfigurationPtr B = A->getSubsection( name, false );
    CHECK( !B );
  }
  catch( ... ) {
    FAIL("Exception Thrown" );
  }
}



TEST( XmlCheck2, ConfigurationImpl )
{
  g_log("Second Xml Serialization check\n");
  
  stringstream sscheck( ios::out );
  Configuration::writeXml( A, sscheck );

  // This xml has newlines in critical places so 
  // the serialized version matches it exactly.
  // In addition, it is the cumulative result of
  // all the setName(), setValue(), addNameValue() calls
  // to the global A in the proceeding tests.
  string expectedVal1 =
    "<Freddy_the_Freeloader\n StringAttr='String Value'\n IntAttr='42'\n DoubleAttr='3.1415000000000002'\n SampleAttr='-2 -1 0 1 2'\n>\nThe Value\n<Point\n X='1'\n Y='2'\n/>\n</Freddy_the_Freeloader>\n";

  CHECK( expectedVal1 == sscheck.str() );

}

TEST( XmlCheck3, ConfigurationImpl )
{
  g_log("Third Xml Serialization check\n");
  
  // Whip up a simple subsection:
  ConfigurationPtr B( new ConfigurationImpl( "JustValue" ) );
  B->setValue( "STRING" );

  A->getSubsection( "Point" )->addSubsection( B );

  stringstream sscheck( ios::out );
  Configuration::writeXml( A, sscheck );

  string expectedVal1 =
    "<Freddy_the_Freeloader\n StringAttr='String Value'\n IntAttr='42'\n DoubleAttr='3.1415000000000002'\n SampleAttr='-2 -1 0 1 2'\n>\nThe Value\n<Point\n X='1'\n Y='2'\n>\n<JustValue>\nSTRING\n</JustValue>\n</Point>\n</Freddy_the_Freeloader>\n";

  CHECK( expectedVal1 == sscheck.str() );

}

TEST( XmlParse0, ConfigurationImpl )
{
  g_log( "Parse XML check\n" );

  string testVal =
    "<Freddy_the_Freeloader\n StringAttr='String Value'\n IntAttr='42'\n DoubleAttr='3.1415000000000002'\n SampleAttr='-2 -1 0 1 2'\n>\nThe Value\n<Point\n X='1'\n Y='2'\n>\n<JustValue>\nSTRING\n</JustValue>\n</Point>\n</Freddy_the_Freeloader>\n";

  stringstream ss( testVal, ios::in );
  ConfigurationPtr B = Configuration::readXml( ss );

  stringstream sscheck( ios::out );
  Configuration::writeXml( B, sscheck );

  CHECK( testVal == sscheck.str() );

}  

TEST( XmlParse1, ConfigurationImpl )
{
  g_log( "Parse XML exception check 1\n");

  string badxml = "ajskfjsdk";

  stringstream ss( badxml, ios::in );

  try {
    ConfigurationPtr B = Configuration::readXml( ss );
    FAIL( "No Exception Thrown" );
  }
  catch( XmlParseException& e ) {
    g_log( " (ok) exception = %s\n", e.what() );
  }
  catch( ... ) {
    FAIL( "Wrong Exception Thrown" );
  }
}
  
TEST( XmlParse2, ConfigurationImpl )
{
  g_log( "Parse XML exception check 1\n");

  string badxml = "<Starttag></EndTag>";

  stringstream ss( badxml, ios::in );

  try {
    ConfigurationPtr B = Configuration::readXml( ss );
    FAIL( "No Exception Thrown" );
  }
  catch( XmlParseException& e ) {
    g_log( " (ok) exception = %s\n", e.what() );
  }
  catch( ... ) {
    FAIL( "Wrong Exception Thrown" );
  }
}
  
