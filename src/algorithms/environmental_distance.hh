//
// Generic environmental distance algorithm
//
// Description: Generic algorithm based on distances.
//
// Author:      Danilo J. S. Bellini <danilo.estagio@gmail.com>
// Copyright:   See COPYING file that comes with this distribution
// Date:        2006-09-18
//

#ifndef _DISTANCESH_
#define _DISTANCESH_

#include <openmodeller/om.hh>

// Matrix burocracy
#include "matrix.hh"
#ifndef _NO_NAMESPACE
using namespace std;
using namespace math;
#define STD std
#else
#define STD
#endif
#ifndef _NO_EXCEPTION
#  define TRYBEGIN()    try {
#  define CATCHERROR()  } catch (const STD::exception& e) { \
                     cerr << "Error: " << e.what() << endl; }
#else
#  define TRYBEGIN()
#  define CATCHERROR()
#endif
typedef matrix<Scalar> Matrix; // Now we have the matrix free for use

#define FIRST_DISTANCE_TYPE   1
#define AMOUNT_DISTANCE_TYPES 4

class EnvironmentalDistance : public AlgorithmImpl{

   public: // All methods here are inherited from AlgorithmImpl
      EnvironmentalDistance();  // Constructor, don't have init algorithm routines
      ~EnvironmentalDistance(); // Destructor

      int initialize();  // Called by oM to initialize the algorithm
      int done() const { return _done; } // Tell oM when the algorithm finished its work
      Scalar getValue(const Sample& x) const; // Returns the occurence probability

   private:
      // Common-use attributes
      bool _initialized;  // Flag to indicate that the algorithm was initialized.
      bool _done;         // Flag to indicate when the work is finished;
      int _layer_count;     // Amount of layers used (dimension of environmental space)
      int _presence_count;  // Amount of presence points
      std::vector<Sample> _presence_points; // Have the presence points data

      // Parameters
      Scalar _par_dist;
      int _par_dist_type;
      int _par_point_qnt;

      bool _use_chisq; // Flag indicating when chi-square probabilities should be returned (used only for Mahalanobis distance!)

      // Algorithm-specific methods and attributes
      void _calc_covariance_matrix();
      inline Scalar _distance(const Sample& x, const Sample& y) const;
      bool _init_distance_type();
      Matrix * _cov_matrix;    // Covariance matrix
      Matrix * _cov_matrix_inv; // Inverse of covariance matrix
      Sample _average_point; // Average of all presence points

      // Alias for the distance types
      typedef enum{
         EuclideanDistance = FIRST_DISTANCE_TYPE,
         MahalanobisDistance,
         ManhattanDistance,
         ChebyshevDistance,
      } DistanceType;

     /** Compute probability of probability of normal z value.
      *  Code based on a C implementation in the public domain written by Gary Perlman 
      *  from the Wang Institute, Tyngsboro, MA:
      *  http://www.stat.uchicago.edu/~mcpeek/software/CCQLSpackage1.3/z.c
      *  Original notes:
      *  Adapted from a polynomial approximation in:
      *     Ibbetson D, Algorithm 209
      *     Collected Algorithms of the CACM 1963 p. 616
      *  This routine has six digit accuracy, so it is only useful for absolute
      *  z values < 6.  For z values >= to 6.0, poz() returns 0.0
      * @param z normal z value
      * @return cumulative probability from -oo to z
      */
      Scalar _poz(Scalar z) const;

     /** Compute probability of chi square value for the result not being due to chance.
      *  Code based on a C implementation in the public domain written by Gary Perlman 
      *  from the Wang Institute, Tyngsboro, MA:
      *  http://www.stat.uchicago.edu/~mcpeek/software/CCQLSpackage1.3/chisq.c
      *  Original notes:
      *  Adapted from:
      *     Hill, I. D. and Pike, M. C.  Algorithm 299
      *     Collected Algorithms for the CACM 1967 p. 243
      *  Updated for rounding errors based on remark in ACM TOMS June 1985, page 185
      * @param x obtained chi-square value
      * @param df degrees of freedom (number of environmental variables - 1)
      * @return probability of chi square value.
      */
      Scalar _pochisq(Scalar x, int df) const;

   protected:
      virtual void _getConfiguration(ConfigurationPtr&) const;
      virtual void _setConfiguration(const ConstConfigurationPtr&);
};

#endif
