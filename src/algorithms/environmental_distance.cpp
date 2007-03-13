//
// Generic environmental distance algorithm
//
// Description: Generic algorithm based on distances.
//
// Author:      Danilo J. S. Bellini <danilo.estagio@gmail.com>
// Copyright:   See COPYING file that comes with this distribution
// Date:        2006-09-18
//

#include "environmental_distance.hh"
#include <openmodeller/Exceptions.hh>

//
// METADATA
//
#define DATA_MIN 0.0
#define DATA_MAX 1.0
#define NUM_PARAM 3

#define PARDIST    "Distance"
#define PARDISTMIN 0.0
#define PARDISTMAX 1.0
#define PARDISTTYPE "Distance Type"
#define PARPOINTQNT "Nearest points quantity to get a mean"

static AlgParamMetadata parameters[NUM_PARAM] = { // Parameters
   { // 1st parameter
      PARDISTTYPE, // Id
      "Metric",    // Name
      "Integer",   // Type
      "Metric used to calculate distances: " // Overview
         "1=Euclidean, "
         "2=Mahalanobis, "
         "3=Gower.",
      "Metric used to calculate distances: " // Description
         "1=Euclidean, "
         "2=Mahalanobis, "
         "3=Gower.",
      1,                                               // Not zero if the parameter has lower limit
      FIRST_DISTANCE_TYPE,                             // Parameter's lower limit
      1,                                               // Not zero if the parameter has upper limit
      FIRST_DISTANCE_TYPE + AMOUNT_DISTANCE_TYPES - 1, // Parameter's upper limit
      "1"         // Parameter's typical (default) value
   },
   { // 2nd parameter
      PARPOINTQNT,      // Id
      "Nearest \'n\' points", // Name
      "Integer",        // Type
      "Nearest \'n\' points whose mean value will be the reference when calculating environmental distances.", // Overview
      "Nearest \'n\' points whose mean value will be the reference when calculating environmental distances. When set to 1, distances will be measured to the closest point, which is the same behavior of the minimum distance algorithm. When set to 0, distances will be measured to the average of all presence points, which is the same behavior of the distance to average algorithm. Intermediate values between 1 and the total number of presence points are now accepted.", // Description
      1,          // Not zero if the parameter has lower limit
      0,          // Parameter's lower limit
      0,          // Not zero if the parameter has upper limit
      0,          // Parameter's upper limit
      "1"         // Parameter's typical (default) value
   },
   { // 3rd parameter
      PARDIST,            // Id
      "Maximum distance", // Name
      "Real",             // Type
      "Maximum distance to the reference in the environmental space.", // Overview
      "Maximum distance to the reference in the environmental space, above which the conditions will be considered unsuitable for presence. Since 1 corresponds to the biggest possible distance between any two points in the environment space, setting the maximum distance to this value means that all points in the environmental space will have an associated probability. The probability of presence for points that fall within the range of the maximum distance is inversely proportional to the distance to the reference point (linear decay).", // Description
      1,          // Not zero if the parameter has lower limit
      PARDISTMIN, // Parameter's lower limit
      1,          // Not zero if the parameter has upper limit
      PARDISTMAX, // Parameter's upper limit
      "0.1"       // Parameter's typical (default) value
   },
};

static AlgMetadata metadata = { // General metadata
  "EnvironmentalDistance",      // Id
  "Environmental Distance",     // Name
  "0.1",                        // Version
  "Generic algorithm based on environmental dissimilarity metrics.", // Overview
  "Generic algorithm based on environmental dissimilarity metrics.", // Description
  "Mauro E. S. Munoz, Renato De Giovanni",    // Algorithm author
  "",                                         // Bibliography
  "Danilo J. S. Bellini",                     // Code author
  "danilo.estagio [at] gmail.com",            // Code author's contact
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
   _initialized = false;
   covMatrix = covMatrixInv = NULL;
}

// Destructor for the algorithm class
EnvironmentalDistance::~EnvironmentalDistance()
{
   if(_initialized){
      switch(ParDistType){
         case MahalanobisDistance:
            if(covMatrix!=NULL){
               delete covMatrix;
               delete covMatrixInv;
            }
            break;
         //case GowerDistance:
         //case EuclideanDistance:
         //default:
      }
   }
}


//
// ALGORITHM GENERIC METHODS (virtual AlgorithmImpl methods)
//

// Initialize the algorithm
int EnvironmentalDistance::initialize(){

   // Test the parameters' data types
   if(!getParameter(PARDIST,&ParDist)){
      Log::instance()->error(1, "Parameter '" PARDIST "' wasn't set properly.\n");
      return 0;
   }
   if(!getParameter(PARDISTTYPE,&ParDistType)){
      Log::instance()->error(1, "Parameter '" PARDISTTYPE "' wasn't set properly.\n");
      return 0;
   }
   if(!getParameter(PARPOINTQNT,&ParPointQnt)){
      Log::instance()->error(1, "Parameter '" PARPOINTQNT "' wasn't set properly.\n");
      return 0;
   }

   // Impose limits to the parameters, if somehow the user don't obey
   if     (ParDist>PARDISTMAX) ParDist = PARDISTMAX;
   else if(ParDist<PARDISTMIN) ParDist = PARDISTMIN;

   //_samp->environmentallyUnique(); // Debug

   // Initialize some common-use attributes
   layerCount    = _samp->numIndependent();
   presenceCount = _samp->numPresence();

   // Load all environmental data of presence points
   if(presenceCount == 0){
      Log::instance()->error(1, "There is no presence point.\n");
      return 0;
   }
   OccurrencesPtr presences = _samp->getPresences();
   for(int i = 0 ; i < presenceCount ; i++)
      presencePoints.push_back((*presences)[i]->environment());
   // Calcs the mean of all presence points
   averagePoint = presencePoints[0]; // There is at least one presence point
   for(int i = 1 ; i < presenceCount ; i++)
      averagePoint += presencePoints[i];
   averagePoint /= presenceCount;

   InitDistanceType(); // Allow using "Distance" method and normalize ParDist
   _done = true;       // Needed for not-iterative algorithms
   _initialized = true;
   return 1; // There was no problem in initialization
}

// Normalize all environmental data to [DATA_MIN,DATA_MAX]
int EnvironmentalDistance::needNormalization(Scalar *min, Scalar *max) const{
  *min = DATA_MIN;
  *max = DATA_MAX;
  return 1;
}

// Returns the occurrence probability
Scalar EnvironmentalDistance::getValue(const Sample& x) const{
   Scalar dist;

   //
   // Distance to average
   //
   if((ParPointQnt >= presenceCount) || (ParPointQnt <= 0))
      dist = Distance(x, averagePoint);

   //
   // Minimum distance (not really needed)
   //
   else if(ParPointQnt == 1){
      Scalar distIterator;
      dist = -1;
      for(int i = 0 ; i < presenceCount ; i++){ // Iterate for each presence point
         distIterator = Distance(x, presencePoints[i]);
         if((distIterator < dist || dist < 0))
            dist = distIterator;
      }

   //
   // Mean of ParPointQnt nearest points
   //
   }else{
      Scalar distIterator, distTmp;
      int indexIterator, indexTmp;
      Sample nearMean;
      std::vector<int> nearestIndex(ParPointQnt);
      std::vector<Scalar> nPdist(ParPointQnt);
      //Log::instance()->info("\nStarting with distances:\n"); // Debug
      for(int i = 0 ; i < ParPointQnt ; i++){ // We know that ParPointQnt < presenceCount
         nPdist[i] = Distance(x, presencePoints[i]);
         nearestIndex[i] = i;
      //Log::instance()->info("   dist[%d]=%.8g\n", nearestIndex[i], nPdist[i]); // Debug
      }

      //Log::instance()->info("\nNewer values:\n"); // Debug
      for(int i = ParPointQnt ; i < presenceCount ; i++){ // This loop finds the nearest points
         distIterator = Distance(x, presencePoints[i]);
         indexIterator = i;
         //Log::instance()->info("dist[%d] = %.8g:\n", indexIterator, distIterator); // Debug
         for(int j = 0 ; j < ParPointQnt ; j++){ // Trade pointIterator with the first smaller point
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
         //for(int j = 0 ; j < ParPointQnt ; j++) // Debug
            //Log::instance()->info("   dist[%d]=%.8g\n", nearestIndex[j], nPdist[j]);
      }

      // Now we have the nearest points. Let's get its mean:
      nearMean = presencePoints[nearestIndex[0]]; // There is at least one point
      for(int i = 1 ; i < ParPointQnt ; i++)
         nearMean += presencePoints[nearestIndex[i]];
      nearMean /= ParPointQnt;

      dist = Distance(x, nearMean);
      //Log::instance()->info("\ndistance=%.8g\n\n", dist); // Debug
   }

   // Now finishes the algorithm calculating the probability
   if(dist < 0) // There isn't any occurrence
      return 0.0;
   else
      return 1.0 - (dist / ParDist);
}

// Initialize covMatrix and covMatrixInv
void EnvironmentalDistance::CalcCovarianceMatrix(){
   if(covMatrix!=NULL){ // Garbage collector
      delete covMatrix;
      delete covMatrixInv;
   }
   covMatrix = new Matrix(layerCount,layerCount); // Alloc memory for new matrices
   covMatrixInv = new Matrix(layerCount,layerCount);

   // Calcs the cross-covariance for each place in the matrix
   for(int i = 0 ; i < layerCount ; i++)
      for(int j = i ; j < layerCount ; j++){
         (*covMatrix)(i,j) = 0;
         for(int k = 0 ; k < presenceCount ; k++)
            (*covMatrix)(i,j) += (presencePoints[k][i]-averagePoint[i]) *
                                 (presencePoints[k][j]-averagePoint[j]);
         (*covMatrix)(i,j) /= presenceCount;
         (*covMatrix)(j,i) = (*covMatrix)(i,j);
      }

   try{
      (*covMatrixInv) = !(*covMatrix); // All covariance matrices are positive
                                       // definite, so they always have an inverse
   }
   catch ( std::exception& e ) {
      string msg = e.what();
      msg.append( "\nThis experiment has no solution using Mahalanobis distance" );
      throw AlgorithmException( msg.c_str() );
   }
   //std::cout << (*covMatrixInv); // Debug
}

// Calcs the distance between x and y using ParDistType
inline Scalar EnvironmentalDistance::Distance(const Sample& x, const Sample& y) const{
   Scalar dist=0;
   switch(ParDistType){

      //
      // Mahalanobis Distance
      //
      case MahalanobisDistance:{
         Matrix lineMatrix(1,layerCount);
         // Make lineMatrix = x - y
         for(int i=0; i<layerCount; i++)
            lineMatrix(0,i) = x[i] - y[i];
         // Definition of Mahalanobis distance
         dist = sqrt(
            (lineMatrix * (*covMatrixInv) * (~lineMatrix))(0,0) // Operator () of a 1x1 matrix
         );
         //Log::instance()->info("\nDISTANCE: %g\n",dist); // Debug
      }break;

      //
      // Gower Distance
      //
      case GowerDistance:{
         Scalar tmp;
         for(int k=0;k<layerCount;k++){
            tmp = x[k] - y[k];
            // We don't need this because we did that in ParDist normalization:
            // tmp /= DATA_MAX - DATA_MIN; // range(k)
            if(tmp < 0)
               dist -= tmp;
            else
               dist += tmp;
         }
         dist /= layerCount;
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

// Initialize the data structures of a distance type and normalize ParDist
void EnvironmentalDistance::InitDistanceType(){
   Scalar distMax;

   // Calcs the maximum distance (distMax)
   switch(ParDistType){

      case MahalanobisDistance:{
         // Distance between oposite edges (vertex) of the hypercube
         Scalar distIterator;
         Sample x,y;
         x.resize(layerCount);
         y.resize(layerCount);
         CalcCovarianceMatrix(); // Initialize covMatrixInv
         distMax = 0.0;
         for(int i=0; i<(1<<(layerCount-1)); i++){ // for(i FROM 0 TO 2 "power" (layerCount - 1))
            for(int k=0; k<layerCount; k++) // This is the same loop used to create
               if(i & (1<<k) != 0){         // binary numbers, but with "max" and
                  x[k] = DATA_MAX;          // "min" instead of "1" and "0"
                  y[k] = DATA_MIN; // y is the oposite of x
               }else{
                  x[k] = DATA_MIN;
                  y[k] = DATA_MAX;
               }
            distIterator = Distance(x,y);
            if(distIterator > distMax)
               distMax = distIterator;
         }
/*
         // Distance between all edges (vertex) of the hypercube
         Scalar distIterator;
         Sample x,y;
         x.resize(layerCount);
         y.resize(layerCount);
         CalcCovarianceMatrix(); // Initialize covMatrixInv
         distMax = 0.0;
         for(int i=0; i<(1<<layerCount)-1; i++){ // for(i FROM 0 TO (2 "power" layerCount) - 2)
            for(int k=0; k<layerCount; k++)         // This is the same loop used to create
               if(i & (1<<k) != 0) x[k] = DATA_MAX; // binary numbers, but with "max" and
               else                x[k] = DATA_MIN; // "min" instead of "1" and "0"
            // Log::instance()->info("Maximum distance: %.8g ; i = %d / %d\r", distMax, i, (1<<layerCount)-1); // Debug
            for(int j=i+1; j<(1<<layerCount); j++){ // Almost the same above
               for(int k=0; k<layerCount; k++)
                  if(j & (1<<k) != 0) y[k] = DATA_MAX;
                  else                y[k] = DATA_MIN;

               distIterator = Distance(x,y); // Calcs the distance between all edges (vertex) of the hypercube
               if(distIterator > distMax)
                  distMax = distIterator;
            }
         }
*/
         //Log::instance()->info("\nMaximum distance: %.8g\n",distMax); // Debug
      }break;

      case GowerDistance:
         // Gower maximum value is always DATA_MAX - DATA_MIN, even for n
         // dimensions, so there's no real initialization for Gower
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
         distMax = sqrt((double)layerCount) * (DATA_MAX - DATA_MIN);
   }

   // Normalize ParDist for its limits
   ParDist = (ParDist - PARDISTMIN)/(PARDISTMAX - PARDISTMIN); // Now ParDist is in [0,1]
   ParDist *= distMax; // Now ParDist is in [0,distMax]
}

// Alg serializer
void EnvironmentalDistance::_getConfiguration(ConfigurationPtr& config) const {
   if (!_done ) return;

   ConfigurationPtr model_config( new ConfigurationImpl("EnvironmentalDistance") );
   config->addSubsection(model_config);
   model_config->addNameValue("Metric",ParDistType);
   model_config->addNameValue("ClosestPoints",ParPointQnt);
   model_config->addNameValue("MaxDistance",ParDist);
   ConfigurationPtr envpoints_config(new ConfigurationImpl("EnvironmentalReferences"));
   model_config->addSubsection(envpoints_config);
   for(unsigned int i=0; i<presencePoints.size(); i++) {
      ConfigurationPtr point_config(new ConfigurationImpl("Reference"));
      envpoints_config->addSubsection(point_config);
      point_config->addNameValue("Value", presencePoints[i]);
   }
   model_config->addNameValue("Average",averagePoint);
}

// Alg deserializer
void EnvironmentalDistance::_setConfiguration(const ConstConfigurationPtr& config) {
   ConstConfigurationPtr model_config = config->getSubsection("EnvironmentalDistance",false);

   if (!model_config) return;

   // Metric
   ParDistType = model_config->getAttributeAsInt("Metric", 1);
   // "n" closest points
   ParPointQnt = model_config->getAttributeAsInt("ClosestPoints", 1);
   // Maximum distance
   ParDist = model_config->getAttributeAsDouble("MaxDistance", 0.0);
   // Environmental points
   ConstConfigurationPtr envpoints_config = model_config->getSubsection("EnvironmentalReferences",false);
   Configuration::subsection_list subs = envpoints_config->getAllSubsections();
   Configuration::subsection_list::iterator begin = subs.begin();
   Configuration::subsection_list::iterator end = subs.end();
   for (; begin != end; ++begin) {
      if ((*begin)->getName() != "Reference") continue;
      Sample point = (*begin)->getAttributeAsSample("Value");
      presencePoints.push_back(point);
   }
   // Average
   averagePoint = model_config->getAttributeAsSample("Average");
   layerCount = (int)averagePoint.size();

   _done = true;
}
