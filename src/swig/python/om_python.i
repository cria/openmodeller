%module om

%include "../../inc/om_defs.hh"

#undef dllexp
#undef dll_log

#define dllexp
#define dll_log

%{
#include <stdio.h>
#include "../../inc/om_defs.hh"
#include "../../inc/om_control.hh"
#include "../../inc/environment.hh"
#include "../../inc/map_format.hh"
#include "../../inc/om_occurrences.hh"
#include "../../inc/om_alg_parameter.hh"
#include "../../inc/om_algorithm_metadata.hh"
#include "../../inc/file_parser.hh"
#include "../../inc/om_serializable.hh"
#include "../../inc/om_area_stats.hh"
#include "../../inc/om_conf_matrix.hh"
#include "../../console/occurrences_file.hh"
%}

// This tells SWIG to treat char ** as a special case
// In particular it is for "counted arrays" of strings, where the first
// parameter is the number of elementes in the second array.
%typemap(in,numargs=1) (int ncount,char **array)
{
  // %typemap(in,numargs=1) (int ncount,char **array)
  /* Check if is a list */
  if (! PySequence_Check($input)) {
    PyErr_SetString(PyExc_TypeError,"not a sequence");
    SWIG_fail;
  } 
  int size = PySequence_Size($input);
  $1 = size;
  int i = 0;
  $2 = (char **) malloc((size+1)*sizeof(char *));
  for (i = 0; i < size; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyString_Check(o))
      $2[i] = PyString_AsString(o);
    else {
      PyErr_SetString(PyExc_TypeError,"list must contain strings");
      free($2);
      return NULL;
    }
  }
  $2[i] = 0;
}

// This cleans up the char ** array we malloc'd before the function call
%typemap(freearg) (int ncount, char **array) {
  // %typemap(freearg) char **
  if ( $2 ) {
    free((char *) $2);
  }
}

// This tells SWIG to treat char ** as a special case
%typemap(in) char **
{
  // %typemap(in) char **
  /* Check if is a list */
  if ( ! PySequence_Check($input)) {
    PyErr_SetString(PyExc_TypeError,"not a sequence");
    SWIG_fail;
  }

  int size = PySequence_Size($input);
  int i = 0;
  $1 = (char **) malloc((size+1)*sizeof(char *));
  for (i = 0; i < size; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyString_Check(o))
      $1[i] = PyString_AsString(o);
    else {
     PyErr_SetString(PyExc_TypeError,"sequence must contain strings");
     SWIG_fail;
    }
  }
  $1[i] = 0;
}

// This cleans up the char ** array we malloc'd before the function call
%typemap(freearg) char **
{
  // %typemap(freearg) char **
  if ( $1 ) {
    free((char *) $1);
  }
}

// Now a test functions
%inline %{

int print_args(char **argv) {
    int i = 0;
    while (argv[i]) {
         printf("argv[%d] = %s\n", i,argv[i]);
         i++;
    }
    return i;
}

int print_alg_params(int n, AlgParameter *param) 
{
    int i;
    for (i = 0; i < n; i++) 
      printf("Param[%d] = (%s, %s)\n", i, param[i].id(), param[i].value());

    return n;
}
%}

%rename(printOccurrences)     Occurrences::print(char *);

class Log {
public:
  typedef enum {
    Debug, Info, Warn, Error
  } Level;
};

%include "../../inc/om_serializable.hh"
%include "../../inc/map_format.hh"
%include "../../inc/om_occurrences.hh"
%include "../../inc/om_alg_parameter.hh"
%include "../../inc/file_parser.hh"
%include "../../inc/om_area_stats.hh"
%include "../../inc/om_conf_matrix.hh"
%include "../../console/occurrences_file.hh"


//*****************************************************************************
//
// om_algorithm_metadata.hh typemaps and supporting delcs.
//
// Defines the AlgMetadata and AlgParamMetadata structs
//
//******************************************************************************

%include "../../inc/om_algorithm_metadata.hh"

%extend AlgMetadata {
  PyObject *getParameterList() {
     int i;
     PyObject * paramMetadata;
     PyObject * list = PyList_New(0);
     for (i = 0; i < self->nparam; i++)
     {
       paramMetadata = SWIG_NewPointerObj((void *) &(metadata->param[i]), 
                                            SWIGTYPE_p_AlgParamMetadata, 1);
       PyList_Append(list, paramMetadata);
     }
     return list;
  }
}

//*****************************************************************************
//
// environment.hh typemaps and supporting delcs.
//
// Defines the Environment object
//
//******************************************************************************

%apply ( int ncount, char **array)
{
  ( int ncateg, char **categs ),
  ( int nmap, char **maps )
}

%include "../../inc/environment.hh"

%clear ( int ncateg, char **categs );
%clear ( int nmap, char **maps );

//*****************************************************************************
//
// om_control.hh typemaps and supporting delcs.
//
// Defines the OpenModeller object
//
//*****************************************************************************

%{
/*
 * The following two defines are HACKS
 * SWIG does not support nested classes at all.  It will not generate wrappers for them
 * (which we don't need in python) and more importantly, does not properly declare its
 * local variables (which is a problem).  Luckily, we have a way around this.
 * When SWIG hits our typemaps below for the ModelCommand* and MapCommand* types,
 * it declares local vars as type "ModelCommand*" ("MapCommand*", resp).  With a simple
 * #define to the proper nested typename, everything is cool.
 */
#define ModelCommand OpenModeller::ModelCommand
#define MapCommand OpenModeller::MapCommand

class PyModelCommand : public ModelCommand {
  public:
    PyModelCommand( PyObject *func ) {
      Py_INCREF( func );
      my_func = func;
    }
    ~PyModelCommand() {
      Py_DECREF( my_func );
    }
    void operator()( int iterations ) {
      PyObject *arg = Py_BuildValue( "(i)", iterations );
      PyObject *result = PyEval_CallObject( my_func, arg );
      Py_DECREF( arg );
      Py_DECREF( result );
    }

    PyObject *my_func;
};

class PyMapCommand : public MapCommand {
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
      Py_DECREF( arg );
      Py_DECREF( result );
    }

    PyObject *my_func;
};

%}

%typemap( in ) ModelCommand*  {
  // %typemap( in ) ModelCommand* 
  $1 = new PyModelCommand( $input );
}

%typemap( in ) MapCommand* {
  // %typemap( in ) MapCommand* 
  $1 = new PyMapCommand( $input );
}

// This tells SWIG to treat AlgParameter * as a special case
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

%typemap(out) AlgMetadata *
{
  // %typemap(out) AlgMetadata *
  int i;
  PyObject * paramMetadata;
  PyObject * list = PyList_New(0);
  for (i = 0; i < $1->nparam; i++)
  {
    paramMetadata = SWIG_NewPointerObj((void *) &($1->param[i]), 
                                      SWIGTYPE_p_AlgParamMetadata, 1);
    PyList_Append(list, paramMetadata);
  }

  $result = list;
}

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

%apply ( int ncount, char **array)
{
  ( int num_categ, char **categ_map ),
  ( int num_continuos, char **continuous_map )
}

%rename(setOutputMapByFile)   OpenModeller::setOutputMap(Scalar mult, char *output_file, char *mask, char *file_with_format);
%rename(setOutputMapByFormat) OpenModeller::setOutputMap(Scalar mult, char *output_file, char *mask, MapFormat *format);

%rename(createMapNative) OpenModeller::createMap(Environment *, char *, char *);
%rename(createMapProj)   OpenModeller::createMap(char *, char *);

%include "../../inc/om_control.hh"
