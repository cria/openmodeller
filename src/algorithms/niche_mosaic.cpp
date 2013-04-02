/**
 * Declaration of class NicheMosaic
 *
 * @author Missae Yamamoto (missae at dpi . inpe . br)
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2009 by INPE -
 * Instituto Nacional de Pesquisas Espaciais
 *
 * http://www.inpe.br
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

#include "niche_mosaic.hh"

#include <openmodeller/Configuration.hh>

#include <openmodeller/Random.hh>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 1

#define NUMITERATIONS_ID "NumberOfIterations"

/*************************************/
/*** Algorithm parameters metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    NUMITERATIONS_ID,       // Id
    "Number of iterations", // Name.
    Integer,                // Type.

    // Overview
    "Number of iterations.",

    // Description.
    "Number of iterations.",

    1,       // Not zero if the parameter has lower limit.
    1000,    // Parameter's lower limit.
    0,       // Not zero if the parameter has upper limit.
    0,       // Parameter's upper limit.
    "2000"   // Parameter's typical (default) value.
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "NICHE_MOSAIC",   // Id.
  "Niche Mosaic",   // Name.
  "0.1",    // Version.

  // Overview
  "This algorithm is still experimental. You may not use it in publications without the author's permission.",

  // Description.
  "This algorithm is still experimental. You may not use it in publications without the author's permission.",

  "Missae Yamamoto",  // Author

  // Bibliography.
  "",

  "Missae Yamamoto",         // Code author.
  "missae [at] dpi.inpe.br", // Code author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};

/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new NicheMosaic();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/*************************************************************/
/************************ Niche Mosaic ***********************/

NicheMosaic::NicheMosaic() :
  AlgorithmImpl( &metadata ),
  _num_iterations(0),
  _num_points(0),
  _num_points_test(0),
  _num_points_absence_test(0),
  _num_layers(0),
  _minimum(),
  _maximum(),
  _delta(),
  _my_presences(0),
  _my_presences_test(0),
  _my_absence_test(0),
  _bestCost(0),
  _done( false ),
  _progress( 0.0 )
{
}

NicheMosaic::~NicheMosaic()
{
}

/******************/
/*** initialize ***/
int
NicheMosaic::initialize()
{
  // Check the number of presences
  int num_presences = _samp->numPresence();

  if ( num_presences < 10 ) {

    Log::instance()->warn( "Niche Mosaic needs at least 10 presence points.\n" );
    return 0;
  }

  // Check number of layers
  _num_layers = _samp->numIndependent();

  if ( _num_layers < 2 ) 
  {
    std::string msg = "Niche Mosaic needs at least 2 layers.\n";

    Log::instance()->error( msg.c_str() );
    return 0;
  }

  // Get parameters
  if ( ! getParameter( NUMITERATIONS_ID, &_num_iterations ) ) {

    Log::instance()->error( "Parameter '" NUMITERATIONS_ID "' not passed.\n" );
    return 0;
  }

  if ( _num_iterations < 1000 ) {

    Log::instance()->error( "Parameter '" NUMITERATIONS_ID "' must be greater than 999.\n" );
    return 0;
  }
  
  // remove discrepancy presence points
  OccurrencesPtr cleanPresences = cleanOccurrences( _samp->getPresences() );
  _sampp = createSampler( _samp->getEnvironment(), cleanPresences, _samp->getAbsences() );

  // generate pseudo absence points using simple algorithm.
  size_t num_abs = (size_t)(0.40 * num_presences);
  if (num_abs < 10) num_abs = 10;
  int dim = _sampp->numIndependent();
  Sample minimum(dim), maximum(dim);
  OccurrencesPtr pres = _sampp->getPresences();
  pres->getMinMax( &minimum, &maximum );

  double delta;
  for( unsigned int i=0; i<minimum.size(); i++) {
    delta = (maximum[i] - minimum[i]) * 0.10;
    minimum[i] = minimum[i] - delta;
    maximum[i] = maximum[i] + delta;
  }

  _my_absence_test = _sampp->getPseudoAbsences( num_abs, &minimum, &maximum ); 

  _num_points_absence_test = _my_absence_test->numOccurrences(); 

  _my_presences = new OccurrencesImpl( _my_absence_test->label(), _my_absence_test->coordSystem() );

  _my_presences_test = new OccurrencesImpl( _my_absence_test->label(), _my_absence_test->coordSystem() );

  return 1;
}

/***************/
/*** iterate ***/
int
NicheMosaic::iterate()
{
  std::vector<Scalar> deltaBest( _num_layers );
  size_t costBest, num_points_train_test, bestCost2=0;
  int bestIter = 0;

  num_points_train_test = _sampp->getPresences()->numOccurrences();
  _model_min_best.resize( num_points_train_test );
  _model_max_best.resize( num_points_train_test );

  for ( unsigned int i = 0; i < _model_min_best.size(); i++ ) {
	_model_min_best[i] = ScalarVector( _num_layers );
    _model_max_best[i] = ScalarVector( _num_layers );
  }//end for


  // Split sampler in test/train
  splitOccurrences( _sampp->getPresences(), _my_presences, _my_presences_test );
  _num_points_test = _my_presences_test->numOccurrences();
  _num_points = _my_presences->numOccurrences();
  
  if ( 0 == setMinMaxDelta() ) {return 0;}
  
  int endDo = _num_layers * 10;
  do{
    findSolution(costBest, deltaBest, bestIter, bestCost2);

	_num_iterations = _num_iterations + 8000;
	if (_num_iterations > 30000) break;
  }while( (bestIter < endDo) || (costBest < (size_t)(_num_points_test*0.8)) );

  if ( (size_t)(_num_points_absence_test*0.6) > bestCost2 ){

    int dim = _sampp->numIndependent();
    Sample minimum(dim), maximum(dim);
    OccurrencesPtr pres = _sampp->getPresences();
    pres->getMinMax( &minimum, &maximum );

    double delta;
    for( unsigned int i=0; i<minimum.size(); i++) {
      delta = (maximum[i] - minimum[i]) * 0.10;
      minimum[i] = minimum[i] - delta;
      maximum[i] = maximum[i] + delta;
    }

    _my_absence_test = _sampp->getPseudoAbsences( 100, &minimum, &maximum ); 

	  
	_num_points_absence_test = _my_absence_test->numOccurrences();  

	_num_iterations = 10000;
    findSolution(costBest, deltaBest, bestIter, bestCost2);
  }

  if (costBest < _num_points_test)
	  improveModel(deltaBest);

  _done = 1;

  return 1;
}

float 
NicheMosaic::getProgress() const
{
  if (done()) {

    return 1.0;
  }
  else {

    return _progress;
  }
}

/*****************/
/*** get Value ***/   /********matchRules classes******/
Scalar
NicheMosaic::getValue( const Sample& x ) const
{
  int i, j, k = 0;
  Scalar percent;

  //_num_points represents the number of rules
  for (i = 0; i < _num_points; i++) {
    for (j = 0; j < _num_layers; j++) {

      if ( ( _model_min_best[i][j] <= x[j] ) && ( x[j] <= _model_max_best[i][j] ) )
        continue;
      else
        break;
    }//end for

    if ( j == _num_layers ) {
      k++;
    }//end if
  }//end for

  percent = (Scalar)k /(Scalar)_num_points;
  if (percent == 0.0) return 0.0;
  else if (percent < 0.10) return 0.5;
  else if (percent < 0.25) return 0.7;
  else if (percent < 0.90) return 0.9;
  else return 1.0; 
}


int 
NicheMosaic::setMinMaxDelta()
{
  OccurrencesImpl::const_iterator oc = _my_presences->begin();
  OccurrencesImpl::const_iterator end = _my_presences->end();

  Sample const & sample = (*oc)->environment();
  _minimum = sample;
  _maximum = sample;
  ++oc;

  while ( oc != end ) {
    Sample const& sample = (*oc)->environment();
    _minimum &= sample;
    _maximum |= sample;
    ++oc;
  }

  _delta = _maximum;
  _delta -= _minimum;

  for ( int j = 0; j < _num_layers; j++ ) {
    if (_delta[j] == 0.0) {
      Log::instance()->error( "No delta for layer %d\n", j );
      return 0;
    }
  }

  return 1;
}

void 
NicheMosaic::createModel( std::vector<ScalarVector> &model_min, std::vector<ScalarVector> &model_max, const std::vector<Scalar> &delta )
{
  size_t i=0;
  OccurrencesImpl::const_iterator oc = _my_presences->begin();
  OccurrencesImpl::const_iterator end = _my_presences->end();

  while ( oc != end ) {
    Sample const& sample = (*oc)->environment();

    for ( int j = 0; j < _num_layers; j++ ) {
      model_min[i][j] = sample[j] - delta[j];
      model_max[i][j] = sample[j] + delta[j];
    }//end for

    ++oc;
    ++i;
  }//end while
}

void 
NicheMosaic::editModel( std::vector<ScalarVector> &model_min, std::vector<ScalarVector> &model_max, const std::vector<Scalar> &delta, size_t i_layer )
{
  size_t i=0;
  OccurrencesImpl::const_iterator oc = _my_presences->begin();
  OccurrencesImpl::const_iterator end = _my_presences->end();

  while ( oc != end ) {
    Sample const& sample = (*oc)->environment();

    model_min[i][i_layer] = sample[i_layer] - delta[i_layer];
    model_max[i][i_layer] = sample[i_layer] + delta[i_layer];

    ++oc;
    ++i;
  }//end while
}

size_t
NicheMosaic::calculateCostPres( const std::vector<ScalarVector> &model_min, const std::vector<ScalarVector> &model_max )
{
  OccurrencesImpl::iterator it = _my_presences_test->begin(); 
  OccurrencesImpl::iterator last = _my_presences_test->end();

  int i, j, npresence = 0;

  //presence
  while ( it != last ) {

    Sample const& sample = (*it)->environment();

    //_num_points eh o numero de regras do modelo
    for (i = 0; i < _num_points; i++) {

      for (j = 0; j < _num_layers; j++) {

        if ( ( model_min[i][j] <= sample[j] ) && ( sample[j] <= model_max[i][j] ) )
          continue;
        else
          break;
      }//end for

      if ( j == _num_layers ) {
        npresence++;
		break;
      }//end if
    }//end for
    ++it;
  }//end while

  return npresence;
}

size_t
NicheMosaic::calculateCostAus( const std::vector<ScalarVector> &model_min, const std::vector<ScalarVector> &model_max )
{
  int i, j, nabsence = 0;

  //absence
  OccurrencesImpl::iterator it_absence = _my_absence_test->begin(); 
  OccurrencesImpl::iterator last_absence = _my_absence_test->end();

  while ( it_absence != last_absence ) 
  {     
    Sample const& samp = (*it_absence)->environment();

    for (i = 0; i < _num_points; i++){ //_num_points eh o numero de regras do modelo

      for (j = 0; j < _num_layers; j++){

        if ( ( model_min[i][j] <= samp[j] ) && ( samp[j] <= model_max[i][j] ) )
          continue;
        else
          break;
      }//end for

      if ( j == _num_layers )
        break;
    }//end for

    if ( i == _num_points)
      nabsence++;

    ++it_absence;
  }//end while

  return nabsence;
}

size_t 
NicheMosaic::getRandomLayerNumber()
{
  Random random;

  return random( 0, _num_layers );
}

Scalar 
NicheMosaic::getRandomPercent(const std::vector<Scalar> &delta, const size_t i_layer, size_t &costPres)
{
  int size = 100, r;

  double min_percent = 0.12, max_percent = 0.4;

  double new_percent, half_percent = (max_percent - min_percent) / 2 + min_percent;

  Random random;

  r = random( 0, size );
  new_percent = (max_percent - min_percent) * ( (double) r / (double) size ) + min_percent;
    
  if ( (costPres < _num_points_test) && (new_percent < half_percent) ){

    new_percent = new_percent + half_percent - min_percent;
  }

  return new_percent;
}

void
NicheMosaic::renewTabuDegree(std::vector<size_t> &tabuDegree)
{
  for (int i = 0; i < _num_layers; i++) {

    if (tabuDegree[i] > 0)
      tabuDegree[i] = tabuDegree[i] - 1;
  }
}

void
NicheMosaic::saveBestModel(const std::vector<ScalarVector> &model_min, const std::vector<ScalarVector> &model_max)
{
  for (int i = 0; i < _num_points; i++) {
    for (int j = 0; j < _num_layers; j++) {
      _model_min_best[i][j] = model_min[i][j];
      _model_max_best[i][j] = model_max[i][j];
    }
  }
}

void 
NicheMosaic::improveModel( const std::vector<Scalar> &deltaBest )
{
  OccurrencesImpl::iterator it = _my_presences_test->begin(); 
  OccurrencesImpl::iterator last = _my_presences_test->end();

  int i, j, flag = 0, n = _num_points;

  while ( it != last ) {

    Sample const& sample = (*it)->environment();

    //_num_points eh o numero de regras do modelo
    for (i = 0; i < n; i++) {

      for (j = 0; j < _num_layers; j++) {

        if ( ( _model_min_best[i][j] <= sample[j] ) && ( sample[j] <= _model_max_best[i][j] ) )
          continue;
        else
          break;
      }//end for

      if ( j == _num_layers ) {
        flag = 1;
		break;
      }//end if
    }//end for
	if (flag){
		flag = 0;
	}else {
	    for (j = 0; j < _num_layers; j++) {
	        _model_min_best[n][j] = sample[j] - deltaBest[j];
			_model_max_best[n][j] = sample[j] + deltaBest[j];

		}//end for
		n++;
	}//end if
    ++it;
  }//end while
  _num_points = n;
}

void 
NicheMosaic::findSolution(size_t &costBest, std::vector<Scalar> &deltaBest, int &bestIter, size_t &bestCost2)
{
  size_t cost1, cost2, i_layer;
  Scalar importance = 1.0, cost, deltaIni=0.4;
  std::vector<Scalar> delta( _num_layers );

  size_t nTabu = (size_t)floor(sqrt((double)(_num_layers)));
  std::vector<size_t> tabuDegree( _num_layers );

  for( int l = 0; l < _num_layers; l++ ) 
    tabuDegree[l] = 0;

  //model
  std::vector<ScalarVector> model_min( _num_points );
  std::vector<ScalarVector> model_max( _num_points );

  for ( unsigned int i = 0; i < model_min.size(); i++ ) {
    model_min[i] = ScalarVector( _num_layers );
    model_max[i] = ScalarVector( _num_layers );
  }//end for

  for ( int j = 0; j < _num_layers; j++ ){

    delta[j] = _delta[j] * deltaIni;
	deltaBest[j] = delta[j];
  }//end for

  createModel( model_min, model_max, delta );
  cost1 = calculateCostPres( model_min, model_max );
  costBest = cost1;
  cost2 = calculateCostAus( model_min, model_max );
  cost = (Scalar)cost1*importance + (Scalar)cost2;

 _bestCost = cost;
  saveBestModel(model_min, model_max);
  if (_bestCost == ((Scalar)_num_points_test*importance + (Scalar)_num_points_absence_test) )
	  return;

  for (int iter=0; iter < _num_iterations; iter++) {

    _progress = (float)iter/(float)_num_iterations;
	i_layer = getRandomLayerNumber();

	Scalar deltaAux = delta[i_layer];
	delta[i_layer] = _delta[i_layer] * getRandomPercent(delta, i_layer, cost1);
	editModel( model_min, model_max, delta, i_layer );
	cost1 = calculateCostPres( model_min, model_max );
	cost2 = calculateCostAus( model_min, model_max );
	cost = (Scalar)cost1*importance + (Scalar)cost2;

	if ( (cost > _bestCost) || ( (cost == _bestCost) && (cost1 >= costBest) ) ) {
	  renewTabuDegree(tabuDegree);
	  tabuDegree[i_layer] = nTabu;
	  costBest = cost1;
	  _bestCost = cost;
	  deltaBest[i_layer] = delta[i_layer];
	  saveBestModel(model_min, model_max);
	  bestIter = iter;
	  bestCost2=cost2;

	  if (_bestCost == ((Scalar)_num_points_test*importance + (Scalar)_num_points_absence_test) )
		break;
	}else {
		if (tabuDegree[i_layer] == 0) {
			renewTabuDegree(tabuDegree);
			tabuDegree[i_layer] = nTabu;
		}else{
			delta[i_layer] = deltaAux;
			editModel( model_min, model_max, delta, i_layer );
		}//end if
	}//end if
  }//end for
}

/****************************************************************/
/****************** configuration *******************************/
void
NicheMosaic::_getConfiguration( ConfigurationPtr& config ) const
{

  if ( !_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl( "NicheMosaic" ) );
  config->addSubsection( model_config );

  model_config->addNameValue( "NumLayers", _num_layers );
  model_config->addNameValue( "NumPoints", _num_points );

  for (int i = 0; i < _num_points; i++) {

    ConfigurationPtr rule_config( new ConfigurationImpl( "Rule" ) );
	model_config->addSubsection( rule_config );

    Sample min_best( _model_min_best[i] );
    Sample max_best( _model_max_best[i] );

    rule_config->addNameValue( "Min", min_best );
    rule_config->addNameValue( "Max", max_best );

  }
}

void
NicheMosaic::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "NicheMosaic" );

  if (!model_config)
    return;

  _num_layers = model_config->getAttributeAsInt( "NumLayers", 0 );
  _num_points = model_config->getAttributeAsInt( "NumPoints", 0 );

  _model_min_best.resize( _num_points );
  _model_max_best.resize( _num_points );

  Configuration::subsection_list subelements = model_config->getAllSubsections();

  Configuration::subsection_list::iterator subelement = subelements.begin();
  Configuration::subsection_list::iterator last_subelement = subelements.end();

  int i = 0;

  for ( ; subelement != last_subelement; ++subelement ) {

    if ( (*subelement)->getName() == "Rule" ) {

      _model_min_best[i] = (*subelement)->getAttributeAsVecDouble( "Min" );
      _model_max_best[i] = (*subelement)->getAttributeAsVecDouble( "Max" );

      ++i;
    }
  }

  _done = true;

  return;
}

OccurrencesPtr
NicheMosaic::cleanOccurrences( const OccurrencesPtr& occurrences )
{
  OccurrencesPtr presence( new OccurrencesImpl(0.0) );
  OccurrencesPtr presenceAux( new OccurrencesImpl(0.0) );
  
  double dist, distLimit=8.0, x, y, xmin, xmax, ymin, ymax, deltax, deltay;
  unsigned int flag = 0, i = 0, itrain=0, ktrain=0, ioccur=0;
  std::vector<double> occurTransformx( occurrences->numOccurrences() );
  std::vector<double> occurTransformy( occurrences->numOccurrences() );
  std::vector<int> testId( occurrences->numOccurrences() );

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

  flag = 0, itrain=0, ktrain=0, ioccur=0;

  it = occurrences->begin();

  presenceAux->insert( new OccurrenceImpl( *(*it) ) );

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
        presence->insert( new OccurrenceImpl( *(*it) ) );
	    flag = 1;
	    break;
      }
	}

  	if (!flag){
      presenceAux->insert( new OccurrenceImpl( *(*it) ) );
      testId[ktrain] = ioccur;
	  ktrain++;
	}else{
      flag = 0;
	}
	ioccur++;
    ++it;
  }
  
  //verify presenceAux points 
  SamplerPtr samp = createSampler( _samp->getEnvironment(), presence, _samp->getAbsences() );
  Sample mean;
  Sample deviation;
  computeMeanDeviation( presence, mean, deviation  );

  OccurrencesImpl::const_iterator itt = presenceAux->begin();
  OccurrencesImpl::const_iterator finn = presenceAux->end();

  double prob;
  OccurrencePtr occ;
  while( itt != finn ) {
    occ = (*itt);

	Sample dif = occ->environment();
    dif -= mean;
    prob = 1.0;
    for( unsigned int i=0; i<dif.size(); i++) {
      Scalar cutoff = deviation[i];

      if ( dif[i] > cutoff || dif[i] < -cutoff ) {
        prob = 0.0;
		break;
      }
    }

    if ( prob == 1.0 ){
      presence->insert( new OccurrenceImpl( *(*itt) ) );
    }
    ++itt;
  }

  return presence;
}

void
NicheMosaic::computeMeanDeviation( const OccurrencesPtr& occs, Sample& mean, Sample& deviation  )
{
  // compute mean
  OccurrencesImpl::const_iterator oc = occs->begin();
  OccurrencesImpl::const_iterator end = occs->end();

  Sample min, max;

  Sample const & sample = (*oc)->environment();
  min = sample;
  max = sample;
  mean = sample;   
  ++oc;
 
  while ( oc != end ) {
      
    Sample const& sample = (*oc)->environment();
      
    mean += sample;
    min &= sample;
    max |= sample;

    ++oc;
  }
  mean /= Scalar( occs->numOccurrences() );

  // compute the std deviation 
  deviation.resize( mean.size() );
  oc = occs->begin();

  // compute the variance.
  while ( oc != end ) {
    Sample tmp( (*oc)->environment() );
    tmp -= mean;
    tmp *= tmp;
    deviation += tmp;
    ++oc;
  }

  // divide by (npnt - 1)
  Scalar npts = Scalar( occs->numOccurrences() - 1 );
  deviation /= npts;
  deviation.sqrt();
  deviation *= 5.0;
}
