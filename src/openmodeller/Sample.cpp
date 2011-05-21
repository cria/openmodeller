
#include <Sample.hh>

// check for malloc.h or stdlib.h
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <stdlib.h>
#endif

// cmath is included for std::sqrt
#include <cmath>

// String stream is included for the dump method
#include <sstream>

// Algorithm is included for std::min and std::max.
#include <algorithm>

// vector is used in a temporary variable in operator>>
#include <vector>

// iterator is needed for istream_iterator
#include <iterator>

// FIXME - SampleExpr.hh needs to be included after <vector>
#include <SampleExpr.hh>

// Log is included for the dump method
#include <openmodeller/Log.hh>

// To throw an out of memory exception
#include <openmodeller/Exceptions.hh>


using namespace std;

Sample::Sample() :
  size_( 0 ),
  value_( 0 ),
  start_( 0 )
{ }

Sample::Sample( size_t size ) :
  size_( 0 ),
  value_( 0 ),
  start_( 0 )
{
  if ( size == 0 ) {
    return;
  }

  alloc( size );
  Scalar *v = value_;

  for ( size_t i = 0; i < size; ++i, ++v ) {
    *v = Scalar(0);
  }
}

Sample::Sample( std::size_t size, Scalar value ) :
  size_( 0 ),
  value_( 0 ),
  start_( 0 )
{
  if ( size == 0 ) {
    return;
  }
  alloc( size );

  Scalar *v = value_;
  for( size_t i = 0; i<size; ++i ) {
    *v++ = value;
  }
}

Sample::Sample( size_t size, Scalar const * values ) :
  size_( 0 ),
  value_( 0 ),
  start_( 0 )
{
  if ( size == 0 ) {
    return;
  }
  alloc( size );
  copy( size, values );
}

Sample::Sample( std::vector<Scalar> values ) :
  size_( 0 ),
  value_( 0 ),
  start_( 0 )
{
  size_ = values.size();
  if ( size_ == 0 ) {
    return;
  }

  alloc( size_ );

  std::vector<Scalar>::const_iterator it = values.begin();
  std::vector<Scalar>::const_iterator end = values.end();

  Scalar *v = value_;
  while (it != end) {
    *v = (*it);
    ++v;
    ++it;
  }
}

Sample::Sample( const Sample & rhs ) :
  size_( 0 ),
  value_( 0 ),
  start_( 0 )
{
  if ( rhs.size_ == 0 ) {
    return;
  }
  alloc( rhs.size_ );
  copy( rhs.size_, rhs.value_ );
}

Sample::~Sample()
{
  if ( value_ ) {
    free( value_ );
  }
}

Sample&
Sample::operator=( const Sample & rhs )
{
  if ( this == &rhs ) {
    return *this;
  }

  if ( this->size_ != rhs.size_ ) {
    if ( value_ ) {
      free( value_ );
    }
    alloc( rhs.size_ );
  }

  copy( rhs.size_, rhs.value_ );

  start_ = rhs.start_;

  return *this;
}

void
Sample::resize( size_t size )
{
  // Do the easy case first.
  // The size hasn't changed, then just return.
  if ( size == this->size_ ) {
    return;
  }

  // Now check if the size is changing to 0.
  if ( size == 0 ) {
    // Since above, we tested for size == this->size,
    // we know that this->size != 0, and therefore
    // value_ != NULL;
    free( value_ );
    value_ = 0;
    this->size_ = 0;
    this->start_ = 0;
    return;
  }

  // Use realloc to make new space and copy.
  value_ = (Scalar*)realloc( value_, size*sizeof(Scalar) );

  // Now we need to loop through the new values, and initialize
  for ( Scalar *v = value_ + this->size_;
	v < value_ + size;
	++v ) {
    *v = Scalar(0);
  }

  // Finally, update this->size.
  this->size_ = size;

  this->start_ = min( size, this->start_ );
}


void
Sample::setCategoricalThreshold( std::size_t index )
{
  index = max( (std::size_t)0, index ); // avoid negative values
  start_ = min( index, size_ ); // avoid values greater than the size
}


void
Sample::alloc( size_t size )
{
  this->size_ = size;
  this->start_ = 0;
  value_ = (Scalar*)malloc( size * sizeof( Scalar ) );

  if ( ! value_ ) {

    throw MemoryException( "Out of memory during Sample alloc" );
  }
}

void
Sample::copy( size_t size, Scalar const * values )
{
  Scalar *v = value_;
  for( size_t i = 0; i<size; ++i ) {
    *v++ = *values++;
  }
}

Scalar& 
Sample::operator[]( std::size_t index ) 
{
  if ( index >= size_ ) {

    ostringstream ss;
    ss << "Sample index (" << index << ") out of bounds (" << size_ << ")";

    throw MemoryException( ss.str().c_str() );
  }
 
  return value_[index]; 
}

Scalar 
Sample::operator[]( std::size_t index ) const 
{
  if ( index >= size_ ) {

    ostringstream ss;
    ss << "Sample index (" << index << ") out of bounds (" << size_ << ")";

    throw MemoryException( ss.str().c_str() );
  }
 
  return value_[index]; 
}

bool
Sample::equals( const Sample& rhs ) const
{
  // Zero vectors are equal.
  if ( size_ == 0 && rhs.size_ == 0 )
    return true;

  // If sizes are different, Samples must be different.
  if ( size_ != rhs.size_ )
    return false;

  // Check each value in Sample.
  Scalar *l = value_;
  Scalar *r = rhs.value_;
  for( size_t i = 0; i < size_; ++i ) {
    // If they are not equal, then Samples not equal
    if ( *l != *r )
      return false;
    ++l;
    ++r;
  }

  return true;
}

void
Sample::dump() const
{
  ostringstream ss;
  ss << "[";
  Scalar *vl = value_;
  size_t count = size_;
  for( size_t i = 0; i < count; ++i ) {
    ss << *vl++;
    if ( i < count-1 )
      ss << ", ";
  }
  ss << "]";
  Log::instance()->info( "Sample %s\n", ss.str().c_str() );
}

Sample&
Sample::operator+=( const Sample& rhs )
{
  Scalar *vl = value_;
  Scalar *vr = rhs.value_;
  size_t count = min( size_, rhs.size_);
  for( size_t i = 0; i < count; ++i,*vl++,*vr++ ) {
    if ( i >= start_ ) {
      *vl += *vr;
    }
  }
  (void) vr; // avoid unused var warning
  return *this;
}

Sample&
Sample::operator+=( const Scalar& rhs )
{
  iterator it = begin();
  for( size_t i = 0; it != end() ; ++it,++i ) {
    if ( i >= start_ ) {
      *it += rhs;
    }
  }
  return *this;
}

Sample&
Sample::operator-=( const Sample& rhs )
{
  Scalar *vl = value_;
  Scalar *vr = rhs.value_;
  size_t count = min( size_, rhs.size_);
  for( size_t i = 0; i < count; ++i,*vl++,*vr++ ) {
    if ( i >= start_ ) {
      *vl -= *vr;
    }
  }
  return *this;
}

Sample&
Sample::operator-=( const Scalar& rhs )
{
  iterator it = begin();
  for( size_t i = 0; it != end() ; ++it,++i ) {
    if ( i >= start_ ) {
      *it -= rhs;
    }
  }
  return *this;
}

Sample&
Sample::operator*=( const Sample& rhs )
{
  Scalar *vl = value_;
  Scalar *vr = rhs.value_;
  size_t count = min( size_, rhs.size_);
  for( size_t i = 0; i < count; ++i,*vl++,*vr++ ) {
    if ( i >= start_ ) {
      *vl *= *vr;
    }
  }
  return *this;
}

Sample&
Sample::operator*=( const Scalar& rhs )
{
  Scalar *vl = value_;
  size_t count = size_;
  for( size_t i = 0; i < count; ++i,*vl++ ) {
    if ( i >= start_ ) {
      *vl *= rhs;
    }
  }
  return *this;
}

Sample&
Sample::operator/=( const Sample& rhs )
{
  Scalar *vl = value_;
  Scalar *vr = rhs.value_;
  size_t count = min( size_, rhs.size_);
  for( size_t i = 0; i < count; ++i,*vl++,*vr++ ) {
    if ( i >= start_ ) {
      *vl /= *vr;
    }
  }
  return *this;
}

Sample&
Sample::operator/=( const Scalar& rhs )
{
  Scalar *vl = value_;
  size_t count = size_;
  for( size_t i = start_; i < count; ++i,*vl++ ) {
    if ( i >= start_ ) {
     *vl /= rhs;
    }
  }
  return *this;
}

Sample&
Sample::operator&=( const Sample& rhs )
{
  Scalar *vl = value_;
  Scalar *vr = rhs.value_;
  size_t count = min( size_, rhs.size_);
  for( size_t i = 0; i < count; ++i,vl++,vr++ ) {
    // The increments are not in the min() statement
    // because gcc has problems producing good code
    // when using -fno-inline.  For safety sake
    // the increments are explicitly after the min.
    if ( i >= start_ ) {
      *vl = min(*vl,*vr);
    }
  }
  return *this;
}

Sample&
Sample::operator|=( const Sample& rhs )
{
  Scalar *vl = value_;
  Scalar *vr = rhs.value_;
  size_t count = min( size_, rhs.size_);
  for( size_t i = start_; i < count; ++i,vl++,vr++ ) {
    // The increments are not in the max() statement
    // because gcc has problems producing good code
    // when using -fno-inline.  For safety sake
    // the increments are explicitly after the max.
    if ( i >= start_ ) {
      *vl = max(*vl,*vr);
    }
  }
  return *this;
}

Sample&
Sample::sqr()
{
  iterator it = begin();
  for( size_t i = 0; it != end() ; ++it,++i ) {
    if ( i >= start_ ) {
      *it = (*it) * (*it);
    }
  }
  return *this;
}

Sample&
Sample::sqrt()
{
  iterator it = begin();
  for( size_t i = 0; it != end() ; ++it,++i ) {
    if ( i >= start_ ) {
      *it = std::sqrt( *it );
    }
  }
  return *this;
}

Scalar
Sample::norm() const
{
  Scalar norm = 0.0;
  const_iterator it = begin();
  for( size_t i = 0; it != end() ; ++it,++i ) {
    if ( i >= start_ ) {
      norm += *it* *it;
    }
  }
  return std::sqrt(norm);
}

Scalar
Sample::dotProduct( const Sample& rhs ) const
{
  Scalar norm = 0.0;
  const_iterator lhs_it = begin();
  const_iterator rhs_it = rhs.begin();
  for( size_t i = 0; lhs_it != end() && rhs_it != rhs.end() ; ++lhs_it, ++rhs_it, ++i ) {
    if ( i >= start_ ) {
      norm += *lhs_it* *rhs_it;
    }
  }
  return std::sqrt(norm);
}

ostream&
operator<<( ostream& os, const Sample& value )
{
  if ( value.size() == 0 )
    return os;

  int oldprecision = os.precision(25);

  copy( value.begin(), value.end(),
	ostream_iterator<Scalar>(os," ") );

  os.precision( oldprecision );
  return os;
}

istream&
operator>>( istream& is, Sample& value )
{
  // Temporarily spool values into a vector<double>
  vector<double>v;

  copy( istream_iterator<double>(is),
	istream_iterator<double>(),
	back_inserter( v ) );

  // Copy from temporary vector<double> into value
  value.resize( v.size() );

  copy( v.begin(),
	v.end(),
	value.begin() );

  return is;

}
