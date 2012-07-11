//
// Generic environmental distance algorithm
//
// Description: Generic algorithm based on distances.
//
// Authors:      Danilo J. S. Bellini <danilo.estagio@gmail.com>
//               Renato De Giovanni <renato@cria.org.br>
// Copyright:   See COPYING file that comes with this distribution
// Date:        2006-09-18
//

#include "environmental_distance.hh"
#include <openmodeller/Exceptions.hh>
#include <openmodeller/ScaleNormalizer.hh>

//
// METADATA
//
#define DATA_MIN 0.0
#define DATA_MAX 1.0
#define NUM_PARAM 3

#define PARDISTTYPE "DistanceType"
#define PARPOINTQNT "NearestPoints"
#define PARDIST     "MaxDistance"
#define PARDISTMIN 0.0
#define PARDISTMAX 1.0

#define LOG_SQRT_PI 0.5723649429247000870717135 /* log (sqrt (pi)) */
#define I_SQRT_PI   0.5641895835477562869480795 /* 1 / sqrt (pi) */
#define BIGX        20.0  /* max value to represent exp (x) */
#define ex(x)       (((x) < -BIGX) ? 0.0 : exp (x))
#define Z_MAX       6.0   /* maximum meaningful z value */

static AlgParamMetadata parameters[NUM_PARAM] = { // Parameters
   { // 1st parameter
      PARDISTTYPE, // Id
      "Metric",    // Name
      Integer,     // Type
      "Metric used to calculate distances: " // Overview
         "1=Euclidean, "
         "2=Mahalanobis, "
         "3=Manhattan/Gower, "
         "4=Chebyshev.",
      "Metric used to calculate distances: " // Description
         "1=Euclidean, "
         "2=Mahalanobis, "
         "3=Manhattan/Gower, "
         "4=Chebyshev",
      1,                                               // Not zero if the parameter has lower limit
      FIRST_DISTANCE_TYPE,                             // Parameter's lower limit
      1,                                               // Not zero if the parameter has upper limit
      FIRST_DISTANCE_TYPE + AMOUNT_DISTANCE_TYPES - 1, // Parameter's upper limit
      "1"         // Parameter's typical (default) value
   },
   { // 2nd parameter
      PARPOINTQNT,            // Id
      "Nearest \'n\' points", // Name
      Integer,                // Type
      "Nearest \'n\' points whose mean value will be the reference when calculating environmental distances.", // Overview
      "Nearest \'n\' points whose mean value will be the reference when calculating environmental distances. When set to 1, distances will be measured to the closest point, which is the same behavior of the previously existing minimum distance algorithm. When set to 0, distances will be measured to the average of all presence points, which is the same behavior of the previously existing distance to average algorithm. Intermediate values between 1 and the total number of presence points are now accepted.", // Description
      1,          // Not zero if the parameter has lower limit
      0,          // Parameter's lower limit
      1,          // Not zero if the parameter has upper limit
      1000,       // Parameter's upper limit
      "1"         // Parameter's typical (default) value
   },
   { // 3rd parameter
      PARDIST,            // Id
      "Maximum distance", // Name
      Real,               // Type
      "Maximum distance to the reference in the environmental space.", // Overview
      "Maximum distance to the reference in the environmental space, above which the conditions will be considered unsuitable for presence. Since 1 corresponds to the biggest possible distance between any two points in the environment space, setting the maximum distance to this value means that all points in the environmental space will have an associated probability. The probability of presence for points that fall within the range of the maximum distance is inversely proportional to the distance to the reference point (linear decay). The only exception is when the maximum distance is 1 and the metric is Mahalanobis, which will produce probabilities following the chi-square distribution.", // Description
      1,          // Not zero if the parameter has lower limit
      PARDISTMIN, // Parameter's lower limit
      1,          // Not zero if the parameter has upper limit
      PARDISTMAX, // Parameter's upper limit
      "0.1"       // Parameter's typical (default) value
   },
};

static AlgMetadata metadata = { // General metadata
  "ENVDIST",                    // Id
  "Environmental Distance",     // Name
  "0.5",                        // Version
  "Generic algorithm based on environmental dissimilarity metrics.", // Overview
  "Generic algorithm based on environmental dissimilarity metrics. When used with the Gower metric and maximum distance 1, this algorithm should produce the same result of the algorithm known as DOMAIN.", // Description
  "Mauro E. S. Munoz, Renato De Giovanni, Danilo J. S. Bellini",    // Algorithm author
  "Carpenter G, Gillison AN, Winter J (1993) DOMAIN: A flexible modeling procedure for mapping potential distributions of animals and plants. Biodiversity and Conservation 2: 667-680. Farber O & Kadmon R 2003. Assessment of alternative approaches for bioclimatic modeling with special emphasis on the Mahalanobis distance. Ecological Modelling 160: 115–130.", // Bibliography
  "Danilo J. S. Bellini, Renato De Giovanni", // Code author
  "danilo . estagio [at] gmail . com, renato [at] cria . org . br", // Code author's contact
  0,                    // Does not accept categorical data
  0,                    // Does not need (pseudo)absence points
  NUM_PARAM, parameters // Algorithm's parameters
};


//
// LINK TO OM
//

// Needed code to link this algorithm to oM
OM_ALG_DLL_EXPORT
AlgorithmImpl *algorithmFactory(){
   return new EnvironmentalDistance(); // Create an instance of the algorithm's class
}
OM_ALG_DLL_EXPORT
AlgMetadata const *algorithmMetadata(){
   return &metadata;
}


//
// ALGORITHM CONSTRUCTOR/DESTRUCTOR
//

// Constructor for the algorithm class
EnvironmentalDistance::EnvironmentalDistance() : AlgorithmImpl(&metadata){
   _cov_matrix = _cov_matrix_inv = NULL;
   _normalizerPtr = new ScaleNormalizer( DATA_MIN, DATA_MAX, true );
}

// Destructor for the algorithm class
EnvironmentalDistance::~EnvironmentalDistance()
{
   switch(_par_dist_type){
      case MahalanobisDistance:
         if(_cov_matrix!=NULL){
            delete _cov_matrix;
            delete _cov_matrix_inv;
         }
         break;
      //case ManhattanDistance:
      //case ChebyshevDistance:
      //case EuclideanDistance:
      //default:
   }
}


//
// ALGORITHM GENERIC METHODS (virtual AlgorithmImpl methods)
//

// Initialize the algorithm
int EnvironmentalDistance::initialize(){

   // Test the parameters' data types
   if(!getParameter(PARDIST,&_par_dist)){
      Log::instance()->error("Parameter '" PARDIST "' was not passed.\n");
      return 0;
   }
   if(!getParameter(PARDISTTYPE,&_par_dist_type)){
      Log::instance()->error("Parameter '" PARDISTTYPE "' was not passed.\n");
      return 0;
   }
   if(!getParameter(PARPOINTQNT,&_par_point_qnt)){
      Log::instance()->error("Parameter '" PARPOINTQNT "' was not passed.\n");
      return 0;
   }

   // Impose limits to the parameters, if somehow the user don't obey
   if     (_par_dist>PARDISTMAX) _par_dist = PARDISTMAX;
   else if(_par_dist<PARDISTMIN) _par_dist = PARDISTMIN;

   // Check distance type
   switch(_par_dist_type){
      case EuclideanDistance:
         Log::instance()->debug("Using Euclidean distance\n");
         break;
      case MahalanobisDistance:
         Log::instance()->debug("Using Mahalanobis distance\n");
         break;
      case ManhattanDistance:
         Log::instance()->debug("Using Manhattan distance\n");
         break;
      case ChebyshevDistance:
         Log::instance()->debug("Using Chebyshev distance\n");
         break;
      default:
         Log::instance()->error("Parameter '" PARDISTTYPE "' wasn't set properly. It should be an integer between 1 and 4.\n");
         return 0;
   }

   //_samp->environmentallyUnique(); // Debug

   // Initialize some common-use attributes
   _layer_count    = _samp->numIndependent();
   _presence_count = _samp->numPresence();

   // Load all environmental data of presence points
   if(_presence_count == 0){
      Log::instance()->error("There is no presence point.\n");
      return 0;
   }

   // Check number of nearest points parameter
   if (_par_point_qnt > _presence_count)
      Log::instance()->warn("Parameter '" PARPOINTQNT "' is greater than the number of presence points\n");
   else if (_par_point_qnt < 0) _par_point_qnt = 0;

   OccurrencesPtr presences = _samp->getPresences();
   for(int i = 0 ; i < _presence_count ; i++)
      _presence_points.push_back((*presences)[i]->environment());
   // Calcs the mean of all presence points
   _average_point = _presence_points[0]; // There is at least one presence point
   for(int i = 1 ; i < _presence_count ; i++)
      _average_point += _presence_points[i];
   _average_point /= _presence_count;

   _use_chisq = false;

   // Allow using "Distance" method and normalize _par_dist
   if(!_init_distance_type()){
      Log::instance()->error("Could not determine maximum distance in the environmental space.\n");
      return 0;
   }

   _done = true;       // Needed for not-iterative algorithms
   return 1; // There was no problem in initialization
}

// Returns the occurrence probability
Scalar EnvironmentalDistance::getValue(const Sample& x) const{
   Scalar dist;

   //
   // Distance to average
   //
   if((_par_point_qnt >= _presence_count) || (_par_point_qnt <= 0))
      dist = _distance(x, _average_point);

   //
   // Minimum distance (not really needed)
   //
   else if(_par_point_qnt == 1){
      Scalar distIterator;
      dist = -1;
      for(int i = 0 ; i < _presence_count ; i++){ // Iterate for each presence point
         distIterator = _distance(x, _presence_points[i]);
         if((distIterator < dist || dist < 0)){
            dist = distIterator;
         }
      }

   //
   // Mean of _par_point_qnt nearest points
   //
   }else{
      Scalar distIterator, distTmp;
      int indexIterator, indexTmp;
      Sample nearMean;
      std::vector<int> nearestIndex(_par_point_qnt);
      std::vector<Scalar> nPdist(_par_point_qnt);
      //Log::instance()->debug("Starting with distances:\n"); // Debug
      for(int i = 0 ; i < _par_point_qnt ; i++){ // We know that _par_point_qnt < _presence_count
         nPdist[i] = _distance(x, _presence_points[i]);
         //x.dump(); // debug
         //_presence_points[i].dump(); // debug
         nearestIndex[i] = i;
         //Log::instance()->debug("   dist[%d]=%.8g\n", nearestIndex[i], nPdist[i]); // Debug
      }

      //Log::instance()->debug("Nearest points:\n"); // Debug
      for(int i = _par_point_qnt ; i < _presence_count ; i++){ // This loop finds the nearest points
         distIterator = _distance(x, _presence_points[i]);
         indexIterator = i;
         //Log::instance()->debug("dist[%d] = %.8g:\n", indexIterator, distIterator); // Debug
         for(int j = 0 ; j < _par_point_qnt ; j++){ // Trade pointIterator with the first smaller point
            if(nPdist[j] > distIterator){
               distTmp = distIterator;
               indexTmp = indexIterator;
               distIterator = nPdist[j];
               indexIterator = nearestIndex[j];
               nPdist[j] = distTmp;
               nearestIndex[j] = indexTmp;
               j = -1; // Re-start the for loop for the new value
            }
         }
         //for(int j = 0 ; j < _par_point_qnt ; j++) // Debug
         //   Log::instance()->debug("   dist[%d]=%.8g\n", nearestIndex[j], nPdist[j]);
      }

      // Now we have the nearest points. Let's get its mean:
      nearMean = _presence_points[nearestIndex[0]]; // There is at least one point
      for(int i = 1 ; i < _par_point_qnt ; i++)
         nearMean += _presence_points[nearestIndex[i]];
      nearMean /= _par_point_qnt;

      dist = _distance(x, nearMean);
   }

   //Log::instance()->debug("distance=%.8g\n\n", dist); // Debug
   //Log::instance()->debug("max dist=%.8g\n\n", _par_dist); // Debug

   // Now finishes the algorithm calculating the probability
   if(dist < 0) // There isn't any occurrence
      return 0.0;
   else if(_use_chisq) // Only for Mahalanobis distance when maxdist == 1
      return _pochisq(dist, _layer_count-1 );
   else if(dist > _par_dist) // Point is too faraway from nearest point
      return 0.0;
   else
      return 1.0 - (dist / _par_dist);
}

// Initialize _cov_matrix and _cov_matrix_inv
void EnvironmentalDistance::_calc_covariance_matrix(){
   if(_cov_matrix!=NULL){ // Garbage collector
      delete _cov_matrix;
      delete _cov_matrix_inv;
   }
   _cov_matrix = new Matrix(_layer_count,_layer_count); // Alloc memory for new matrices
   _cov_matrix_inv = new Matrix(_layer_count,_layer_count);

   if(_presence_count > 1){
      // Calcs the cross-covariance for each place in the matrix
      for(int i = 0 ; i < _layer_count ; i++)
         for(int j = i ; j < _layer_count ; j++){
            (*_cov_matrix)(i,j) = 0;
            for(int k = 0 ; k < _presence_count ; k++)
               (*_cov_matrix)(i,j) += (_presence_points[k][i]-_average_point[i]) *
                                    (_presence_points[k][j]-_average_point[j]);
            (*_cov_matrix)(i,j) /= _presence_count;
            (*_cov_matrix)(j,i) = (*_cov_matrix)(i,j);
         }
   }
   else{
      // Use an identity matrix if there's only one point
      for(int i = 0 ; i < _layer_count ; i++)
         for(int j = i ; j < _layer_count ; j++){
            (*_cov_matrix)(i,j) = (i == j) ? 1 : 0;
         }
   }
   //Log::instance()->debug("Cov matrix:\n");
   //std::cout << (*_cov_matrix); // Debug

   try{
      (*_cov_matrix_inv) = !(*_cov_matrix);
   }
   catch ( std::exception& e ) {
      string msg = e.what();
      msg.append( "\nExperiment has no solution using Mahalanobis distance.\n" );
      throw AlgorithmException( msg.c_str() );
   }
   //Log::instance()->debug("Inv cov matrix:\n");
   //std::cout << (*_cov_matrix_inv); // Debug
}

// Calcs the distance between x and y using _par_dist_type
inline Scalar EnvironmentalDistance::_distance(const Sample& x, const Sample& y) const{
   Scalar dist=0;
   switch(_par_dist_type){

      //
      // Mahalanobis Distance
      //
      case MahalanobisDistance:{
         Matrix lineMatrix(1,_layer_count);
         // Make lineMatrix = x - y
         for(int i=0; i<_layer_count; i++)
            lineMatrix(0,i) = x[i] - y[i];
         // Definition of Mahalanobis distance
         dist = (lineMatrix * (*_cov_matrix_inv) * (~lineMatrix))(0,0); // Operator () of a 1x1 matrix
         if(!_use_chisq)
            dist = sqrt(dist);
         //Log::instance()->info("\nDISTANCE: %g\n",dist); // Debug
      }break;

      //
      // Manhattan Distance
      //
      case ManhattanDistance:{
         Scalar tmp;
         for(int k=0;k<_layer_count;k++){
            tmp = x[k] - y[k];
            // We don't need this because we did that in _par_dist normalization:
            // tmp /= DATA_MAX - DATA_MIN; // range(k)
            if(tmp < 0)
               dist -= tmp;
            else
               dist += tmp;
         }
         dist /= _layer_count;
      }break;

      //
      // ChebyshevDistance
      //
      case ChebyshevDistance:
      {
         Scalar tmp;
         for(int i=0; i<_layer_count; i++){
            tmp = x[i] - y[i];
            if(tmp < 0)
              tmp = -tmp;
            if(tmp > dist)
              dist = tmp;
         }
      }break;

      //
      // Euclidean Distance
      //
      case EuclideanDistance:
      default:{
         Sample delta = x;
         delta -= y;
         dist = delta.norm(); // The usual norm of the "delta" vector have
      }break;                 // Euclidean distance for a n-dimensional space
   }

   return dist;
}

// Initialize the data structures of a distance type and normalize _par_dist
bool EnvironmentalDistance::_init_distance_type(){
   Scalar distMax;

   // Calcs the maximum distance (distMax)
   switch(_par_dist_type){

      case MahalanobisDistance:{
         _calc_covariance_matrix(); // Initialize _cov_matrix_inv
         if(_par_dist < 1.0) {
            Log::instance()->info("Using normalized maximum distance\n"); // Debug
            Scalar distIterator;
            Sample x,y;
            x.resize(_layer_count);
            y.resize(_layer_count);
            distMax = 0.0;
            bool foundDist = false;
            // Distance between oposite edges (vertex) of the hypercube
            for(int i=0; i<(1<<(_layer_count-1)); i++){ // for(i FROM 0 TO 2 "power" (_layer_count - 1))
               for(int k=0; k<_layer_count; k++) // This is the same loop used to create
                  if((i & (1<<k)) != 0){         // binary numbers, but with "max" and
                     x[k] = DATA_MAX;          // "min" instead of "1" and "0"
                     y[k] = DATA_MIN; // y is the oposite of x
                  }else{
                     x[k] = DATA_MIN;
                     y[k] = DATA_MAX;
                  }
               distIterator = _distance(x,y);
               if(distIterator > distMax){
                  distMax = distIterator;
                  foundDist = true;
               }
            }
            if(!foundDist)
               return false;
         }
         else {
            // In this case, chi-square probabilities will be used, 
            // so no need to find the max distance
            Log::instance()->info("Using chi-square probabilities\n");
            _use_chisq = true;
            return true;
         }
      }break;

      case ManhattanDistance:
         // Manhattan maximum value is always DATA_MAX - DATA_MIN, even for n
         // dimensions, so there's no real initialization for Manhattan
      case ChebyshevDistance:
         // Chebyshev and Manhattan distances has the same maximum value
         distMax = DATA_MAX - DATA_MIN;
         break;

      case EuclideanDistance:
      default:
         //   For a 1 dimensional world, we impose a size limit of L
         //   For a 2D world, we have a square, with size limit of L*sqrt(2)
         // since each side is a 1 dimensional world with max = 1
         //   In a 3D world, size limit is L*sqrt(3) because we have a cube
         //   For a n-dimensional world, we have the maximum dist equals
         // L*sqrt(n), because sqrt((L*sqrt(n-1))^2 + L^2) = L*sqrt(n)
         // (indution using Pythagoras).
         distMax = sqrt((double)_layer_count) * (DATA_MAX - DATA_MIN);
   }

   // Normalize _par_dist for its limits
   _par_dist = (_par_dist - PARDISTMIN)/(PARDISTMAX - PARDISTMIN); // Now _par_dist is in [0,1]
   _par_dist *= distMax; // Now _par_dist is in [0,distMax]
   return true;
}

Scalar EnvironmentalDistance::_poz(Scalar z) const {
   Scalar  y, x, w;
   if (z == 0.0)
      x = 0.0;
   else {
      y = 0.5 * fabs (z);
      if (y >= (Z_MAX * 0.5))
         x = 1.0;
      else if (y < 1.0) {
         w = y*y;
         x = ((((((((0.000124818987 * w
                    -0.001075204047) * w +0.005198775019) * w
                    -0.019198292004) * w +0.059054035642) * w
                    -0.151968751364) * w +0.319152932694) * w
                    -0.531923007300) * w +0.797884560593) * y * 2.0;
      }
      else {
         y -= 2.0;
         x = (((((((((((((-0.000045255659 * y
                          +0.000152529290) * y -0.000019538132) * y
                          -0.000676904986) * y +0.001390604284) * y
                          -0.000794620820) * y -0.002034254874) * y
                          +0.006549791214) * y -0.010557625006) * y
                          +0.011630447319) * y -0.009279453341) * y
                          +0.005353579108) * y -0.002141268741) * y
                          +0.000535310849) * y +0.999936657524;
       }
    }
  return (z > 0.0 ? ((x + 1.0) * 0.5) : ((1.0 - x) * 0.5));
}

Scalar EnvironmentalDistance::_pochisq(Scalar x, int df) const {
   Scalar  a, y = 0, s;
   Scalar  e, c, z;
   int     even;     /* true if df is an even number */
   if (x <= 0.0 || df < 1)
      return (0.0);
   a = 0.5 * x;
   even = (2*(df/2)) == df;
   if (df > 1)
      y = ex (-a);
   s = (even ? y : (2.0 * _poz(-sqrt (x))));
   if (df > 2) {
      x = 0.5 * (df - 1.0);
      z = (even ? 1.0 : 0.5);
      if (a > BIGX) {
         e = (even ? 0.0 : LOG_SQRT_PI);
         c = log (a);
         while (z <= x) {
            e = log (z) + e;
            s += ex (c*z-a-e);
            z += 1.0;
         }
         return (s);
      }
      else {
         e = (even ? 1.0 : (I_SQRT_PI / sqrt(a)));
         c = 0.0;
         while (z <= x) {
            e = e * (a / z);
            c = c + e;
            z += 1.0;
         }
         return (c * y + s);
      }
   }
   else
      return (s);
}

// Alg serializer
void EnvironmentalDistance::_getConfiguration(ConfigurationPtr& config) const {
   if (!_done ) return;

   ConfigurationPtr model_config( new ConfigurationImpl("EnvironmentalDistance") );
   config->addSubsection(model_config);
   model_config->addNameValue("MaxDistance",_par_dist);
   ConfigurationPtr envpoints_config(new ConfigurationImpl("EnvironmentalReferences"));
   model_config->addSubsection(envpoints_config);
   for(unsigned int i=0; i<_presence_points.size(); i++) {
      ConfigurationPtr point_config(new ConfigurationImpl("Reference"));
      envpoints_config->addSubsection(point_config);
      point_config->addNameValue("Value", _presence_points[i]);
   }
   model_config->addNameValue("Average",_average_point);
}

// Alg deserializer
void EnvironmentalDistance::_setConfiguration(const ConstConfigurationPtr& config) {
   ConstConfigurationPtr model_config = config->getSubsection("EnvironmentalDistance",false);

   if (!model_config) return;

   // Metric
   if(!getParameter(PARDISTTYPE,&_par_dist_type)){
      Log::instance()->error("Parameter '" PARDISTTYPE "' was not found in serialized model.\n");
      return;
   }
   // "n" closest points
   if(!getParameter(PARPOINTQNT,&_par_point_qnt)){
      Log::instance()->error("Parameter '" PARPOINTQNT "' was not found in serialized model.\n");
      return;
   }
   Scalar max_distance;
   if(!getParameter(PARDIST,&max_distance)){
      Log::instance()->error("Parameter '" PARDIST "' was not found in serialized model.\n");
      return;
   }
   _use_chisq = false;
   if (_par_dist_type == MahalanobisDistance && max_distance == 1.0) {
      _use_chisq = true;
   }
   // Maximum distance
   _par_dist = model_config->getAttributeAsDouble("MaxDistance", 0.0);
   // Environmental points
   ConstConfigurationPtr envpoints_config = model_config->getSubsection("EnvironmentalReferences",false);
   Configuration::subsection_list subs = envpoints_config->getAllSubsections();
   Configuration::subsection_list::iterator begin = subs.begin();
   Configuration::subsection_list::iterator end = subs.end();
   for (; begin != end; ++begin) {
      if ((*begin)->getName() != "Reference") continue;
      Sample point = (*begin)->getAttributeAsSample("Value");
      _presence_points.push_back(point);
   }
   // Average
   _average_point = model_config->getAttributeAsSample("Average");

   _layer_count = (int)_average_point.size();
   _presence_count = (int)_presence_points.size();

   if ( _par_dist_type == MahalanobisDistance ) {
      _calc_covariance_matrix(); // Initialize _cov_matrix_inv
   }

   _done = true;
}
