

#include <Sample.hh>
#include <SampleExpr.hh>
#include <CppUnitLite/TestHarness.h>

#include <om_log.hh>

#include <cmath>
#include <algorithm>

#define SAMPLESIZE 10

Sample a(SAMPLESIZE);
Sample b(SAMPLESIZE);
Sample c(SAMPLESIZE);

TEST( Initialize, SampleExpr )
{
  g_log("Initialize variables:\n");
  for( int i=0; i<SAMPLESIZE; ++i ) {
    a[i] = i+1;
    b[i] = SAMPLESIZE-i;
  }
}

TEST( UnaryNeg, SampleExpr )
{
  g_log("SampleExpr Unary Neg:\n");
  c = -a;
  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == -a[i] );
  }
}

TEST( Sqr, SampleExpr )
{
  g_log("SampleExpr Unary Sqr:\n");
  c = sqr(a);

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == a[i]*a[i] );
  }
}

TEST( Sqrt, SampleExpr )
{
  g_log("SampleExpr Unary Sqrt:\n");
  c = sqrt(a);

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], std::sqrt(a[i]), 1e-15 );
  }
}

TEST( NestUnary1, SampleExpr )
{
  g_log("SampleExpr Nested Unary Test 1:\n");
  c = - - a;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == a[i] );
  }
}

TEST( NestUnary2, SampleExpr )
{
  g_log("SampleExpr Nested Unary Test 2:\n");
  c = sqr( sqrt( a ) );

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i], 1e-15 );
  }
}

TEST( NestUnary3, SampleExpr )
{
  g_log("SampleExpr Nested Unary Test 3:\n");
  c = sqrt( sqr( a ) );

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i], 1e-15 );
  }
}

TEST( NestUnary4, SampleExpr )
{
  g_log("SampleExpr Nested Unary Test 4:\n");
  c = sqr( - sqrt( a ) );

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i], 1e-15 );
  }
}

TEST( Add, SampleExpr )
{
  g_log("SampleExpr Sample+Sample:\n");
  c = a + b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == a[i]+b[i] );
  }
}

TEST( Sub, SampleExpr )
{
  g_log("SampleExpr Sample-Sample:\n");
  c = a - b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == a[i]-b[i] );
  }
}

TEST( Mult, SampleExpr )
{
  g_log("SampleExpr Sample*Sample:\n");
  c = a * b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == a[i]*b[i] );
  }
}

TEST( Div, SampleExpr )
{
  g_log("SampleExpr Sample/Sample:\n");
  c = a / b;

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i]/b[i], 1e-15 );
  }
}

TEST( Min, SampleExpr )
{
  g_log("SampleExpr min(Sample,Sample):\n");
  c = min(a ,b);

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == std::min(a[i],b[i]) );
  }
}

TEST( Max, SampleExpr )
{
  g_log("SampleExpr max(Sample,Sample):\n");
  c = max(a ,b);

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == std::max(a[i],b[i]) );
  }
}

TEST( NestBin1, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 1 (sqr(a) + b):\n");
  c = sqr(a) + b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == a[i]*a[i] + b[i]  );
  }
}

TEST( NestBin2, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 2 ( -a - b):\n");
  c = - a - b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == - a[i] - b[i]  );
  }
}

TEST( NestBin3, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 3 ( -a * b):\n");
  c = - a * b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == - a[i] * b[i]  );
  }
}

TEST( NestBin4, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 4 ( sqr(a) / b ):\n");
  c = sqr(a)/b;

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i]*a[i]/b[i], 1e-10 );
  }
}

TEST( NestBin5, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 5 ( min(sqr(a),b) ):\n");
  c = min(sqr(a),b);

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == std::min(a[i]*a[i],b[i]) );
  }
}

TEST( NestBin6, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 6 ( max(sqr(a),b) ):\n");
  c = max(sqr(a),b);

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == std::max(a[i]*a[i],b[i]) );
  }
}

TEST( NestBin7, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 7 ( a + -b):\n");
  c = a+ -b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == a[i] - b[i]  );
  }
}

TEST( NestBin8, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 8 ( a - sqrt(b)):\n");
  c = a - sqrt(b);

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i] - std::sqrt(b[i]), 1e-15  );
  }
}

TEST( NestBin9, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 9 ( a * -b):\n");
  c = a * -b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == - a[i] * b[i]  );
  }
}

TEST( NestBin10, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 10 ( a / -b ):\n");
  c = a/-b;

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], -a[i]/b[i], 1e-15 );
  }
}

TEST( NestBin11, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 5 ( min(a,sqr(b)) ):\n");
  c = min(a,sqr(b));

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == std::min(a[i],b[i]*b[i]) );
  }
}

TEST( NestBin12, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 12 ( max(a,sqr(b)) ):\n");
  c = max(a,sqr(b));

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == std::max(a[i],b[i]*b[i]) );
  }
}


TEST( NestBin13, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 13 ( -a + -b):\n");
  c = -a +-b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == -( a[i] + b[i] )  );
  }
}

TEST( NestBin14, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 14 ( -a - -b):\n");
  c = -a - -b;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] == - a[i] + b[i] );
  }
}

TEST( NestBin15, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 15 ( sqr(a) * sqr(b)):\n");
  c = sqr(a) * sqr(b);

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] ==  a[i]*a[i] * b[i]*b[i]  );
  }
}

TEST( NestBin16, SampleExpr )
{
  g_log("SampleExpr Nest Binary Test 16 ( sqr(a) / -b ):\n");
  c = sqr(a)/-b;

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i], a[i]*a[i]/-b[i], 1e-10 );
  }
}

TEST( ScalarAdd1, SampleExpr )
{
  g_log("SampleExpr Scalar Add (a+1):\n");
  c = a + 1;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] ==  a[i]+1  );
  }
}

TEST( ScalarAdd2, SampleExpr )
{
  g_log("SampleExpr Scalar Add (a+2.0):\n");
  c = a + 2.0;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] ==  a[i]+2.0  );
  }
}

TEST( ScalarAdd3, SampleExpr )
{
  g_log("SampleExpr Scalar Add (5+a+1):\n");
  c = 5 + a + 1;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] ==  a[i]+6  );
  }
}

TEST( ScalarAdd4, SampleExpr )
{
  g_log("SampleExpr Scalar Add (30.7+a-2.0):\n");
  c = 30.7 + a - 2.0;

  for( int i=0; i<a.size(); ++i ) {
    DOUBLES_EQUAL( c[i],a[i]+28.7,1e-10  );
  }
}

TEST( ScalarMult1, SampleExpr )
{
  g_log("SampleExpr Scalar Mult (a*3):\n");
  c = a * 3;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] ==  a[i]*3  );
  }
}

TEST( ScalarMult2, SampleExpr )
{
  g_log("SampleExpr Scalar Mult (6.0*a):\n");
  c = 6.0*a;

  for( int i=0; i<a.size(); ++i ) {
    CHECK( c[i] ==  a[i]*6.0  );
  }
}

