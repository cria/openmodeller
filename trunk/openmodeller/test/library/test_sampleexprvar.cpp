

#include <SampleExprVar.hh>

#include <CppUnitLite/TestHarness.h>

#include <om_log.hh>

#include <cmath>
#include <algorithm>

#define SAMPLESIZE 10

Sample a(SAMPLESIZE);
Sample b(SAMPLESIZE);
Sample c(SAMPLESIZE);

//  sqr(_2 * - sqrt( _1 + _3 ) )
SExprType< SQR< TIMES< _2<Sample>, NEG< SQRT < PLUS< _1<Sample>, _3<Sample> > > > > > >::type expr;

TEST( Initialize, SampleExpr )
{
  g_log("Initialize variables:\n");
  for( int i=0; i<SAMPLESIZE; ++i ) {
    a[i] = i+1;
    b[i] = SAMPLESIZE-i;
    c[i] = SAMPLESIZE - 2*i;
  }
}

TEST( First, SampleExpr )
{
  g_log("SampleExprVar First Test:\n");

  Sample d = expr(a)(b)(c);

  for( int i=0; i<a.size(); ++i ) {
    Scalar val = b[i] * std::sqrt( a[i] + c[i] );
    val *= val;
    DOUBLES_EQUAL( d[i], val, 1e-10 );
  }
}

TEST( Second, SampleExpr )
{
  g_log("SampleExprVar Second Test:\n");

  Sample d = expr(b)(c)(a);

  for( int i=0; i<a.size(); ++i ) {
    Scalar val = c[i] * std::sqrt( b[i] + a[i] );
    val *= val;
    DOUBLES_EQUAL( d[i], val, 1e-10 );
  }
}


TEST( Third, SampleExpr )
{
  g_log("SampleExprVar Third Test:\n");

  Sample d = expr(a)(b)(c);
  Sample e = expr(a)(b)(c);

  for( int i=0; i<a.size(); ++i ) {
    Scalar val = b[i] * std::sqrt( a[i] + c[i] );
    val *= val;
    DOUBLES_EQUAL( d[i], val, 1e-10 );
    DOUBLES_EQUAL( e[i], val, 1e-10 );
  }
}

