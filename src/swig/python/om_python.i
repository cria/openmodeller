%module om

%include "openmodeller/om_defs.hh"

#undef dllexp
#undef dll_log

#define dllexp
#define dll_log

%{
#include <stdio.h>
#include <sstream>
#include <string>
#include <stdexcept>
#include "openmodeller/AlgorithmFactory.hh"
#include "openmodeller/Configuration.hh"
#include "openmodeller/env_io/Map.hh"
#include "openmodeller/Environment.hh"
#include "openmodeller/MapFormat.hh"
#include "openmodeller/Model.hh"
#include "openmodeller/models/AverageModel.hh"
#include "openmodeller/AlgParameter.hh"
#include "openmodeller/Algorithm.hh"
#include "openmodeller/AlgMetadata.hh"
#include "openmodeller/AreaStats.hh"
#include "openmodeller/ConfusionMatrix.hh"
#include "openmodeller/OpenModeller.hh"
#include "openmodeller/om_defs.hh"
#include "openmodeller/Occurrences.hh"
#include "openmodeller/Projector.hh"
#include "openmodeller/Sampler.hh"
#include "openmodeller/refcount.hh"
#include "openmodeller/env_io/RasterFactory.hh"
#include "openmodeller/occ_io/OccurrencesFactory.hh"
#include "openmodeller/occ_io/OccurrencesReader.hh"
%}

%include "std_string.i"
%include "std_vector.i"

%init %{
  AlgorithmFactory::searchDefaultDirs();
%}

//*****************************************************************************
//
// Supplemental c++ code.
//
//*****************************************************************************
%inline %{

int print_args(char **argv) {
    int i = 0;
    while (argv[i]) {
         printf("argv[%d] = %s\n", i,argv[i]);
         i++;
    }
    return i;
}

%}

//*****************************************************************************
//
// Wrap the C++ exceptions.
//
//*****************************************************************************
%include "exception.i"

%exception {
  try {
    $function
  }
  catch( std::runtime_error& e ) {
    SWIG_exception( SWIG_RuntimeError, e.what() );
  }
  catch( ... ) {
    SWIG_exception( SWIG_RuntimeError, "Unknown Exception in $name" );
  }
}


//*****************************************************************************
//
// Templates are used heavily in the reference counting classes.
// Define a SWIG macro to help define them below
//
//******************************************************************************

%define RCP_WRAP(name, impl)
%template() UnConst< impl >;
%template(name) ReferenceCountedPointer< impl >;
%enddef

// Hack here.
// This is a pidgeon of the ReferenceCountedPointer class contained in
// refcount.hh.
// It is enough to get swig to work correctly.
//%include "openmodeller/refcount.hh"
template< class U > struct UnConst
{	
    typedef U* PointerType;
    typedef U* PlainPointerType;
    typedef U& ReferenceType;
    typedef U& PlainReferenceType;
};
template< class U > struct UnConst<const U>
{
    typedef U const * PointerType;
    typedef U* PlainPointerType;
    typedef const U & ReferenceType;
    typedef U& PlainReferenceType;
};
template< class T >
class ReferenceCountedPointer {
public:
  typedef typename UnConst<T>::PointerType PointerType;
  typedef typename UnConst<T>::ReferenceType ReferenceType;
  typedef typename UnConst<T>::PlainPointerType PlainPointerType;
  inline PointerType operator->() const;
private:
  PlainPointerType _p;
};

//*****************************************************************************
//
// Generic typemaps used at various points below.
//
//******************************************************************************

// Typemap for std::vector<std::string>
%typemap(in) std::vector<std::string> ( std::vector<std::string> v )
{
  // %typemap(in) ( std::vector<std::string> )
  /* Check if is a list */
  if (! PySequence_Check($input)) {
    PyErr_SetString(PyExc_TypeError,"not a sequence");
    SWIG_fail;
  } 
  int size = PySequence_Size($input);
  for (int i = 0; i < size; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyString_Check(o)) {
      v.push_back(  static_cast<std::string>( PyString_AsString(o) ) );
    } else {
      PyErr_SetString(PyExc_TypeError,"list must contain strings");
      return NULL;
    }
  }
  $1 = v;
}
%typecheck(SWIG_TYPECHECK_POINTER) ( std::vector<std::string> )
{
  // %typecheck(SWIG_TYPECHECK_POINTER) ( std::vector<std::string> )
  $1 = (PySequence_Check( $input ) ) ? 1 : 0;
}


// Typemap for std::vector<double>
%typemap(in) std::vector<double> ( std::vector<double> v )
{
  // %typemap(in) ( std::vector<double > )
  /* Check if is a list */
  if (! PySequence_Check($input)) {
    PyErr_SetString(PyExc_TypeError,"not a sequence");
    SWIG_fail;
  } 
  int size = PySequence_Size($input);
  for (int i = 0; i < size; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyInt_Check(o)) {
      v.push_back(  static_cast<double >( PyInt_AsLong(o) ) );
    } 
    else if (PyFloat_Check(o)) {
      v.push_back(  static_cast<double >( PyFloat_AsDouble(o) ) );
    } 
    else {
      PyErr_SetString(PyExc_TypeError,"list must contain numbers");
      return NULL;
    }
  }
  $1 = v;
}
%typecheck(SWIG_TYPECHECK_POINTER) ( std::vector<double > )
{
  // %typecheck(SWIG_TYPECHECK_POINTER) ( std::vector<double > )
  $1 = (PySequence_Check( $input ) ) ? 1 : 0;
}

//*****************************************************************************
//
// XXXPtr conversion into ConstXXXPtr
//
//******************************************************************************
// Typemap to help with the conversion from XXXPtr to ConstXXXPtr
%define RCP_CONST_TYPEMAP( const_type, nonconst_type )
%typemap(in) const const_type &
{
  //%typemap(in) const const_type &
  if ( ((SWIG_ConvertPtr($input, (void**) &$1, $1_descriptor, SWIG_POINTER_EXCEPTION | 0 )) == -1)
       &&
       ((SWIG_ConvertPtr($input, (void**) &$1, $descriptor( nonconst_type *), SWIG_POINTER_EXCEPTION | 0 )) == -1)
     )
   {
     SWIG_fail;
   }
   else
   {
     PyErr_Clear();
   }
}
%typecheck(SWIG_TYPECHECK_POINTER) const const_type &
{
  //%typecheck(SWIG_TYPECHECK_POINTER) const const_type &
  void *ptr;
  if (SWIG_ConvertPtr($input, (void**) &ptr, $descriptor( nonconst_type *) , 0 ) == -1)
  {
    $1 = 0;
  } else {
    $1 = 1;
  }
}
%enddef


//*****************************************************************************
//
// Log enumeration.  taken from om_log.hh
//
//******************************************************************************

%ignore Log::LogCallback;
%ignore Log::OstreamCallback;
%ignore Log::setCallback;
%include "openmodeller/Log.hh"

//*****************************************************************************
//
// Wrap Configuration
//
//******************************************************************************

RCP_WRAP( ConfigurationPtr, ConfigurationImpl );
RCP_CONST_TYPEMAP( ConstConfigurationPtr, ConfigurationPtr );
%ignore ConfigurationPtr;
%ignore ConfigurationImpl;

%extend Configuration {
  static ConfigurationPtr readXmlFromString( const std::string & in ) {
    std::stringstream is( in ,std::ios::in );
    return Configuration::readXml( is );
  }
  static std::string writeXmlToString( const ConstConfigurationPtr& config ) {
    std::stringstream os( std::ios::out );
    Configuration::writeXml( config, os );
    return os.str();
  }
}

%inline %{
  ConfigurationPtr makeConfiguration( const char *name ) {
    return ConfigurationPtr( new ConfigurationImpl( name ) );
  }
%}

%include "openmodeller/Configuration.hh"

//*****************************************************************************
//
// Sampler classes.... ?????
//
//******************************************************************************
RCP_WRAP( SamplerPtr, SamplerImpl );
%ignore SamplerPtr;

%ignore SamplerImpl;

%include "openmodeller/Sampler.hh"

//*****************************************************************************
//
// Occurrences.
//
//******************************************************************************
RCP_WRAP( OccurrencesPtr, OccurrencesImpl );
RCP_CONST_TYPEMAP( ConstOccurrencesPtr, OccurrencesPtr );
//%ignore OccurrencesPtr;
%ignore ConstOccurrencesPtr;

%ignore OccurrencesImpl;
%ignore OccurrencesImpl::print;

%include "openmodeller/Occurrences.hh"

%inline %{
  ReferenceCountedPointer<OccurrencesImpl> createOccurrences( char *species_name, char *cs ) {
    return ReferenceCountedPointer<OccurrencesImpl>( new OccurrencesImpl( species_name, cs ) );
  }

  OccurrencesPtr readPresences( char const *source, char const *coord_sys, char *sp_name ) {
    OccurrencesReader * occ = OccurrencesFactory::instance().create( source, coord_sys );
    return occ->getPresences( sp_name );
  }

  OccurrencesPtr readAbsences( char const *source, char const *coord_sys, char *sp_name ) {
    OccurrencesReader * occ = OccurrencesFactory::instance().create( source, coord_sys );
    return occ->getAbsences( sp_name );
  }
%}

//*****************************************************************************
//
// Other things...?
//
//******************************************************************************

%include "openmodeller/AlgParameter.hh"
%include "openmodeller/AreaStats.hh"
%include "openmodeller/ConfusionMatrix.hh"
%include "openmodeller/MapFormat.hh"

//*****************************************************************************
//
// Model.hh and AverageModel.hh
//
//
//******************************************************************************

RCP_WRAP( Model, ModelImpl );
%ignore Model;
%ignore ModelImpl;
%include "openmodeller/Model.hh"

RCP_WRAP( AverageModelPtr, AverageModelImpl );
%ignore AverageModelPtr;
%ignore AverageModelImpl;
%include "openmodeller/models/AverageModel.hh"

%inline %{
  ReferenceCountedPointer<AverageModelImpl> makeAverageModel()
  {
    return ReferenceCountedPointer<AverageModelImpl> ( new AverageModelImpl() );
  }
%}

//*****************************************************************************
//
// Projector.hh
//
//
//******************************************************************************
class Projector {
public:
%extend {
  static void createMap( const ReferenceCountedPointer<AverageModelImpl>& model, const EnvironmentPtr& env, char *filename, Projector::MapCommand *mc = 0 )
  {
    MapFormat mf;
    Map *mask = env->getMask();
    if (!mask)
      mask = env->getLayer(0);
    mf.copyDefaults( *mask );
    Map map( RasterFactory::instance().create( filename, mf ) );
    Projector::createMap( Model(model), env, &map, 0, mc );
  }
} // %extend
private:
  Projector();
}; // class Projector

//*****************************************************************************
//
// AlgMetadata.hh typemaps and supporting delcs.
//
// Defines the AlgMetadata and AlgParamMetadata structs
//
//******************************************************************************

%include "openmodeller/AlgMetadata.hh"

%extend AlgMetadata {
  PyObject *getParameterList() {
     int i;
     PyObject * paramMetadata;
     PyObject * list = PyList_New(0);
     for (i = 0; i < self->nparam; i++)
     {
       paramMetadata = SWIG_NewPointerObj((void *) &(self->param[i]), 
                                            SWIGTYPE_p_AlgParamMetadata, 1);
       PyList_Append(list, paramMetadata);
     }
     return list;
  }
}

//*****************************************************************************
//
// Environment.hh typemaps and supporting delcs.
//
// Defines the Environment object
//
//******************************************************************************

RCP_WRAP( EnvironmentPtr, EnvironmentImpl );
RCP_CONST_TYPEMAP( ConstEnvironmentPtr, EnvironmentPtr );

%rename (makeEnvironmentFromConfig) createEnvironment( const ConstConfigurationPtr& );
%ignore createEnvironment;

%inline %{
  EnvironmentPtr makeEnvironment( std::vector<std::string> cat_maps, std::vector<std::string> cont_maps, std::string mask )
  {
    return createEnvironment( cat_maps, cont_maps, mask );
  }
%}

// This is a hack.
// Since we're using typemaps to change the number of arguments
// the overloading & wrapping mechanism for SWIG is confused.
// Since I like the typemaps, I think it's best to just
// ignore the currently unneeded constructors.
%ignore EnvironmentImpl::EnvironmentImpl();
%ignore EnvironmentImpl::EnvironmentImpl( char *, int, char **, int, char **, char * );


//
// Ignore Environment because it is a smart pointer class
%ignore EnvironmentPtr;
%ignore EnvironmentImpl;

%include "openmodeller/Environment.hh"

//*****************************************************************************
//
// OpenModeller.hh typemaps and supporting delcs.
//
// Defines the OpenModeller object
//
//*****************************************************************************

%{
/*
 * The following define is a HACK
 * SWIG does not support nested classes at all.  It will not generate wrappers for them
 * (which we don't need in python) and more importantly, does not properly declare its
 * local variables (which is a problem).  Luckily, we have a way around this.
 * When SWIG hits our typemaps below for the ModelCommand* type,
 * it declares local vars as type "ModelCommand*".  With a simple
 * #define to the proper nested typename, everything is cool.
 *
 * Interestingly enough, Swig does not have this problem with Projector::MapCommand.
 */

class PyModelCommand : public Algorithm::ModelCommand {
  public:
    PyModelCommand( PyObject *func ) {
      Py_INCREF( func );
      my_func = func;
    }
    ~PyModelCommand() {
      Py_DECREF( my_func );
    }
    void operator()( float progress ) {
      PyObject *arg = Py_BuildValue( "(f)", progress );
      PyObject *result = PyEval_CallObject( my_func, arg );
      // If an error occurred in the Callback
      if ( PyErr_Occurred() ) {
        // Print it out, clear the error
        PyErr_Print();
      }
      Py_DECREF( arg );
      // If the callback raises an expcetion, the result is a null pointer.
      // we use Py_XDECREF here since it tests for null
      Py_XDECREF( result );
    }

    PyObject *my_func;
};

class PyMapCommand : public Projector::MapCommand {
  public:
    PyMapCommand( PyObject *func ) {
      Py_INCREF( func );
      my_func = func;
    }
    ~PyMapCommand() {
      Py_DECREF( my_func );
    }
    void operator()( float progress ) {
      PyObject *arg = Py_BuildValue( "(f)", progress );
      PyObject *result = PyEval_CallObject( my_func, arg );
      // If an error occurred in the Callback
      if ( PyErr_Occurred() ) {
        // Print it out, clear the error
        PyErr_Print();
      }
      Py_DECREF( arg );
      // If the callback raises an expcetion, the result is a null pointer.
      // we use Py_XDECREF here since it tests for null
      Py_XDECREF( result );
    }

    PyObject *my_func;
};

%}

%typemap( in ) ModelCommand*  {
  // %typemap( in ) ModelCommand* 
  if ( $input == NULL || $input == Py_None ) {
    $1 = NULL;
    Py_XDECREF( $input );
  } else {
    $1 = new PyModelCommand( $input );

  }
}

%typemap( in ) Projector::MapCommand* {
  // %typemap( in ) Projector::MapCommand* 
  $1 = new PyMapCommand( $input );
}

//// This tells SWIG to treat AlgParameter * as a special case
%typemap(in,numargs=1) (int nparam, AlgParameter *param) 
{
  // %typemap(in,numargs=1) (int nparam, AlgParameter *param) 
  /* Check if is a list */
  if ( ! PyList_Check($input)) 
  {
    PyErr_SetString(PyExc_TypeError,"not a list");
    SWIG_fail;
  }

  int size = PyList_Size($input);
  int algsize = size*sizeof(AlgParameter);
  int i = 0;
  $1 = size;
  $2 = (AlgParameter *) malloc(algsize);
  memset($2, 0, algsize);
  for (i = 0; i < size; i++) 
  {
    PyObject *o = PyList_GetItem($input, i);
    // this should also be a list with 2 elements: a pair (name, value) for each param

    if (! PySequence_Check(o) )
    {
      PyErr_SetString(PyExc_TypeError,"list must contain 2 element lists (param name, param value pairs)");
      SWIG_fail;
    }
    int innerListSize = PySequence_Size(o);

    if (innerListSize != 2)
    {
      PyErr_SetString(PyExc_TypeError,"inner list must contain 2 elements (param name, param value pair)");
      SWIG_fail;
    }

    PyObject *a = PySequence_GetItem(o,0);
    PyObject *b = PySequence_GetItem(o,1);

    if ( ! PyString_Check(a) || 
         ! PyString_Check(b) )
    {
      PyErr_SetString(PyExc_TypeError,"inner lists must contain strings");
      SWIG_fail;
    }

    char * name = PyString_AsString( a );
    char * value = PyString_AsString( b );
    $2[i].setId(name);
    $2[i].setValue(value);
  }

}

// This cleans up the char ** array we malloc'd before the function call
%typemap(freearg) (int nparam, AlgParameter *param) 
{
  // %typemap(freearg) (int nparam, AlgParameter *param) 
  if ( $2 ) {
    free((char *) $2);
  }
}

%typemap(out) AlgMetadata ** 
{
  // %typemap(out) AlgMetadata ** 
    // get size of metadata 
    int i = 0;
    PyObject * list = PyList_New(0);
    AlgMetadata * algMetadata = $1[0];
    while (algMetadata) 
    {
	PyObject * pyObj = SWIG_NewPointerObj(algMetadata, SWIGTYPE_p_AlgMetadata, 1);
        PyList_Append(list, pyObj);
	algMetadata = $1[++i];
    }
    $result = list;
    delete[] $1;
}

// Note: I don't know why someone included this typemap, since it maps an alg
// metadata to its list of parameters! This code will remain commented out unless
// we find a specific reason for the behaviour below. (Renato)
//
//%typemap(out) AlgMetadata *
//{
//  // %typemap(out) AlgMetadata *
//  int i;
//  PyObject * paramMetadata;
//  PyObject * list = PyList_New(0);
//  for (i = 0; i < $1->nparam; i++)
//  {
//    paramMetadata = SWIG_NewPointerObj((void *) &($1->param[i]), 
//                                      SWIGTYPE_p_AlgParamMetadata, 1);
//    PyList_Append(list, paramMetadata);
//  }
//
//  $result = list;
//}

%typemap(out) int 
{
  // %typemap(out) int
  if ( $1 == 0 ) {
    PyErr_SetString(PyExc_RuntimeError,"Exception in method: $symname" );
    SWIG_fail;
  }
  $result = PyInt_FromLong( $1 );
}

/* TODO:
 * Replace the above typemap(out), which is overly general and hard to control
 * its application to specific methods, with a collection of %exception
 * declarations such as this one:
 *
 *%exception OpenModeller::setOccurrences 
 *{
 *  // %exception OpenModeller::setOccurrences
 *  $action
 *  if ( result == 0 ) {
 *    PyErr_SetString(PyExc_RuntimeError,"Exception in method: $symname" );
 *    SWIG_fail;
 *  }
 *  $result = PyInt_FromLong( result );
 *}
 *
 * Would need to have a different %exception for each method this applies to
 * since %apply does not appear to work for %exceptions :(
 */

%extend OpenModeller {
  void projectNativeRange( char *outputfile ) {
    self->createMap( outputfile );
  }
  void projectNativeRange( char *outputfile, MapFormat& format ) {
    self->createMap( outputfile, format );
  }
}

%newobject OpenModeller::getConfusionMatrix;

%include "openmodeller/OpenModeller.hh"

//*****************************************************************************
//
// Algorithm.hh
//
//
//******************************************************************************
RCP_WRAP( Algorithm, AlgorithmImpl );
RCP_CONST_TYPEMAP( ConstAlgorithmPtr, AlgorithmPtr );

%inline %{
AlgorithmPtr makeAlgorithm( char const *id ) {
  return AlgorithmFactory::newAlgorithm( id );
}

AlgorithmPtr makeAlgorithmFromConfig( const ConstConfigurationPtr& config ) {
  return AlgorithmFactory::newAlgorithm( config );
}
%}

%ignore Algorithm;
%ignore AlgorithmImpl;
%ignore algorithmFactory();
%ignore algorithmMetadata();
%include "openmodeller/Algorithm.hh"

