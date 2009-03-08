#ifdef WIN32
// avoid warnings caused by problems in VC headers
#define _SCL_SECURE_NO_DEPRECATE
#endif

#include <openmodeller/ignorecase_traits.hh>

using std::size_t;

int
ignorecase_traits::compare(const char* s1, const char* s2, size_t n)
{
  for( size_t i=0; i<n; ++i ) {
    if (!eq(s1[i],s2[i])) {
      return lt(s1[i],s2[i])?-1:1;
    }
  }
  return 0;
}

const char*
ignorecase_traits::find( const char* s, size_t n, const char& c )
{
  for ( size_t i=0; i<n; ++i ) {
    if (eq(s[i],c)) {
      return &(s[i]);
    }
  }
  return 0;
}

