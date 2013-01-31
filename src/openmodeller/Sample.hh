/**
 * Declaration of Sample class.
 *
 * Implements a value object wrapping an array of Scalars
 *
 * For best efficiency, do not use the copy constructor
 * but it is provided in case it is necessary.
 *
 */

#ifndef _SAMPLE_HH_
#define _SAMPLE_HH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/os_specific.hh>
#include <cstddef>
#include <iostream>
#include <vector>
//Needed for using free in template function with gcc4.3.2
#include <stdlib.h>

// Decl of SExp which is defined in SampleExpr.hh
template< typename T > class SExpr;

class dllexp Sample
{

public:
  // Construct an empty one.
  Sample();

  // Construct one of this size initialized with zeros
  explicit Sample( std::size_t size );

  // Construct one with the same value in all elements
  Sample ( std::size_t size, Scalar value );

  // Construct one with these values.
  // Copies the contents of the values array.
  Sample( std::size_t size, Scalar const * values );

  // Construct one with these values from a std::vector<Scalar>
  Sample( std::vector<Scalar> );


  // Copy one.
  // Currently implemented slowly by doing an
  // allocation and deep copy.
  Sample( const Sample & rhs );

  ~Sample();

  // Assignment operator.
  Sample& operator=( const Sample & rhs );

  // Assignment from an Expression Template.
  // Evalute the SExpr and assign to this.
  template< typename T > inline
  Sample( const SExpr<T>& rhs );
  
  template< typename T > inline
  Sample& operator=( const SExpr<T>& rhs );
 
  // Redimensions this.
  // Slow operation since it does a memcpy.
  // Any new elements are initialized to 0.
  void resize( std::size_t size );

  // First iteration mechanism works with indexing.
  inline std::size_t size() const { return size_; }

  // Set the index of the first attribute related to a continuous variable.
  // When a Sample contains attributes from both categorical and continuous
  // variables, categorical attributes always come first and their values 
  // should not be changed in most operations.
  void setCategoricalThreshold( std::size_t index );

  // Return an lvalue.
  Scalar& operator[]( std::size_t index );

  // Return an rvalue from a const reference.
  Scalar operator[]( std::size_t index ) const;

  // Second iteration mechanism works with pointers.
  // Looks like a real iterator;
  typedef Scalar* iterator;
  inline iterator begin() { return value_; }
  inline iterator end() { return value_ + size_; }

  typedef Scalar const * const_iterator;
  inline const_iterator begin() const { return value_; }
  inline const_iterator end() const  { return value_ + size_; }

  // define an equality check
  bool equals( const Sample& ) const;

  // dump values
  void dump() const;

  // define some mutating operators for convience
  
  // pointwise addition
  Sample& operator+= ( const Sample& );
  Sample& operator+= ( const Scalar& );
 
  // pointwise subtraction
  Sample& operator-= ( const Sample& );
  Sample& operator-= ( const Scalar& );
 
  // pointwise multiplication
  Sample& operator*= ( const Sample& );
  Sample& operator*= ( const Scalar& );

  // pointwise division
  Sample& operator/= ( const Sample& );
  Sample& operator/= ( const Scalar& );

  // pointwise minimum - this may not seem like a natural
  // overload but "and" in lattices means minimum.
  Sample& operator&= ( const Sample& );

  // pointwise maximum
  Sample& operator|= ( const Sample& );

  // Take the square of this and return this
  Sample& sqr();

  // Take the root of this and return this
  Sample& sqrt();

  // Compute the vector "norm" = sqrt ( sum squares )
  Scalar norm() const;

  // Compute the vector dot product with another vector
  Scalar dotProduct( const Sample& rhs ) const;

private:

  std::size_t size_;
  Scalar *value_;

  std::size_t start_; // index of the first attribute of a continuous variable

  void alloc( std::size_t size );

  void copy( std::size_t size, Scalar const * values );

};

//
// IO operator decls
//
dllexp std::ostream&
operator<<(std::ostream&,const Sample&);

dllexp std::istream&
operator>>(std::istream&, Sample&);


// Definitions of inlined functions
dllexp bool inline
operator==( const Sample& lhs, const Sample& rhs )
{
  return lhs.equals( rhs );
}

dllexp bool inline
operator!=( const Sample& lhs, const Sample& rhs )
{
  return !lhs.equals( rhs );
}

template< typename T >
inline
Sample::Sample( const SExpr<T>& rhs ) :
  size_(0),
  value_(0)
{
  operator=(rhs);
}

template< typename T >
inline Sample&
Sample::operator=( const SExpr<T>& rhs )
{
  if ( this->size_ != rhs.size() ) {
    if ( value_ ) {
      free( value_ );
    }
    alloc( rhs.size() );
  }
  rhs.reset();
  iterator meIter = begin();
  while ( meIter != end() ) {
    *meIter = *rhs;
    ++meIter;
    ++rhs;
  }
  
  return *this;
  
}

#endif
