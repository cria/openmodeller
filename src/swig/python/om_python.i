%module om

%include "../../inc/om_defs.hh"

#undef dllexp
#undef dll_log

#define dllexp
#define dll_log

%{
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

// This tells SWIG to treat an double * argument with name 'OutValue' as
// an output value.  We'll append the value to the current result which 
// is guaranteed to be a List object by SWIG.

%typemap(out) AlgMetadata ** 
{
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

%inline %{

PyObject * getParameterList(AlgMetadata * metadata)
{
    int i;
    PyObject * paramMetadata;
    PyObject * list = PyList_New(0);
    for (i = 0; i < metadata->nparam; i++)
    {
	paramMetadata = SWIG_NewPointerObj((void *) &(metadata->param[i]), 
                                           SWIGTYPE_p_AlgParamMetadata, 1);
        PyList_Append(list, paramMetadata);
    }

    return list;
}

%}

%typemap(argout) double *OutValue {
    PyObject *o, *o2, *o3;
    o = PyFloat_FromDouble(*$1);
    if ((!$result) || ($result == Py_None)) {
        $result = o;
    } else {
        if (!PyTuple_Check($result)) {
            PyObject *o2 = $result;
            $result = PyTuple_New(1);
            PyTuple_SetItem(target,0,o2);
        }
        o3 = PyTuple_New(1);
        PyTuple_SetItem(o3,0,o);
        o2 = $result;
        $result = PySequence_Concat(o2,o3);
        Py_DECREF(o2);
        Py_DECREF(o3);
    }
}


// This tells SWIG to treat char ** as a special case
%typemap(in) char ** {
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (char **) malloc((size+1)*sizeof(char *));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyString_Check(o))
	$1[i] = PyString_AsString(PyList_GetItem($input,i));
      else {
	PyErr_SetString(PyExc_TypeError,"list must contain strings");
	free($1);
	return NULL;
      }
    }
    $1[i] = 0;
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}

// This cleans up the char ** array we malloc'd before the function call
%typemap(freearg) char ** {
  free((char *) $1);
}

// This tells SWIG to treat AlgParameter * as a special case
%typemap(in) AlgParameter * 
{
  /* Check if is a list */
  if (PyList_Check($input)) 
  {
    int size = PyList_Size($input);
    int algsize = size*sizeof(AlgParameter);
    int i = 0;
    $1 = (AlgParameter *) malloc(algsize);
    memset($1, 0, algsize);
    for (i = 0; i < size; i++) 
    {
      PyObject *o = PyList_GetItem($input, i);
      // this should also be a list with 2 elements: a pair (name, value) for each param

      if (PyList_Check(o))
      {
        int innerListSize = PyList_Size(o);
        if (innerListSize == 2)
        {
          
          if (PyString_Check(PyList_GetItem(o,0)) && 
              PyString_Check(PyList_GetItem(o,1)))
          {
	    char * name = PyString_AsString(PyList_GetItem(o,0));
            char * value = PyString_AsString(PyList_GetItem(o,1));
	    $1[i].setId(name);
	    $1[i].setValue(value);
          }
          else 
          {
	    PyErr_SetString(PyExc_TypeError,"inner lists must contain strings");
            free($1);
	    return NULL;
          }
        }

        else
        {
	  PyErr_SetString(PyExc_TypeError,"innser list must contain 2 elements (param name, param value pair)");
	  free($1);
	  return NULL;
        }
      }

      else
      {
	PyErr_SetString(PyExc_TypeError,"list must contain 2 element lists (param name, param value pairs)");
	free($1);
	return NULL;
      }
    }
  } 
  else 
  {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}

// This cleans up the char ** array we malloc'd before the function call
%typemap(freearg) AlgParameter * 
{
  free((char *) $1);
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

%rename(setOutputMapByFile)   OpenModeller::setOutputMap(Scalar mult, char *output_file, char *mask, char *file_with_format);
%rename(setOutputMapByFormat) OpenModeller::setOutputMap(Scalar mult, char *output_file, char *mask, MapFormat *format);

%rename(createMapNative) OpenModeller::createMap(Environment *, char *, char *);
%rename(createMapProj)   OpenModeller::createMap(char *, char *);

%rename(printOccurrences)     Occurrences::print(char *);

%include "../../inc/om_defs.hh"
%include "../../inc/om_serializable.hh"
%include "../../inc/om_control.hh"
%include "../../inc/environment.hh"
%include "../../inc/map_format.hh"
%include "../../inc/om_occurrences.hh"
%include "../../inc/om_alg_parameter.hh"
%include "../../inc/om_algorithm_metadata.hh"
%include "../../inc/file_parser.hh"
%include "../../inc/om_area_stats.hh"
%include "../../inc/om_conf_matrix.hh"
%include "../../console/occurrences_file.hh"

