#ifndef _REFCOUNT_HH
#define _REFCOUNT_HH

#include <openmodeller/Log.hh>

#undef DEBUG_MEMORY

//
// This is a very small templated TypeTraits class which really only
// provides a convient way to strip const qualifiers.  It is needed
// by the ReferenceCountedPointer<> template below to create
// ReferenceCountedPointer< const T * >.  Without the PlainPointerType,
// the underlying type would be const T* instead of the correct
// T*.
//
template< typename T >
class TypeTraits {
private:
  template< class U > struct UnConst
  {
    typedef U* PointerType;
    typedef U* PlainPointerType;
    typedef U& ReferenceType;
    typedef U& PlainReferenceType;
  };
  template< class U > struct UnConst<const U> {
    typedef U const * PointerType;
    typedef U* PlainPointerType;
    typedef const U & ReferenceType;
    typedef U& PlainReferenceType;
  };
public:
  typedef typename UnConst<T>::PointerType PointerType;
  typedef typename UnConst<T>::ReferenceType ReferenceType;
  typedef typename UnConst<T>::PlainPointerType PlainPointerType;
};

//
// Declaration of ReferenceCountedObject.
//
// This class contains the _ref_count and is used to decorate
// objects which ReferenceCountedPointer points to.
// This is an intrusive reference count (you need to derive from
// this class).  This mechanism has been chosen because
// we have the source of the library and can change the code
// to derive from this class, and it's space and runtime overhead
// is minimal.
//
class ReferenceCountedObject
{

  /* I'd like to get friendship to work here */
  /* Instead each derived class from this base needs
     To have it's container's class declared a friend */
  template< typename T >
  friend class ReferenceCountedPointer;

protected:
  ReferenceCountedObject() :
    _ref_count(0)
  {}

  virtual inline ~ReferenceCountedObject() = 0;

private:

  int _ref_count;

};

template< class T >
class ReferenceCountedPointer {
public:

  typedef typename TypeTraits<T>::PointerType PointerType;
  typedef typename TypeTraits<T>::ReferenceType ReferenceType;
  typedef typename TypeTraits<T>::PlainPointerType PlainPointerType;

  template< typename S >
  friend class ReferenceCountedPointer;

  //
  // Default constructor creates pointer to null.
  inline ReferenceCountedPointer( );

  //
  // implict conversion constructor from T*.
  // Provides for the following syntax
  // ReferenceCountedPointer<T> ptr = new T();
  // ... ReferenceCountedPoitner<T> ( new T() );
  //
  inline ReferenceCountedPointer( PointerType );
  //
  // Conversion from class S derived from T
  template< class S >
  inline ReferenceCountedPointer( S* );

  //
  // copy constructor.
  inline ReferenceCountedPointer( const ReferenceCountedPointer & );
  //
  // copy constructor from derived classes
  template< class S >
  inline ReferenceCountedPointer( const ReferenceCountedPointer<S> & );

  //
  // destructor
  inline virtual ~ReferenceCountedPointer();

  inline ReferenceType operator*() const;
  inline PointerType operator->() const;

  //
  // Assignment operator.
  inline ReferenceCountedPointer<T>& operator=( const ReferenceCountedPointer& );
  template< class S >
  inline ReferenceCountedPointer<T>& operator=( const ReferenceCountedPointer<S>& );

  //
  // Assignment from plain pointer classes.
  inline ReferenceCountedPointer<T>& operator=( PointerType& );
  template< class S >
  inline ReferenceCountedPointer<T>& operator=( S*& );

  // Two ways to test for NULL-ness.
  inline bool operator!() const;
  inline operator bool() const;

  // Need a member for swap since VC doesn't allow friend templates
  inline void swap( ReferenceCountedPointer<T>& rhs) {
    PlainPointerType tmp = this->_p;
    this->_p = rhs._p;
    rhs._p = tmp;
  }

private:
  inline void releasePointer();
  inline void takePointer( PlainPointerType ptr );

  PlainPointerType _p;

};

template< class T >
void
swap( ReferenceCountedPointer<T>& lhs, ReferenceCountedPointer<T>& rhs ) {
  lhs.swap(rhs);
}

/*
 *
 * Implementation for ReferenceCountedPointer inlines
 *
 */
template< class T >
ReferenceCountedPointer<T>::ReferenceCountedPointer() :
  _p( 0 )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::ReferenceCountedPointer()\n");
# endif
}

template< class T >
ReferenceCountedPointer<T>::ReferenceCountedPointer( const ReferenceCountedPointer & rhs )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::ReferenceCountedPointer( const ReferenceCountedPointer & rhs )\n" );
# endif
  takePointer( rhs._p );
}

template< class T >
template< class S >
ReferenceCountedPointer<T>::ReferenceCountedPointer( const ReferenceCountedPointer<S> & rhs )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::ReferenceCountedPointer( const ReferenceCountedPointer<S> & rhs )\n" );
# endif
  takePointer( rhs._p );
}

template< class T >
ReferenceCountedPointer<T>::ReferenceCountedPointer( PointerType rhs )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::ReferenceCountedPointer( T* rhs )\n" );
# endif
  takePointer( rhs );
}

template< class T >
template< class S >
ReferenceCountedPointer<T>::ReferenceCountedPointer( S* rhs )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::ReferenceCountedPointer( S* rhs )\n" );
# endif
  takePointer( rhs );
}

template< class T >
ReferenceCountedPointer<T>::~ReferenceCountedPointer()
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::~ReferenceCountedPointer()\n" );
# endif
  releasePointer();
}

template< class T >
ReferenceCountedPointer<T>& 
ReferenceCountedPointer<T>::operator=( const ReferenceCountedPointer& rhs )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::operator=( const ReferenceCountedPointer<T>& rhs )\n" );
# endif
  if ( this == &rhs )
    return *this;

  if ( this->_p == rhs._p )
    return *this;

  releasePointer();
  takePointer( rhs._p );
  return *this;
}

template< class T >
template< class S >
ReferenceCountedPointer<T>& 
ReferenceCountedPointer<T>::operator=( const ReferenceCountedPointer<S>& rhs )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::operator=( const ReferenceCountedPointer<S>& rhs )\n" );
# endif
  if ( this == &rhs )
    return *this;

  if ( this->_p == rhs._p )
    return *this;

  releasePointer();
  takePointer( rhs._p );
  return *this;
}

template< class T >
ReferenceCountedPointer<T>& 
ReferenceCountedPointer<T>::operator=( PointerType& rhs )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::operator=( T*& rhs )\n" );
# endif
  if ( this->_p == rhs )
    return *this;

  releasePointer();
  takePointer( rhs );
  return *this;
}

template< class T >
template< class S >
ReferenceCountedPointer<T>& 
ReferenceCountedPointer<T>::operator=( S*& rhs )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "ReferenceCountedPointer<T>::operator=( S*& rhs )\n" );
# endif
  if ( this->_p == rhs )
    return *this;

  releasePointer();
  takePointer( rhs );
  return *this;
}

template< class T >
typename TypeTraits<T>::ReferenceType
ReferenceCountedPointer<T>::operator*() const
{
  return *_p;
}

template< class T >
typename TypeTraits<T>::PointerType
ReferenceCountedPointer<T>::operator->() const
{
  return _p;
}

template< class T >
bool ReferenceCountedPointer<T>::operator!() const
{
  return (_p == 0);
}

template< class T >
ReferenceCountedPointer<T>::operator bool() const
{
  return (_p != 0);
}

template< class T >
void
ReferenceCountedPointer<T>::releasePointer()
{
  if ( _p ) {

    --_p->_ref_count;

    if ( _p->_ref_count <= 0 ) {
      delete _p;
      _p = 0;
    }
  }
}

template< class T >
void
ReferenceCountedPointer<T>::takePointer( PlainPointerType ptr )
{
# if defined(DEBUG_MEMORY)
  Log::instance()->debug( "Taking Pointer: %x\n", ptr );
# endif
  _p = ptr;
  if ( _p != 0 ) {

    ++_p->_ref_count;

#   if defined(DEBUG_MEMORY)
    Log::instance()->debug( "Recount is: %d\n",_p->_ref_count);
#   endif

  }
}

ReferenceCountedObject::~ReferenceCountedObject()
{}

#endif
