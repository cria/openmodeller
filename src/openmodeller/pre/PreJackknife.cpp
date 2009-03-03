/**
 * Definition of Jackknife class 
 * 
 * @author Fabr�cio Rodrigues (fabricio . poliusp at gmail . com)
 * @author Renato De Giovanni (renato at cria . org . br)
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c), CRIA, Reference Center on Environmental Information 
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

#include <openmodeller/pre/PreJackknife.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/ConfusionMatrix.hh>
#include <openmodeller/Log.hh>

#include <openmodeller/Exceptions.hh>

#include <string.h>
#include <math.h>

using namespace std;

/*******************/
/*** constructor ***/
PreJackknife::PreJackknife()
{
}


/*******************/
/*** destructor ***/
PreJackknife::~PreJackknife()
{
}

bool 
PreJackknife::checkParameters( const PreParameters& parameters ) const
{
  SamplerPtr samplerPtr;

  if ( ! parameters.retrieve( "Sampler", samplerPtr ) ) {

    Log::instance()->error( "Missing parameter: Sampler. \n" );
    return false;
  }

  AlgorithmPtr algorithmPtr;

  if ( ! parameters.retrieve( "Algorithm", algorithmPtr ) ) {

    Log::instance()->error( "Missing parameter: Algorithm. \n" );
    return false;
  }

  return true;
}

void
PreJackknife::getAcceptedParameters( stringMap& info)
{
	info["Sampler"] = "samplerPtr";
	info["Algorithm"] = "algorithmPtr";
	info["PropTrain"] = "double";
}

void
PreJackknife::getLayersetResultSpec ( stringMap& info)
{
	info["Accuracy"] = "double";
	info["Mean"] = "double";
	info["Variance"] = "double";
	info["Deviation"] = "double";
	info["Estimate"] = "double";
	info["Bias"] = "double";
}

void
PreJackknife::getLayerResultSpec ( stringMap& info)
{
	info["Accuracy without layer"] = "std::multimap<double, int>";
}

bool PreJackknife::runImplementation()
{
  Log::instance()->debug( "Running jackknife\n" );

  SamplerPtr samplerPtr;
  params_.retrieve( "Sampler", samplerPtr );

  AlgorithmPtr algorithmPtr;
  params_.retrieve( "Algorithm", algorithmPtr );

  double propTrain;

  if ( ! params_.retrieve( "PropTrain", propTrain ) ) {

    // default
    propTrain = 0.9;
  }

  if ( ! samplerPtr->getEnvironment() ) {

    std::string msg = "Sampler has no environment.\n";

    Log::instance()->error( msg.c_str() );

    throw InvalidParameterException( msg );
  }

  int num_layers = samplerPtr->numIndependent();

  if ( num_layers < 2 ) {

    std::string msg = "Jackknife needs at least 2 layers.\n";

    Log::instance()->error( msg.c_str() );

    throw InvalidParameterException( msg );
  }

  // Split sampler into test and trainning
  SamplerPtr training_sampler;
  SamplerPtr testing_sampler;

  splitSampler( samplerPtr, &training_sampler, &testing_sampler, propTrain );

  // Calculate reference parameter using all layers
  AlgorithmPtr algorithm_ptr = algorithmPtr->getFreshCopy();
  
  algorithm_ptr->createModel( training_sampler );

  ConfusionMatrix conf_matrix;

  conf_matrix.calculate( algorithm_ptr->getModel(), testing_sampler );

  double out_param = conf_matrix.getAccuracy() * 100; // <------ output 1

  // Calculate reference parameter for each layer by excluding it from the layer set

  std::multimap<double, int> out_params; // <------ output 2

  double mean = 0.0;                 // <------ output 3
  double variance = 0.0;             // <------ output 4
  double std_deviation = 0.0;        // <------ output 5
  double jackknife_estimate = 0.0;   // <------ output 6
  double jackknife_bias = 0.0;       // <------ output 7

  // Work with clones of the occurrences 
  OccurrencesPtr training_presences;
  OccurrencesPtr training_absences;
  OccurrencesPtr testing_presences;
  OccurrencesPtr testing_absences;

  if ( training_sampler->numPresence() ) {

    training_presences = training_sampler->getPresences()->clone();
  }

  if ( training_sampler->numAbsence() ) {

    training_absences = training_sampler->getAbsences()->clone();
  }

  if ( testing_sampler->numPresence() ) {

    testing_presences = testing_sampler->getPresences()->clone();
  }

  if ( testing_sampler->numAbsence() ) {

    testing_absences = testing_sampler->getAbsences()->clone();
  }

  for ( int i = 0; i < num_layers; ++i ) {

    Log::instance()->debug( "Removing layer with index %u\n", i );

    // Copy the original environment
    EnvironmentPtr new_environment = samplerPtr->getEnvironment()->clone();

    // Remove one of the layers
    new_environment->removeLayer( i );

    // Read environment data from the new set of layers
    if ( training_presences ) {

      training_presences->setEnvironment( new_environment );
    }

    if ( training_absences ) {

      training_absences->setEnvironment( new_environment );
    }

    if ( testing_presences ) {

      testing_presences->setEnvironment( new_environment );
    }

    if ( testing_absences ) {

      testing_absences->setEnvironment( new_environment );
    }

    // Create a new sampler for trainning points
    SamplerPtr new_training_sampler = createSampler( new_environment, training_presences, training_absences );

    // Create a new algorithm
    AlgorithmPtr new_algorithm = algorithmPtr->getFreshCopy();

    new_algorithm->createModel( new_training_sampler );

    conf_matrix.reset();

    // Create a new sampler for testing points
    SamplerPtr new_testing_sampler = createSampler( new_environment, testing_presences, testing_absences );

    // Normalize test samples if necessary
    if ( new_algorithm->needNormalization() && ! new_testing_sampler->isNormalized() ) {

    Log::instance()->info( "Computing normalization for test points\n");

    Normalizer * normalizer = new_algorithm->getNormalizer();

      if ( normalizer ) {

        // Note: normalization parameters should have been already computed during model creation
        new_testing_sampler->normalize( normalizer );
      }
      else {

        Log::instance()->error( "Jackknife algorithm requires normalization but did not specify any normalizer\n");
        return false;
      }
    }

    // Calculate parameters
    conf_matrix.reset(); // reuse object
    conf_matrix.calculate( new_algorithm->getModel(), new_testing_sampler );

    double myaccuracy = conf_matrix.getAccuracy() * 100;

    mean += myaccuracy;

    out_params.insert( std::pair<double, int>( myaccuracy, i ) );

// Code for debugging:

//     string file_name = "model_";
//     char num[4];
//     sprintf( num, "%d", i);
//     file_name.append( num );

//     ConfigurationPtr config( new ConfigurationImpl("SerializedModel"));
//     ConfigurationPtr sampler_config( new_sampler->getConfiguration() );
//     config->addSubsection( sampler_config );
//     ConfigurationPtr alg_config( new_algorithm->getConfiguration() );
//     config->addSubsection( alg_config );

//     std::ostringstream model_output;
//     Configuration::writeXml( config, model_output );

//     std::ofstream file( file_name.c_str() );
//     file << model_output.str();
//     file.close();

//     break;
  }

  Log::instance()->debug( "Accuracy with all layers: %.2f%%\n", out_param );

  EnvironmentPtr environment_ptr = samplerPtr->getEnvironment();
  
  mean /= num_layers;
  
  std::multimap<double, int>::const_iterator it = out_params.begin();
  std::multimap<double, int>::const_iterator end = out_params.end();
  for ( ; it != end; ++it ) {

    Log::instance()->debug( "Accuracy without layer %d: %.2f%% (%s)\n", (*it).second, (*it).first, (environment_ptr->getLayerPath( (*it).second )).c_str() );
    variance += ((*it).first - mean)*((*it).first - mean);
  }

  Log::instance()->debug( "Mean = %f\n", mean );

  variance /= num_layers;

  variance /= (num_layers - 1);

  Log::instance()->debug( "Variance = %f\n", variance );

  std_deviation = sqrt(variance);

  Log::instance()->debug( "Standard deviation = %f\n", std_deviation );

  jackknife_bias = (num_layers - 1)*(mean - out_param);

  jackknife_estimate = out_param - jackknife_bias;

  Log::instance()->debug( "Jackknife estimate = %f\n", jackknife_estimate );

  Log::instance()->debug( "Jackknife bias = %f\n", jackknife_bias );

  params_.store( "Accuracy", out_param );
  params_.store( "Accuracy without layer", out_params );
  params_.store( "Mean", mean );
  params_.store( "Variance", variance );
  params_.store( "Deviation", std_deviation );
  params_.store( "Estimate", jackknife_estimate );
  params_.store( "Bias", jackknife_bias );



  return true;
}
