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
#include "csm.hh"

#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_multifit_nlin.h> //remove this when we have proper covar matrix
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>

/** Constructor for Csm
   * 
   * @param Sampler is class that will fetch environment variable values at each occurrence / locality
   */
Csm::Csm( Sampler *samp ): Algorithm( samp )
{
    //set the class member that holds the number of environmental variables
    //we subtract 1 because the first column contains the specimen count
    f_layer_count = samp->dim()-1;
    //set the class member that holds the number of occurences
    f_localityCount = samp->numOccurrences();
    //convert the sampler to a matrix and store in the local gsl_matrix
    if (!SamplerToMatrix(samp))
    {
        printf ("All occurences are outside the masked area!\n");
    }
}


/** This is the descructor for the Csm class */
Csm::~Csm()
{
  gsl_matrix_free (f_gsl_environment_matrix);
  gsl_matrix_free (f_gsl_covariance_matrix);
  gsl_vector_free (f_gsl_avg_vector);
  gsl_vector_free (f_gsl_stddev_vector);
  gsl_vector_free (f_gsl_eigenvalue_vector);
  gsl_matrix_free (f_gsl_eigenvector_matrix);
}

/** This is a utility function to convert a Sampler to a gsl_matrix.
  * @return 0 on error
  */
int Csm::SamplerToMatrix(Sampler *samp)
{
    //Initialise the vector to hold the mean of each column
    f_gsl_avg_vector = gsl_vector_alloc (f_layer_count);
    gsl_vector_set_zero(f_gsl_avg_vector);

    //Initialise the vector to hold the stddev of each column
    f_gsl_stddev_vector = gsl_vector_alloc (f_layer_count) ;
    gsl_vector_set_zero(f_gsl_stddev_vector);

    //create a samples container to hold the values retrieved
    //from the environmental layers at each point - this will be
    //converted to a gsl structure in the next step
    Samples mySamples (f_localityCount,f_layer_count);
    if (!samp->getOccurrences(f_localityCount, &mySamples))
    {
        return 0;
    }

    // Allocate the gsl matrix to store environment data at each locality
    f_gsl_environment_matrix = gsl_matrix_alloc (f_localityCount, f_layer_count);
    // now populate the gsl matrix from the sample data
    for (int i=0;i<f_localityCount;++i)
    {
        for (int j=0;j<f_layer_count;j++)
        {
            //we add one to j in order to omit the specimen count column
            float myCellValue = mySamples(i,j+1);
            gsl_matrix_set (f_gsl_environment_matrix,i,j,myCellValue);
        }
    }

    //calculate the mean  and stddev of each column
    for (int j = 0; j < f_layer_count; j++)
    {
        //get the current column from the array as a vector
        gsl_vector_view myColumn = gsl_matrix_column (f_gsl_environment_matrix, j);
        //calculate the average for the column ...
        double myAverage = gsl_stats_mean (myColumn.vector.data, myColumn.vector.stride, myColumn.vector.size);
        // ...and assign it to the jth element in the column means vector
        gsl_vector_set (f_gsl_avg_vector,j,myAverage);
        //calculate the stddev for the column and ...
        double myStdDev = gsl_stats_sd (myColumn.vector.data, myColumn.vector.stride, myColumn.vector.size);
        // ...and assign it to the jth element in the column stddev vector
        gsl_vector_set (f_gsl_stddev_vector,j,myStdDev);
    }
    //show what we have calculated so far....
    displaySamples();
    //
    //Subtract the column mean from every value in each column
    //Divide each resultant column value by the stddev for that column
    //Note that we are walking the matrix column wise
    //
    printf (" Centering data ...\n");
    for (int j=0;j<f_layer_count;j++)
    {
        //get the stddev and mean for this column
        float myAverage = gsl_vector_get (f_gsl_avg_vector,j);
	float myStdDev = gsl_vector_get (f_gsl_stddev_vector,j);
	
	for (int i=0;i<f_localityCount;++i)
        {
            double myDouble = gsl_matrix_get (f_gsl_environment_matrix,i,j);
	    myDouble = (myDouble-myAverage)/myStdDev;
            //update the gsl_matrix with the new value
	    gsl_matrix_set(f_gsl_environment_matrix,i,j,myDouble);
        }
    }
    
    //show what we have calculated so far....
    displaySamples();

    //Now calculate the covariance matrix:
    f_gsl_covariance_matrix = gsl_matrix_alloc (f_layer_count, f_layer_count);
    //gsl_multifit_covar (f_gsl_environment_matrix, 0.0, f_gsl_covariance_matrix);
    gsl_stats_covariance (f_gsl_environment_matrix->data, 
                          sizeof(float), 
			  f_gsl_environment_matrix->data, 
                          sizeof(float), 
			  (f_gsl_environment_matrix->size1 * f_gsl_environment_matrix->size1));
    //and display the result...
    displayCovarianceMatrix();
    
    //now compute the eigen value and vector
    f_gsl_eigenvalue_vector = gsl_vector_alloc (f_layer_count);
    f_gsl_eigenvector_matrix = gsl_matrix_alloc (f_layer_count, f_layer_count);
    //create a temporary workspace
    gsl_eigen_symmv_workspace * myWorkpace = gsl_eigen_symmv_alloc (f_layer_count);
    gsl_eigen_symmv (f_gsl_covariance_matrix, 
                     f_gsl_eigenvalue_vector, 
		     f_gsl_eigenvector_matrix, 
		     myWorkpace);
    //free the temporary workspace again
    gsl_eigen_symmv_free (myWorkpace);
    //sort the eigen vector by the eigen values (in descending order)
    gsl_eigen_symmv_sort (f_gsl_eigenvalue_vector, f_gsl_eigenvector_matrix,
                        GSL_EIGEN_SORT_ABS_DESC);
    //print out the result
    displayEigen();
    
    //the sum of the eigen values should be equal to the number of columns
    // so we check this quickly
    
    float mySum = 0;
    for ( int i = 0; i < f_layer_count; i++ ) 
    {
         mySum += gsl_vector_get(f_gsl_eigenvalue_vector, i);
    }
    if (mySum != f_layer_count) return 0;
}

//
// Methods used to build the model
//


/** This method is used when you want to ensure that all variables in all
  * environmental layers are scaled to the same value range. 
  * @note This method is inherited from the Algorithm class
  * @param Scalar pointer min that the post normalised will be fitted to
  * @param Scalar pointer max that the post normalised will be fitted to
  * @return 0 if no normalisation is needed
  */
int Csm::needNormalization( Scalar *min, Scalar *max )
{}


/** Initialise the model specifying a threshold / cutoff point.
  * This is optional (model dependent).
  * @note This method is inherited from the Algorithm class
  * @param 
  * @return 0 on error
  */
int Csm::initialize( int ncycle )
{}


/** Start model execution (build the model).
  * @note This method is inherited from the Algorithm class
  * @return 0 on error 
  */
int Csm::iterate()
{




    f_done=1;
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
    return f_done;
}


//
// Methods used to project the model
//


/** This method is used when projecting the model.
  * @note This method is inherited from the Algorithm class
  * @return     
  * @param Scalar *x a pointer to a vector of openModeller Scalar type (currently double). The vector should contain values looked up on the environmental variable layers into which the mode is being projected. */
Scalar Csm::getValue( Scalar *x )
{}

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

void Csm::displaySamples()
{
    //for debuggin print out the env matrix
    printf ("----------------------------------------------\n ");
    for (int i=0;i<f_localityCount;++i)
    {
        for (int j=0;j<f_layer_count;j++)
        {
            double myDouble = gsl_matrix_get (f_gsl_environment_matrix,i,j);
            printf ("%f\t ", myDouble);
        }
        printf ("\n");
    }
    printf ("----------------------------------------------\n ");

    // for debuggin print out the averages
    for (int j=0;j<f_layer_count;j++)
    {
        float myAverage = gsl_vector_get (f_gsl_avg_vector,j);
        printf ("%f\t", myAverage);
    }
    printf (" Average \n----------------------------------------------\n ");

    // for debuggin print out the stddevs
    for (int j=0;j<f_layer_count;j++)
    {
        float myStdDev = gsl_vector_get (f_gsl_stddev_vector,j);
        printf ("%f\t", myStdDev);
    }
    printf (" StdDev \n----------------------------------------------\n ");
}

void Csm::displayCovarianceMatrix()
{
    //for debuggin print out the cov matrix (which should be f_layer_count x f_layer_count in size)
    printf ("\n Covariance Matrix: \n----------------------------------------------\n ");
    for (int i=0;i<f_layer_count;++i)
    {
        for (int j=0;j<f_layer_count;j++)
        {
            double myDouble = gsl_matrix_get (f_gsl_covariance_matrix,i,j);
            printf ("%f\t ", myDouble);
        }
        printf ("\n");
    }
    printf ("----------------------------------------------\n ");
}  

void Csm::displayEigen()
{
    //for debuggin print out the eigen vector and value
    
    //first display the eigen values 
    printf ("\n Eigen values and vector: \n----------------------------------------------\n ");
    for (int j=0;j<f_layer_count;j++)
    {
        float myValue = gsl_vector_get (f_gsl_eigenvalue_vector,j);
        printf ("%f\t", myValue);
    }
    printf ("\n----------------------------------------------\n ");
    //now the eigen vectors
    for (int i=0;i<f_layer_count;++i)
    {
        for (int j=0;j<f_layer_count;j++)
        { 
            double myDouble = gsl_matrix_get (f_gsl_eigenvector_matrix,i,j);
            printf ("%f\t ", myDouble);
        }
        printf ("\n");
    }
    printf ("----------------------------------------------\n ");
}   