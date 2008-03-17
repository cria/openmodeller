#include "maximum_entropy.hh"
#include <openmodeller/MeanVarianceNormalizer.hh>
#include <openmodeller/Sampler.hh>
#include "maxent.h"
#include <iostream>
#include <sstream>

using namespace std;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 0

/******************************/
/*** Algorithm's parameters ***/

  static AlgParamMetadata *parameters = 0;

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {
  
  "MaximumEntropy", 	// Id.
  "Maximum Entropy",    // Name.
  "0.1",       	        // Version.

  // Overview.
  "",

  // Description.
  "",

  "", // Algorithm author.

  "", // Bibliography.

  "Elisangela S. da C. Rodrigues", // Code author.

  "elisangela.rodrigues [at] poli.usp.br", // Code author's contact.

  0, // Does not accept categorical data.
  0, // Does not need (pseudo)absence points.

  NUM_PARAM, // Algorithm's parameters.
  parameters
};

/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new MaximumEntropy();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/****************************************************************/
/************************ Maximum Entropy ***********************/

/*******************/
/*** constructor ***/

MaximumEntropy::MaximumEntropy() :
  AlgorithmImpl(&metadata),
  _done(false),
  num_layers(0)
{ 
  _normalizerPtr = new MeanVarianceNormalizer();
}

/******************/
/*** destructor ***/

MaximumEntropy::~MaximumEntropy()
{
}

/******************/
/*** initialize ***/

int
MaximumEntropy::initialize()
{
  // Check the number of presences.
  int num_presences = _samp->numPresence();

  //Debug
  //cout << "Numero de pontos de presenca " << num_presences << endl;

  if (num_presences == 0){

    Log::instance()->error("MaximumEntropy: No presence points inside the mask!\n");
    return 0;
  }
  
  // Load presence points.
  presences = _samp->getPresences();

  // Check the number of absences.
  int num_absences = _samp->numAbsence();

  //Debug
  //cout << "Numero de pontos de ausencia " << num_absences << endl;

  if (num_absences == 0){

    num_absences = num_presences;

    Log::instance()->info("Generating pseudo-absences.\n");

    OccurrencesImpl * occs = new OccurrencesImpl("", GeoTransform::getDefaultCS());

    absences = ReferenceCountedPointer<OccurrencesImpl>(occs);

    for (int i = 0; i < num_absences; ++i){

      OccurrencePtr oc = _samp->getPseudoAbsence();

      absences->insert(oc);
    }
    
    // Compute normalization with all points
    SamplerPtr mySamplerPtr = createSampler(_samp->getEnvironment(), presences, absences);

    _normalizerPtr->computeNormalization(mySamplerPtr);

    setNormalization(_samp);

    absences->normalize(_normalizerPtr);
  }
  else{
    absences = _samp->getAbsences();
  }
  return 1;
}

/***************/
/*** iterate ***/

int
MaximumEntropy::iterate()
{
  
  add_samples();

  model.train();

  //model.save_to_file("model"); //save the model into a file
  
  _done = true;

  return 1;
}

/************/
/*** done ***/

int
MaximumEntropy::done() const
{
  return _done;
}

/***********************/
/*** Train the Model ***/

void 
MaximumEntropy::add_samples()
{
  // Number of layers.
  num_layers = _samp->numIndependent();

  //Debug
  //cout << "Numero de camadas " << num_layers << endl;

  OccurrencesImpl::const_iterator p_iterator;
  OccurrencesImpl::const_iterator p_end;

  p_iterator = absences->begin();
  p_end = absences->end();

  //int j=1;//debug

  while (p_iterator != p_end){

    Sample point = (*p_iterator)->environment();
    
    ME_Sample s("0");

    for (int i = 0; i < num_layers; ++i){
      stringstream out;
      out << i;
      s.add_feature(out.str(), point[i]);
    }
    //Debug
    //cout << "Absence point " << j << " " << point << endl;
    //j++;

    model.add_training_sample(s);
    
    ++p_iterator;
  }

  p_iterator = presences->begin();
  p_end = presences->end();

  //j=1;//debug

  while (p_iterator != p_end){

    Sample point = (*p_iterator)->environment();

    ME_Sample s("1");    
    
    for (int i = 0; i < num_layers; ++i){
      stringstream out;
      out << i;
      s.add_feature(out.str(), point[i]);
    }
    //Debug
    //cout << "Presence point " << j << " " << point << endl;
    //j++;
    
    model.add_training_sample(s);
    
    ++p_iterator;
  }

}

/*****************/
/*** get Value ***/

Scalar
MaximumEntropy::getValue(const Sample& x) const
{
  ME_Sample s;    

  for (int i = 0; i < num_layers; ++i){
    stringstream out;
    out << i;
    s.add_feature(out.str(), x[i]);
  }

  model.classify(s);

  return (s.label == "1") ? 1.0 : 0.0;
}

/***********************/
/*** get Convergence ***/
/*
int
MaximumEntropy::getConvergence(Scalar *val)
{
  *val = 1.0;
  return 1;
}
*/

/****************************************************************/
/****************** configuration *******************************/

void
MaximumEntropy::_getConfiguration(ConfigurationPtr& config) const
{
  if (!_done)
    return;
  
  ConfigurationPtr model_config(new ConfigurationImpl("MaximumEntropy"));
  config->addSubsection(model_config);

  //////////////////COMPLETAR////////////////////
}

void
MaximumEntropy::_setConfiguration(const ConstConfigurationPtr& config)
{
  ConstConfigurationPtr model_config = config->getSubsection("MaximumEntropy",false );

  if (!model_config)
    return;
  
  _done = true;
}
