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
      int needNormalization(Scalar *min, Scalar *max) const; // Normalize all data to [min,max]
      Scalar getValue(const Sample& x) const; // Returns the occurence probability

   private:
      // Common-use attributes
      bool _initialized;  // Flag to indicate that the algorithm was initialized.
      bool _done;         // Flag to indicate when the work is finished;
      int layerCount;     // Amount of layers used (dimension of environmental space)
      int presenceCount;  // Amount of presence points
      std::vector<Sample> presencePoints; // Have the presence points data

      // Parameters
      Scalar ParDist;
      int ParDistType;
      int ParPointQnt;

      // Algorithm-specific methods and attributes
      void CalcCovarianceMatrix();
      inline Scalar Distance(const Sample& x, const Sample& y) const;
      void InitDistanceType();
      Matrix * covMatrix;    // Covariance matrix
      Matrix * covMatrixInv; // Inverse of covariance matrix
      Sample averagePoint; // Average of all presence points

      // Alias for the distance types
      typedef enum{
         EuclideanDistance = FIRST_DISTANCE_TYPE,
         MahalanobisDistance,
         ManhattanDistance,
         ChebyshevDistance,
      } DistanceType;

   protected:
      virtual void _getConfiguration(ConfigurationPtr&) const;
      virtual void _setConfiguration(const ConstConfigurationPtr&);
};

#endif
