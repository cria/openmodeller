//
// GenericTemplate
//
// Description:
//
//
// Author: CRIA <t.sutton@reading.ac.uk>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "generictemplate.hh"


/** Constructor for GenericTemplate
   * 
   * @param Sampler is class that will fetch environment variable values at each occurrence / locality
   */
GenericTemplate::GenericTemplate( Sampler *samp )
{
  //set the class member that holds the number of environmental variables
  f_dim = samp->dim();
  //set the class member that holds the number of occurences
  f_localityCount = samp->numOccurrences();
}

/** This is the descructor for the GenericTemplate class */
GenericTemplate::~GenericTemplate()
{


}



//
// Methods used to build the model
//

/** Metadata field returning the name of this algorithm.
  * @note This method is inherited from the Algorithm class 
  * @return a character array containing the name of the algorithm
  */
char * GenericTemplate::name()
{
    return "GenericTemplate";
} //replace this name!

/** This method is used when you want to ensure that all variables in all
  * environmental layers are scaled to the same value range. 
  * @note This method is inherited from the Algorithm class
  * @param Scalar pointer min that the post normalised will be fitted to
  * @param Scalar pointer max that the post normalised will be fitted to
  * @return 0 if no normalisation is needed
  */
int GenericTemplate::needNormalization( Scalar *min, Scalar *max ) const
{

}


/** Initialise the model specifying a threshold / cutoff point.
  * This is optional (model dependent).
  * @note This method is inherited from the Algorithm class
  * @param 
  * @return 0 on error
  */
int GenericTemplate::initialize( int ncycle )
{

}


/** Start model execution (build the model).
  * @note This method is inherited from the Algorithm class
  * @return 0 on error 
  */
int GenericTemplate::iterate()
{

}


/** Use this method to find out if the model has completed (e.g. convergence
  * point has been met. 
  * @note This method is inherited from the Algorithm class
  * @return     
  * @return Implementation specific but usually 1 for completion.
  */
int GenericTemplate::done()
{

}


//
// Methods used to project the model
//


/** This method is used when projecting the model.
  * @note This method is inherited from the Algorithm class
  * @return     
  * @param Scalar *x a pointer to a vector of openModeller Scalar type (currently double). The vector should contain values looked up on the environmental variable layers into which the mode is being projected. */
Scalar GenericTemplate::getValue( Scalar const *x ) const
{

}

/** Returns a value that represents the convergence of the algorithm
  * expressed as a number between 0 and 1 where 0 represents model
  * completion. 
  * @return 
  * @param Scalar *val 
*/
int GenericTemplate::getConvergence( Scalar *val )
{

}
