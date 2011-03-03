//
// Enfa
//
// Description:
//
//
// Author: CRIA <chris.yesson@ioz.ac.uk>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
//!todo Make this a runtime selectable parameter

#include <string.h>
#include <cassert>
#include <math.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sort_vector.h>

#include "enfa.hh"


/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 6
#define BACKGROUND_ID      "NumberOfBackgroundPoints"
#define NUM_RETRIES        "NumberOfRetries"
#define DISCARD_METHOD     "DiscardMethod"
#define RETAIN_COMPONENTS  "RetainComponents"
#define RETAIN_VARIATION   "RetainVariation"
#define VERBOSE_DEBUG      "VerboseDebug"

static AlgParamMetadata parameters[NUM_PARAM] = {

  {
    BACKGROUND_ID,                   // Id.
    "Number of background sample points", // Name.
    Integer,                     // Type.
    "Number of background points to be sampled when estimating the mean and standard deviation.", // Overview
    "The ENFA algorithm compares the species presence data with the background environment.  This requires the calculation of the mean, standard deviation and covariances of each environmental layer.  This could be prohibitively slow and expensive for large datasets, so estimate these values by randomly sampling X points from the background data.", // Description.
    1,         // Not zero if the parameter has lower limit.
    10,        // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "10000"        // Parameter's typical (default) value.
  },
  {
    NUM_RETRIES,                   // Id.
    "Number of retries of model", // Name.
    Integer,                     // Type.
    "Number of attempted retries in the case that the model generation fails.", // Overview
    "The algorithm requires the inversion of several matrices, but sometimes the matrix to invert is singular and so the algorithm fails.  This seems to occur when the background data is undersampled or not representative.  Often retrying the model generation (i.e. resampling the background data) makes this problem go away.", // Description.
    1,         // Not zero if the parameter has lower limit.
    1,        // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    100,         // Parameter's upper limit.
    "5"        // Parameter's typical (default) value.
  },
  {
    DISCARD_METHOD,                   // Id.
    "Method for discarding components", // Name.
    Integer,                     // Type.
    "Method for discarding components: 0 - Fixed number (set by RETAIN_COMPONENTS), 1 - Minimum components explaining a fixed variation (set by RETAIN_VARIATION), 2 - Broken stick method", // Overview
    " 0 - Retain a fixed number of components defined by the variable RETAIN_COMPONENTS\n 1 - Retain the top N components that cumulatively explain the level of variation defined by the RETAIN_VARIATION variable\n 2 - Compare the observed explanation of variation to the broken stick distribution retaining those components explaining higher levels of variation", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,        // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    2,         // Parameter's upper limit.
    "2"        // Parameter's typical (default) value.
  },
  {
    RETAIN_COMPONENTS,                   // Id.
    "Number of components to retain", // Name.
    Integer,                     // Type.
    "Specify the number of components to retain (only for DISCARD_METHOD=0)", // Overview
    "If the Discard_method=0, then this variable is used to determine the number of components to retain.", // Description.
    1,         // Not zero if the parameter has lower limit.
    1,        // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    100,         // Parameter's upper limit.
    "2"        // Parameter's typical (default) value.
  },
  {
    RETAIN_VARIATION,                   // Id.
    "Percent varition for component retention", // Name.
    Integer,                     // Type.
    "Specify the amount of variation that the retained components should explain (only for DISCARD_METHOD=1)", // Overview
    "If the Discard_method=1, then this variable is used to determine the number of components to retain, by taking those components that cumulatively account for at least this much variation.", // Description.
    1,         // Not zero if the parameter has lower limit.
    50,        // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    100,         // Parameter's upper limit.
    "75"        // Parameter's typical (default) value.
  },
  {
    VERBOSE_DEBUG,                   // Id.
    "Verbose printing for debugging", // Name.
    Integer,                     // Type.
    "Print lots of details", // Overview
    "", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,        // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1,         // Parameter's upper limit.
    "0"        // Parameter's typical (default) value.
  },
};

/****************************************************************/
/****************************** Enfa *****************************/
static AlgMetadata metadata = { // General metadata
  "ENFA",                    // Id
  "ENFA (Ecological-Niche Factor Analysis)",     // Name
  "0.1.1",                        // Version
  "Algorithm based on presence only data using a modified principal components analysis.", // Overview
  "Ecological-Niche Factor Analysis (Hirzel et al, 2002) uses a modified principal components analysis to develop a model based on presence only data.  The observed environment is compared to the background data of the study area (note that absence points in the occurrence file are treated as background data).  The analysis produces factors similar to a PCA.  The first factor is termed the 'marginality' of the species, marginality is defined as the ecological distance between the species optimum and the mean habitat within the background data. Other factors are termed the 'specialization', and are defined as the ratio of the ecological variance in mean habitat to that observed for the target species.  Model projection uses the geomeans method of Hirzel & Arlettaz (2003)", // Description
  "Hirzel, A.H.; Hausser, J.; Chessel, D. & Perrin, N.",    // Algorithm author
  "Hirzel, A.H.; Hausser, J.; Chessel, D. & Perrin, N. Ecological-niche factor analysis: How to compute habitat-suitability maps without absence data? Ecology, 2002, 83, 2027-2036\nHirzel, A.H & Arlettaz, R. Modeling habitat suitability for complex species distributions by environmental distance geometric mean Environmental Management, 2003, 32, 614-623\n", // Bibliography
  "Chris Yesson",                     // Code author
  "chris.yesson [at] ioz.ac.uk",            // Code author's contact
  0,                    // Does not accept categorical data
  0,                    // Does not need (pseudo)absence points
  NUM_PARAM, parameters // Algorithm's parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new Enfa();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/** Constructor for Enfa
   * 
   * @param Sampler is class that will fetch environment variable values at each occurrence / locality
   */
Enfa::Enfa() :
    AlgorithmImpl( &metadata )
{
  _initialized = 0;
}


/** This is the descructor for the Enfa class */
Enfa::~Enfa()
{
  if ( _initialized )
  {
    gsl_matrix_free (_gsl_background_matrix);
    gsl_matrix_free (_gsl_covariance_background_matrix);
    gsl_matrix_free (_gsl_covariance_matrix);
    gsl_matrix_free (_gsl_covariance_matrix_root_inverse);
    gsl_matrix_free (_gsl_eigenvector_matrix);
    gsl_matrix_free (_gsl_environment_factor_matrix);
    gsl_matrix_free (_gsl_environment_matrix);
    gsl_matrix_free (_gsl_score_matrix);
    gsl_matrix_free (_gsl_workspace_H);
    gsl_matrix_free (_gsl_workspace_W);
    gsl_matrix_free (_gsl_workspace_y);

    gsl_vector_free (_gsl_avg_vector);
    gsl_vector_free (_gsl_avg_background_vector);
    gsl_vector_free (_gsl_eigenvalue_vector);
    gsl_vector_free (_gsl_stddev_vector);
    gsl_vector_free (_gsl_stddev_background_vector);
    gsl_vector_free (_gsl_workspace_z);
    gsl_vector_free (_gsl_factor_weights);
    gsl_vector_free (_gsl_factor_weights_all_components);
    gsl_vector_free (_gsl_geomean_vector);
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
int Enfa::initialize()
{
  _initialized = 1;

  Log::instance()->info( "Base ENFA class initializing\n" );

  //set the class member that holds the number of environmental variables
  //we subtract 1 because the first column contains the specimen count
  _layer_count = _samp->numIndependent();
  //set the class member that holds the number of occurences
  _localityCount = _samp->numPresence();
  //Log::instance()->info( "Checking more than 1 samples exist\n" );
  if (_localityCount<2)
  {
    Log::instance()->warn( "ENFA needs at least two occurrence points..aborting...\n" );
    _initialized = 0;
    return 0;
  }

  // _backgroundCount = _backgroundSamp->numOccurrences();
  //if ( _samp->numAbsence() )
  if ( _samp->numAbsence() )
    {
      _backgroundProvided=1;
      _backgroundCount = _samp->numAbsence();
      Log::instance()->info( "Using background data provided (%i records marked as absences in the occurrence file)\n",_backgroundCount);

    }
  else
    {
      getParameter( BACKGROUND_ID, &_backgroundCount );
      _backgroundProvided=0;
    }

  //use the pseudoabsence generator to do this
  getParameter( NUM_RETRIES, &_numRetries );
  getParameter( DISCARD_METHOD, &_discardMethod);
  getParameter( RETAIN_COMPONENTS, &_retainComponents);
  getParameter( RETAIN_VARIATION, &_retainVariation);
  getParameter( VERBOSE_DEBUG, &_verboseDebug);

  /* sometimes our random background samples produce singular matrices
     so catch this exception and restart the model */

  _retryCount=0;
  bool myFlag=false;

  while (_retryCount<_numRetries && !myFlag)
  {

      _retryCount+=1;
      try
      {

	  //convert the sampler to a matrix and store in the local gsl_matrix
	  //Log::instance()->info( "Converting samples to GSL_Matrix\n" );
	  if (!SamplerToMatrix())
	  {
	      Log::instance()->warn( "All occurences are outside the masked area!\n" );
	      _initialized = 0;
	      return 0;
	  }

	  if (!BackgroundToMatrix())
	  {
	      Log::instance()->warn( "Failed to sample background data!\n" );
	      _initialized = 0;
	      return 0;
	  }

	  myFlag = enfa1();

      }
      catch (InverseFailedException& exception)
      {
	  Log::instance()->warn( "Model failed, retry number %i\n", _retryCount );
	  myFlag=false;
      }

  }

  if (! myFlag)
  {
      Log::instance()->warn( "Model initialization failed!\n" );
  }

  return myFlag;
}

/** This is a utility function to convert the _sampl Sampler to a
  * gsl_matrix.
  * @return 0 on error
  */
int Enfa::SamplerToMatrix()
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
      double myCellValue = (double)(*pit)->environment()[j];
      gsl_matrix_set (_gsl_environment_matrix,i,j,myCellValue);
    }
  }

  //Log::instance()->info( "Enfa::SampleToMatrix: x: %i y: %i\n",_layer_count,_localityCount );
  return 1;
}

// sample background data and store it in a gsl_matrix
int Enfa::BackgroundToMatrix()
{

  //Log::instance()->info("Enfa:BackgroundToMatrix:Generating background samples.\n" );

  // try getting all background samples at once using the absence generator
  // this enables us to ensure geographic uniqueness
  // question: what happens when num-background > number of cells in env layer?
  // allow user to provide background points using the absence parser
  OccurrencesPtr _ocbg;
  OccurrencesImpl::const_iterator pit;
  OccurrencesImpl::const_iterator fin;

  if (_backgroundProvided==1)
    {
      _ocbg = _samp->getAbsences();
    }
  else
    {
      _ocbg = _samp->getPseudoAbsences(_backgroundCount, false, 1, false, false);
    }

  pit = _ocbg->begin();
  fin = _ocbg->end();

  // Allocate the gsl matrix to store environment data at each background point
  _gsl_background_matrix = gsl_matrix_alloc (_backgroundCount, _layer_count);
  // now populate the gsl matrix from the sample data
  for (int i=0; pit != fin; ++pit, ++i)
  {
    for (int j=0;j<_layer_count;j++)
    {
      //we add one to j in order to omit the specimen count column
      double myCellValue = (double)(*pit)->environment()[j];
      gsl_matrix_set (_gsl_background_matrix,i,j,myCellValue);
    }
  }

  return 1;
}



/** NOTE: the mean and stddev vectors MUST be pre-initialised! */
int Enfa::calculateMeanAndSd(gsl_matrix * theMatrix,
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

  return 0;
}

int Enfa::center(gsl_matrix * theMatrix,
		 int spCount)
{
#ifndef WIN32
  assert (theMatrix != 0);
#endif
  //
  //Subtract the column mean from every value in each column
  //Divide each resultant column value by the stddev for that column
  //Note that we are walking the matrix column wise
  //
  //Log::instance()->info( "Centering data\n" );
  //for (int j=0;j<_layer_count;j++)
  int msize=theMatrix->size1;
  for (int j=0;j<_layer_count;j++)
  {
    //get the stddev and mean for this column
    double myAverage = gsl_vector_get (_gsl_avg_background_vector,j);
    double myStdDev = gsl_vector_get (_gsl_stddev_background_vector,j);

    for (int i=0;i<msize;++i)
    {
      double myDouble = gsl_matrix_get (theMatrix,i,j);
      if (myStdDev > 0)
      {
        myDouble = (myDouble-myAverage)/myStdDev;
      }
      else
      {
        myDouble=0.0;
      }
      //update the gsl_matrix with the new value
      gsl_matrix_set(theMatrix,i,j,myDouble);
    }
  }

  return 0;
}


/** Start model execution (build the model).
  * @note This method is inherited from the Algorithm class
  * @return 0 on error 
  */
int Enfa::iterate()
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
int Enfa::done() const
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
Scalar Enfa::getValue( const Sample& x ) const
{
    int ii, cellcount;
    float myFloat;

    //Log::instance()->info( "Enfa:getValue:entering getvalue\n" );

    //first thing we do is convert the oM primitive env value array 
    // to a gsl vector so we can use matrix multplication with it
    gsl_vector * tmp_raw_gsl_vector = gsl_vector_alloc (_layer_count);
    // center data using the avg and stdev
    gsl_vector * tmp_centered_gsl_vector = gsl_vector_alloc(_layer_count);
    double m1, m2, m3, m4;

    for (ii=0;ii<_layer_count;++ii)
    {
	myFloat = static_cast<float>(x[ii]);
	gsl_vector_set (tmp_raw_gsl_vector,ii,myFloat);

	m1=myFloat;
	m2=gsl_vector_get(_gsl_avg_background_vector,ii);
	m3=gsl_vector_get(_gsl_stddev_background_vector,ii);
	m4=(m1-m2)/m3;
	//Log::instance()->info( "getValue: m1: %6.2f, m2: %6.2f, m3: %6.2f, m4: %6.2f, \n", m1, m2, m3, m4 );
	gsl_vector_set(tmp_centered_gsl_vector, ii, m4);
    }

    //if (_verboseDebug)
    //{
    //displayVector(tmp_raw_gsl_vector, "getvalue: tmp_raw_gsl_vector", true);
    //displayVector(tmp_centered_gsl_vector, "getvalue: tmp_centered_gsl_vector", true);
    //}

    gsl_vector_free(tmp_raw_gsl_vector);

    //vector for factored data
    gsl_matrix * tmp_centered_gsl_matrix = gsl_matrix_alloc(1,_layer_count);
    for (ii=0;ii<_layer_count;++ii)
      gsl_matrix_set(tmp_centered_gsl_matrix,0,ii,gsl_vector_get(tmp_centered_gsl_vector,ii));

    gsl_vector_free(tmp_centered_gsl_vector);

    gsl_matrix * tmp_factored_gsl_matrix = gsl_matrix_alloc(1,_layer_count);
    
    // multiply centered data by score matrix to get factored data 
    //gsl_blas_dgemv (CblasTrans, 1.0, _gsl_score_matrix, tmp_centered_gsl_vector, 0.0, tmp_factored_gsl_vector);
    //gsl_blas_dgemv (CblasNoTrans, 1.0, _gsl_score_matrix, tmp_centered_gsl_vector, 0.0, tmp_factored_gsl_vector);

    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
		   1.0, tmp_centered_gsl_matrix, _gsl_score_matrix, 
		   0.0, tmp_factored_gsl_matrix);

    gsl_vector* tmp_factored_gsl_vector = gsl_vector_alloc(_layer_count);
    for (ii=0;ii<_layer_count;++ii)
      gsl_vector_set(tmp_factored_gsl_vector,ii,
		     gsl_matrix_get(tmp_factored_gsl_matrix,0,ii));
    
    //if (_verboseDebug)
    //displayVector(tmp_factored_gsl_vector, "getvalue: tmp_factored_gsl_vector", true);

    // work out geomean from this point to all specimen localites
    double tmp_geomean = getGeomean(tmp_factored_gsl_vector);
    
    gsl_matrix_free(tmp_centered_gsl_matrix);
    gsl_matrix_free(tmp_factored_gsl_matrix);
    gsl_vector_free(tmp_factored_gsl_vector);

    //Log::instance()->info( "getValue: geomean %6.2f\n", tmp_geomean );
    

    /* % OK, now to convert these into habitat suitability values.
       % Determine what percentage of the species presence points
       % are further away from zero than this point */
    
    //cellcount=0;

    //old version checking every value on unsorted vector
    //for (ii=0; ii<_localityCount; ++ii)
    //  if (gsl_vector_get(_gsl_geomean_vector,ii)>=tmp_geomean) cellcount+=1;
    
    // step up the sorted geomeans vector stop when current geomean is higher
    // quickest when most cells are of low suitability
    cellcount=_localityCount;
    for (ii=_localityCount-1; ii>=0; --ii)
      if (gsl_vector_get(_gsl_geomean_vector,ii)<=tmp_geomean)
	{
	  cellcount=_localityCount-ii-1;
	  break;
	}

    //Log::instance()->info( "getValue: cellcount %i\n", cellcount );
    Scalar myReturn=(Scalar)cellcount/_localityCount;

    //Log::instance()->info( "enfa::getValue %6.2f\n", myReturn );

    return myReturn;
}

/** Returns a value that represents the convergence of the algorithm
 * expressed as a number between 0 and 1 where 0 represents model
 * completion. 
 * @return 
 * @param Scalar *val 
 */
int Enfa::getConvergence( Scalar * const val ) const
{
    return 0;
}

//
// General Helper Methods
//


void Enfa::displayVector(const gsl_vector * v, const char * name, const bool roundFlag) const
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


/**********************/
/**** displayMatrix ***/
void Enfa::displayMatrix(const gsl_matrix * m, const char * name, const bool roundFlag) const
{
    if (!roundFlag)
    {
      fprintf( stderr, "\nDisplaying Matrix '%s' (%u / %u): \n----------------------------------------------\n[\n", name, static_cast<unsigned int>(m->size1), static_cast<unsigned int>(m->size2) );
    }
    else
    {
      fprintf( stderr, "\nDisplaying Matrix rounded to 6 decimal places '%s' (%u / %u): \n----------------------------------------------\n[\n", name, static_cast<unsigned int>(m->size1), static_cast<unsigned int>(m->size2) );
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
          fprintf( stderr, "%.6g %s ", myDouble, sep1 );
        }
      }

      fprintf( stderr, "%s\n", sep2 );
    }
    fprintf( stderr, "]\n----------------------------------------------\n" );
}


/*****************************************************************
 function to find the 'square-root' of a matrix:
  the input matrix must be positive semi-definite symmetric matrix 
  note that covariance matrices have these properties */
gsl_matrix* Enfa::sqrtm(gsl_matrix* original_matrix) const
{
    int m_size, i, j;
    double lambda, v;

    /* make a copy of the input matrix */
    // Build a copy of the input matrix to work with
    m_size = original_matrix->size1;

    gsl_matrix* m = gsl_matrix_alloc (m_size, m_size);
    gsl_matrix_memcpy (m, original_matrix);

    //create temporary space for eigenvalues, eigenvectors & workspace
    gsl_vector* eigval_v = gsl_vector_alloc (m_size);
    gsl_matrix* eigvect_m = gsl_matrix_alloc (m_size, m_size);
    gsl_eigen_symmv_workspace * temp_v = gsl_eigen_symmv_alloc (m_size);

    gsl_eigen_symmv (m,
		     eigval_v,
		     eigvect_m,
		     temp_v);

    gsl_eigen_symmv_free(temp_v);

    /* allocate and initialise a new matrix temp_m  */
    /* this is eigvect_m scaled by the root of the eigenvalues */
    gsl_matrix* temp_m = gsl_matrix_alloc (m_size, m_size);
    for (j=0; j<m_size; ++j) {
	// ignore very small negative numbers
	v=gsl_vector_get(eigval_v,j);
	if (v<0 && v>-0.000001)
	{lambda=0.0;}
	else if (v>0)
	{lambda=pow(v,0.5);}
	else
	{
	  std::string msg = "Enfa::sqrtm:Cannot calculate square root for matrix - model will fail\n", v;
	  Log::instance()->error( msg.c_str() );
	  throw InverseFailedException( msg.c_str() );
	}

	for (i=0; i<m_size; ++i) {
	    gsl_matrix_set(temp_m, i, j, gsl_matrix_get(eigvect_m,i,j)*lambda);
	}
    }

    /* calculate the square root =temp_m * eigvect' */
    gsl_matrix* _root = gsl_matrix_alloc (m_size, m_size);
    // multiply using blas (note transpose for second item)
    gsl_blas_dgemm(CblasNoTrans, CblasTrans,
		   1.0, temp_m, eigvect_m,
		   0.0, _root);

    /* code for checking the root - not really needed */
    /**
    gsl_matrix *_testRoot = gsl_matrix_alloc(m_size, m_size);
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,
		   _root,_root,0.0,_testRoot);

    int mycomp, myval, myfailed;
    myfailed=0;
    for (int i=0; i<m_size; i++)
	for (int j=0; j<m_size; j++)
	{
	    // check values equal identity matrix (to accuracy of 6dp)
	    myval=(int)(round(gsl_matrix_get(_testRoot,i,j)*pow(10,6))/pow(10,6));
	    mycomp=(int)(round(gsl_matrix_get(original_matrix,i,j)*pow(10,6))/pow(10,6));
	    if (myval!=mycomp)
	    {
		//Log::instance()->info( "Enfa::inverse failed to invert matrix!\n" );
		displayMatrix(original_matrix, "input matrix", true);
		displayMatrix(_root, "root", true);
		displayMatrix(_testRoot, "check root", true);
	      std::string msg = "Enfa::sqrtm:Cannot calculate square root for matrix\n", v;
	      Log::instance()->error( msg.c_str() );
	      throw InverseFailedException( msg.c_str() );
	    }
	}
    **/

    gsl_matrix_free(temp_m);
    gsl_matrix_free(m);
    gsl_matrix_free(eigvect_m);
    gsl_vector_free(eigval_v);

    return _root;

}

/*****************************************************************
 * Calculate the geometric mean of the distance from point v to 
   all species observation points in factored environmental space
   weighting each factor accordingly 
*****************************************************************/
double Enfa::getGeomean(gsl_vector* v) const
{

    //Log::instance()->info( "enfa::getGeomean\n" );

    //if (_verboseDebug)
    //displayVector(v, "getGeomean input vector v:", true);

    gsl_vector * tmp_v = gsl_vector_alloc(_retained_components_count);
    gsl_vector * tmp_gsl_factor_weights = gsl_vector_alloc(_retained_components_count);

    for (int i=0; i<_retained_components_count; ++i)
      {
	gsl_vector_set(tmp_v, i,gsl_vector_get(v,i));
	gsl_vector_set(tmp_gsl_factor_weights, i, gsl_vector_get(_gsl_factor_weights,i));
      }
    
    gsl_vector * tmp_distance_vector = gsl_vector_alloc(_retained_components_count);
    //gsl_vector * tmp_distance_vector = gsl_vector_alloc(_layer_count);

    gsl_vector_view tmp_matrix_row_view;
    double tmp_geomean=1.0;
    double tmp_dist_workspace;

    // loop through localities
    for (int i=0; i<_localityCount; ++i)
    {
	tmp_matrix_row_view = gsl_matrix_row(_gsl_environment_factor_matrix,i);
	//displayVector(&tmp_matrix_row_view.vector, "getGeomean: locality vector i", true);
	//for (int j=0; j<_layer_count; ++j)
	// dont need to calculate beyonde the retained components
	for (int j=0; j<_retained_components_count; ++j)
		    gsl_vector_set(tmp_distance_vector, j,
			   gsl_vector_get(&tmp_matrix_row_view.vector,j));
	//displayVector(tmp_distance_vector, "getGeomean: locality vector i copy ", true);

	// difference between cell and current locality
	gsl_vector_sub(tmp_distance_vector,tmp_v);
	//gsl_vector_sub(tmp_distance_vector,v);
	//displayVector(tmp_distance_vector, "getGeomean: locality vector i subbed ", true);

	// square the differences
	gsl_vector_mul(tmp_distance_vector, tmp_distance_vector);
	//displayVector(tmp_distance_vector, "getGeomean: locality vector i squared ", true);
	// multiply by the factor weights
	gsl_vector_mul(tmp_distance_vector,tmp_gsl_factor_weights);
	//gsl_vector_mul(tmp_distance_vector,_gsl_factor_weights);
	// square and sum the values to get the distance
	//displayVector(tmp_distance_vector, "getGeomean: locality vector i weighted ", true);

	tmp_dist_workspace=pow(gsl_blas_dasum(tmp_distance_vector),0.5);
	//Log::instance()->info( "euclidean distance: %6.2f\n", tmp_dist_workspace );

	// accumulate for geometric mean calculation
	// if (tmp_dist_workspace!=0) tmp_geomean*=tmp_dist_workspace;
        // log transform values to reduce chance of float overflow
        if (tmp_dist_workspace!=0) tmp_geomean+=log(tmp_dist_workspace);
	//Log::instance()->info( "accumulating distance: %6.2f\n", tmp_geomean );
    }

    //Log::instance()->info( "tmp_geomean1: %6.2f\n", tmp_geomean );    

    // finally work out geometric mean of distances to the species points
    // tmp_geomean=pow(tmp_geomean,(double)1/_localityCount);
    // reverse the log transformation 
    tmp_geomean=exp(tmp_geomean/_localityCount);

    //if (_verboseDebug)
    //  Log::instance()->info( "getgeomean returns: %6.2f\n", tmp_geomean );    

    gsl_vector_free(tmp_distance_vector);
    gsl_vector_free(tmp_v);
    gsl_vector_free(tmp_gsl_factor_weights);

    return tmp_geomean;
}

/* calculate the inverse using cholesky decomposition */
gsl_matrix* Enfa::inverse(gsl_matrix* _m) const
{
    // dont crash on error - catch it
    gsl_error_handler_t* old_handler = gsl_set_error_handler_off();

    int m_size=_m->size1;
    gsl_matrix*_mcopy = gsl_matrix_alloc(m_size, m_size);
    gsl_matrix*_inverse = gsl_matrix_alloc(m_size, m_size);
    gsl_matrix_set_identity(_inverse);
    gsl_matrix_memcpy(_mcopy,_m);
    _gsl_vector_view _MI;

    int choldecomp;
    choldecomp = gsl_linalg_cholesky_decomp(_mcopy);
    //Log::instance()->info( "Cholesky decomp result %i\n", choldecomp );

    int cholsvx;
    for (int i=0; i<m_size; i++)
    {
	_MI=gsl_matrix_row(_inverse,i);
	cholsvx = gsl_linalg_cholesky_svx(_mcopy, &_MI.vector);
	//Log::instance()->info( "Cholesky svx result %i\n", cholsvx );
	//displayVector(&_MI.vector, "&MI.vector", true);
    }


    // sometimes the inverse fails - check it is correct
    gsl_matrix *_testInverse = gsl_matrix_alloc(m_size, m_size);
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,
		   _m,_inverse,0.0,_testInverse);

    int mycomp, myval, myfailed;
    myfailed=0;
    for (int i=0; i<m_size; i++)
	for (int j=0; j<m_size; j++)
	{
	    // check values equal identity matrix (to accuracy of Xdp)
	    myval=(int)(ceil(gsl_matrix_get(_testInverse,i,j)*pow((double)10,6)-0.49999999)/pow((double)10,6));
	    mycomp = (i == j) ? 1 : 0;
	    if (myval!=mycomp)
	    {
		Log::instance()->warn( "Enfa::inverse failed to invert matrix!\n" );
		//displayMatrix(_m, "input matrix", true);
		//displayMatrix(_inverse, "inverse", true);
		//displayMatrix(_testInverse, "check inverse", true);
		std::string msg = "Enfa::inverse failed to invert matrix\n";
		Log::instance()->error( msg.c_str() );
		throw InverseFailedException( msg.c_str() );
	    }
	}

    gsl_matrix_free(_mcopy);
    gsl_matrix_free(_testInverse);

    // restore error handler
    gsl_set_error_handler (old_handler);
	
    return _inverse;
}

/***********************/
/**** autoCovariance ***/
/* calculate covariance matrix as  cov(<matrix>) matlab/octave function  */
gsl_matrix * Enfa::autoCovariance(gsl_matrix * original_matrix)
{
  int j;

  int numrows = original_matrix->size1;
  int numcols = original_matrix->size2;

  // Build a copy of the input matrix to work with
  gsl_matrix * m = gsl_matrix_alloc (numrows, numcols);
  gsl_matrix_memcpy (m, original_matrix);


  if (numrows == 1)
  {
    gsl_matrix_transpose(m);
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

  // x / (n - 1)
  //gsl_matrix_scale (m, (double)1/(numrows-1));

  // multiply by x'
  gsl_matrix * p = gsl_matrix_alloc (numcols, numcols);
  gsl_blas_dgemm(CblasTrans,CblasNoTrans,1.0,
		 m,m,0.0,p);

  // scale the result
  gsl_matrix_scale (p, (double)1/(numrows-1));

  gsl_matrix_free (m);
  gsl_matrix_free (s);

  return p;
}

/**********************************************/
/* decide which components to retain/discard  */
/* using the method defined by _discardMethod */
/**********************************************/
int Enfa::discardComponents() const
{
    double variationTotal=0;
    int returnValue=_layer_count;

    // take the top X components
    if (_discardMethod==0)
    {
	Log::instance()->info( "Discarding components with fixed method (0)\n");
	returnValue=_retainComponents;
	for (int i=0; i<_retainComponents; ++i)
	    variationTotal+=gsl_vector_get(_gsl_factor_weights_all_components,i);
    }
    // take the components that account for X % of variation
    else if (_discardMethod==1)
    {
	Log::instance()->info( "Discarding components with variation method (1)\n");
	for (int i=0; i<_layer_count; ++i)
	{
	    variationTotal+=gsl_vector_get(_gsl_factor_weights_all_components,i);
	    if (variationTotal>=_retainVariation)
	    {
		// variation exceeds specified amount - keep these components
		returnValue=i+1;
		break;
	    }
	}
    }
    // take the components with variation higher than broken stick distribution
    else if (_discardMethod==2)
    {
	Log::instance()->info("Discarding components: Broken stick method (2)\n");
        /* work out the broken stick distribution of expected eigenvalues
	 * based on Jackson's formula for the kth eigenvalue
	 * bk = sum[i=k...p](1/i) where p=number of eigenvalues */
	gsl_vector* _brokenStickDist = gsl_vector_alloc(_layer_count);
	for (int i=0; i<_layer_count; ++i)
	{
	    double tmp_total=0;
	    for (int j=i+1; j<=_layer_count; ++j)
		tmp_total+=(double)1/j;
	    gsl_vector_set(_brokenStickDist,i, (double)tmp_total/_layer_count);
	}

	double myVariation;
	// now compare with the observed variation
	for (int i=0; i<_layer_count; ++i)
	{
	    myVariation=gsl_vector_get(_gsl_factor_weights_all_components,i);
	    if (myVariation < gsl_vector_get(_brokenStickDist,i))
	    {
		if (i==0) // problem - no components selected
		{
		    Log::instance()->warn( "First component explains less variation than the broken stick distribution - retaining all components by default\n" );
		    variationTotal=1;
		    returnValue=_layer_count;
		    break;
		}
		else // this component is discarded but keep earlier ones
		{
		    returnValue=i;
		    break;
		}
	    }
	    variationTotal+=myVariation;
	}
	gsl_vector_free(_brokenStickDist);
    }
    else
    {
	Log::instance()->warn( "Unknown problem whilst discarding components, retaining all components by default\n");
	variationTotal=1;
    }
    Log::instance()->info( "Retained components: %i/%i, variation explained: %.2f\n", returnValue, _layer_count, variationTotal);
    return returnValue;
}



/** Enfa1 is used to produce the model definition */
bool Enfa::enfa1()
{

  //calculate the mean and std deviation for background points
  //Log::instance()->info( "Calculating mean and stddev for background points\n" );
  _gsl_avg_background_vector = gsl_vector_alloc (_gsl_background_matrix->size2);
  _gsl_stddev_background_vector = gsl_vector_alloc (_gsl_background_matrix->size2) ;
  calculateMeanAndSd(_gsl_background_matrix,_gsl_avg_background_vector,_gsl_stddev_background_vector);

  if (_verboseDebug)
    {
      displayVector(_gsl_avg_background_vector, "Background Means", true);
      displayVector(_gsl_stddev_background_vector, "Background Stdev", true);
    }

  //calculate the mean and std deviation for centered presence points
  _gsl_avg_vector = gsl_vector_alloc (_gsl_environment_matrix->size2);
  _gsl_stddev_vector = gsl_vector_alloc (_gsl_environment_matrix->size2) ;
  calculateMeanAndSd(_gsl_environment_matrix,_gsl_avg_vector,_gsl_stddev_vector);

  if (_verboseDebug)
    {
      displayVector(_gsl_avg_vector, "Species Means", true);
      displayVector(_gsl_stddev_vector, "Species Stdev", true);
    }

  //center and standardise the data based on background means
  center(_gsl_environment_matrix,_localityCount);
  center(_gsl_background_matrix,_backgroundCount);

  //displayMatrix(_gsl_background_matrix, "_gsl_background_matrix", true);

  //calculate the mean and std deviation for centered presence points
  calculateMeanAndSd(_gsl_environment_matrix,_gsl_avg_vector,_gsl_stddev_vector);

  if (_verboseDebug)
    {
      displayVector(_gsl_avg_vector, "Species Means - centered", true);
      displayVector(_gsl_stddev_vector, "Species Stdev - centered", true);
    }

  //Now calculate the covariance matrix:
  //Log::instance()->info( "Calculating covariance matrix\n" );
  _gsl_covariance_matrix = autoCovariance(_gsl_environment_matrix);
  if (_verboseDebug)
    displayMatrix(_gsl_covariance_matrix, "Species Covariance Matrix", true);

  // now get covariance for background data
  _gsl_covariance_background_matrix = autoCovariance(_gsl_background_matrix);
  if (_verboseDebug)
    displayMatrix(_gsl_covariance_background_matrix, "Background Covariance Matrix", true);

  // get root inverse of species covariance matrix
  _gsl_covariance_matrix_root_inverse = gsl_matrix_alloc(_gsl_covariance_matrix->size1,_gsl_covariance_matrix->size2);
  gsl_matrix* gsl_sqrtm_matrix = sqrtm(_gsl_covariance_matrix);
  _gsl_covariance_matrix_root_inverse = inverse(gsl_sqrtm_matrix);
  gsl_matrix_free(gsl_sqrtm_matrix);

  //z = cov_species^-0.5 * m';
  _gsl_workspace_z = gsl_vector_alloc (_layer_count);
  gsl_blas_dgemv (CblasNoTrans, 1.0, _gsl_covariance_matrix_root_inverse, _gsl_avg_vector, 0.0, _gsl_workspace_z);

  if (_verboseDebug)
    displayVector(_gsl_workspace_z, "z", true);

  //W = cov_species^-0.5 * cov_global * cov_species^-0.5;
  gsl_matrix* _gsl_workspace_W_temp = gsl_matrix_alloc(_layer_count, _layer_count);
  _gsl_workspace_W = gsl_matrix_alloc(_layer_count, _layer_count);
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,
		 _gsl_covariance_matrix_root_inverse,
		 _gsl_covariance_background_matrix,0.0,
		 _gsl_workspace_W_temp);

  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,
		 _gsl_workspace_W_temp, 
		 _gsl_covariance_matrix_root_inverse,0.0,
		 _gsl_workspace_W);

  gsl_matrix_free(_gsl_workspace_W_temp);
  
  if (_verboseDebug)
    displayMatrix(_gsl_workspace_W, "W", true);

  //y = z / sqrtm(z' * z);
  //require view of z as an Nx1 matrix rather than a vector
  gsl_matrix_view _gsl_workspace_z_matrix_view;
  _gsl_workspace_z_matrix_view = gsl_matrix_view_vector(_gsl_workspace_z,
							1,_layer_count);
  double zz;
  gsl_blas_ddot(_gsl_workspace_z, _gsl_workspace_z, &zz);

  _gsl_workspace_y = gsl_matrix_alloc(1,_layer_count);
  gsl_matrix_memcpy(_gsl_workspace_y, &_gsl_workspace_z_matrix_view.matrix);
  gsl_matrix_scale(_gsl_workspace_y,pow(zz, -0.5));

  if (_verboseDebug)
    displayMatrix(_gsl_workspace_y, "y", true);

  //eye(N)=identity matrix with dimension NxN (use gsl_matrix_set_identity())
  //H = (eye(no_egvs) - y * y') * W * (eye(no_egvs) - y * y');
  _gsl_workspace_H = gsl_matrix_alloc(_layer_count,_layer_count);
  gsl_matrix* _gsl_workspace_H_temp1 = gsl_matrix_alloc(_layer_count,_layer_count);
  gsl_matrix* _gsl_workspace_H_temp2 = gsl_matrix_alloc(_layer_count,_layer_count);
  gsl_matrix* _gsl_workspace_H_temp3 = gsl_matrix_alloc(_layer_count,_layer_count);

  //initialise identity matrix
  gsl_matrix_set_identity(_gsl_workspace_H_temp1);

  // first step work out (y * y')
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, 
		 _gsl_workspace_y, 
		 _gsl_workspace_y,
		 0.0, _gsl_workspace_H_temp2);

  // now subtract from identity
  gsl_matrix_sub(_gsl_workspace_H_temp1,_gsl_workspace_H_temp2);

  // product of this with W
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, 
		 _gsl_workspace_H_temp1, 
		 _gsl_workspace_W,
		 0.0, _gsl_workspace_H_temp3);
  
  // finally product of this with temp1
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, 
		 _gsl_workspace_H_temp3, 
		 _gsl_workspace_H_temp1, 
		 0.0, _gsl_workspace_H);

  gsl_matrix_free(_gsl_workspace_H_temp1);
  gsl_matrix_free(_gsl_workspace_H_temp2);
  gsl_matrix_free(_gsl_workspace_H_temp3);

  if (_verboseDebug)
    displayMatrix(_gsl_workspace_H, "H", true);

  //work out eigen-vectors/values of H
  _gsl_eigenvalue_vector = gsl_vector_alloc (_layer_count);
  gsl_vector * _gsl_eigenvalue_vector_copy = gsl_vector_alloc (_layer_count);
  _gsl_eigenvector_matrix = gsl_matrix_alloc (_layer_count, _layer_count);
  gsl_eigen_symmv_workspace * _gsl_eigen_workpace = gsl_eigen_symmv_alloc (_layer_count);
  gsl_eigen_symmv (_gsl_workspace_H,
		   _gsl_eigenvalue_vector,
		   _gsl_eigenvector_matrix,
		   _gsl_eigen_workpace);

  //free the temporary workspace
  gsl_eigen_symmv_free (_gsl_eigen_workpace);

  /* Replace the min eigenvalue with the difference of the trace of W & H
     this becomes eigenvalue 1, the remaining eigenvalues move to index 2+
     t = eigenvalues ~= min(eigenvalues);
     eigenvalues(2:no_egvs) = eigenvalues(t);
     eigenvalues(1) = trace(W) - trace(H)*/

  _gsl_vector_min = gsl_vector_min_index(_gsl_eigenvalue_vector);
  //Log::instance()->info( "min eigevalue: %i\n",  _gsl_vector_min);
  //Log::instance()->info( "min eigevalue: %6.2f\n",  gsl_vector_min(_gsl_eigenvalue_vector));

  gsl_vector_memcpy (_gsl_eigenvalue_vector_copy,
		     _gsl_eigenvalue_vector);

  // note starting at 1 as position 0 will be overwritten later
  for (int i=1; i<_layer_count; i++)
  {
      if (_gsl_vector_min>=i)
      {
	  gsl_vector_set(_gsl_eigenvalue_vector,i,
			 gsl_vector_get(_gsl_eigenvalue_vector_copy,i-1));
      }
  }

  //displayVector(_gsl_eigenvalue_vector, "_gsl_eigenvalue_vector:A", true);
  //displayVector(_gsl_eigenvalue_vector_copy, "_gsl_eigenvalue_vector_copy:A", true);

  gsl_vector_free(_gsl_eigenvalue_vector_copy);

  // now set first eigenvalue to trace(W) - trace(H)
  double traceW=0.0;
  for (int i=0; i<_layer_count; ++i) traceW+=gsl_matrix_get(_gsl_workspace_W, i, i);
  double traceH=0.0;
  for (int i=0; i<_layer_count; ++i) traceH+=gsl_matrix_get(_gsl_workspace_H, i, i);
  gsl_vector_set(_gsl_eigenvalue_vector,0,traceW-traceH);

  /* % Figure out which eigenvalue has gone AWOL
     score_matrix = cov_species^-0.5 * v;*/

  _gsl_score_matrix=gsl_matrix_alloc(_layer_count,_layer_count);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, 
		 _gsl_covariance_matrix_root_inverse,
		 _gsl_eigenvector_matrix,
		 0.0, _gsl_score_matrix);

  /* remove column associated with dodgy eigenvector (_gsl_vector_min) */
  /* Replace the first column with the vector of means */
  /* and the move other columns down */
  gsl_matrix * _gsl_score_matrix_copy=gsl_matrix_alloc(_layer_count,_layer_count);
  gsl_matrix_memcpy(_gsl_score_matrix_copy, _gsl_score_matrix);

  for (int i=0; i<_layer_count; ++i)
      for (int j=0; j<_layer_count; ++j)
      {
	  if (j==0)
	  {
	    gsl_matrix_set(_gsl_score_matrix,i,j,gsl_vector_get(_gsl_avg_vector,i));
	  }
	  else if (_gsl_vector_min>=j)
	  {	      
	    gsl_matrix_set(_gsl_score_matrix,i,j,
			   gsl_matrix_get(_gsl_score_matrix_copy,i,j-1));
	  }
      }

  gsl_matrix_free(_gsl_score_matrix_copy);

  double score2norm;
  /* Norm the columns of the score_matrix
     - that is divide each value by the 2-norm of its column */
  for (int i=0; i<_layer_count; ++i)
  {
      // work out the 2-norm for this column = sqrt(sum(x^2))
      double unorm=0.0;
      for (int j=0; j<_layer_count; ++j)
	unorm+=pow(gsl_matrix_get(_gsl_score_matrix,j,i),2);
      unorm=pow(unorm,0.5);

      //Log::instance()->info( "col i: %i norm: %6.2f\n",  i, unorm);

      // and apply this normalisation to the matrix
      for (int j=0; j<_layer_count; ++j)
      {
	score2norm = gsl_matrix_get(_gsl_score_matrix,j,i);
	//Log::instance()->info( "score 2 norm: %6.2f\n",  score2norm);
	score2norm = score2norm/unorm;
	//Log::instance()->info( "score 2 normed: %6.2f\n",  score2norm);
	gsl_matrix_set(_gsl_score_matrix,j,i,score2norm);
      }
  }

  /* sort eigenvalues and eigenvectors */
  gsl_vector * _gsl_eigenvalue_vector_presort = gsl_vector_alloc(_layer_count);
  gsl_vector_memcpy(_gsl_eigenvalue_vector_presort, _gsl_eigenvalue_vector);
  gsl_eigen_symmv_sort (_gsl_eigenvalue_vector, _gsl_eigenvector_matrix,
			GSL_EIGEN_SORT_VAL_DESC);
  gsl_eigen_symmv_sort (_gsl_eigenvalue_vector_presort, _gsl_score_matrix,
			GSL_EIGEN_SORT_VAL_DESC);
  //Log::instance()->info( "Eigenvector sorted\n" );
  if (_verboseDebug)
  {
      displayVector(_gsl_eigenvalue_vector, "Eigenvalues", true);
      displayMatrix(_gsl_eigenvector_matrix, "Eigenvectors", true);
      displayMatrix(_gsl_score_matrix, "Score Matrix", true);
  }
  gsl_vector_free(_gsl_eigenvalue_vector_presort);

  /* marginality = geometric mean of means 
     marginality = sqrt(sum(m.^2));
     marginality = marginality / 1.96 */

  _marginality=0.0;
  for (int i=0; i<_layer_count; ++i)
      _marginality+=pow(gsl_vector_get(_gsl_avg_vector,i),2);
  _marginality=pow(_marginality,0.5)/1.96;
  Log::instance()->info( "Marginality: %6.2f\n", _marginality );

  /* Calculate global specialization
     specialization = sqrt(sum(eigenvalues)/length(m)) */
  _specialisation=pow((gsl_blas_dasum(_gsl_eigenvalue_vector)/_layer_count),0.5);
  Log::instance()->info( "Specialisation: %6.2f\n", _specialisation );

  /* work out factor weights = eigenvalues/sum(eigenvalues) 
   * dicarded components are given a zero weighting */
  _gsl_factor_weights_all_components = gsl_vector_alloc(_layer_count);
  for (int i=0; i<_layer_count; ++i)
  {
      gsl_vector_set(_gsl_factor_weights_all_components,i,
		     fabs(gsl_vector_get(_gsl_eigenvalue_vector,i)));
  }
  gsl_vector_scale(_gsl_factor_weights_all_components, 1.0/gsl_blas_dasum(_gsl_factor_weights_all_components));

  //After the model is generated, we can discard unwanted components!
  _retained_components_count=discardComponents();

  /* work out factor weights = eigenvalues/sum(eigenvalues) 
   * dicarded components are given a zero weighting */
  _gsl_factor_weights = gsl_vector_alloc(_layer_count);
  gsl_vector_set_zero(_gsl_factor_weights);
  for (int i=0; i<_retained_components_count; ++i)
  {
      gsl_vector_set(_gsl_factor_weights,i,
		     fabs(gsl_vector_get(_gsl_eigenvalue_vector,i)));
  }
  gsl_vector_scale(_gsl_factor_weights, 1.0/gsl_blas_dasum(_gsl_factor_weights));

  // print factor weights
  for (int i=0; i<_retained_components_count; ++i)
  {
      Log::instance()->info("Factor %i weight: %.2f, variation explained: %.2f\n", i, gsl_vector_get(_gsl_factor_weights,i), gsl_vector_get(_gsl_factor_weights_all_components,i));
  }

  /* work out geometric mean of the distance between each species locality and 
     every other locality based on the factored data */
  _gsl_geomean_vector = gsl_vector_alloc(_localityCount);
  _gsl_environment_factor_matrix = gsl_matrix_alloc(_localityCount,_layer_count);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
		 1.0, _gsl_environment_matrix, _gsl_score_matrix, 
		 0.0, _gsl_environment_factor_matrix);

  gsl_vector_view _gsl_geomean_workspace;

  double _tmp_geomean;

  //Log::instance()->info( "Calculating geometric means\n" );
  //displayMatrix(_gsl_environment_factor_matrix, "_gsl_environment_factor_matrix", true);

  // loop through the localities and calculate the geomean
  for (int i=0; i<_localityCount; ++i)
  {
      _gsl_geomean_workspace=gsl_matrix_row(_gsl_environment_factor_matrix,i);
      _tmp_geomean=getGeomean(&_gsl_geomean_workspace.vector);
      gsl_vector_set(_gsl_geomean_vector, i, _tmp_geomean);
  }

  // finally sort the geomeans to speed up processing later
  gsl_sort_vector(_gsl_geomean_vector);

  Log::instance()->info( "ENFA Model Generation Completed\n" );

  return true;

}



/****************************************************************/
/****************** configuration *******************************/
void
Enfa::_getConfiguration( ConfigurationPtr& config ) const
{
  if (!_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("Enfa") );
  config->addSubsection( model_config );

  // _marginality
  model_config->addNameValue( "Marginality", _marginality );

  // _specialisation
  model_config->addNameValue( "Specialisation", _specialisation );

  // _retained_components_count
  model_config->addNameValue( "RetainedComponents", _retained_components_count );

  double *values = new double[_layer_count];

  // _gsl_avg_background_vector
  for (int i=0; i < _layer_count; ++i)
    values[i] = gsl_vector_get(_gsl_avg_background_vector, i);

  model_config->addNameValue( "AvgBackgroundVector", values, _layer_count );

  // _gsl_stddev_background_vector
  for (int i=0; i < _layer_count; ++i)
    values[i] = gsl_vector_get(_gsl_stddev_background_vector, i);

  model_config->addNameValue( "StddevBackgroundVector", values, _layer_count );

  // _gsl_avg_vector
  for (int i=0; i < _layer_count; ++i)
    values[i] = gsl_vector_get(_gsl_avg_vector, i);

  model_config->addNameValue( "AvgVector", values, _layer_count );

  // _gsl_stddev_vector
  for (int i=0; i < _layer_count; ++i)
    values[i] = gsl_vector_get(_gsl_stddev_vector, i);

  model_config->addNameValue( "StddevVector", values, _layer_count );

  // _gsl_eigenvalue_vector
  for (int i=0; i < _layer_count; ++i)
    values[i] = gsl_vector_get(_gsl_eigenvalue_vector, i);

  model_config->addNameValue( "EigenvalueVector", values, _layer_count );

  // _gsl_factor_weights
  for (int i=0; i < _layer_count; ++i)
    values[i] = gsl_vector_get(_gsl_factor_weights, i);

  model_config->addNameValue( "FactorWeights", values, _layer_count );

  delete[] values;

  // _gsl_eigenvector_matrix
  int num_cells = _layer_count * _layer_count;

  double *flat_eigenvector_matrix = new double[num_cells];

  int cnt = 0;

  for (int i=0; i < _layer_count; ++i)
    for (int j=0; j < _layer_count; ++j, ++cnt)
      flat_eigenvector_matrix[cnt] = gsl_matrix_get( _gsl_eigenvector_matrix, i, j );

  model_config->addNameValue( "EigenvectorMatrix", flat_eigenvector_matrix, num_cells );

  delete[] flat_eigenvector_matrix;

  // _gsl_score_matrix
  num_cells = _layer_count * _layer_count;

  double *flat_score_matrix = new double[num_cells];

  cnt = 0;

  for (int i=0; i < _layer_count; ++i)
    for (int j=0; j < _layer_count; ++j, ++cnt)
      flat_score_matrix[cnt] = gsl_matrix_get( _gsl_score_matrix, i, j );

  model_config->addNameValue( "ScoreMatrix", flat_score_matrix, num_cells );

  delete[] flat_score_matrix;

  // _gsl_environment_factor_matrix
  num_cells = _localityCount * _layer_count;

  double *flat_environment_factor_matrix = new double[num_cells];

  cnt = 0;

  for (int i=0; i < _localityCount; ++i)
    for (int j=0; j < _layer_count; ++j, ++cnt)
      flat_environment_factor_matrix[cnt] = gsl_matrix_get( _gsl_environment_factor_matrix, i, j );

  model_config->addNameValue( "EnvironmentFactorMatrix", flat_environment_factor_matrix, num_cells );

  delete[] flat_environment_factor_matrix;

  // _gsl_geomean_vector
  values = new double[_localityCount];

  for (int i=0; i < _localityCount; ++i)
    values[i] = gsl_vector_get(_gsl_geomean_vector, i);

  model_config->addNameValue( "LocalityGeomeans", values, _localityCount );

  delete[] values;

}

void
Enfa::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "Enfa",false );

  if (!model_config)
    return;

  // _retained_components_count
  _retained_components_count = model_config->getAttributeAsInt( "RetainedComponents", 0 );

  // _marginality
  _marginality = model_config->getAttributeAsDouble( "Marginality", 0.0 );

  // _specialisation
  _specialisation = model_config->getAttributeAsDouble( "Specialisation", 0.0 );

  // _gsl_avg_vector
  std::vector<double> stl_vector = model_config->getAttributeAsVecDouble( "AvgVector" );

  _layer_count = stl_vector.size();

  _gsl_avg_vector = gsl_vector_alloc( _layer_count );

  for (int i=0; i < _layer_count; ++i)
    gsl_vector_set( _gsl_avg_vector, i, stl_vector[i] );

  // _gsl_avg_background_vector
  stl_vector = model_config->getAttributeAsVecDouble( "AvgBackgroundVector" );

  _layer_count = stl_vector.size();

  _gsl_avg_background_vector = gsl_vector_alloc( _layer_count );

  for (int i=0; i < _layer_count; ++i)
    gsl_vector_set( _gsl_avg_background_vector, i, stl_vector[i] );

  // _gsl_stddev_vector
  stl_vector = model_config->getAttributeAsVecDouble( "StddevVector" );

  _gsl_stddev_vector = gsl_vector_alloc( _layer_count );

  for (int i=0; i < _layer_count; ++i)
    gsl_vector_set( _gsl_stddev_vector, i, stl_vector[i] );

  // _gsl_stddev_background_vector
  stl_vector = model_config->getAttributeAsVecDouble( "StddevBackgroundVector" );

  _gsl_stddev_background_vector = gsl_vector_alloc( _layer_count );

  for (int i=0; i < _layer_count; ++i)
    gsl_vector_set( _gsl_stddev_background_vector, i, stl_vector[i] );

  // _gsl_eigenvalue_vector
  stl_vector = model_config->getAttributeAsVecDouble( "EigenvalueVector" );

  _layer_count = stl_vector.size();

  _gsl_eigenvalue_vector = gsl_vector_alloc( _layer_count );

  for (int i=0; i < _layer_count; ++i)
    gsl_vector_set( _gsl_eigenvalue_vector, i, stl_vector[i] );

  // _gsl_factor_weights
  stl_vector = model_config->getAttributeAsVecDouble( "FactorWeights" );

  _layer_count = stl_vector.size();

  _gsl_factor_weights = gsl_vector_alloc( _layer_count );

  for (int i=0; i < _layer_count; ++i)
    gsl_vector_set( _gsl_factor_weights, i, stl_vector[i] );

  // _gsl_eigenvector_matrix
  stl_vector = model_config->getAttributeAsVecDouble( "EigenvectorMatrix" );

  _gsl_eigenvector_matrix = gsl_matrix_alloc( _layer_count, _layer_count );

  int cnt = 0;

  for (int i=0; i < _layer_count; ++i)
    for (int j=0; j < _layer_count; ++j, ++cnt)
      gsl_matrix_set( _gsl_eigenvector_matrix, i, j, stl_vector[cnt] );


  // _gsl_score_matrix
  stl_vector = model_config->getAttributeAsVecDouble( "ScoreMatrix" );

  _gsl_score_matrix = gsl_matrix_alloc( _layer_count, _layer_count );

  cnt = 0;

  for (int i=0; i < _layer_count; ++i)
    for (int j=0; j < _layer_count; ++j, ++cnt)
      gsl_matrix_set( _gsl_score_matrix, i, j, stl_vector[cnt] );


  // _gsl_environment_factor_matrix
  stl_vector = model_config->getAttributeAsVecDouble( "EnvironmentFactorMatrix" );

  _localityCount = stl_vector.size()/_layer_count;

  _gsl_environment_factor_matrix = gsl_matrix_alloc( _localityCount, _layer_count );

  cnt = 0;

  for (int i=0; i < _localityCount; ++i)
    for (int j=0; j < _layer_count; ++j, ++cnt)
      gsl_matrix_set( _gsl_environment_factor_matrix, i, j, stl_vector[cnt] );


  // _gsl_geomean_vector
  stl_vector = model_config->getAttributeAsVecDouble( "LocalityGeomeans" );

  _gsl_geomean_vector = gsl_vector_alloc( _localityCount );

  for (int i=0; i < _localityCount; ++i)
    gsl_vector_set( _gsl_geomean_vector, i, stl_vector[i] );

  _done = true;

}
