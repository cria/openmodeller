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
    f_layer_count = samp->dimEnv();
    //set the class member that holds the number of occurences
    f_localityCount = samp->numPresence();

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
  * Any model definition building stuff is done here.
  * This is optional (model dependent).
  * @note This method is inherited from the Algorithm class
  * @param 
  * @return 0 on error
  */
int Csm::initialize( int ncycle )
{
    printf ("\n*************************************************\n");
    printf ("Csm::initialise - Csm Model Definition Commencing (ncycle = %i)\n", ncycle);
    printf ("\n*************************************************\n\n");
    //convert the sampler to a matrix and store in the local gsl_matrix
    if (!SamplerToMatrix())
    {
        printf ("All occurences are outside the masked area!\n");
    }
    //show what we have calculated so far....
    displayMatrix(f_gsl_environment_matrix,"Environemntal Layer Samples");
    //calculate the mean and std deviation
    calculateMeanAndSd();
    //center and standardise the data
    center();
    //show what we have calculated so far....
    displayMatrix(f_gsl_environment_matrix,"Environemntal Layer Samples (after centering)");

    //Now calculate the covariance matrix:
    f_gsl_covariance_matrix = autoCovariance(f_gsl_environment_matrix);
    //the rows and columns in the cavariance matrix should be equal, otherwise abort
    if (f_gsl_covariance_matrix->size1 != f_gsl_covariance_matrix->size2)
    {
        printf ("\n\n\nCsm :: A critical error has occurred - cavariance matrix is not square...aborting\n\n\n");
        return 0;
    }
    //and display the result...
    displayMatrix(f_gsl_covariance_matrix,"Covariance Matrix");

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
    //Initialise the retained components count (to be used further down and in displayEigen())
    f_retained_components_count = f_layer_count;
    //show the eigen before sorting
    printf ("\n\nBefore sorting : \n");
    displayVector(f_gsl_eigenvalue_vector,"Eigen Values");
    displayMatrix(f_gsl_eigenvector_matrix,"Eigen Vector");
    //sort the eigen vector by the eigen values (in descending order)
    gsl_eigen_symmv_sort (f_gsl_eigenvalue_vector, f_gsl_eigenvector_matrix,
                          GSL_EIGEN_SORT_VAL_DESC);
    //print out the result
    printf ("\n\nAfter sorting : \n");
    displayVector(f_gsl_eigenvalue_vector,"Eigen Values");
    displayMatrix(f_gsl_eigenvector_matrix,"Eigen Vector");
    discardComponents();
    //print out the result
    printf ("\n\nAfter discarding unwanted components : \n");
    displayVector(f_gsl_eigenvalue_vector,"Eigen Values");
    displayMatrix(f_gsl_eigenvector_matrix,"Eigen Vector");
    printf ("\n*************************************************\n");
    printf ("        CSM Model Generation Completed ");
    printf ("\n*************************************************\n");
}
/** This is a utility function to convert the f_sampl Sampler to a
  * gsl_matrix.
  * @return 0 on error
  */
int Csm::SamplerToMatrix()
{

    //create a samples container to hold the values retrieved
    //from the environmental layers at each point - this will be
    //converted to a gsl structure in the next step
    //f_samp is defined in the Algorithm class from which Csm inherits
    SampledData localities;
    if ( ! _samp->getPresence(&localities, f_localityCount))
      return 0;
    Scalar *mySamples = localities.pnt;
    f_localityCount   = localities.npnt;

    // Allocate the gsl matrix to store environment data at each locality
    f_gsl_environment_matrix = gsl_matrix_alloc (f_localityCount, f_layer_count);
    // now populate the gsl matrix from the sample data
    for (int i=0;i<f_localityCount;++i)
    {
        for (int j=0;j<f_layer_count;j++)
        {
            //we add one to j in order to omit the specimen count column
            float myCellValue = mySamples[i * f_layer_count + j];
            gsl_matrix_set (f_gsl_environment_matrix,i,j,myCellValue);
        }
    }
    return 1;
}

int Csm::center()
{
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
}


int Csm::discardComponents()
{
    // Discard any columns from the eigenvector where the eigenvalue is < 1
    // (Keiser-Gutman method)
    // because the eigenvalues are sorted, we can discard any columns after the
    // first one is encountered.

    int myColumnNo = -1;
    // for debuggin print out the averages
    for (int j=0;j<f_layer_count;j++)
    {
        float myValue = gsl_vector_get (f_gsl_eigenvalue_vector,j);
        if (myValue < 1)
        {
            myColumnNo = j;
        }
    }
    if (myColumnNo > -1)
    {
        f_retained_components_count = myColumnNo-1;
        printf ("\n\nNumber of components retained: %i of %i\n",
                f_retained_components_count,
                f_layer_count);
        //so now create a local copy of the eigvect and eigval...
        //first clone the vector and matrix...
        gsl_vector * tmp_gsl_eigenvalue_vector = gsl_vector_alloc (f_layer_count);
        gsl_matrix * tmp_gsl_eigenvector_matrix = gsl_matrix_alloc (f_layer_count, f_layer_count);

        gsl_matrix_memcpy (tmp_gsl_eigenvector_matrix, f_gsl_eigenvector_matrix);
        gsl_vector_memcpy (tmp_gsl_eigenvalue_vector,f_gsl_eigenvalue_vector);

        //now clear our current eig vec and matrix

        gsl_vector_free (f_gsl_eigenvalue_vector);
        gsl_matrix_free (f_gsl_eigenvector_matrix);

        //next we reassign them to the reduced size (ie without unwanted components)
        f_gsl_eigenvalue_vector = gsl_vector_alloc (f_retained_components_count);
        f_gsl_eigenvector_matrix = gsl_matrix_alloc (f_layer_count, f_retained_components_count);

        //now copy over just the components we intend to keep
        //...first the vector
        for (int j=0;j<f_retained_components_count;j++)
        {
            float myFloat = gsl_vector_get (tmp_gsl_eigenvalue_vector,j);
            gsl_vector_set (f_gsl_eigenvalue_vector,j,myFloat);
        }
        //...now the matrix
        gsl_vector * tmp_gsl_vector = gsl_vector_alloc (f_layer_count);
        for (int j=0;j<f_retained_components_count;j++)
        {
            gsl_matrix_get_col (tmp_gsl_vector, tmp_gsl_eigenvector_matrix, j);
            gsl_matrix_set_col (f_gsl_eigenvector_matrix,j,tmp_gsl_vector);
        }
        //now clear away the temporary vars
        gsl_vector_free (tmp_gsl_vector);
        gsl_vector_free (tmp_gsl_eigenvalue_vector);
        gsl_matrix_free (tmp_gsl_eigenvector_matrix);


    }
    else
    {
        //we keep all the components!
        f_retained_components_count = f_layer_count;
        printf ("\n\nNumber of components retained: %i of %i\n",
                f_retained_components_count,
                f_layer_count);
    }
    return 1;
}

int Csm::calculateMeanAndSd()
{
    //Initialise the vector to hold the mean of each column
    f_gsl_avg_vector = gsl_vector_alloc (f_layer_count);
    gsl_vector_set_zero(f_gsl_avg_vector);

    //Initialise the vector to hold the stddev of each column
    f_gsl_stddev_vector = gsl_vector_alloc (f_layer_count) ;
    gsl_vector_set_zero(f_gsl_stddev_vector);


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

    return 0;
}

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
  * @return Scalar of the probablitiy of occurence must be between 0 and 1  
  * @param Scalar *x a pointer to a vector of openModeller Scalar type (currently double). The vector should contain values looked up on the environmental variable layers into which the mode is being projected. */
Scalar Csm::getValue( Scalar *x )
{
    float myFloat;
    //first thing we do is convert the oM primitive env value array to a gsl matrix
    //with only one row so we can do matrix multplication with it
    gsl_matrix * tmp_gsl_matrix = gsl_matrix_alloc (1,f_layer_count);
    for (int i=0;i<f_layer_count;++i)
    {
        myFloat = static_cast<float>(x[i]);
        //get the stddev and mean for this column
        float myAverage = gsl_vector_get (f_gsl_avg_vector,i);
        float myStdDev = gsl_vector_get (f_gsl_stddev_vector,i);
        //subtract the mean from the value then divide by the standard deviation
        myFloat = (myFloat-myAverage)/myStdDev;
        //assign the result to our vector
        gsl_matrix_set (tmp_gsl_matrix,0,i,myFloat);
        //printf ("%f\t", myFloat );
    }

    gsl_matrix * p = product(tmp_gsl_matrix, f_gsl_eigenvector_matrix);

    //create a temporary vector and place component 1 in it - could make this a class member to improve performance!
    //gsl_vector * component1_gsl_vector = gsl_vector_alloc (f_layer_count);
    //gsl_matrix_get_col (component1_gsl_vector, f_gsl_eigenvector_matrix, 0);

    //multiply the result vector by the first component in the eigenvector
    //gsl_vector_mul (tmp_gsl_vector, component1_gsl_vector);

    //probabily for the cell is now the sum of the tmp vector
    myFloat=0;
    for (int i=0;i<p->size1;i++)
        for (int j=0;j<p->size2;j++)
            myFloat+=gsl_matrix_get (p,i,j);

    //printf ("Prob: %f\n",myFloat);
    //now we
    //now clear away the temporary vars
    gsl_matrix_free (p);
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
    gsl_matrix_memcpy (original_matrix, m);

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

