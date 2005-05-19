//
// CsmBS
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
#include <iostream>
#include "csmbs.hh"

#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_multifit_nlin.h> //remove this when we have proper covar matrix
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sf_gamma.h>

#include <math.h>
//next includes needed for rand 
#include <stdlib.h>
#include <time.h>
/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 5


/************************************/
/*** Algorithm parameter metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    "Randomisations",          // Id.
    "Number of random eigenvalues",        // Name.
    "Integer",        // Type.
    "The number of eigenvalues to generate from randomly 'shuffled' environment data.", //overview
    "The Broken Stick method of selecting the number of components to keep \n\
is carried out by randomising the row order of each column in the environmental \n\
matrix and then obtaining the eigen value for the randomised matrix. \n\
This is repeatedly carried out for the amount of times specified by the user here.", // Description.

    1,     // Not zero if the parameter has lower limit.
    1,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    1000,   // Parameter's upper limit.
    "8"  // Parameter's typical (default) value.
  },
  
  {
    "RandomiserRepeats",          // Id.
    "Number of shuffles per column",        // Name.
    "Integer",        // Type.
    "Increase this number to increase randomness of data used for component selection eigen vector\n", //overview
    "How many times each column should be shuffled to ensure randomness when generating \n\
the randomised eigen vector used in the broken stick component selection method.\n\
Note that increasing this parameter will cause models to take longer to run.\n", // Description.
    1,     // Not zero if the parameter has lower limit.
    1,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    1000,   // Parameter's upper limit.
    "50"  // Parameter's typical (default) value.
  }
  ,
  {
    "StandardDeviations",          // Id.
    "Number of standard deviations",        // Name.
    "Real",        // Type.
    "The number of standard deviations added to the randomised eigen value.", //overview
    "When all the eigen values for the 'shuffled' environmental matrix have been summed \n\
this number of standard deviations is added to the mean of the eigen values. \n\
Any components whose eigen values are above this threshold are retained.", // Description.

    1,     // Not zero if the parameter has lower limit.
    -10,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    10,   // Parameter's upper limit.
    "0.05"  // Parameter's typical (default) value.
  }
  ,
  {
    "MinComponents",          // Id.
    "Minimum number of components in model",        // Name.
    "Integer",        // Type.
    "The minimum number of components that the model must have.", //overview
    "If not enough components are selected, the model produced will be erroneous or fail. \
Usually three or more components are acceptable", // Description.
    1,     // Not zero if the parameter has lower limit.
    2,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    20,   // Parameter's upper limit.
    "2"  // Parameter's typical (default) value.
  }
  ,
  {
    "MaxAttempts",          // Id.
    "How many attempts wil be made",        // Name.
    "Integer",        // Type.
    "If not enough components are selected, the model will retry this many times", //overview
    "If not enough components are selected, the model be rerun. If MaxAttempts is reached \
Csm Broken Stick will give up and abort.", // Description.
    1,     // Not zero if the parameter has lower limit.
    1,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    10,   // Parameter's upper limit.
    "1"  // Parameter's typical (default) value.
  }
};


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "CSMBS",                 // Id.
  "Climate Space Model", // Name.
  "0.2 beta",           // Version.

  
  "Climate Space Model [CSM] is a principle components based \
algorithm developed by Dr. Neil Caithness",//Overview

  "Climate Space Model [CSM] is a principle components based \
algorithm developed by Dr. Neil Caithness. The component \
selection process int this algorithm implementation is \
based on the Broken-Stick cutoff where any component with \
an eigenvalue < (n stddevs above a randomised sample) is discarded.\
\n\
The original CSM was written as series of Matlab functions. ", //description

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
}


/** This is the descructor for the Csm class */
CsmBS::~CsmBS()
{
}

int CsmBS::initialize()
{
  g_log.debug( "Starting CSM - Broken Stick\n" );
  //set up parameters
  if ( ! getParameter( "Randomisations", &numberOfRandomisationsInt ) )
  {
    g_log.warn( "Parameter Randomisations not set properly.\n");
    return 0;
  }
  if ( ! getParameter( "StandardDeviations", &numberOfStdDevsFloat) )
  {
    g_log.warn( "Parameter StandardDeviations not set properly.\n");
    return 0;
  }
  if ( ! getParameter( "RandomiserRepeats", &randomiserRepeatsInt) )
  {
    g_log.warn( "Parameter RandomiserRepeats not set properly.\n");
    return 0;
  }
  if ( ! getParameter( "MinComponents", &minComponentsInt) )
  {
    g_log.warn( "Parameter MinComponents not set properly.\n");
    return 0;
  }
  if ( ! getParameter( "MaxAttempts", &maxAttemptsInt) )
  {
    g_log.warn( "Parameter MaxAttempts not set properly.\n");
    return 0;
  }
  g_log.debug( "Randomisations parameter set to: %d\n", numberOfRandomisationsInt );
  g_log.debug( "RandomiserRepeats parameter set to: %d\n", randomiserRepeatsInt );
  g_log.debug( "StandardDeviations parameter set to: %.4f\n", numberOfStdDevsFloat );
  g_log.debug( "MinComponents parameter set to: %d\n", minComponentsInt );
  g_log.debug( "MaxAttempts parameter set to: %d\n", maxAttemptsInt );
  
  if ( numberOfRandomisationsInt <= 0 || numberOfRandomisationsInt > 1000 )
  {
    g_log.warn( "CSM - Broken Stick - Randomisations parameter out of range: %f\n",
            numberOfRandomisationsInt );
    return 0;
  }
  if ( numberOfStdDevsFloat<= -10 || numberOfStdDevsFloat> 10 )
  {
    g_log.warn( "CSM - Broken Stick - StandardDeviations parameter out of range: %f\n",
            numberOfRandomisationsInt );
    return 0;
  }
  if ( randomiserRepeatsInt< 1 || randomiserRepeatsInt> 1000 )
  {
    g_log.warn( "CSM - Broken Stick - RandomiserRepeats parameter out of range: %f\n",
            randomiserRepeatsInt);
    return 0;
  }
  if (minComponentsInt < 1 ||minComponentsInt > 20 )
  {
    g_log.warn( "CSM - Broken Stick - MinComponents parameter out of range: %f\n",
            minComponentsInt);
    return 0;
  }
  if ( maxAttemptsInt< 1 || maxAttemptsInt> 10 )
  {
    g_log.warn( "CSM - Broken Stick - MaxAttempts parameter out of range: %f\n",
            maxAttemptsInt);
    return 0;
  }

  //call the superclass initialier now...
  return Csm::initialize();
}
int CsmBS::discardComponents()
{
  int i, j;

  g_log.debug( "Discarding components\n" );

  //create a matrix that will store the eigenvalue vector of each of the
  //the randomised environment variables we create
  gsl_matrix * myMatrixOfEigenValueVectors = 
      gsl_matrix_alloc (numberOfRandomisationsInt,_gsl_environment_matrix->size2);
  g_log.debug( "Calculating %i randomised matrices...\n", numberOfRandomisationsInt );
  for (i=0; i<numberOfRandomisationsInt;i++)
  {
     g_log.debug( "Calculating randomised matrix: %i\n", i );

    //    
    //retreive centered and standardised environmental matrix & clone it
    //

    // displayMatrix(_gsl_environment_matrix,"Cloning centered and standardised matrix:");
    gsl_matrix * m = gsl_matrix_alloc (_gsl_environment_matrix->size1, _gsl_environment_matrix->size2);
    gsl_matrix_memcpy (m,_gsl_environment_matrix );
    int myRandUpperBoundInt=m->size1; //number of rows
    time_t mySeconds;
    //loop through the matrix columns
    for (j=0; j < m->size2; j++)
    {
      // retrieve this column as a gsl_vector : 
      // int gsl_matrix_get_col (gsl_vector * v, const gsl_matrix * m, size_t j)
      // This function copies the elements of the j-th column of the matrix m into the vector v. 
      // The length of the vector must be the same as the length of the column.

      gsl_vector * myColumnVector = gsl_vector_alloc (myRandUpperBoundInt);
      gsl_matrix_get_col (myColumnVector, m, j);


      //
      //             shuffle / randomise the row ordering in each column
      //
      //
      //  -------------------------------------------------------------------------------------        
      //  -------------------------------------------------------------------------------------        
      //  -------------------------------------------------------------------------------------        
      //  e.g. before:      e.g. after
      //  33 | 56 | 88  |   12| 12 | 34
      //  34 | 12 | 63  |   34 | 44 | 63
      //  12 | 44 | 34  |   33 | 56 | 88
      //  -------------------------------------------------------------------------------------        
      //  Exchanging elements (GSL Documentation)
      //
      //  The following function can be used to exchange, or permute, the elements of a vector.
      //  Function: int gsl_vector_swap_elements (gsl_vector * v, size_t i, size_t j)
      //  This function exchanges the i-th and j-th elements of the vector v in-place. 
      //  Function: int gsl_vector_reverse (gsl_vector * v)
      //  This function reverses the order of the elements of the vector v. 
      //  -------------------------------------------------------------------------------------        
      //  Approach (based on chat with Mauro on irc):
      //  You can call this method (gsl swap) in a loop for k = 0,...,N and r = random between 0 and N...
      //  so you are saying always swap the kth element with rth element?
      //  thats sounds good 

      //this extra loop is to increase the amount of shuffling that takes place!  
      for (int myRandomiserRepeats=0;myRandomiserRepeats<randomiserRepeatsInt;myRandomiserRepeats++)
      {
        //loop through each cell in the column swapping it with another cell
        for (int k=0; k < m->size1; k++)
        {
          time(&mySeconds); //get time from sys clock
          srand((unsigned int) mySeconds); //seed randomiser
          int myRandomRowInt = rand() % (myRandUpperBoundInt-1) ; //get random number from 0 to row count
          //now do the swap
          gsl_vector_swap_elements (myColumnVector,k,myRandomRowInt);
        } //k loop
      }//myRandomiserRepeats loop
      //replace the column with the new randomised column
      gsl_matrix_set_col(m,j,myColumnVector);
      //clean up
      gsl_vector_free (myColumnVector);
    }//j loop
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
    gsl_matrix_free (m); 
    //  repeat as many times as numberOfRandomisationsInt, ading a new row to the matrix each time
  }//i loop

  //  now calculate the mean and stddev of each col of output matrix
  //  note we must initialise these vectors before calling mean&stddev fn
  gsl_vector * myMeanVector = gsl_vector_alloc(myMatrixOfEigenValueVectors->size2);
  gsl_vector * myStdDevVector = gsl_vector_alloc(myMatrixOfEigenValueVectors->size2);;
  calculateMeanAndSd( myMatrixOfEigenValueVectors,myMeanVector,myStdDevVector);

  //  in a new vector save the mean plus (numberOfStdDeviationsFloat * stddev)
  gsl_vector * myMeanPlusStdDevsVector = gsl_vector_alloc(myMeanVector->size);
  for (i=0; i<myMeanVector->size; ++i)
  {
    double myMean = gsl_vector_get (myMeanVector,i);
    double myStdDev = gsl_vector_get (myStdDevVector,i);
    gsl_vector_set(myMeanPlusStdDevsVector,i,myMean+(myStdDev*numberOfStdDevsFloat));
  }

  // First determine how many components we are going to keep
  // We do this by iterating through the eigenvalues, checking which are above the mean+stddev

  float sumOfEigenValues = 0;//sum should total number of layers
  for (i=0; i<myMeanPlusStdDevsVector->size; ++i)
  {
    float myFloat = gsl_vector_get(myMeanPlusStdDevsVector,i); 
    sumOfEigenValues += myFloat; 
    if (myFloat < gsl_vector_get(_gsl_eigenvalue_vector,i))
    {
      std::cerr << gsl_vector_get(_gsl_eigenvalue_vector,i) 
          << " > "
          << myFloat
          << ": Component "
          << i
          << " is greater than randomised component... retaining it."
          << std::endl;
    }
    else
    {
      --_retained_components_count;
      std::cerr << gsl_vector_get(_gsl_eigenvalue_vector,i) 
          << " < "
          << myFloat
          << ": Component "
          << i
          << " is less than randomised component... discarding it."
          << std::endl;
    }
  }
  std::cerr << "Sum of eigenvalues is "  
            << sumOfEigenValues
            << "(should be "
            << _layer_count
            << ")\n";
  std::cerr << "Small differences are acceptable here." << std::endl;
  //it seems we need at least 4 components to produce a decent model
  if (_retained_components_count < minComponentsInt) 
  {
    g_log.debug( "Only %i component(s) retained. %i required. \nAborting discard components routine\n",_retained_components_count, minComponentsInt );
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
  for (j=0;j<_retained_components_count;j++)
  {
    float myFloat = gsl_vector_get (tmp_gsl_eigenvalue_vector,j);
    if (myFloat > gsl_vector_get (myMeanPlusStdDevsVector,j))
    {
      //Good this is a component we want to keep
      //...first copy over the vector value we are retaining
      gsl_vector_set (_gsl_eigenvalue_vector,myCurrentColInt,myFloat);
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

  g_log.debug( "Completed CSM - Broken Stick\n" );
  g_log.debug( "%i out of %i components retained \n", _retained_components_count, _layer_count );
  return 1;
}
