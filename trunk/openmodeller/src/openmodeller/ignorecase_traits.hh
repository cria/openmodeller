#ifndef _ICSTRING_HH
#define _ICSTRING_HH

#include <string>
#include <cctype>    // for std::toupper in this file and std::size_t in the .cpp

#include <openmodeller/om_defs.hh>

// Define a char_traits struct to make strings case insensitive.
// Taken from Josuttis.
struct dllexp ignorecase_traits : std::char_traits<char>
{
  static inline bool eq( const char& c1, const char& c2 ) {
    return std::toupper(c1)==std::toupper(c2);
  }
  static inline bool lt( const char& c1, const char& c2 ) {
    return std::toupper(c1)<std::toupper(c2);
  }
  static int compare(const char* s1, const char* s2, std::size_t n);
  static const char* find( const char* s,std::size_t n, const char& c );
};

class icstring : public std::basic_string< char, ignorecase_traits > {
public:
  
  inline
  icstring() :
    std::basic_string<char,ignorecase_traits>()
  {}

  inline
  icstring( const std::string & rhs ) :
    std::basic_string<char,ignorecase_traits>( rhs.c_str() )
  {}
  
  inline
  icstring( const char* chrs ) :
    std::basic_string<char,ignorecase_traits>( chrs )
  {}

  inline operator std::string() const {
    return std::string( this->c_str() );
  }
};



#endif
