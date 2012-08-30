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
//!todo Make this a runtime selectable parameter

#include <string.h>
#include <cassert>
#include "csm.hh"

#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_multifit_nlin.h> //remove this when we have proper covar matrix
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sf_gamma.h>
//for chi function gsl_ran_chisq_pdf
#include <gsl/gsl_randist.h>
//for gsl_cdf_chisq_Q
#include <gsl/gsl_cdf.h>

#include <math.h>

#ifdef MSVC
#include <float.h>
#define isnan _isnan
#endif

#if defined(__APPLE__) && (__APPLE_CC__ < 4000)
extern "C"
{
  int isnan(double);
}
#endif

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
Csm::Csm(AlgMetadata const * metadata) :
    AlgorithmImpl( metadata )
{
  _initialized = 0;
  verboseDebuggingBool=false;
  if (parameters != 0)
  {
    std::cout << "Error __LINE__, __FILE__ : this is an abstract class it should have no params" << std::endl;
  }
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

  Log::instance()->debug( "Base CSM class initializing\n" );

  //set the class member that holds the number of environmental variables
  //we subtract 1 because the first column contains the specimen count
  _layer_count = _samp->numIndependent();
  //set the class member that holds the number of occurences
  _localityCount = _samp->numPresence();
  Log::instance()->debug( "Checking more than 1 samples exist\n" );
  if (_localityCount<2)
  {
    Log::instance()->warn( "CSM needs at least two occurrence points..aborting...\n" );
    _initialized = 0;
    return 0;
  }

  //convert the sampler to a matrix and store in the local gsl_matrix
  Log::instance()->debug( "Converting samples to GSL_Matrix\n" );
  if (!SamplerToMatrix())
  {
    Log::instance()->warn( "All occurences are outside the masked area!\n" );
    _initialized = 0;
    return 0;
  }
  //show what we have calculated so far....
  //displayMatrix(_gsl_environment_matrix,"Environemntal Layer Samples");
  bool myFlag = csm1();
  if (myFlag)
  {
    Log::instance()->debug( "Model initialization completed ok!\n" );
  }
  else
  {
    Log::instance()->warn( "Model initialization failed!\n" );
  }
  return myFlag;
}
/** This is a utility function to convert the _sampl Sampler to a
  * gsl_matrix.
  * @return 0 on error
  */
int Csm::SamplerToMatrix()
{
  if (_localityCount < 1)
  {
    return 0;
  }

  OccurrencesImpl::const_iterator pit = _samp->getPresences()->begin();
  OccurrencesImpl::const_iterator fin = _samp->getPresences()->end();

  // Allocate the gsl matrix to store environment data at each locality
  _gsl_environment_matrix = gsl_matrix_alloc (_localityCount, _layer_count);
  // now populate the gsl matrix from the sample data
  for (int i=0; pit != fin; ++pit, ++i)
  {
    for (int j=0;j<_layer_count;j++)
    {
      //we add one to j in order to omit the specimen count column
      float myCellValue = (float)(*pit)->environment()[j];
      gsl_matrix_set (_gsl_environment_matrix,i,j,myCellValue);
    }
  }
  Log::instance()->debug( "Csm::SampleToMatrix: x: %i y: %i\n",_layer_count,_localityCount );
  //for debugging - write matrix to file
  //FILE * myFile = fopen("/tmp/csm_debug_sample_matrix.dat","w");
  //gsl_matrix_fwrite(myFile, _gsl_environment_matrix);
  //fclose(myFile);
  return 1;
}



/** NOTE: the mean and stddev vectors MUST be pre-initialised! */
int Csm::calculateMeanAndSd(gsl_matrix * theMatrix,
                            gsl_vector * theMeanVector,
                            gsl_vector * theStdDevVector)
{
#ifndef WIN32
  assert (theMatrix != 0);
  assert (theMeanVector !=0);
  assert (theStdDevVector !=0);
#endif
  //Initialise the vector to hold the mean of each column
  gsl_vector_set_zero(theMeanVector);

  //Initialise the vector to hold the stddev of each column
  gsl_vector_set_zero(theStdDevVector);

  //Log::instance()->debug( "Memory location of theMeanVector is %X\n", &theMeanVector );
  //Log::instance()->debug( "Memory location of gsl_vector_set_zero is %X\n", &gsl_vector_set_zero );
  //Log::instance()->debug( "Memory location of _gsl_avg_vector is %X\n", &_gsl_avg_vector );
  //Log::instance()->debug( "Memory location of _gsl_stddev_vector is %X\n", &_gsl_stddev_vector );

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
  Log::instance()->debug( "Centering data\n" );
  for (int j=0;j<_layer_count;j++)
  {
    //get the stddev and mean for this column
    double myAverage = gsl_vector_get (_gsl_avg_vector,j);
    double myStdDev = gsl_vector_get (_gsl_stddev_vector,j);

    for (int i=0;i<_localityCount;++i)
    {
      double myDouble = gsl_matrix_get (_gsl_environment_matrix,i,j);
      if (myStdDev > 0)
      {
        myDouble = (myDouble-myAverage)/myStdDev;
      }
      else
      {
        myDouble=0;
      }
      //update the gsl_matrix with the new value
      gsl_matrix_set(_gsl_environment_matrix,i,j,myDouble);
    }
  }

  return 0;
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
int Csm::done() const
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
Scalar Csm::getValue( const Sample& x ) const
{

  float myFloat;
  //bool myAllAreZeroFlag=true;
  //first thing we do is convert the oM primitive env value array to a gsl matrix
  //with only one row so we can do matrix multplication with it
  gsl_matrix * tmp_gsl_matrix = gsl_matrix_alloc (1,_layer_count);
  gsl_matrix * tmp_raw_gsl_matrix = gsl_matrix_alloc (1,_layer_count);
  for (signed int i = 0;i<_layer_count;++i)
  {
    myFloat = static_cast<float>(x[i]);
    gsl_matrix_set (tmp_raw_gsl_matrix,0,i,myFloat);

    //if (myFloat!=0)
    //{
    //  myAllAreZeroFlag=false;
    //}
    //Log::instance()->debug( "myFloat = %f\n", myFloat );
    //get the stddev and mean for this column
    float myAverage = (float)gsl_vector_get (_gsl_avg_vector,i);
    float myStdDev = (float)gsl_vector_get (_gsl_stddev_vector,i);
    //subtract the mean from the value then divide by the standard deviation
    if (myStdDev > 0)
    {
      myFloat = (myFloat-myAverage)/myStdDev;
    }
    else
    {
      myFloat=myFloat-myAverage;
    }
    //assign the result to our vector
    gsl_matrix_set (tmp_gsl_matrix,0,i,myFloat);
    //Log::instance()->debug( "myFloat = %f\n", myFloat );
  }
  //Log::instance()->debug( " ----  end of scalar \n");
  displayMatrix(tmp_raw_gsl_matrix,"Voxel passed to getValue",false);
  displayMatrix(tmp_gsl_matrix,"Voxel passed to getValue -Mean / stddev");
  displayVector(_gsl_avg_vector,"Averages");
  displayVector(_gsl_stddev_vector,"Stddevs");
  //if (myAllAreZeroFlag) {return 0;}


  gsl_matrix * z = product(tmp_gsl_matrix, _gsl_eigenvector_matrix);
  displayMatrix(z,"z - Product of voxel passed to getValue -Mean / stddev");

  // z should match the dimensions of tmp_gsl_matrix so do some error checking
  if (z->size1 != tmp_gsl_matrix->size1)
  {
    Log::instance()->warn( "Error during creation of product Z in CSM getValue - number of rows don't match\n" );
    exit(0);
  }

  // number of cols in z should == number of components
  if (z->size2 != tmp_gsl_matrix->size1)
  {
    //Log::instance()->warn( "Error during creation of product Z in CSM getValue - number of cols don't match number of components\n" );
    //exit(0);
  }

  //displayMatrix(z,"z ");
  // now we standardise the values in z
  // we do this by dividing each element in z by the square root of its associated element in
  // the eigenvalues vector

  for (unsigned int i=0;i<z->size2;i++)
  {
    gsl_matrix_set(z,0,i,gsl_matrix_get (z,0,i)/sqrt(gsl_vector_get(_gsl_eigenvalue_vector,i)));
  }
  displayMatrix(z,"Standardised : Each value in z / sqrt of associated element in the eigenvalues vector");
  // now we square each element and sum them
  double mySumOfSquares=0;
  for (unsigned int i=0;i<z->size2;i++)
  {
    double myValue=gsl_matrix_get (z,0,i);
    if (!isnan(myValue))
    {
      mySumOfSquares+= pow(gsl_matrix_get (z,0,i), 2);
      //Warning uncommenting the next line will spew a lot of stuff to stderr!!!!
      //Log::instance()->debug( "myValue : %f Cumulative : %f\n", myValue , myFloat );
    }
  }
  
  //now work out the probability of myFloat between 0 and 1
  double myHalfComponentCountDouble=(z->size2)/2;
  double myHalfSumOfSquaresDouble=mySumOfSquares/2;
  //Log::instance()->debug( "Component count %f , Half sum of squares %f\n", myHalfComponentCountDouble, myHalfSumOfSquaresDouble );
  //
  //This way id deprecated in favour of the chi square test
  //
  //float myProbability=1-gsl_sf_gamma_inc_Q (myHalfSumOfSquaresDouble,myHalfComponentCountDouble);
  //float myProbability=1-gsl_ran_chisq_pdf(mySumOfSquares,z->size2);
  double myProbability=gsl_cdf_chisq_Q(mySumOfSquares,z->size2);
  if (verboseDebuggingBool)
  {
    printf("\n-------------------------------\n");
    printf("Component count : %u\n",static_cast<unsigned int>(z->size2));
    printf("Component count / 2: %f\n",myHalfComponentCountDouble);
    printf("Sum of squares : %f\n",mySumOfSquares);
    printf("Sum of squares / 2: %f\n",myHalfSumOfSquaresDouble);
    printf("Probability: %f\n\n", myProbability);
    printf("-------------------------------\n");
    
  }

  //Log::instance()->debug( "Prob: %f \r", myProbability);
  //now clear away the temporary vars
  gsl_matrix_free (z);
  //gsl_vector_free (component1_gsl_vector);
  gsl_matrix_free (tmp_gsl_matrix);
  gsl_matrix_free (tmp_raw_gsl_matrix);
  return myProbability;
}

/** Returns a value that represents the convergence of the algorithm
  * expressed as a number between 0 and 1 where 0 represents model
  * completion. 
  * @return 
  * @param Scalar *val 
*/
int Csm::getConvergence( Scalar * const val ) const
{
  return 0;
}

//
// General Helper Methods
//


void Csm::displayVector(const gsl_vector * v, const char * name, const bool roundFlag) const
{
  if (verboseDebuggingBool)
  {
    if (roundFlag)
    {

      fprintf( stderr, "\nDisplaying Vector rounded to 4 decimal places '%s' (%u): \n----------------------------------------------\n[  ", name, static_cast<unsigned int>(v->size) );
    }
    else
    {
      fprintf( stderr, "\nDisplaying Vector '%s' (%u): \n----------------------------------------------\n[  ", name, static_cast<unsigned int>(v->size) );
    }

    char sep1[] = ", ";

    for (unsigned int i=0;i<v->size;++i)
    {
      if (i == v->size -1)
        strcpy(sep1, " ]");

      double myDouble = gsl_vector_get (v,i);
      if (roundFlag)
      {
        fprintf( stderr, "%.4g %s", myDouble, sep1 );
      }
      else
      {
        fprintf( stderr, "%g %s", myDouble, sep1 );
      }
    }
    fprintf( stderr, "\n----------------------------------------------\n" );
  }
}


/**********************/
/**** displayMatrix ***/
void Csm::displayMatrix(const gsl_matrix * m, const char * name, const bool roundFlag) const
{
  if (verboseDebuggingBool)
  {
    if (!roundFlag)
    {
      fprintf( stderr, "\nDisplaying Matrix '%s' (%u / %u): \n----------------------------------------------\n[\n", name, static_cast<unsigned int>(m->size1), static_cast<unsigned int>(m->size2) );
    }
    else
    {
      fprintf( stderr, "\nDisplaying Matrix rounded to 4 decimal places '%s' (%u / %u): \n----------------------------------------------\n[\n", name, static_cast<unsigned int>(m->size1), static_cast<unsigned int>(m->size2) );
    }
    for (unsigned int i=0;i<m->size1;++i)
    {
      char sep1[] = ",";
      char sep2[] = ";";

      for (unsigned int j=0;j<m->size2;j++)
      {
        double myDouble = gsl_matrix_get (m,i,j);

        if (j == m->size2 -1)
          strcpy(sep1, "");
        if (!roundFlag)
        {
          fprintf( stderr, "%g %s ", myDouble, sep1 );
        }
        else
        {
          fprintf( stderr, "%.4g %s ", myDouble, sep1 );
        }
      }

      fprintf( stderr, "%s\n", sep2 );
    }
    fprintf( stderr, "]\n----------------------------------------------\n" );
  }
}


/******************/
/**** transpose ***/
gsl_matrix * Csm::transpose (gsl_matrix * m)
{
  gsl_matrix * t = gsl_matrix_alloc (m->size2, m->size1);

  for (unsigned int i = 0; i < m->size1; i++)
  {
    gsl_vector * v = gsl_vector_alloc(m->size2);
    gsl_matrix_get_row (v, m, i);
    gsl_matrix_set_col (t, i, v);
    gsl_vector_free(v);
  }

  return t;
}

/************************/
/**** Vectors product ***/
double Csm::product (gsl_vector * va, gsl_vector * vb) const
{
  // fix me: need to check if vectors are of the same size !!!

  double res = 0.0;

  for (unsigned int i = 0; i < va->size; i++)
  {
    res += gsl_vector_get(va, i)*gsl_vector_get(vb, i);
  }

  return res;
}

/*************************/
/**** Matrices product ***/
gsl_matrix * Csm::product (gsl_matrix * a, gsl_matrix * b) const
{
  // fix me: need to check if a->size2 is equal to b->size1 !!!

  gsl_matrix * p = gsl_matrix_alloc (a->size1, b->size2);

  for (unsigned int i = 0; i < a->size1; i++)
  {
    gsl_vector * va = gsl_vector_alloc(a->size2);

    gsl_matrix_get_row (va, a, i);

    for (unsigned int j = 0; j < b->size2; j++)
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
  int j;

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

      for (j = 0; j < numrows; j++)
      {
        val += gsl_matrix_get (m, j, i);
      }

      gsl_vector_set (v, i, val);

      for (j = 0; j < numrows; j++)
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

  // multiply by x'
  gsl_matrix * p = product(mt, m);

  // Note: scaling should happen after calculating the product with x-transpose 
  // x / (n - 1)
  gsl_matrix_scale (p, (double)1/(numrows-1));

  gsl_matrix_free (mt);
  gsl_matrix_free (m);
  gsl_matrix_free (s);

  return p;
}

/** Csm1 is used to produce the model definition */
bool Csm::csm1()
{
  //calculate the mean and std deviation
  Log::instance()->debug( "Calculating mean and stddev\n" );
  _gsl_avg_vector = gsl_vector_alloc (_gsl_environment_matrix->size2);
  _gsl_stddev_vector = gsl_vector_alloc (_gsl_environment_matrix->size2) ;
  calculateMeanAndSd(_gsl_environment_matrix,_gsl_avg_vector,_gsl_stddev_vector);
  //displayVector(_gsl_avg_vector,"Average vector");
  //displayVector(_gsl_stddev_vector,"Standard Deviation vector");
  //center and standardise the data
  Log::instance()->debug( "Centering and standardising\n" );
  center();
  //show what we have calculated so far....
  // displayMatrix(_gsl_environment_matrix,"Environemntal Layer Samples (after centering)");

  //Now calculate the covariance matrix:
  Log::instance()->debug( "Calculating covariance matrix\n" );
  _gsl_covariance_matrix = autoCovariance(_gsl_environment_matrix);
  //the rows and columns in the cavariance matrix should be equal, otherwise abort
  if (_gsl_covariance_matrix->size1 != _gsl_covariance_matrix->size2)
  {
    Log::instance()->warn( "CSM critical error - covariance matrix is not square!\n" );
    return false;
  }
  //and display the result...
  //displayMatrix(_gsl_covariance_matrix,"Covariance Matrix");

  //now compute the eigen value and vector
  Log::instance()->debug( "Calculating eigenvalue and eigenvector\n" );
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
  //Log::instance()->debug( "\nBefore sorting : \n" );
  //displayVector(_gsl_eigenvalue_vector,"Unsorted Eigen Values");
  //displayMatrix(_gsl_eigenvector_matrix,"Unsorted Eigen Vector");
  //sort the eigen vector by the eigen values (in descending order)
  gsl_eigen_symmv_sort (_gsl_eigenvalue_vector, _gsl_eigenvector_matrix,
          GSL_EIGEN_SORT_VAL_DESC);
  //print out the result
  Log::instance()->debug( "Eigenvector sorted\n" );
  //displayVector(_gsl_eigenvalue_vector,"Sorted Eigen Values");
  //displayMatrix(_gsl_eigenvector_matrix,"Sorted Eigen Vector");

  Log::instance()->debug( "CSM Model Generation Completed\n" );

  //After the model is generated, we can discard unwanted components!
  if (discardComponents())
  {
    Log::instance()->debug( "Unwanted components discarded\n" );
    return true;
  }
  else
  {
    Log::instance()->debug( "Discard components retained too few components - model can not be generated!\n" );
    Log::instance()->warn( "Could not generate a model with sufficient components!\n" );
  }
  return false;
  //print out the result
}



/****************************************************************/
/****************** configuration *******************************/
void
Csm::_getConfiguration( ConfigurationPtr& config ) const
{
  if (!_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("Csm") );
  config->addSubsection( model_config );

  // _gsl_avg_vector
  double *values = new double[_layer_count];

  for (int i=0; i < _layer_count; ++i)
    values[i] = gsl_vector_get(_gsl_avg_vector, i);

  model_config->addNameValue( "AvgVector", values, _layer_count );

  // _gsl_stddev_vector
  for (int i=0; i < _layer_count; ++i)
    values[i] = gsl_vector_get(_gsl_stddev_vector, i);

  model_config->addNameValue( "StddevVector", values, _layer_count );

  // _gsl_eigenvalue_vector
  for (int i=0; i < _retained_components_count; ++i)
    values[i] = gsl_vector_get(_gsl_eigenvalue_vector, i);

  model_config->addNameValue( "EigenvalueVector", values, _retained_components_count );

  delete[] values;

  // _gsl_eigenvector_matrix
  int num_cells = _layer_count * _retained_components_count;

  double *flat_eigenvector_matrix = new double[num_cells];

  int cnt = 0;

  for (int i=0; i < _layer_count; ++i)
    for (int j=0; j < _retained_components_count; ++j, ++cnt)
      flat_eigenvector_matrix[cnt] = gsl_matrix_get( _gsl_eigenvector_matrix, i, j );

  model_config->addNameValue( "EigenvectorMatrix", flat_eigenvector_matrix, num_cells );

  delete[] flat_eigenvector_matrix;
}

void
Csm::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "Csm",false );

  if (!model_config)
    return;

  // _gsl_avg_vector
  std::vector<double> stl_vector = model_config->getAttributeAsVecDouble( "AvgVector" );

  _layer_count = stl_vector.size();

  _gsl_avg_vector = gsl_vector_alloc( _layer_count );

  for (int i=0; i < _layer_count; ++i)
    gsl_vector_set( _gsl_avg_vector, i, stl_vector[i] );

  // _gsl_stddev_vector
  stl_vector = model_config->getAttributeAsVecDouble( "StddevVector" );

  _gsl_stddev_vector = gsl_vector_alloc( _layer_count );

  for (int i=0; i < _layer_count; ++i)
    gsl_vector_set( _gsl_stddev_vector, i, stl_vector[i] );

  // _gsl_eigenvalue_vector
  stl_vector = model_config->getAttributeAsVecDouble( "EigenvalueVector" );

  _retained_components_count = stl_vector.size();

  _gsl_eigenvalue_vector = gsl_vector_alloc( _retained_components_count );

  for (int i=0; i < _retained_components_count; ++i)
    gsl_vector_set( _gsl_eigenvalue_vector, i, stl_vector[i] );

  // _gsl_eigenvector_matrix
  stl_vector = model_config->getAttributeAsVecDouble( "EigenvectorMatrix" );

  _gsl_eigenvector_matrix = gsl_matrix_alloc( _layer_count, _retained_components_count );

  int cnt = 0;

  for (int i=0; i < _layer_count; ++i)
    for (int j=0; j < _retained_components_count; ++j, ++cnt)
      gsl_matrix_set( _gsl_eigenvector_matrix, i, j, stl_vector[cnt] );

  _done = true;
}
