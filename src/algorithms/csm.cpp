//
// Csm
//
// Description:
//
//
// Author: CRIA <t.sutton@reading.ac.uk>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <string.h>
#include <cassert>
#include "csm.hh"

#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_multifit_nlin.h> //remove this when we have proper covar matrix
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sf_gamma.h>

#include <math.h>

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 0
static AlgParamMetadata *parameters = 0;

/****************************************************************/
/****************************** Csm *****************************/

/** Constructor for Csm
   * 
   * @param Sampler is class that will fetch environment variable values at each occurrence / locality
   */
Csm::Csm(AlgMetadata * metadata): Algorithm( metadata )
{
  _initialized = 0;
}


/** This is the descructor for the Csm class */
Csm::~Csm()
{
  if ( _initialized )
    {
      gsl_matrix_free (_gsl_environment_matrix);
      gsl_matrix_free (_gsl_covariance_matrix);
      gsl_vector_free (_gsl_avg_vector);
      gsl_vector_free (_gsl_stddev_vector);
      gsl_vector_free (_gsl_eigenvalue_vector);
      gsl_matrix_free (_gsl_eigenvector_matrix);
    }
}


//
// Methods used to build the model
//




/** Initialise the model specifying a threshold / cutoff point.
  * Any model definition building stuff is done here.
  * This is optional (model dependent).
  * @note This method is inherited from the Algorithm class
  * @param 
  * @return 0 on error
  */
int Csm::initialize()
{
    _initialized = 1;

    //set the class member that holds the number of environmental variables
    //we subtract 1 because the first column contains the specimen count
    _layer_count = _samp->numIndependent();
    //set the class member that holds the number of occurences
    _localityCount = _samp->numPresence();

    printf ("\n*************************************************\n");
    printf ("Csm::initialise - Csm Model Definition Commencing\n");
    printf ("\n*************************************************\n\n");
    //convert the sampler to a matrix and store in the local gsl_matrix
    printf ("\nConverting samples to GSL_Matrix\n");
    if (!SamplerToMatrix())
    {
        printf ("All occurences are outside the masked area!\n");
    }
    //show what we have calculated so far....
    //displayMatrix(_gsl_environment_matrix,"Environemntal Layer Samples");
    bool myFlag = csm1();
    if (myFlag)
    {
       printf("Model initialisation completed ok.!\n");
    }
    else
    {
       printf("Model initialisation failed! Aborting!\n");
      
    }
    return myFlag;
}
/** This is a utility function to convert the _sampl Sampler to a
  * gsl_matrix.
  * @return 0 on error
  */
int Csm::SamplerToMatrix()
{

    //create a samples container to hold the values retrieved
    //from the environmental layers at each point - this will be
    //converted to a gsl structure in the next step
    //_samp is defined in the Algorithm class from which Csm inherits
    SampledData localities;
    if ( ! _samp->getPresence(&localities, _localityCount))
      return 0;
    _localityCount = localities.numSamples();

    // Allocate the gsl matrix to store environment data at each locality
    _gsl_environment_matrix = gsl_matrix_alloc (_localityCount, _layer_count);
    // now populate the gsl matrix from the sample data
    for (int i=0;i<_localityCount;++i)
    {
        for (int j=0;j<_layer_count;j++)
        {
            //we add one to j in order to omit the specimen count column
            float myCellValue = localities.getIndependent(i,j);
            gsl_matrix_set (_gsl_environment_matrix,i,j,myCellValue);
        }
    }
    return 1;
}



/** NOTE: the mean and stddev vectors MUST be pre-initialised! */
int Csm::calculateMeanAndSd(gsl_matrix * theMatrix, 
                            gsl_vector * theMeanVector,
                            gsl_vector * theStdDevVector)
{
    assert (theMatrix != 0);
    assert (theMeanVector !=0);
    assert (theStdDevVector !=0);
    //Initialise the vector to hold the mean of each column
    gsl_vector_set_zero(theMeanVector);

    //Initialise the vector to hold the stddev of each column
    gsl_vector_set_zero(theStdDevVector);

    //printf ("Memory location of theMeanVector is %X\n", &theMeanVector);
    //printf ("Memory location of gsl_vector_set_zero is %X\n", &gsl_vector_set_zero);
    //printf ("Memory location of _gsl_avg_vector is %X\n", &_gsl_avg_vector);
    //printf ("Memory location of _gsl_stddev_vector is %X\n", &_gsl_stddev_vector);

    //calculate the mean  and stddev of each column
    for (int j = 0; j < _layer_count; j++)
    {
        //get the current column from the array as a vector
        gsl_vector_view myColumn = gsl_matrix_column (theMatrix, j);
        //calculate the average for the column ...
        double myAverage = gsl_stats_mean (myColumn.vector.data, myColumn.vector.stride, myColumn.vector.size);
        // ...and assign it to the jth element in the column means vector
        gsl_vector_set (theMeanVector,j,myAverage);
        //calculate the stddev for the column and ...
        double myStdDev = gsl_stats_sd (myColumn.vector.data, myColumn.vector.stride, myColumn.vector.size);
        // ...and assign it to the jth element in the column stddev vector
        gsl_vector_set (theStdDevVector,j,myStdDev);
    }
    //displayVector(theMeanVector,"Average vector - theMeanVector");
    //displayVector(theStdDevVector,"Standard Deviation vector - theStdDevVector");
    //displayVector(_gsl_avg_vector,"Average vector - _gsl_avg_vector");
    //displayVector(_gsl_stddev_vector,"Standard Deviation vector - _gsl_stddev_vector");

    return 0;
}

int Csm::center()
{
    //
    //Subtract the column mean from every value in each column
    //Divide each resultant column value by the stddev for that column
    //Note that we are walking the matrix column wise
    //
    printf ("Centering data ...\n");
    for (int j=0;j<_layer_count;j++)
    {
        //get the stddev and mean for this column
        float myAverage = gsl_vector_get (_gsl_avg_vector,j);
        float myStdDev = gsl_vector_get (_gsl_stddev_vector,j);

        for (int i=0;i<_localityCount;++i)
        {
            double myDouble = gsl_matrix_get (_gsl_environment_matrix,i,j);
            myDouble = (myDouble-myAverage)/myStdDev;
            //update the gsl_matrix with the new value
            gsl_matrix_set(_gsl_environment_matrix,i,j,myDouble);
        }
    }
}


/** Start model execution (build the model).
  * @note This method is inherited from the Algorithm class
  * @return 0 on error 
  */
int Csm::iterate()
{
    _done=1;
    return 1;
}


/** Use this method to find out if the model has completed (e.g. convergence
  * point has been met. 
  * @note This method is inherited from the Algorithm class
  * @return     
  * @return Implementation specific but usually 1 for completion.
  */
int Csm::done()
{
    return _done;
}


//
// Methods used to project the model
//


/** This method is used when projecting the model.
  * @note This method is inherited from the Algorithm class
  * @return Scalar of the probablitiy of occurence must be between 0 and 1  
  * @param Scalar *x a pointer to a vector of openModeller Scalar type (currently double). The vector should contain values looked up on the environmental variable layers into which the mode is being projected. */
Scalar Csm::getValue( Scalar *x )
{
    float myFloat;
    bool myAllAreZeroFlag=true;
    //first thing we do is convert the oM primitive env value array to a gsl matrix
    //with only one row so we can do matrix multplication with it
    gsl_matrix * tmp_gsl_matrix = gsl_matrix_alloc (1,_layer_count);
    for (int i=0;i<_layer_count;++i)
    {
        myFloat = static_cast<float>(x[i]);
        if (myFloat!=0) 
        { 
          myAllAreZeroFlag=false; 
        }
        //printf("%f ",myFloat);
        //get the stddev and mean for this column
        float myAverage = gsl_vector_get (_gsl_avg_vector,i);
        float myStdDev = gsl_vector_get (_gsl_stddev_vector,i);
        //subtract the mean from the value then divide by the standard deviation
        myFloat = (myFloat-myAverage)/myStdDev;
        //assign the result to our vector
        gsl_matrix_set (tmp_gsl_matrix,0,i,myFloat);
        //printf ("%f\t", myFloat );
    }
    //printf(" ----  end of scalar\n ");
    //displayMatrix(tmp_gsl_matrix,"tmp_gsl_matrix before matrix multiplication");
    //if (myAllAreZeroFlag) {return 0;}
    

    gsl_matrix * z = product(tmp_gsl_matrix, _gsl_eigenvector_matrix);

    // z should match the dimensions of tmp_gsl_matrix so do some error checking
    if (z->size1 != tmp_gsl_matrix->size1)
    {
      printf("Error during creation of product Z in CSM getValue - number of rows dont match\n");
      exit(0);
    }

    // number of cols in z should == number of components
    if (z->size2 != tmp_gsl_matrix->size1)
    {
      //printf("Error during creation of product Z in CSM getValue - number of cols dont match number of components\n");
      //exit(0);
    }

    //displayMatrix(z,"z ");
    // now we standardise the values in z
    // we do this by dividing each element in z by the square root of its associated element in 
    // the eigenvalues vector

    for (int i=0;i<z->size2;i++)
    {
      gsl_matrix_set(z,0,i,gsl_matrix_get (z,0,i)/sqrt(gsl_vector_get(_gsl_eigenvalue_vector,i)));
    }
    //displayMatrix(z,"After standardising z");
    // now we square each element and sum them    
    myFloat=0;
    for (int i=0;i<z->size2;i++)
    {
      float myValue=gsl_matrix_get (z,0,i);
      if (!isnan(myValue))
      {
        myFloat+= pow(gsl_matrix_get (z,0,i), 2); 
        //Warning uncommenting the next line will spew a lot of stuff to stdout!!!!
        //printf ("myValue : %f Cumulative : %f\n",myValue,myFloat);
      }
    }       
    
 
    //now work out the probability of myFloat between 0 and 1 
    double myHalfComponentCountDouble=(z->size2)/2;
    double myHalfSumOfSquaresDouble=myFloat/2;
    //printf ("Component count %f , Half sum of squares %f ,",myHalfComponentCountDouble,myHalfSumOfSquaresDouble);
    myFloat=1-gsl_sf_gamma_inc_Q (myHalfSumOfSquaresDouble,myHalfComponentCountDouble);
 
    printf ("Prob: %f \r",myFloat);
    //now we
    //now clear away the temporary vars
    gsl_matrix_free (z);
    //gsl_vector_free (component1_gsl_vector);
    gsl_matrix_free (tmp_gsl_matrix);

    return myFloat;
}

/** Returns a value that represents the convergence of the algorithm
  * expressed as a number between 0 and 1 where 0 represents model
  * completion. 
  * @return 
  * @param Scalar *val 
*/
int Csm::getConvergence( Scalar *val )
{}

//
// General Helper Methods
//


void Csm::displayVector(gsl_vector * v, char * name)
{
    printf ("\n Displaying Vector %s (%i): \n----------------------------------------------\n[ ", name, v->size);
    for (int i=0;i<v->size;++i)
    {
        char sep1[] = ",";

        double myDouble = gsl_vector_get (v,i);


        printf ("%g %s ", myDouble, sep1);
    }
    printf ("]\n----------------------------------------------\n ");
}


/**********************/
/**** displayMatrix ***/
void Csm::displayMatrix(gsl_matrix * m, char * name)
{
    printf ("\n Displaying Matrix %s (%i / %i): \n----------------------------------------------\n[ ", name, m->size1, m->size2);
    for (int i=0;i<m->size1;++i)
    {
        char sep1[] = ",";
        char sep2[] = ";";

        for (int j=0;j<m->size2;j++)
        {
            double myDouble = gsl_matrix_get (m,i,j);

            if (j == m->size2 -1)
                strcpy(sep1, "");

            printf ("%g %s ", myDouble, sep1);
        }

        if (i == m->size1 -1)
            strcpy(sep2, "");

        printf ("%s\n", sep2);
    }
    printf ("]\n----------------------------------------------\n ");
}


/******************/
/**** transpose ***/
gsl_matrix * Csm::transpose (gsl_matrix * m)
{
    gsl_matrix * t = gsl_matrix_alloc (m->size2, m->size1);

    for (int i = 0; i < m->size1; i++)
    {
        gsl_vector * v = gsl_vector_alloc(m->size2);
        gsl_matrix_get_row (v, m, i);
        gsl_matrix_set_col (t, i, v);
    }

    return t;
}

/************************/
/**** Vectors product ***/
double Csm::product (gsl_vector * va, gsl_vector * vb)
{
    // fix me: need to check if vectors are of the same size !!!

    double res = 0.0;

    for (int i = 0; i < va->size; i++)
    {
        res += gsl_vector_get(va, i)*gsl_vector_get(vb, i);
    }

    return res;
}

/*************************/
/**** Matrices product ***/
gsl_matrix * Csm::product (gsl_matrix * a, gsl_matrix * b)
{
    // fix me: need to check if a->size2 is equal to b->size1 !!!

    gsl_matrix * p = gsl_matrix_alloc (a->size1, b->size2);

    for (int i = 0; i < a->size1; i++)
    {
        gsl_vector * va = gsl_vector_alloc(a->size2);

        gsl_matrix_get_row (va, a, i);

        for (int j = 0; j < b->size2; j++)
        {
            gsl_vector * vb = gsl_vector_alloc(a->size2);

            gsl_matrix_get_col (vb, b, j);

            double vp = product(va, vb);

            gsl_matrix_set (p, i, j, vp);

	    gsl_vector_free (vb);
        }

	gsl_vector_free (va);
    }

    return p;
}

/***********************/
/**** autoCovariance ***/
/**
 
This method tries to mimic the octave "cov" function when it
receives only one parameter:
 
function c = cov (x)
 
  if (rows (x) == 1)
    x = x';
  endif
 
  n = rows (x);
 
  x = x - ones (n, 1) * sum (x) / n;
  c = conj (x' * x / (n - 1));
 
endfunction
 
*/
gsl_matrix * Csm::autoCovariance(gsl_matrix * original_matrix)
{
    // Build a copy of the input matrix to work with
    gsl_matrix * m = gsl_matrix_alloc (original_matrix->size1, original_matrix->size2);
    gsl_matrix_memcpy (m, original_matrix);

    int numrows = m->size1;
    int numcols = m->size2;

    if (numrows == 1)
    {
        m = transpose(m);
    }

    // compute: ones (n, 1) * sum (x)
    gsl_matrix * s = gsl_matrix_alloc (numrows, numcols);

    if (numrows == 1)
    {
        gsl_matrix_memcpy (m, s);
    }
    else
    {
        gsl_vector * v = gsl_vector_alloc(numcols);

        for (int i = 0; i < numcols; i++)
        {
            double val = 0.0;

            for (int j = 0; j < numrows; j++)
            {
                val += gsl_matrix_get (m, j, i);
            }

            gsl_vector_set (v, i, val);

            for (int j = 0; j < numrows; j++)
            {
                gsl_matrix_set_row (s, j, v);
            }
        }
	gsl_vector_free(v);
    }

    // divide by "n"
    gsl_matrix_scale (s, (double)1/numrows);

    // subtract the result from x
    gsl_matrix_sub (m, s);

    // get x'
    gsl_matrix * mt = transpose(m);

    // x / (n - 1)
    gsl_matrix_scale (m, (double)1/(numrows-1));

    // multiply by x'
    gsl_matrix * p = product(mt, m);

    gsl_matrix_free (mt);
    gsl_matrix_free (m);
    gsl_matrix_free (s);

    return p;
}

/** Csm1 is used to produce the model definition */
bool Csm::csm1()
{
  //calculate the mean and std deviation
  printf ("Calculating mean and stddev\n");
  _gsl_avg_vector = gsl_vector_alloc (_gsl_environment_matrix->size2);
  _gsl_stddev_vector = gsl_vector_alloc (_gsl_environment_matrix->size2) ;
  calculateMeanAndSd(_gsl_environment_matrix,_gsl_avg_vector,_gsl_stddev_vector);
  //displayVector(_gsl_avg_vector,"Average vector");
  //displayVector(_gsl_stddev_vector,"Standard Deviation vector");
  //center and standardise the data
  printf ("Centering and standardising\n");
  center();
  //show what we have calculated so far....
  // displayMatrix(_gsl_environment_matrix,"Environemntal Layer Samples (after centering)");

  //Now calculate the covariance matrix:
  printf ("Calculating covariance matrix");
  _gsl_covariance_matrix = autoCovariance(_gsl_environment_matrix);
  //the rows and columns in the cavariance matrix should be equal, otherwise abort
  if (_gsl_covariance_matrix->size1 != _gsl_covariance_matrix->size2)
  {
    printf ("\n\n\nCsm :: A critical error has occurred - cavariance matrix is not square...aborting\n\n\n");
    return 0;
  }
  //and display the result...
  //displayMatrix(_gsl_covariance_matrix,"Covariance Matrix");

  //now compute the eigen value and vector
  printf("Calculating eigenvalue and eigenvector");
  _gsl_eigenvalue_vector = gsl_vector_alloc (_layer_count);
  _gsl_eigenvector_matrix = gsl_matrix_alloc (_layer_count, _layer_count);
  //create a temporary workspace
  gsl_eigen_symmv_workspace * myWorkpace = gsl_eigen_symmv_alloc (_layer_count);
  gsl_eigen_symmv (_gsl_covariance_matrix,
          _gsl_eigenvalue_vector,
          _gsl_eigenvector_matrix,
          myWorkpace);
  //free the temporary workspace again
  gsl_eigen_symmv_free (myWorkpace);
  //Initialise the retained components count (to be used further down and in displayEigen())
  _retained_components_count = _layer_count;
  //show the eigen before sorting
  //printf ("\n\nBefore sorting : \n");
  //displayVector(_gsl_eigenvalue_vector,"Eigen Values");
  //displayMatrix(_gsl_eigenvector_matrix,"Eigen Vector");
  //sort the eigen vector by the eigen values (in descending order)
  gsl_eigen_symmv_sort (_gsl_eigenvalue_vector, _gsl_eigenvector_matrix,
          GSL_EIGEN_SORT_VAL_DESC);
  //print out the result
  printf ("\n\nEigenvector sorted : \n");
  //displayVector(_gsl_eigenvalue_vector,"Eigen Values");
  //displayMatrix(_gsl_eigenvector_matrix,"Eigen Vector");
  //displayVector(_gsl_eigenvalue_vector,"Eigen Values");
  //displayMatrix(_gsl_eigenvector_matrix,"Eigen Vector");
  printf ("\n*************************************************\n");
  printf ("        CSM Model Generation Completed ");
  printf ("\n*************************************************\n");

  //After the mode is generated, we can discard unwanted components!
  int myNumberOfAttempts=0;
  while ( myNumberOfAttempts++ < maxAttemptsInt)
  {
    if (discardComponents())
    {
      printf ("\n\nUnwanted components discarded \n");
      return true;
    }
    else
    {
      printf ("Discard components retained to few components - trying again!\n");
    }
  }
  printf ("Error could not generate a model with sufficient components!\n");
  return false;
  //print out the result
}
