/**
 * Definition of MeanVarianceNormalizer class
 * 
 * @author Renato De Giovanni
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2007 by CRIA -
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

#include <openmodeller/MeanVarianceNormalizer.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/Log.hh>

/*******************/
/*** constructor ***/
MeanVarianceNormalizer::MeanVarianceNormalizer() :
  _mean(),
  _stddev()
{
}

/*******************/
/*** destructor ***/
MeanVarianceNormalizer::~MeanVarianceNormalizer() {}

/****************/
/*** get Copy ***/
Normalizer * MeanVarianceNormalizer::getCopy() {

  return new MeanVarianceNormalizer( *this );
}

/*****************************/
/*** compute Normalization ***/
void MeanVarianceNormalizer::computeNormalization( const ReferenceCountedPointer<const SamplerImpl>& samplerPtr ) {

  int dim = samplerPtr->numIndependent();

  _mean.resize(dim);
  _stddev.resize(dim);

  int numPoints = samplerPtr->numPresence() + samplerPtr->numAbsence();

  // Join all occurrences
  OccurrencesPtr presences = samplerPtr->getPresences();
  OccurrencesPtr absences = samplerPtr->getAbsences();
  
  OccurrencesPtr allOccs( new OccurrencesImpl( presences->label(), presences->coordSystem() ) );

  allOccs->appendFrom( presences );
  allOccs->appendFrom( absences );
  
  // Compute mean  
  OccurrencesImpl::const_iterator p_iterator = allOccs->begin();
  OccurrencesImpl::const_iterator p_end = allOccs->end();

  while ( p_iterator != p_end ) {

    Sample point = (*p_iterator)->environment();

    _mean += point;

    ++p_iterator;
  }

  _mean /= Scalar( numPoints );

  // Compute standard deviation
  p_iterator = allOccs->begin();

  while ( p_iterator != p_end ) {

    Sample point = (*p_iterator)->environment();

    point -= _mean;
    point *= point;

    _stddev += point;
    
    ++p_iterator;
  }
 
  _stddev /= Scalar( numPoints - 1 );
  _stddev.sqrt();
}

/*****************/
/*** normalize ***/
void MeanVarianceNormalizer::normalize( Sample * samplePtr ) {

  if ( samplePtr->size() != 0 ) {

    *samplePtr -= _mean;
    *samplePtr /= _stddev;
  }
}

/*************************/
/*** get configuration ***/
ConfigurationPtr MeanVarianceNormalizer::getConfiguration() const {

  ConfigurationPtr config( new ConfigurationImpl("Normalization") );

  config->addNameValue( "Class", "MeanVarianceNormalizer" );

  config->addNameValue( "Mean", _mean );
  config->addNameValue( "StdDev", _stddev );

  return config;
}

/*************************/
/*** set configuration ***/
void MeanVarianceNormalizer::setConfiguration( const ConstConfigurationPtr &config ) {

  _mean   = config->getAttributeAsSample( "Mean" );
  _stddev = config->getAttributeAsSample( "StdDev" );
}
