//
// CsmKG
//
// Description: Csm implementation using Keiser-Gutman cutoff to discard components
//
//
// Author: CRIA <t.sutton@reading.ac.uk>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <string.h>
#include "csmkg.hh"

#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_multifit_nlin.h> //remove this when we have proper covar matrix
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sf_gamma.h>

#include <math.h>

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 0


/************************************/
/*** Algorithm parameter metadata ***/

static AlgParamMetadata *parameters = 0;
/*
AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    "First parameter id",          // Id.
    "First parameter name",        // Name.
    "First parameter type",        // Type.
    "First parameter overview",    // Overview.
    "First parameter description", // Description.

    1,     // Not zero if the parameter has lower limit.
    0.0,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    1.0,   // Parameter's upper limit.
    "0.1"  // Parameter's typical (default) value.
  },
};
*/


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "CSMKG",                 // Id.
  "Climate Space Model - Kaiser-Gutman", // Name.
  "0.1 alpha",           // Version.

  // Overview
  "Climate Space Model [CSM] is a principle components based \
algorithm developed by Dr. Neil Caithness",

  // Description.
  "Climate Space Model [CSM] is a principle components based \
algorithm developed by Dr. Neil Caithness. The component \
selection process int this algorithm implementation is \
based on the Keiser-Gutman cutoff where any component with \
an eigenvalue < 1 is discarded.\
\n \
The original CSM was written as series of Matlab functions. \
   ",

  "Neil Caithness",  // Author
  "",                 // Bibliography.

  "Tim Sutton, Renato De Giovanni",  // Code author.
  "t.sutton [at] reading.ac.uk",     // Code author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

dllexp
AlgorithmImpl *
algorithmFactory()
{
  return new CsmKG();
}

dllexp
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/****************************************************************/
/****************************** Csm *****************************/

/** Constructor for CsmKG
   * 
   * @param Sampler is class that will fetch environment variable values at each occurrence / locality
   */
CsmKG::CsmKG(): Csm(&metadata)
{
  _initialized = 0;
}


/** This is the descructor for the Csm class */
CsmKG::~CsmKG()
{
  //This is handled by the parent class I think...
  /*
  if ( _initialized )
    {
      gsl_matrix_free (_gsl_environment_matrix);
      gsl_matrix_free (_gsl_covariance_matrix);
      gsl_vector_free (_gsl_avg_vector);
      gsl_vector_free (_gsl_stddev_vector);
      gsl_vector_free (_gsl_eigenvalue_vector);
      gsl_matrix_free (_gsl_eigenvector_matrix);
    }
    */
}
int CsmKG::discardComponents()
{
  printf ("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
  printf ("     Starting CSM - Kaiser-Gutman \n");
  printf ("     Component discarding routine \n");
  printf ("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
    // Discard any columns from the eigenvector where the eigenvalue is < 1
    // (Keiser-Gutman method)
    // because the eigenvalues are sorted, we can discard any columns after the
    // first one is encountered.

    int myColumnNo = -1;
    // for debuggin print out the averages
    for (int j=0;j<_layer_count;j++)
    {
        float myValue = gsl_vector_get (_gsl_eigenvalue_vector,j);
        if (myValue < 1)
        {
            myColumnNo = j;
        }
    }
    if (myColumnNo > -1)
    {
        int j;
        _retained_components_count = myColumnNo-1;
        printf ("\n\nNumber of components retained: %i of %i\n",
                _retained_components_count,
                _layer_count);
        //so now create a local copy of the eigvect and eigval...
        //first clone the vector and matrix...
        gsl_vector * tmp_gsl_eigenvalue_vector = gsl_vector_alloc (_layer_count);
        gsl_matrix * tmp_gsl_eigenvector_matrix = gsl_matrix_alloc (_layer_count, _layer_count);

        gsl_matrix_memcpy (tmp_gsl_eigenvector_matrix, _gsl_eigenvector_matrix);
        gsl_vector_memcpy (tmp_gsl_eigenvalue_vector,_gsl_eigenvalue_vector);

        //now clear our current eig vec and matrix

        gsl_vector_free (_gsl_eigenvalue_vector);
        gsl_matrix_free (_gsl_eigenvector_matrix);

        //next we reassign them to the reduced size (ie without unwanted components)
        _gsl_eigenvalue_vector = gsl_vector_alloc (_retained_components_count);
        _gsl_eigenvector_matrix = gsl_matrix_alloc (_layer_count, _retained_components_count);

        //now copy over just the components we intend to keep
        //...first the vector
        for (j=0;j<_retained_components_count;j++)
        {
            float myFloat = gsl_vector_get (tmp_gsl_eigenvalue_vector,j);
            gsl_vector_set (_gsl_eigenvalue_vector,j,myFloat);
        }
        //...now the matrix
        gsl_vector * tmp_gsl_vector = gsl_vector_alloc (_layer_count);
        for (j=0;j<_retained_components_count;j++)
        {
            gsl_matrix_get_col (tmp_gsl_vector, tmp_gsl_eigenvector_matrix, j);
            gsl_matrix_set_col (_gsl_eigenvector_matrix,j,tmp_gsl_vector);
        }
        //now clear away the temporary vars
        gsl_vector_free (tmp_gsl_vector);
        gsl_vector_free (tmp_gsl_eigenvalue_vector);
        gsl_matrix_free (tmp_gsl_eigenvector_matrix);


    }
    else
    {
        //we keep all the components!
        _retained_components_count = _layer_count;
        printf ("\n\nNumber of components retained: %i of %i\n",
                _retained_components_count,
                _layer_count);
    }
    return 1;
}
