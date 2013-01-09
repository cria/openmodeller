//
// CsmBS
//
// Description: Csm implementation using Broken Stick method to discard components
//
//
// Author: CRIA <t.sutton@reading.ac.uk>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef WIN32
// avoid warnings caused by problems in VC headers
#define _SCL_SECURE_NO_DEPRECATE
#endif

#include <string.h>
#include <iostream>
#include "csmbs.hh"

#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_multifit_nlin.h> //remove this when we have proper covar matrix
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_heapsort.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>

#include <math.h>
//next includes needed for rand
#include <stdlib.h>
#include <time.h>
/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 4


/************************************/
/*** Algorithm parameter metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

      // Metadata of the first parameter.
      {
        "Randomisations", // Id.
        "Number of random eigenvalues", // Name.
        Integer, // Type.
        "The number of eigenvalues to generate from randomly 'shuffled' environment data.", //overview
        "The Broken Stick method of selecting the number of components to keep \
is carried out by randomising the row order of each column in the environmental \
matrix and then obtaining the eigen value for the randomised matrix. \
This is repeatedly carried out for the amount of times specified by the user here.", // Description.

        1, // Not zero if the parameter has lower limit.
        1, // Parameter's lower limit.
        1, // Not zero if the parameter has upper limit.
        1000, // Parameter's upper limit.
        "8" // Parameter's typical (default) value.
      }
      ,
      {
        "StandardDeviations", // Id.
        "Number of standard deviations", // Name.
        Real, // Type.
        "The number of standard deviations added to the randomised eigen value.", //overview
        "When all the eigen values for the 'shuffled' environmental matrix have been summed \
this number of standard deviations is added to the mean of the eigen values. \
Any components whose eigen values are above this threshold are retained.", // Description.

        1, // Not zero if the parameter has lower limit.
        -10, // Parameter's lower limit.
        1, // Not zero if the parameter has upper limit.
        10, // Parameter's upper limit.
        "2.0" // Parameter's typical (default) value.
      }
      ,
      {
        "MinComponents", // Id.
        "Minimum number of components in model", // Name.
        Integer, // Type.
        "The minimum number of components that the model must have.", //overview
        "If not enough components are selected, the model produced will be erroneous or fail. \
Usually three or more components are acceptable", // Description.
        1, // Not zero if the parameter has lower limit.
        1, // Parameter's lower limit.
        1, // Not zero if the parameter has upper limit.
        20, // Parameter's upper limit.
        "1" // Parameter's typical (default) value.
      }
      ,
      {
        "VerboseDebugging", // Id.
        "Show very detailed debugging info", // Name.
        Integer, // Type.
        "Warning this will cause a large amount of information to be printed ", //overview
        "Set this to 1 to show extremely verbose diagnostics. \
Set this to 0 to disable verbose diagnostics (this is default behaviour).", // Description.
        1, // Not zero if the parameter has lower limit.
        0, // Parameter's lower limit.
        1, // Not zero if the parameter has upper limit.
        1, // Parameter's upper limit.
        "0" // Parameter's typical (default) value.
      }
    };


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

    "CSMBS",               // Id.
    "Climate Space Model", // Name.
    "0.4",                 // Version.

    "Climate Space Model [CSM] is a principle components based \
algorithm developed by Dr. Neil Caithness", //Overview

    "Climate Space Model [CSM] is a principle components based \
algorithm developed by Dr. Neil Caithness. The component \
selection process int this algorithm implementation is \
based on the Broken-Stick cutoff where any component with \
an eigenvalue less than (n stddevs above a randomised sample) is discarded. \n\
The original CSM was written as series of Matlab functions. ", //description

    "Neil Caithness",  // Author
    "Robertson M.P., Caithness N., Villet M.H. (2001) A PCA-based modelling technique for predicting environmental suitability for organisms from presence records. Diversity and Distributions, 7:15-27",  // Bibliography.

    "Tim Sutton, Renato De Giovanni",  // Code author.
    "t.sutton [at] reading.ac.uk",     // Code author's contact.

    0,  // Does not accept categorical data.
    0,  // Does not need (pseudo)absence points.

    NUM_PARAM,   // Algorithm's parameters.
    parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new CsmBS();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/****************************************************************/
/****************************** Csm *****************************/

/** Constructor for CsmBS
   * 
   * @param Sampler is class that will fetch environment variable values at each occurrence / locality
   */
CsmBS::CsmBS() : Csm(&metadata)
{
  _initialized = 0;
  //setup gsl random number generator
  const gsl_rng_type * myRngType;
  gsl_rng_env_setup();
  myRngType = gsl_rng_default;
  _randomNumberGenerator = gsl_rng_alloc(myRngType);
  //seed the random number generator
  gsl_rng_set(_randomNumberGenerator, (unsigned) time( NULL ));
}


/** This is the descructor for the Csm class */
CsmBS::~CsmBS()
{
  gsl_rng_free(_randomNumberGenerator);
}


int CsmBS::initialize()
{
  Log::instance()->debug( "Starting CSM - Broken Stick\n" );
  //set up parameters
  if ( ! getParameter( "Randomisations", &numberOfRandomisationsInt ) )
  {
    Log::instance()->warn( "Parameter Randomisations not set.\n");
    return 0;
  }
  if ( ! getParameter( "StandardDeviations", &numberOfStdDevsFloat) )
  {
    Log::instance()->warn( "Parameter StandardDeviations not set.\n");
    return 0;
  }
  if ( ! getParameter( "MinComponents", &minComponentsInt) )
  {
    Log::instance()->warn( "Parameter MinComponents not set.\n");
    return 0;
  }
  int myTempInt=0;
  if ( ! getParameter( "VerboseDebugging", &myTempInt) )
  {
    Log::instance()->warn( "Verbose debugging parameter not set.\n");
    return 0;
  }
  
  Log::instance()->debug( "Randomisations parameter set to: %d\n", numberOfRandomisationsInt );
  Log::instance()->debug( "StandardDeviations parameter set to: %.4f\n", numberOfStdDevsFloat );
  Log::instance()->debug( "MinComponents parameter set to: %d\n", minComponentsInt );

  if ( ! ( myTempInt == 0 || myTempInt == 1 ) )
  {
    Log::instance()->warn( "CSM - Broken Stick - Verbose debugging parameter out of range: %d\n",
                 myTempInt);
    return 0;
  }

  if (myTempInt!=0)
  {
    verboseDebuggingBool=true;
    Log::instance()->debug( "VerboseDebugging parameter set to: TRUE\n" );
  }
  else
  {
    verboseDebuggingBool=false;
    Log::instance()->debug( "VerboseDebugging parameter set to: FALSE \n" );
  }

  if ( numberOfRandomisationsInt <= 0 || numberOfRandomisationsInt > 1000 )
  {
    Log::instance()->warn( "CSM - Broken Stick - Randomisations parameter out of range: %f\n",
                numberOfRandomisationsInt );
    return 0;
  }
  if ( numberOfStdDevsFloat<= -10 || numberOfStdDevsFloat> 10 )
  {
    Log::instance()->warn( "CSM - Broken Stick - StandardDeviations parameter out of range: %f\n",
                numberOfRandomisationsInt );
    return 0;
  }
  if (minComponentsInt < 1 ||minComponentsInt > 20 )
  {
    Log::instance()->warn( "CSM - Broken Stick - MinComponents parameter out of range: %f\n",
                minComponentsInt);
    return 0;
  }

  //call the superclass initialier now...
  return Csm::initialize();
}

gsl_matrix * CsmBS::createRandomMatrix(int size1, int size2)
{

  //populate the matrix with random nos
  gsl_matrix * m = gsl_matrix_alloc (size1, size2);
  for (unsigned int j=0; j < m->size2; j++)
  {
    for (unsigned int k=0; k < m->size1; k++)
    {
      double myNo = gsl_rng_uniform_pos(_randomNumberGenerator);
      gsl_matrix_set(m,k,j,myNo);
    }
  }
  return m;
}

/* randomise the data within each column */
gsl_matrix * CsmBS::randomiseColumns(gsl_matrix * original_matrix)
{
  //for debugging only
  //gsl_matrix * myIndexMatrix = gsl_matrix_alloc (original_matrix->size1, original_matrix->size2);
  gsl_matrix * myOuputMatrix = gsl_matrix_alloc (original_matrix->size1, original_matrix->size2);
  gsl_matrix * m = createRandomMatrix (original_matrix->size1, original_matrix->size2);
  //loop through the matrix columns
  for (unsigned int j=0; j < m->size2; j++)
  {
    //get the column of random numbers
    gsl_vector * myRandomColumnVector = gsl_vector_alloc (m->size1);
    gsl_vector * myOriginalColumnVector = gsl_vector_alloc (original_matrix->size1);
    gsl_matrix_get_col (myRandomColumnVector, m, j);
    gsl_matrix_get_col (myOriginalColumnVector, original_matrix, j);
    gsl_permutation * myPermutation = gsl_permutation_alloc(m->size1);
    //compute the index positions of the sorted random col
    gsl_sort_vector_index(myPermutation,myRandomColumnVector);
    //assign values in the output array based on their index relative to the sorted
    //randomised matrix
    for (unsigned int k=0; k < m->size1; k++)
    {
      double myDouble = gsl_vector_get(myOriginalColumnVector,myPermutation->data[k]);
      //for debugging only
      //gsl_matrix_set(myIndexMatrix,k,j,myPermutation->data[k]);
      //write the elemnt straight into the output matrix
      gsl_matrix_set(myOuputMatrix,k,j,myDouble);
    } //k loop
    //set the output column to the randomly sorted column
    //clean up
    gsl_permutation_free (myPermutation);
    gsl_vector_free (myRandomColumnVector);
    gsl_vector_free (myOriginalColumnVector);
  }//j loop
  gsl_matrix_free(m);
  //for debuggin only
  //displayMatrix(myIndexMatrix,"Random matrix indexes");
  //gsl_matrix_free(myIndexMatrix);
  return myOuputMatrix;
}

int CsmBS::discardComponents()
{
  Log::instance()->debug( "Discarding components\n" );

  //create a matrix that will store the eigenvalue vector of each of the
  //the randomised environment variables we create
  gsl_matrix * myMatrixOfEigenValueVectors =
    gsl_matrix_alloc (numberOfRandomisationsInt,_gsl_environment_matrix->size2);
  Log::instance()->debug( "Calculating %i randomised matrices...\n", numberOfRandomisationsInt );
  for (int i=0; i<numberOfRandomisationsInt;i++)
  {
    Log::instance()->debug( "Calculating randomised matrix: %i\n", i );

    //
    //retreive centered and standardised environmental matrix & clone it
    //

    // displayMatrix(_gsl_environment_matrix,"Cloning centered and standardised matrix:");
    gsl_matrix * m = randomiseColumns(_gsl_environment_matrix);
    //displayMatrix(m,"Randomised matrix:");
    //
    //  build a cov matrix on the randomised environmental matrix
    //
    gsl_matrix * myGslCovarianceMatrix = autoCovariance(m);

    //  obtain eigenvalue and eigenvector on the new cov matrix
    gsl_vector * myGslEigenValueVector = gsl_vector_alloc (m->size2);
    gsl_matrix * myGslEigenVectorMatrix = gsl_matrix_alloc (m->size2, m->size2);
    //create a temporary workspace
    gsl_eigen_symmv_workspace * myWorkpace = gsl_eigen_symmv_alloc (m->size2);
    gsl_eigen_symmv (myGslCovarianceMatrix,
                     myGslEigenValueVector,
                     myGslEigenVectorMatrix,
                     myWorkpace);
    //free the temporary workspace again
    gsl_eigen_symmv_free (myWorkpace);
    displayVector(myGslEigenValueVector, "Randomised eigenvaluevector");
    //
    //   sort the eigenvalues from > to  < then discard eigenvector
    //
    gsl_eigen_symmv_sort (myGslEigenValueVector, myGslEigenVectorMatrix,
                          GSL_EIGEN_SORT_VAL_DESC);

    //  keep only the eigen values and add them as a new row to a matrix
    //  int gsl_matrix_set_row (gsl_matrix * m, size_t i, const gsl_vector * v)
    gsl_matrix_set_row (myMatrixOfEigenValueVectors,i,myGslEigenValueVector);

    // clean up temp vectors and matrix
    gsl_vector_free (myGslEigenValueVector);
    gsl_matrix_free(myGslEigenVectorMatrix);
    //  clear temp covariance matrix
    gsl_matrix_free(myGslCovarianceMatrix);
    gsl_matrix_free (m);
    //  repeat as many times as numberOfRandomisationsInt, ading a new row to the matrix each time
  }//i loop

  //  now calculate the mean and stddev of each col of output matrix
  //  note we must initialise these vectors before calling mean&stddev fn
  gsl_vector * myMeanVector = gsl_vector_alloc(myMatrixOfEigenValueVectors->size2);
  gsl_vector * myStdDevVector = gsl_vector_alloc(myMatrixOfEigenValueVectors->size2);;
  calculateMeanAndSd( myMatrixOfEigenValueVectors,myMeanVector,myStdDevVector);
  displayVector (myMeanVector,"Mean of randomised Eigen Vectors");
  //  in a new vector save the mean plus (numberOfStdDeviationsFloat * stddev)
  gsl_vector * myMeanPlusStdDevsVector = gsl_vector_alloc(myMeanVector->size);
  for (unsigned int i=0; i<myMeanVector->size; ++i)
  {
    double myMean = gsl_vector_get (myMeanVector,i);
    double myStdDev = gsl_vector_get (myStdDevVector,i);
    gsl_vector_set(myMeanPlusStdDevsVector,i,myMean+(myStdDev*numberOfStdDevsFloat));
  }
  displayVector (myMeanPlusStdDevsVector,"Mean of randomised Eigen Vectors plus std deviation");
  // First determine how many components we are going to keep
  // We do this by iterating through the eigenvalues, checking which are above the mean+stddev

  //
  // Note: as soon as the first non kept compoinent is encountered, all 
  // remaining components are discarded. This is because on 64bit
  // platform it can happen that some component scores at the 'small'
  // end of the components have a value above the random threshold
  // which is not desired.
  //
  
  double sumOfEigenValues = 0;//sum should total number of layers
  _retained_components_count=0;
  for (unsigned int i=0; i<myMeanPlusStdDevsVector->size; ++i)
  {
    double cmpValue = gsl_vector_get(myMeanPlusStdDevsVector,i);
    sumOfEigenValues += gsl_vector_get(_gsl_eigenvalue_vector,i);
    if (cmpValue < gsl_vector_get(_gsl_eigenvalue_vector,i))
    {
      ++_retained_components_count;
      //std::cerr << gsl_vector_get(_gsl_eigenvalue_vector,i) << " > " << cmpValue << ": Component "
      //  << i << " is greater than randomised component... retaining it." << std::endl;
    }
    else
    {
      break;
    }
  }
  //std::cerr << "Sum of eigenvalues is " << sumOfEigenValues << " (layer count is " << _layer_count  << ")\n";
  //std::cerr << "Difference between sum of eigenvalues and layer count = number of invariant layers" << std::endl;
  if (_retained_components_count < minComponentsInt)
  {
    Log::instance()->debug( "Only %i component(s) retained. %i required. \nAborting discard components routine\n",_retained_components_count, minComponentsInt );
    gsl_vector_free (myMeanVector);
    gsl_vector_free (myStdDevVector);
    gsl_vector_free (myMeanPlusStdDevsVector);
    gsl_matrix_free (myMatrixOfEigenValueVectors);
    return 0;
  }

  //  this vector is then used to discard component from the eigenvalue vector where
  //  the eigen value is greater than the randomised eigen value

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

  //now copy over just the components we intend to keep into the blank resized
  //_gsl_eigenvalue_vector and _gsl_eigenvector_matrix
  int myCurrentColInt=0;
  for (int j=0;j<_retained_components_count;j++)
  {
    double cmpValue = gsl_vector_get (tmp_gsl_eigenvalue_vector,j);
    if (cmpValue > gsl_vector_get (myMeanPlusStdDevsVector,j))
    {
      //Good this is a component we want to keep
      //...first copy over the vector value we are retaining
      gsl_vector_set (_gsl_eigenvalue_vector,myCurrentColInt,cmpValue);
      //...next copy over the associated matrix column for the component we are retaining
      gsl_vector * tmp_gsl_vector = gsl_vector_alloc (_layer_count);
      gsl_matrix_get_col (tmp_gsl_vector, tmp_gsl_eigenvector_matrix, j);
      gsl_matrix_set_col (_gsl_eigenvector_matrix,myCurrentColInt,tmp_gsl_vector);
      gsl_vector_free (tmp_gsl_vector);
      //increment the column counter
      myCurrentColInt++;
    }//if check for retained componnet
  }//j loop
  //now clear away the temporary vars
  gsl_vector_free (tmp_gsl_eigenvalue_vector);
  gsl_matrix_free (tmp_gsl_eigenvector_matrix);

  //clean up
  gsl_vector_free (myMeanVector);
  gsl_vector_free (myStdDevVector);
  gsl_vector_free (myMeanPlusStdDevsVector);
  gsl_matrix_free (myMatrixOfEigenValueVectors);

  displayVector( _gsl_eigenvalue_vector, "Vector of retained eigen values:");
  displayMatrix( _gsl_eigenvector_matrix,"Matrix of retained eigen vector:");

  Log::instance()->debug( "Completed CSM - Broken Stick\n" );
  Log::instance()->debug( "%i out of %i components retained \n", _retained_components_count, _layer_count );
  return 1;
}
