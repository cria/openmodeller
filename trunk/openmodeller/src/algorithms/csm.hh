/**
 * This is a generic template intended to be used as the basis for
 * creating new algorithms.
 * 
 * @file
 * @author Tim Sutton (t.sutton@reading.ac.uk)
 * @date   2003-09-12
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2003 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details:
 * 
 * http://www.gnu.org/copyleft/gpl.html
 */
 
#ifndef CSM_H
#define CSM_H

#include "om.hh"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

/**

Herewith follows a detailed explanation of the Climate Space Model (CSM). 
Note that the CSM model was developed by Dr Neil Caithness.
This implementation of CSM was written by Tim Sutton and Renato De Giovanni.

//////////////////////////////////////////////////////
// Model Creation
//////////////////////////////////////////////////////

Inputs:

File contiaing xy point localties
List of gdal layers
----------------------------------

Look up values at each locality in each layer

  |x|y|var1|var2 |var3 |.... |varN        |
-------------------------------------------
1 | | |    |     |     |     |            |
-------------------------------------------
2 | | |    |     |     |     |            |
-------------------------------------------
3 | | |    |     |     |     |            |
-------------------------------------------
4 | | |    |     |     |     |            |
-------------------------------------------
5 | | |    |     |     |     |            |
-------------------------------------------
6 | | |    |     |     |     |            |
-------------------------------------------
7 | | |    |     |     |     |            |
-------------------------------------------
8 | | |    |     |     |     |            |
-------------------------------------------
etc.

Now remove any rows with nans in (GDAL NO_DATA)
Now remove any rows which are duplicates {optional step!]
After duplicates have been removed, lat and long columns can be removed.

Now we need to center and standardise the data (auto)
Before:
   . .  |
 .  ..  |
  . . . |
        |
-----------------
 .      |.
.       |
  .  .  |
   ..   |

After:
        |
        |
       .|.
      ..|..
-----------------
      ..|..
       .|.
        |
        |

To do this:

Calculate the mean for every column (excluding lat/long)
Calculate the stddev for every column (excluding lat/long)
Subtract the column mean from every value in that column
Divide each restultant column value by the stddev for that column
Make sure you remember the column stddev and mean for each column for later use.

Now calculate the covariance matrix:

Pass the data matrix to a covariance function (e.g. in GSL?) - note the datamatrix 
should not include the column stddev and mean values.
The resulting covariance matrix will have the same number of rows as columns i.e. it is square.
Note that the data in the covariance matrix no longer resembles the input point
lookup data!
-----------------------------------------
    |var1|var2 |var3 |.... |varN        |
-----------------------------------------
1   |    |     |     |     |            |
-----------------------------------------
2   |    |     |     |     |            |
-----------------------------------------
3   |    |     |     |     |            |
-----------------------------------------
... |    |     |     |     |            |
-----------------------------------------
N   |    |     |     |     |            |
-----------------------------------------


Now obtain the eigenvalues and eigenvector of the covariance matrix using GSL

The eigenvector will look something like this:

-------------------------------------------
      |  1 |  2  |  3  |.... |component N |
-------------------------------------------
Var 1 |    |     |     |     |            |
-------------------------------------------
Var 2 |    |      |     |     |           |
-------------------------------------------
Var 3 |    |     |      |     |           |
-------------------------------------------
..... |    |     |     |      |           |
-------------------------------------------
Var N |    |     |     |     |            |
-------------------------------------------

Each column represents one component, and each row represents one of the input variables transposed 
order of original covariance matrix columns. 
The cell values represent the loading / weight of that variable in that component.



The eigenvalues are the values through the diagonal of the output of the eigenvalues funtion. (prefixed with x above)
-------------------------------------------
      |  1 |  2  |  3  |.... |component N |
-------------------------------------------
Var 1 | x5 | 0   |  0  |  0  |      0     |
-------------------------------------------
Var 2 | 0  | x8  |  0  |  0  |      0     |
-------------------------------------------
Var 3 |  0 |  0  |  x1  |  0  |     0     |
-------------------------------------------
..... |  0 | 0   |  0  |  x4  |     0     |
-------------------------------------------
Var N |  0 |  0  |  0  |  0  |     xN     |
-------------------------------------------

This is a separate vector to the one created by the eigenvector function.
The sum of the eigenvalues should be equal to the number of columns!
Next we arrange the column order of the eigenvector according to the descending values of the 
eigenvalues.

-------------------------------------------
      |  2 |  1  |  4  |.... |component N |
-------------------------------------------
Var 1 | x8 |     |     |     |            |
-------------------------------------------
Var 2 |    |  x5 |     |     |           |
-------------------------------------------
Var 3 |    |     |  x4  |     |           |
-------------------------------------------
..... |    |     |     |  x1  |           |
-------------------------------------------
Var N |    |     |     |     |     xN     |
-------------------------------------------


The next step is to remove any column from the eigenvector where the eigenvalue is less than 1.
This is called the Keiser-Gutman method. There are other approaches to deciding which values to discard, which 
could be substituted at this be point.


-------------------------------------
      |  2 |  1  |  4  |component N |
-------------------------------------
Var 1 |    |     |     |            |
-------------------------------------
Var 2 |    |     |     |            |
-------------------------------------
Var 3 |    |     |     |            |
-------------------------------------
..... |    |     |     |            |
-------------------------------------
Var N |    |     |     |            |
-------------------------------------

That complete the CSM model definition



//////////////////////////////////////////////////////
// Model Projection:
//////////////////////////////////////////////////////

Inputs: 

Data layers that will be used as the basis for the model projection (must match the dimensions and units of the input dataset).
The standard deviation for each of the layers as calculated in the model definition process.
The mean of each layer as calculated in the model definition process.

Now for each layer visit each cell, subtract the mean (xbar) and divide the result by the standard deviation.
This step is called 'auto'.
Note these must be the mean and standard deviation particular to that layer as calculated in the model definition process.

Next we create the scores.
This is carried out by performing matrix multiplication - multiplying the independent variable layers (produced by auto above) by the eigenvectors.
The output is one new 'layer' (actually a component) for each of the components kept during the model building process.

 Layers after auto
+----------------+
|a               | Layer 1
|      + - - - - |---------+
|      | b       |         | Layer 2
|                |         |   .
|      |         |         |      .
+----------------+         |       Layer n
       |                   |
       |                   |
       +-------------------+



-------------------------------------
        | 2  |  1  |  4  |component N |
-------------------------------------
Layer 1 |    |     |     |            |
-------------------------------------
Layer 2 |    |     |     |            |
-------------------------------------
Layer 3 |    |     |     |            |
-------------------------------------
.....   |    |     |     |            |
-------------------------------------
Var N   |    |     |     |            |
-------------------------------------

@author Tim Sutton, Renato De Giovanni
*/
class Csm : public Algorithm
{
public:

    /** Constructor for Csm
      * 
      * @param Sampler is class that will fetch environment variable 
      * values at each occurrence / locality
      */
    Csm( Sampler *samp );
    /** This is the descructor for the Csm class */
    ~Csm();
    


  //
  // Methods used to build the model
  //
  
  /** Metadata field returning the name of this algorithm.
    * @note This method is inherited from the Algorithm class 
    * @return a character array containing the name of the algorithm
    */
  char *name()    { return "Csm"; } //replace this name!
  
  /** This method is used when you want to ensure that all variables in all
    * environmental layers are scaled to the same value range. 
    * @note This method is inherited from the Algorithm class
    * @param Scalar pointer min that the post normalised will be fitted to
    * @param Scalar pointer max that the post normalised will be fitted to
    * @return 0 if no normalisation is needed
    */
  int needNormalization( Scalar *min, Scalar *max );
  
  /** Initialise the model specifying a threshold / cutoff point.
    * This is optional (model dependent).
    * @note This method is inherited from the Algorithm class
    * @return 0 on error
    */
  int initialize( int ncycle );
  
  /** Start model execution (build the model).     
    * @note This method is inherited from the Algorithm class
    * @return 0 on error 
    */
  int   iterate();
  
  /** Use this method to find out if the model has completed (e.g. convergence
    * point has been met. 
    * @note This method is inherited from the Algorithm class
    * @return     
    * @return Implementation specific but usually 1 for completion.
    */
  int   done();

  //
  // Methods used to project the model
  //
  
  
  /** This method is used when projecting the model.  
    * @note This method is inherited from the Algorithm class
    * @return Scalar of the probablitiy of occurence    
    * @param Scalar *x a pointer to a vector of openModeller Scalar type (currently double). The vector should contain values looked up on the environmental variable layers into which the mode is being projected. */
  Scalar getValue( Scalar *x );
  
  /** Returns a value that represents the convergence of the algorithm
    * expressed as a number between 0 and 1 where 0 represents model
    * completion. 
    * @return 
    * @param Scalar *val 
  */
  int    getConvergence( Scalar *val );


private:
  /** This is a utility function to convert a Sampler to a gsl_matrix.
    * @return 0 on error
    */
    int SamplerToMatrix();
  
  /** Calculate the mean and standard deviation of the environment
    * variables at the occurence points.
    * @note This method must be called after SamplerToMatrix
    * @return 0 on error
    */
  int calculateMeanAndSd();
          
  /** Center and standardise.
    * Subtract the column mean from every value in each column
    * Divide each resultant column value by the stddev for that column
    * @note This method must be called after calculateMeanAndSd
    * @return 0 on error    
    */
  int center();
  
  /** Discard unwanted components.
    * Currently this is done using the Keiser-Gutman method
    * where all eigenvectors with an eigenvalue < 1 are discarded.
    * @note This method must be called after center
    * @return 0 on error    
    */  
  int discardComponents();
  
  /** This a utility function to display the content of a gsl vector.
   * @param v gsl_vector Input vector
   * @param name char Vector name / message
   */
  void displayVector(gsl_vector * v, char * name);

  /** This a utility function to display the content of a gsl matrix.
   * @param m gsl_matrix Input matrix
   * @param name char Matrix name / message
   */
  void displayMatrix(gsl_matrix * m, char * name);

  /** This a utility function to calculate a transposed gsl matrix.
   * @param m gsl_matrix Input matrix
   * @return gsl_matrix Transposed matrix
   */
  gsl_matrix * transpose (gsl_matrix * m);

  /** This a utility function to calculate the internal product of two gsl vectors.
   * @param va gsl_vector Input vector a
   * @param vb gsl_vector Input vector b
   * @return double Result
   */
  double product (gsl_vector * va, gsl_vector * vb);

  /** This a utility function to calculate the product between two gsl matrices.
   * @param a gsl_matrix Input matrix a
   * @param b gsl_matrix Input matrix b
   * @return gsl_matrix Output matrix
   */
  gsl_matrix * product (gsl_matrix * a, gsl_matrix * b);

  /** This a utility function to calculate the auto covariance of a gsl matrix.
   * @param m gsl_matrix Input matrix
   * @return gsl_matrix Output matrix
   */
  gsl_matrix * autoCovariance(gsl_matrix * m);
    
  
  /** This member variable is used to indicate whether the model 
    * building process has completed yet. */
  int    f_done;
  /** This is a pointer to a gsl matrix containing the 'looked up' environmental 
  * variables at each locality. It is converted to a gsl matrix from the oM 
  * Sampler.samples primitive structure. */
  gsl_matrix * f_gsl_environment_matrix;
  /** This is a pointer to a gsl matrix that will hold the covariance matrix generated
  from the environmental data matrix */
  gsl_matrix * f_gsl_covariance_matrix;
  /** This is a pointer to a gsl vector that will hold the mean of each environmental 
  variable column */
  gsl_vector * f_gsl_avg_vector;
  /** This is a pointer to a gsl vector that will hold the stddev of each environmental variable column */
  gsl_vector * f_gsl_stddev_vector;
  /** This is a pointer to a gsl vector that will hold the eigen values */
  gsl_vector * f_gsl_eigenvalue_vector;
  /** This is a pointer to a gsl matrix that will hold the eigen vectors */
  gsl_matrix * f_gsl_eigenvector_matrix;
  /** Dimension of environmental space. */
  int f_layer_count; 
  /** Number of components that are actually kept after Keiser-Gutman test */
  int f_retained_components_count;
  /** the number of localities used to construct the model */
  int f_localityCount; 

};

#endif
