

#include <Sample.hh>
#include <CppUnitLite/TestHarness.h>

#include <om_log.hh>

#include <algorithm>

#include <istream>
#include <sstream>

using namespace std;

Sample a;
Sample b;

TEST( DefaultCon, Sample )
{
  g_log("Sample Default Constructor:\n");
  //CHECK( a.getRawVector() == 0 );
  CHECK( a.size() == 0 );
}

TEST( Resize, Sample )
{
  g_log("Sample Resize:\n");
  a.resize(10);
  CHECK( a.size() == 10 );
}

TEST( SingleValueCon, Sample )
{
  g_log("Sample Constructor initialized with single Scalar\n" );
  Sample x(10, -0.1);

  for( int i=0; i<x.size(); ++i ) {
    DOUBLES_EQUAL( -0.1, x[i], 1e-10 );
  }
}

TEST( ArrayOp, Sample )
{
  g_log("Sample assign through operator[]\n" );
  for( int i=0; i<a.size(); ++i )
    a[i] = i;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( a[i] == i );
  }

}

TEST( Equals, Sample )
{
  g_log("Sample equals( const Sample& ) and operator==()\n");

  Sample c( 3 );
  CHECK( ! c.equals(a) );
  CHECK( ! (c==a) );
  CHECK( c != a );

  Sample d( a.size() );
  CHECK( ! d.equals(a) );
  CHECK( ! (d==a) );
  CHECK( d != a );

  for( int i=0; i<a.size(); ++i )
    d[i] = a[i];

  for( int i=0; i<a.size(); ++i ) {
    CHECK( a[i] == i );
  }
  CHECK( d.equals(a) );
  CHECK( d==a );
  CHECK( ! (d!=a) );
}

TEST( CopyCon, Sample )
{
  g_log("Sample Copy Constructor\n" );
  Sample c(a);

  //CHECK( a.getRawVector() != c.getRawVector() );
  CHECK( a == c );

}

TEST( CopyAssign, Sample )
{
  g_log("Sample operator=( const Sample & )\n");
  b = a;
  CHECK( b == a );
}

TEST( Stream, Sample )
{
  g_log("Sample operator>> and operator<<\n" );

  cerr << "A = " << a << endl;
  stringstream ss( ios::in | ios::out );
  ss << a;
  Sample b;
  ss >> b;
  cerr << "B = " << b << endl;
  CHECK( a == b );
}

TEST( StreamDouble, Sample )
{

  g_log( "Sample operator>> and operator<< precision\n" );

  //
  // These values will actually result in doubles with a lot
  // of decimal places.
  string testString="1.0 1.01 1.001 1.0001 1.00001 1.000001 1.0000001 1.00000001 1.000000001";
  stringstream ss( testString, ios::in );

  Sample b;
  ss >> b;
  cerr << "B = " << b << endl;

  stringstream s2( ios::in | ios::out );
  s2 << b;

  Sample c;
  s2 >> c;

  CHECK ( b == c );
}

TEST( AddSample, Sample )
{
  g_log("Sample operator+=(Sample)\n");
  Sample c(a);

  c += a;
  
  for( int i=0;i<c.size(); ++i ) {
    CHECK( c[i] == 2*a[i] );
  }
}

TEST( AddScalar, Sample )
{
  g_log("Sample operator+=(Scalar)\n");
  Sample c(a);

  c += 35;

  for( int i=0;i<c.size(); ++i ) {
    CHECK( c[i] == 35+a[i] );
  }
}

TEST( SubSample, Sample )
{
  g_log("Sample operator-=(Sample)\n");
  Sample c(a);

  c -= a;

  for( int i=0;i<c.size(); ++i ) {
    CHECK( c[i] == 0 );
  }

}

TEST( SubScalar, Sample )
{
  g_log("Sample operator-=(Scalar)\n");
  Sample c(a);

  c -= 35;

  for( int i=0;i<c.size(); ++i ) {
    CHECK( c[i] == a[i] - 35 );
  }
}

TEST( MultSample, Sample )
{
  g_log("Sample operator*=(Sample)\n");
  Sample c(a);

  c *= a;

  for( int i=0;i<c.size(); ++i ) {
    CHECK( c[i] == a[i]*a[i] );
  }

}

TEST( MultScalar, Sample )
{
  g_log("Sample operator*=(Scalar)\n");
  Sample c(a);

  c *= 35;

  for( int i=0;i<c.size(); ++i ) {
    CHECK( c[i] == 35*a[i] );
  }
}

TEST( DivSample, Sample )
{
  g_log("Sample operator/=(Sample)\n");
  Sample c(a);
  Sample d(a);

  d += 1;

  c /= d;

  for( int i=0;i<c.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i]/d[i], 1e-10 );
  }

}

TEST( DivScalar, Sample )
{
  g_log("Sample operator/=(Scalar)\n");
  Sample c(a);

  c /= 35;

  for( int i=0;i<c.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i]/35, 1e-10 );
  }
}

TEST( MinSample, Sample )
{
  g_log("Sample operator&=(Scalar)\n");
  Sample b(a);

  for( int i=0;i<b.size(); ++i ) {
    b[i] = i-b[i];
  }

  Sample c(a);

  c &= b;

  for( int i=0;i<c.size(); ++i ) {
    CHECK( c[i] == std::min(a[i], b[i]) );
  }
}

TEST( MaxSample, Sample )
{
  g_log("Sample operator|=(Scalar)\n");
  Sample b(a);

  for( int i=0;i<b.size(); ++i ) {
    b[i] = i-b[i];
  }

  Sample c(a);

  c |= b;

  for( int i=0;i<c.size(); ++i ) {
    CHECK( c[i] == std::max(a[i], b[i]) );
  }
}

TEST( Sqr, Sample )
{
  g_log( "Sample sqr()\n");
  Sample c(3);
  c[0] = -5;
  c[1] = 0;
  c[2] = 0.5;

  c.sqr();

  CHECK( c[0] == 25.0 );
  CHECK( c[1] == 0.0 );
  CHECK( c[2] == 0.25 );
}

TEST( Sqrt, Sample )
{
  g_log( "Sample sqr()\n");
  Sample c(3);
  c[0] = 0;
  c[1] = 25;
  c[2] = 625;

  c.sqrt();

  CHECK( c[0] == 0.0 );
  CHECK( c[1] == 5.0 );
  CHECK( c[2] == 25.0 );
}

TEST( Norm, Sample )
{
  g_log( "Sample norm()\n");
  Sample c(3);
  c[0] = 0;
  c[1] = 3;
  c[2] = 4;

  CHECK( c.norm() == 5.0 );

}

TEST( DotProd, Sample )
{
  g_log( "Sample dotProduct()\n");
  Sample b(3);
  Sample c(3);

  b[0] = 0;
  b[1] = 3;
  b[2] = 2;

  c[0] = 10;
  c[1] = 3;
  c[2] = 8;

  CHECK( b.dotProduct(c) == 5.0 );
}
