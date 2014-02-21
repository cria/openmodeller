#ifndef _SAMPLEEXPR_HH
#define _SAMPLEEXPR_HH

#include <openmodeller/om_defs.hh>
#include <openmodeller/Sample.hh>

#include <cmath>
#include <algorithm>

template< typename T >
class SExpr
{
  friend class Sample;
public:

  inline
  SExpr() :
    i_(1),
    t_()
  {}
  inline
  SExpr( const T& t ) :
    i_(1),
    t_(t)
  {}

  inline
  SExpr( const SExpr<T>& t ) :
    i_( t.i_ ),
    t_( t.t_ )
  {}

  template<typename U>
  inline
  SExpr& operator()(const U& u ) {
    t_(i_, u );
    ++i_;
    return *this;
  }

  inline
  const T& expr() const {
    return t_;
  }

private:
  inline
  size_t size() const {
    return t_.size();
  }

  inline
  Scalar const operator*() const {
    return *t_;
  }

  inline
  void operator++() const {
    ++t_;
  }

  inline
  void reset() const {
    t_.reset();
    i_ = 1;
  }

  mutable int i_;
  T t_;

};

/*
 * Wrapper for Sample object in an Expression
 */
class SExprSample
{
public:
  inline
  SExprSample() :
    begin_(),
    it_(),
    size_(0)
  {};

  inline
  SExprSample( const Sample& t ) :
    begin_( t.begin() ),
    it_( begin_ ),
    size_( t.size() )
  {}

  inline
  SExprSample( const SExprSample& t ) :
    begin_( t.begin_ ),
    it_( t.it_ ),
    size_( t.size_ )
  {}

  inline
  SExprSample& operator=( const Sample& t ) {
    begin_ = t.begin();
    it_ = begin_;
    size_ = t.size();
    return *this;
  }

  template< typename U>
  inline
  void operator()( int i, const U& u ) {
    return;
  }

  inline
  size_t size() const {
    return size_;
  }

  inline
  Scalar operator*() const {
    return *it_;
  }

  inline
  void operator++() const {
    ++it_;
  }

  inline
  void reset() const {
    it_ = begin_;
  }

private:
  Sample::const_iterator begin_;
  mutable Sample::const_iterator it_;
  size_t size_;
};

/*
 * Wrapper for Scalar object in an Expression
 */
class SExprScalar
{
public:
  inline
  SExprScalar( ) :
    value_(0.0)
  {}

  inline
  SExprScalar( Scalar v ) :
    value_(v)
  {}

  inline
  SExprScalar( const SExprScalar& v ) :
    value_( v.value_ )
  {}

  inline
  SExprScalar& operator=( Scalar v ) {
    value_ = v;
    return *this;
  }

  template< typename U>
  inline
  void operator()( int i, const U& u ) {
    return;
  }

  inline
  size_t size() const {
    return 0;
  }

  inline
  Scalar operator*() const {
    return value_;
  }

  inline
  void operator++() const {
    return;
  }

  inline
  void reset() const {
    return;
  }

private:
  Scalar value_;
};

/********************************************************************
 *
 */

/*
 * Mini traits required for the operators below
 */
template<typename T>
struct CreateLeaf {
  typedef T Leaf_t;
  inline static
  Leaf_t make( const T& arg ) {
    return Leaf_t(arg);
  }
};

template<typename T>
struct CreateLeaf< SExpr<T> > {
  typedef T Leaf_t;
  inline static
  Leaf_t make( const SExpr<T>& arg ) {
    return Leaf_t( arg.expr() );
  }
};

template<>
struct CreateLeaf<Sample>
{
  typedef SExprSample Leaf_t;
  inline static
  Leaf_t make( const Sample& arg ) {
    return Leaf_t( arg );
  }
};

template<>
struct CreateLeaf<int>
{
  typedef SExprScalar Leaf_t;
  inline static
  Leaf_t make( const Scalar& arg ) {
    return Leaf_t( arg );
  }
};

template<>
struct CreateLeaf<Scalar>
{
  typedef SExprScalar Leaf_t;
  inline static
  Leaf_t make( const Scalar& arg ) {
    return Leaf_t( arg );
  }
};

/********************************************************************
 *
 */

/*
 * Template class which encodes a unary operation
 * on one
 * the operation type has to come first so
 * the argument type can be inferred by ADL
 */
template< typename OP, typename Arg >
class SExprUnaryOp
{
public:
  inline explicit
  SExprUnaryOp() :
    arg_(),
    op_()
  {}

  inline explicit
  SExprUnaryOp( const typename CreateLeaf<Arg>::Leaf_t& a ) :
    arg_( a ),
    op_()
  {}

  inline
  SExprUnaryOp( const SExprUnaryOp<OP,Arg> & a ) :
    arg_( a.arg_ ),
    op_()
  {}

  template< typename U>
  inline
  void operator()( int i, const U& u ) {
    arg_(i,u);
  }

  inline
  size_t size() const {
    return arg_.size();
  }

  inline
  Scalar const operator*() const {
    return op_( *arg_ );
  }

  inline
  void operator++() const {
    ++arg_;
  }

  inline
  void reset() const {
    arg_.reset();
  }

private:
  typename CreateLeaf<Arg>::Leaf_t arg_;
  OP op_;

};

/********************************************************************
 *
 */

/*
 * Template class which encodes a binary operation
 * on two arguments.
 * the operation type has to come first so
 * the argument types can be inferred by ADL
 */
template< typename OP, typename Arg1, typename Arg2 >
class SExprBinOp
{
public:
  inline
  SExprBinOp() :
    arg1_(),
    arg2_(),
    op_()
  {}

  inline explicit
  SExprBinOp( const typename CreateLeaf<Arg1>::Leaf_t& a1,
	      const typename CreateLeaf<Arg2>::Leaf_t& a2 ) :
    arg1_( a1 ),
    arg2_( a2 ),
    op_()
  {}

  inline
  SExprBinOp( const SExprBinOp<OP,Arg1,Arg2>& rhs ) :
    arg1_( rhs.arg1_ ),
    arg2_( rhs.arg2_ ),
    op_()
  {}

  template< typename U>
  inline
  void operator()( int i, const U& u ) {
    arg1_(i,u);
    arg2_(i,u);
  }

  inline
  size_t size() const {
    return setsize();
  }

  inline
  Scalar const operator*() const {
    return op_( *arg1_, *arg2_ );
  }

  inline
  void operator++() const {
    ++arg1_;
    ++arg2_;
  }

  inline
  void reset() const {
    arg1_.reset();
    arg2_.reset();
  }

private:
  inline
  size_t setsize() const {
    if ( arg1_.size() == 0 ) {
      return arg2_.size();
    }
    if ( arg2_.size() == 0 ) {
      return arg1_.size();
    }

    return std::min( arg1_.size(), arg2_.size() );
  }

  typename CreateLeaf<Arg1>::Leaf_t arg1_;
  typename CreateLeaf<Arg2>::Leaf_t arg2_;
  OP op_;

};

/********************************************************************
 *
 */

/*
 * Class for unary operator -
 */
struct SExprUnaryOpNeg {
  SExprUnaryOpNeg() {}
  inline Scalar operator()( Scalar a ) const {
    return -a;
  }
};

/*
 * Class for unary sqrt
 */
struct SExprUnaryOpSqrt {
  SExprUnaryOpSqrt() {}
  inline Scalar operator()( Scalar a ) const {
    return std::sqrt(a);
  }
};

/*
 * Class for unary sqr
 */
struct SExprUnaryOpSqr {
  SExprUnaryOpSqr() {}
  inline Scalar operator()( Scalar a ) const {
    return a*a;
  }
};

/*
 * Class for binary operator +
 */
struct SExprBinOpPlus {
  SExprBinOpPlus() {};
  inline Scalar operator()( Scalar a, Scalar b ) const {
    return a+b;
  }
};

/*
 * Class for binary operator -
 */
struct SExprBinOpMinus {
  SExprBinOpMinus() {};
  inline Scalar operator()( Scalar a, Scalar b ) const {
    return a-b;
  }
};

/*
 * Class for binary operator *
 */
struct SExprBinOpProd {
  SExprBinOpProd() {};
  inline Scalar operator()( Scalar a, Scalar b ) const {
    return a*b;
  }
};

/*
 * Class for binary operator /
 */
struct SExprBinOpQuotent {
  SExprBinOpQuotent() {};
  inline Scalar operator()( Scalar a, Scalar b ) const {
    return a/b;
  }
};

/*
 * Class for binary operator min
 */
struct SExprBinOpMin {
  SExprBinOpMin() {};
  inline Scalar operator()( Scalar a, Scalar b ) const {

// avoid overlap with windows.h macros
#ifdef WIN32
#undef min
#endif

    return std::min(a,b);
  }
};

/*
 * Class for binary operator max
 */
struct SExprBinOpMax {
  SExprBinOpMax() {};
  inline Scalar operator()( Scalar a, Scalar b ) const {

// avoid overlap with windows.h macros
#ifdef WIN32
#undef max
#endif

    return std::max(a,b);
  }
};

/********************************************************************
 * Convience templates for constructing expressions.
 */

// unary -
template< typename Arg >
inline
SExpr< SExprUnaryOp<SExprUnaryOpNeg,Arg > >
operator- ( const Arg& arg ) {
  typedef SExpr< SExprUnaryOp<SExprUnaryOpNeg,Arg > > expr_t;
  return expr_t ( SExprUnaryOp<SExprUnaryOpNeg,Arg >
		  ( CreateLeaf<Arg>::make( arg ) ) );
}

// sqrt
template< typename Arg >
inline
SExpr< SExprUnaryOp<SExprUnaryOpSqrt,Arg > >
sqrt ( Arg arg ) {
  typedef SExpr< SExprUnaryOp<SExprUnaryOpSqrt,Arg > > expr_t;
  return  expr_t ( SExprUnaryOp<SExprUnaryOpSqrt,Arg >
		   ( CreateLeaf<Arg>::make( arg ) ) );
}

// sqr
template< typename Arg >
inline
SExpr< SExprUnaryOp<SExprUnaryOpSqr,Arg > >
sqr ( const Arg & arg ) {
  typedef SExpr< SExprUnaryOp<SExprUnaryOpSqr,Arg > > expr_t;
  return expr_t ( SExprUnaryOp<SExprUnaryOpSqr,Arg >
		  ( CreateLeaf<Arg>::make( arg ) ) );
}

/*
 * Now define the convient operator syntax
 */
// binary +
template< typename Arg1, typename Arg2 >
inline
SExpr< SExprBinOp<SExprBinOpPlus,Arg1,Arg2 > >
operator+ ( const Arg1 & arg1, const Arg2 & arg2 ) {
  typedef SExpr< SExprBinOp<SExprBinOpPlus,Arg1,Arg2 > > expr_t;
  return  expr_t ( SExprBinOp<SExprBinOpPlus,Arg1,Arg2 >
		   ( CreateLeaf<Arg1>::make(arg1),
		     CreateLeaf<Arg2>::make(arg2) ) );
}

// binary -
template< typename Arg1, typename Arg2 >
inline
SExpr< SExprBinOp<SExprBinOpMinus,Arg1,Arg2> >
operator- ( const Arg1 & arg1, const Arg2 & arg2 ) {
  typedef SExpr< SExprBinOp<SExprBinOpMinus,Arg1,Arg2> > expr_t;
  return expr_t ( SExprBinOp< SExprBinOpMinus,Arg1,Arg2>
		  ( CreateLeaf<Arg1>::make(arg1),
		    CreateLeaf<Arg2>::make(arg2) ) );
}

// binary *
template< typename Arg1, typename Arg2 >
inline
SExpr< SExprBinOp<SExprBinOpProd,Arg1,Arg2> >
operator* ( Arg1 const & arg1, Arg2 const & arg2 ) {
  typedef SExpr< SExprBinOp<SExprBinOpProd,Arg1,Arg2> > expr_t;
  return expr_t ( SExprBinOp<SExprBinOpProd,Arg1,Arg2>
		  ( CreateLeaf<Arg1>::make(arg1),
		    CreateLeaf<Arg2>::make(arg2) ) );
}

// binary /
template< typename Arg1, typename Arg2 >
inline
SExpr< SExprBinOp<SExprBinOpQuotent,Arg1,Arg2> >
operator/ ( const Arg1 & arg1, const Arg2 & arg2 ) {
  typedef SExpr< SExprBinOp<SExprBinOpQuotent,Arg1,Arg2> > expr_t;
  return expr_t ( SExprBinOp<SExprBinOpQuotent,Arg1,Arg2>
		  ( CreateLeaf<Arg1>::make(arg1),
		    CreateLeaf<Arg2>::make(arg2) ) );
}


// binary min
template< typename Arg1 ,typename Arg2 >
SExpr< SExprBinOp< SExprBinOpMin,Arg1,Arg2> >
inline
min ( Arg1 arg1, Arg2 arg2 ) {
  typedef SExpr< SExprBinOp< SExprBinOpMin,Arg1,Arg2> > expr_t;
  return expr_t ( SExprBinOp< SExprBinOpMin, Arg1, Arg2>
		  ( CreateLeaf<Arg1>::make(arg1),
		    CreateLeaf<Arg2>::make(arg2) ) );
}

// binary max
template< typename Arg1, typename Arg2 >
inline
SExpr< SExprBinOp< SExprBinOpMax, Arg1, Arg2 > >
max ( Arg1 arg1, Arg2 arg2 ) {
  typedef SExpr< SExprBinOp< SExprBinOpMax, Arg1, Arg2 > > expr_t;
  return expr_t ( SExprBinOp< SExprBinOpMax, Arg1, Arg2 >
		  ( CreateLeaf<Arg1>::make(arg1),
		    CreateLeaf<Arg2>::make(arg2) ) );
}

#endif
