#ifndef _SAMPLEEXPRVAR_HH
#define _SAMPLEEXPRVAR_HH

#include <openmodeller/SampleExpr.hh>

template< int I, typename T >
class SExprPlaceHolder
{
public:
  typedef SExprPlaceHolder<I,T> type;
  inline
  SExprPlaceHolder() :
    t_()
  {}
  inline
  SExprPlaceHolder( const T& t ) :
    t_(t)
  {}
  inline
  SExprPlaceHolder( const SExprPlaceHolder<I,T>& t ) :
    t_( t.t_ )
  {}
  inline
  SExprPlaceHolder&
  operator=( const T& t ) {
    t_ = t;
    return *this;
  }

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
  }

  inline
  void operator()( int i, const T& t ) {
    if ( i == I ) {
      t_ = t;
    }
  }

  template< typename U >
  inline
  void operator()( int i, const U& u ) {
    return;
  }

private:
  typename CreateLeaf<T>::Leaf_t t_;

};

template<typename U> struct NEG {};
template<typename U> struct SQR {};
template<typename U> struct SQRT {};
template<typename U, typename V> struct PLUS {};
template<typename U, typename V> struct MINUS {};
template<typename U, typename V> struct TIMES {};
template<typename U, typename V> struct DIV {};
template<typename U, typename V> struct MIN {};
template<typename U, typename V> struct MAX {};

template< typename T >
struct SExprType {
  typedef T type;
};

template< typename T >
struct SExprType< SExpr<T> > {
  typedef typename SExprType<T>::type type;
};

template< typename Arg1 >
struct SExprType< NEG<Arg1> > {
  typedef SExpr< SExprUnaryOp<SExprUnaryOpNeg, typename SExprType<Arg1>::type > > type;
};
template< typename Arg1 >
struct SExprType< SQR<Arg1> > {
  typedef SExpr< SExprUnaryOp<SExprUnaryOpSqr, typename SExprType<Arg1>::type > > type;
};
template< typename Arg1 >
struct SExprType< SQRT<Arg1> > {
  typedef SExpr< SExprUnaryOp<SExprUnaryOpSqrt, typename SExprType<Arg1>::type > > type;
};

template< typename Arg1, typename Arg2 >
struct SExprType< PLUS<Arg1,Arg2> > {
  typedef SExpr< SExprBinOp<SExprBinOpPlus, typename SExprType<Arg1>::type, typename SExprType<Arg2>::type> > type;
};
template< typename Arg1, typename Arg2 >
struct SExprType< MINUS<Arg1,Arg2> > {
  typedef SExpr< SExprBinOp<SExprBinOpMinus, typename SExprType<Arg1>::type, typename SExprType<Arg2>::type> > type;
};
template< typename Arg1, typename Arg2 >
struct SExprType< TIMES<Arg1,Arg2> > {
  typedef SExpr< SExprBinOp<SExprBinOpProd, typename SExprType<Arg1>::type, typename SExprType<Arg2>::type> > type;
};
template< typename Arg1, typename Arg2 >
struct SExprType< DIV<Arg1,Arg2> > {
  typedef SExpr< SExprBinOp<SExprBinOpQuotent, typename SExprType<Arg1>::type, typename SExprType<Arg2>::type> > type;
};
template< typename Arg1, typename Arg2 >
struct SExprType< MIN<Arg1,Arg2> > {
  typedef SExpr< SExprBinOp<SExprBinOpMin, typename SExprType<Arg1>::type, typename SExprType<Arg2>::type> > type;
};
template< typename Arg1, typename Arg2 >
struct SExprType< MAX<Arg1,Arg2> > {
  typedef SExpr< SExprBinOp<SExprBinOpMax, typename SExprType<Arg1>::type, typename SExprType<Arg2>::type> > type;
};

template< typename S >
class _1 : public SExprPlaceHolder<1,S> {};

template< typename S >
class _2 : public SExprPlaceHolder<2,S> {};

template< typename S >
class _3 : public SExprPlaceHolder<3,S> {};

template< typename S >
class _4 : public SExprPlaceHolder<4,S> {};

#endif
