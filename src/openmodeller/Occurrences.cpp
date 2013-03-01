/**
 * Definition of Occurrences class.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-02-25
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

#include <openmodeller/Occurrences.hh>

#include <openmodeller/Occurrence.hh>
#include <openmodeller/Random.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/env_io/GeoTransform.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/os_specific.hh>

#include <string>
using std::string;

// String stream is included for the dump method
#include <sstream>
using std::ostringstream;

#include <algorithm> // needed for random_shuffle

#include <math.h>

/****************************************************************/
/************************ Occurrences ***************************/

/*******************/
/*** Constructor ***/

OccurrencesImpl::~OccurrencesImpl()
{
  delete gt_;
}

void
OccurrencesImpl::setCoordinateSystem( const string& cs )
{
  cs_ = cs;
  initGeoTransform();
}

void
OccurrencesImpl::initGeoTransform()
{
  if ( gt_ ) {

    delete gt_;
  }

  gt_ = new GeoTransform( cs_, GeoTransform::getDefaultCS() );
}

/*********************/
/*** configuration ***/

ConfigurationPtr
OccurrencesImpl::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Occurrences") );

  config->addNameValue( "Label", name() );

  ConfigurationPtr cs( new ConfigurationImpl( "CoordinateSystem" ) );
  cs->setValue( coordSystem() );

  config->addSubsection( cs );

  config->addNameValue( "Count", int(occur_.size()) );

  const_iterator oc = occur_.begin();
  const_iterator end = occur_.end();

  while ( oc != end ) {

    ConfigurationPtr cfg( new ConfigurationImpl("Point") );
    std::string id = (*oc)->id();
    Scalar x = (*oc)->x();
    Scalar y = (*oc)->y();
    gt_->transfIn( &x, &y );
    cfg->addNameValue( "Id", id );
    cfg->addNameValue( "X", x );
    cfg->addNameValue( "Y", y );
    if ( (*oc)->hasEnvironment() ) {

      cfg->addNameValue( "Sample", (*oc)->originalEnvironment() );
    }
    config->addSubsection( cfg );

    oc++;
  }

  return config;
}

void
OccurrencesImpl::setConfiguration( const ConstConfigurationPtr& config )
{
  name_ = config->getAttribute("Label");
  
  ConstConfigurationPtr cs_config = config->getSubsection( "CoordinateSystem", false );
  
  if ( ! cs_config ) {

    Log::instance()->warn( "Occurrences has no Coordinate System. Assuming LatLong WSG84\n" );
    cs_ = GeoTransform::getDefaultCS();
  }
  else {

    cs_ = cs_config->getValue();
  }

  initGeoTransform( );

  Configuration::subsection_list subs = config->getAllSubsections();

  Configuration::subsection_list::iterator begin = subs.begin();
  Configuration::subsection_list::iterator end = subs.end();

  std::vector<Scalar> attrs;

  for ( ; begin != end; ++begin ) {

    if ( (*begin)->getName() != "Point" ) {

      continue;
    }

    std::string id = (*begin)->getAttribute("Id");
    Scalar x = (*begin)->getAttributeAsDouble( "X", 0.0 );
    Scalar y = (*begin)->getAttributeAsDouble( "Y", 0.0 );
    Scalar abundance = (*begin)->getAttributeAsDouble( "Abundance", default_abundance_ );

    try {

      // If present, load environmental values from XML
      std::vector<Scalar> unnormenv = (*begin)->getAttributeAsVecDouble( "Sample" );
      createOccurrence( id, x, y, 0, abundance, attrs, unnormenv );
    }
    catch ( AttributeNotFound& e ) { 

      // Sample attribute is optional
      createOccurrence( id, x, y, 0, abundance, 0, 0, 0, 0 );
      UNUSED(e);
    }
  }
}

void 
OccurrencesImpl::setEnvironment( const EnvironmentPtr& env, const char *type )
{
  if ( isEmpty() ) {

    return;
  }

  OccurrencesImpl::iterator oc = occur_.begin();
  OccurrencesImpl::iterator fin = occur_.end();

  while ( oc != fin ) {

    Sample sample = env->getUnnormalized( (*oc)->x(), (*oc)->y() );

    if ( sample.size() == 0 ) {

      Log::instance()->warn( "%s Point \"%s\" at (%f,%f) has no environment. It will be discarded.\n", type, ((*oc)->id()).c_str(), (*oc)->x(), (*oc)->y() );

      oc = occur_.erase( oc );
      fin = occur_.end();
    } 
    else {

      (*oc)->setUnnormalizedEnvironment( sample );
      (*oc)->setNormalizedEnvironment( Sample() );

      ++oc;
    }
  }
}

/*****************/
/*** normalize ***/
void 
OccurrencesImpl::normalize( Normalizer * normalizerPtr, size_t categoricalThreshold )
{
  if ( ! normalizerPtr ) {

    return;
  }

  OccurrencesImpl::const_iterator occ = occur_.begin();
  OccurrencesImpl::const_iterator end = occur_.end();
  
  // set the normalized values 
  while ( occ != end ) {

    (*occ)->normalize( normalizerPtr, categoricalThreshold );
    ++occ;
  }
}

/***************************/
/*** reset Normalization ***/
void 
OccurrencesImpl::resetNormalization()
{
  OccurrencesImpl::const_iterator occ = occur_.begin();
  OccurrencesImpl::const_iterator end = occur_.end();
  
  while ( occ != end ) {

    (*occ)->setNormalizedEnvironment( (*occ)->originalEnvironment() );
    ++occ;
  }
}

/******************/
/*** get MinMax ***/
void
OccurrencesImpl::getMinMax( Sample * min, Sample * max ) const
{
  OccurrencesImpl::const_iterator occ = occur_.begin();
  OccurrencesImpl::const_iterator end = occur_.end();

  *min = Sample( (*occ)->environment() );
  *max = Sample( (*occ)->environment() );

  // grab max and min values per variable
  while ( occ != end ) {

      Sample sample = (*occ)->environment();
      *min &= sample;
      *max |= sample;
      ++occ;
  }
}


/**************/
/*** insert ***/
void
OccurrencesImpl::createOccurrence( const std::string& id, 
                                   Coord longitude, Coord latitude,
                                   Scalar error, Scalar abundance,
                                   int num_attributes, Scalar *attributes,
                                   int num_env, Scalar *env )
{
  // Transforms the given coordinates in the common openModeller
  // coordinate system.
  gt_->transfOut( &longitude, &latitude );
  
  insert( new OccurrenceImpl( id, longitude, latitude, error, abundance,
			      num_attributes, attributes,
			      num_env, env ) );
  
}

void 
OccurrencesImpl::createOccurrence( const std::string& id, 
                                   Coord longitude, Coord latitude,
                                   Scalar error, Scalar abundance,
                                   std::vector<double> attributes,
                                   std::vector<double> env)
{
  // Transforms the given coordinates in the common openModeller
  // coordinate system.
  gt_->transfOut( &longitude, &latitude );
  
  insert( new OccurrenceImpl( id, longitude, latitude, error, abundance,
			      attributes, env ) );
  
}

void
OccurrencesImpl::insert( const OccurrencePtr& oc )
{
  occur_.push_back( oc );
}

OccurrencesImpl*
OccurrencesImpl::clone() const
{
  
  const_iterator it = occur_.begin();
  const_iterator end = occur_.end();
  
  OccurrencesImpl* clone = new OccurrencesImpl( name_, cs_ );

  while( it != end ) {
    
    clone->insert( new OccurrenceImpl( *(*it) ) );
    
    it++;
  }

  return clone;
}

bool
OccurrencesImpl::hasEnvironment() const
{
  if ( ! numOccurrences() ) {

      return false;
  }

  const_iterator it = occur_.begin();

  return (*it)->hasEnvironment();
}

int
OccurrencesImpl::dimension() const
{
  if ( hasEnvironment() ) {

      const_iterator it = occur_.begin();

      return (*it)->environment().size();
  }
  else { 

      return 0;
  }
}

/******************/
/*** get Random ***/
ConstOccurrencePtr
OccurrencesImpl::getRandom() const
{
  Random rnd;
  int selected = (int) rnd( numOccurrences() );

  return occur_[ selected ];
}

OccurrencesImpl::iterator
OccurrencesImpl::erase( const iterator& it ) 
{
  swap( occur_.back(), (*it) );
  occur_.pop_back();
  return it;
}


void 
OccurrencesImpl::appendFrom( const OccurrencesPtr& source )
{
  if ( ! source ) {

    return;
  }

  const_iterator it = source->begin();
  const_iterator end = source->end();

  while ( it != end ) {

      insert(*it);
      ++it;
  }
}


/******************************/
/*** get Environment Matrix ***/
std::vector<ScalarVector> 
OccurrencesImpl::getEnvironmentMatrix()
{
  std::vector<ScalarVector> matrix( dimension() );

  // Initialize matrix
  for ( unsigned int i = 0; i < matrix.size(); i++ ) {

    matrix[i] = ScalarVector( numOccurrences() );
  }

  const_iterator c = occur_.begin();
  const_iterator end = occur_.end();

  int j = 0;

  // For each Occurrence
  while ( c != end ) {
      
    Sample const& sample = (*c)->environment();

    // For each layer
    for ( unsigned int i = 0; i < matrix.size(); i++ ) {

      // Feed new matrix
      matrix[i][j] = sample[i];
    }

    ++c;
    ++j;
  }

  return matrix;
}


/*************/
/*** print ***/
void
OccurrencesImpl::dump( std::string msg ) const
{
  Log::instance()->info( "%s\n", msg.c_str() );

  // Occurrences general data.
  Log::instance()->info( "Name: %s\n", name_.c_str() );
  Log::instance()->info( "\nOccurrences: %d\n\n", numOccurrences() );

  const_iterator c = occur_.begin();
  const_iterator end = occur_.end();

  while ( c != end ) {

    // Get attributes

    ostringstream ss;
    
    Sample::const_iterator attr = (*c)->attributes().begin();
    Sample::const_iterator end = (*c)->attributes().end();
    ss << "( ";

    while ( attr != end ) {

      ss << *attr << " ";
      attr++;
    }

    ss << ")\n";

    Log::instance()->info( "(%+8.4f, %+8.4f)\n", (*c)->x(), (*c)->y() );
    
    (*c)->dump();
    
    c++;
  }
}


/***************************/
/**** split Occurrences ****/
void splitOccurrences(const OccurrencesPtr& occurrences, 
                      OccurrencesPtr& trainOccurrences, 
                      OccurrencesPtr& testOccurrences, 
                      double propTrain)
{
  // add all samples to an array
  int i;
  int n = occurrences->numOccurrences();
  int k = (int) (n * propTrain);
  std::vector<int> goToTrainSet(n);

  // first k are set to go to train set
  for ( i = 0; i < k; i++ ) {

    goToTrainSet[i] = 1;
  }

  // all others are set to go to test set
  for ( ; i < n; i++ ) {

    goToTrainSet[i] = 0;
  }

  // shuffle elements well
  initRandom();

  std::random_shuffle( goToTrainSet.begin(), goToTrainSet.end() );

  // traverse occurrences copying them to the right sampler
  OccurrencesImpl::const_iterator it = occurrences->begin();
  OccurrencesImpl::const_iterator fin = occurrences->end();

  i = 0;

  while( it != fin ) {

    if ( goToTrainSet[i] ) {

      trainOccurrences->insert( new OccurrenceImpl( *(*it) ) );
    }
    else {

      testOccurrences->insert( new OccurrenceImpl( *(*it) ) );
    }

    ++i; ++it;
  }
}

/***************************/
/**** split Occurrences in train/test using distance between points( Missae 09/2009 ) ****/
void splitOccurrences(const OccurrencesPtr& occurrences, 
                      OccurrencesPtr& trainOccurrences, 
                      OccurrencesPtr& testOccurrences)
{
  double dist, distLimit=8.0, x, y, xmin, xmax, ymin, ymax, deltax, deltay;
  unsigned int flag = 0, i = 0, itrain=0, ktrain=0, ioccur=0, flagOk=0;
  std::vector<double> occurTransformx( occurrences->numOccurrences() );
  std::vector<double> occurTransformy( occurrences->numOccurrences() );
  std::vector<int> testId( occurrences->numOccurrences() );
  int n = occurrences->numOccurrences(), icont=0;
  int nptTeste = (int) (n * 0.40) + 2;

  OccurrencesImpl::const_iterator it = occurrences->begin();
  OccurrencesImpl::const_iterator fin = occurrences->end();
  
  xmin = xmax = (*it)->x();
  ymin = ymax = (*it)->y();
  
  ++it;
  while( it != fin ) {
    if ( (*it)->x() < xmin ) xmin = (*it)->x();
	else  if ( (*it)->x() > xmax) xmax = (*it)->x();
	if ( (*it)->y() < ymin) ymin = (*it)->y();
	else  if ( (*it)->y() > ymax) ymax = (*it)->y();
    ++it;
  }
  deltax = xmax - xmin;
  deltay = ymax - ymin;

  it = occurrences->begin();
  while( it != fin ) {
    occurTransformx[i] = 100 * ( (*it)->x() - xmin ) / deltax;
    occurTransformy[i] = 100 * ( (*it)->y() - ymin ) / deltay;
    i++;
    ++it;
  }
  do{
    flagOk=0, flag = 0, itrain=0, ktrain=0, ioccur=0, icont=0;

    it = occurrences->begin();

    trainOccurrences->insert( new OccurrenceImpl( *(*it) ) );
    testOccurrences->insert( new OccurrenceImpl( *(*it) ) );

    ++it;
    testId[ktrain] = ioccur;
    ktrain++;
    ioccur++;

    while( it != fin ) {

      for ( i = 0; i < ktrain; i++ ) {
	    itrain = testId[i];
	    x = occurTransformx[ioccur] - occurTransformx[itrain];
	    y = occurTransformy[ioccur] - occurTransformy[itrain];
        dist = sqrt(  (x*x) + (y*y)  );

        if ( dist < distLimit) {
          testOccurrences->insert( new OccurrenceImpl( *(*it) ) );
		  flag = 1;
		  icont++;
		  break;
        }
	  }

	  if (icont > nptTeste){
	    OccurrencesImpl::iterator it = testOccurrences->begin();
 	    OccurrencesImpl::iterator last = testOccurrences->end();
		--last;
	    while ( it != last ) {
		  it = testOccurrences->erase(it);
		  last = testOccurrences->end();
		  --last;
	    }
		it = testOccurrences->erase(it);

	    OccurrencesImpl::iterator itt = trainOccurrences->begin();
 	    OccurrencesImpl::iterator lastt = trainOccurrences->end();
		--lastt;
	    while ( itt != lastt ) {
		  itt = trainOccurrences->erase(itt);
		  lastt = trainOccurrences->end();
		  --lastt;
	    }
		itt = trainOccurrences->erase(itt);

        if (distLimit > 1.0) distLimit = distLimit - 1.0;
		else if (distLimit > 0.2) distLimit = distLimit - 0.2;
		else distLimit = distLimit - 0.02;
	    flagOk=1;
        break;
	  }
  	  if (!flag){
        trainOccurrences->insert( new OccurrenceImpl( *(*it) ) );
        testId[ktrain] = ioccur;
	    ktrain++;
	  }else{
        flag = 0;
	  }
	  ioccur++;
      ++it;
    }
  }while(flagOk == 1);
}
