//
// GenericTemplate
//
// Description:
//
//
// Author: CRIA <tim@linfiniti.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "generictemplate.hh"

// This include is only necessary if you want to work with normalized values
// ScaleNormalizer is one of the available normalizers. Choose the one you need.
#include <openmodeller/ScaleNormalizer.hh>

/** Constructor for GenericTemplate
   * 
   * @param Sampler is class that will fetch environment variable values at each occurrence / locality
   */
GenericTemplate::GenericTemplate()
{
  // Instantiate a normalizer object when it's necessary to work with normalized values
  _normalizerPtr = new ScaleNormalizer( 0.0, 1.0, true );
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


/** Initialise the model specifying a threshold / cutoff point.
  * This is optional (model dependent).
  * @note This method is inherited from the Algorithm class
  * @param 
  * @return 0 on error
  */
int GenericTemplate::initialize( int ncycle )
{
  //set the class member that holds the number of environmental variables
  f_dim = samp->dim();
  //set the class member that holds the number of occurences
  f_localityCount = samp->numOccurrences();
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
