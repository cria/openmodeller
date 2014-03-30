/**
 * Definition of Environment class.
 * 
 * @author Mauro E S Mu�oz (mauro@cria.org.br)
 * @date   2003-03-13
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


#include <openmodeller/Environment.hh>

#include <openmodeller/os_specific.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/RasterFactory.hh>
#include <openmodeller/env_io/GeoTransform.hh>
#include <openmodeller/Random.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Occurrence.hh>
#include <openmodeller/Exceptions.hh>

#if defined (HAVE_VALUES_H) && !defined(WIN32)
#include <values.h>
#else
#include <float.h>
#define MAXFLOAT FLT_MAX
#endif

using std::string;
using std::vector;

#undef DEBUG_GET

/****************************************************************/
/*********************** factory methods ************************/
EnvironmentPtr createEnvironment( const std::vector<std::string>& categs,
                                  const std::vector<std::string>& maps,
                                  const std::string& mask_file )
{
  return EnvironmentPtr( new EnvironmentImpl( categs, maps, mask_file ) );
}

EnvironmentPtr createEnvironment( const std::vector<std::string>& categs,
                                  const std::vector<std::string>& maps )
{
  return EnvironmentPtr( new EnvironmentImpl( categs, maps, "" ) );
}

EnvironmentPtr createEnvironment( const ConstConfigurationPtr& config )
{
  EnvironmentPtr env( new EnvironmentImpl() );

  env->setConfiguration( config );

  return env;
}

EnvironmentPtr createEnvironment( )
{
  return EnvironmentPtr( new EnvironmentImpl( ) );
}


/****************************************************************/
/******************* static utility functions *******************/

ConfigurationPtr
EnvironmentImpl::getLayerConfig( const layer& l, bool basicConfig ) {

  ConfigurationPtr cfg( new ConfigurationImpl() );

  cfg->addNameValue( "Id", l.first );

  if ( ! basicConfig ) {

    cfg->addNameValue( "IsCategorical", l.second->isCategorical() );

    if ( l.second->hasMinMax() ) {

      Scalar min;
      Scalar max;
      l.second->getMinMax( &min, &max );
      cfg->addNameValue( "Min", min );
      cfg->addNameValue( "Max", max );
    }
  }

  return cfg;
}

EnvironmentImpl::layer
EnvironmentImpl::makeLayer( const ConstConfigurationPtr& config, Map *map ) {

  string filename = config->getAttribute( "Id" );
  // map should already have the categorical attribute set!
  //int categ = config->getAttributeAsInt( "IsCategorical", 0 );

  layer l = makeLayer( filename, map );

  try {

    // The calls to getAttribute( string ) will throw if
    // the attribute is not found.
    config->getAttribute("Min");
    config->getAttribute("Max");

    // If we make it here, the attributes exist, and the
    // hard coded default value of 0.0 will not be used.
    double min = config->getAttributeAsDouble( "Min", 0.0 );
    double max = config->getAttributeAsDouble( "Max", 0.0 );

    l.second->setMinMax( min, max );
  }
  catch (AttributeNotFound& e) {

    UNUSED(e);
  }

  return l;
}

EnvironmentImpl::layer
EnvironmentImpl::makeLayer( const string& filename, int categ ) {

  Map *map = new Map( RasterFactory::instance().create( filename, categ ) );

  return makeLayer( filename, map );
}

EnvironmentImpl::layer
EnvironmentImpl::makeLayer( const string& filename, Map *map ) {

  layer l;

  if ( !map ) {

    Log::instance()->warn( "Cannot read environment file: '%s'\n", filename.c_str() );
  }
  else {

    l.first = filename;
    l.second = map;
  }

  return l;
}


/****************************************************************/
/************************* EnvironmentImpl **************************/

/*******************/
/*** constructor ***/

EnvironmentImpl::EnvironmentImpl() :
  _layers(),
  _mask(),
  _xmin(0),
  _ymin(0),
  _xmax(0),
  _ymax(0),
  _normalizerPtr(0)
{
}

EnvironmentImpl::EnvironmentImpl( const std::vector<std::string>& categs,
				  const std::vector<std::string>& maps, 
				  const std::string& mask )
{
  initialize( categs, maps, mask );
}

void
EnvironmentImpl::initialize( const std::vector<std::string>& categs,
			     const std::vector<std::string>& maps, 
			     const std::string& mask )
{
  _normalizerPtr = 0;

  // Initialize mask and read its region.
  changeMask( mask );
  changeLayers( categs, maps );
}


/******************/
/*** destructor ***/

EnvironmentImpl::~EnvironmentImpl()
{
  clearLayers();
  clearMask();

  if ( _normalizerPtr ) {

    delete _normalizerPtr;
  }
}


/*****************/
/*** deep copy ***/

EnvironmentImpl*
EnvironmentImpl::clone() const
{
  std::vector<std::string> categs;
  std::vector<std::string> maps;

  layers::const_iterator lay = _layers.begin();
  layers::const_iterator end = _layers.end();

  while ( lay != end ) {

    if ( lay->second->isCategorical() ) {

      categs.push_back( lay->first );
    }
    else {

      maps.push_back( lay->first );
    }

    ++lay;
  }

  EnvironmentImpl* clone = new EnvironmentImpl( categs, maps, _mask.first );

  if ( _normalizerPtr ) {

    clone->normalize( _normalizerPtr );
  }

  return clone;
}

void
EnvironmentImpl::clearLayers() {
  if (_layers.size() < 1) {
    return;
  }
  layers::iterator first = _layers.begin();
  layers::iterator end = _layers.end();
  for ( ; first != end; ++ first ) {
    delete (*first).second;
  }
  _layers.clear();
}

void
EnvironmentImpl::clearMask() {
  if ( _mask.second )
    delete _mask.second;

  _mask.first = "";
  _mask.second = 0;
}

/*********************/
/*** configuration ***/

ConfigurationPtr
EnvironmentImpl::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Environment") );

  config->addNameValue( "NumLayers", (int) _layers.size() );

  layers::const_iterator l = _layers.begin();
  layers::const_iterator end = _layers.end();

  for( ; l != end; ++l ) {

    ConfigurationPtr cfg( getLayerConfig( *l ) );
    cfg->setName("Map");
    config->addSubsection( cfg );
  }

  if ( _mask.second ) {

    bool onlyBasicConfig = true;
    ConfigurationPtr maskcfg( getLayerConfig( _mask, onlyBasicConfig ) );
    maskcfg->setName( "Mask" );
    config->addSubsection( maskcfg );
  }

  return config;
}

void
EnvironmentImpl::setConfiguration( const ConstConfigurationPtr & config )
{
  clearMask();
  clearLayers();

  // Store configuration references for each layer
  // Important: users can mix continuous and categorical layers, but oM
  //            requires categorical layers to be first, so having two
  //            vectors can handle this.
  std::vector<ConstConfigurationPtr> categ_layer_confs;
  std::vector<ConstConfigurationPtr> cont_layer_confs;

  // Need to have separate containers, since loadLayers requires
  // knowing if the layer is categorical or not.
  std::vector<string> categ_layer_ids;
  std::vector<Map*> categ_layer_refs;
  std::vector<string> cont_layer_ids;
  std::vector<Map*> cont_layer_refs;

  // Mask config
  ConstConfigurationPtr mask_conf = config->getSubsection( "Mask", false );
  std::vector<string> mask_id;
  std::vector<Map*> mask_ref;

  // Suck in all the filenames.
  Configuration::subsection_list subs = config->getAllSubsections();
  Configuration::subsection_list::const_iterator it = subs.begin();
  while( it != subs.end() ) {

    string subname = (*it)->getName();
    string id = (*it)->getAttribute("Id");

    if ( subname == "Mask" && mask_id.size() == 0 ) {

      // Mask

      mask_id.push_back(id);
      mask_ref.push_back(0);
    }
    else {

      // Environmental layer

      int categ = (*it)->getAttributeAsInt( "IsCategorical", 0 );

      if ( categ == 0 ) {

        cont_layer_ids.push_back(id);
        cont_layer_refs.push_back(0);
        const ConstConfigurationPtr lconf = (*it);
        cont_layer_confs.push_back(lconf);
      }
      else {

        categ_layer_ids.push_back(id);
        categ_layer_refs.push_back(0);
        const ConstConfigurationPtr lconf = (*it);
        categ_layer_confs.push_back(lconf);
      }
    }

    ++it;
  }

  loadLayers( categ_layer_ids, categ_layer_refs, 1 );
  loadLayers( cont_layer_ids, cont_layer_refs, 0 );

  // Fill _layers attribute
  for( unsigned int i = 0; i< categ_layer_confs.size(); i++ ) {

    // Call makeLayer with the config object! This implementation is more complete
    // than just calling it with id and categorical attributes.
    _layers.push_back( makeLayer( categ_layer_confs[i], categ_layer_refs[i] ) );
  }

  for( unsigned int i = 0; i< cont_layer_confs.size(); i++ ) {

    // Call makeLayer with the config object! This implementation is more complete
    // than just calling it with id and categorical attributes.
    _layers.push_back( makeLayer( cont_layer_confs[i], cont_layer_refs[i] ) );
  }

  // Assign mask
  if ( mask_id.size() ) {

    loadLayers( mask_id, mask_ref, 0 );
    _mask = makeLayer( mask_conf, mask_ref[0] );
  }

  calcRegion();
}

/*******************/
/*** load Layers ***/
void
EnvironmentImpl::loadLayers( const std::vector<string>& map_ids, std::vector<Map*>& map_refs, int categ )
{
  bool unfinished_loading = true;
  bool got_one = false;

  while ( unfinished_loading ) {

    unfinished_loading = false;
    got_one = false;

    for( unsigned int i = 0; i< map_ids.size(); i++ ) {

      if ( map_refs[i] == 0 ) {

        try {

          map_refs[i] = new Map( RasterFactory::instance().create( map_ids[i], categ ) );
          got_one = true;
        }
        catch ( RasterException& e ) {

          if ( e.getCode() == 1 ) {

            // layer is being downloaded by other process, 
            // so skip it for now and try others
            unfinished_loading = true;
          }
          else {

            throw;
          }
        }
      }
    }

    if ( unfinished_loading && !got_one ) {

      Sleep(5000);
    }
  }
}

/*********************/
/*** change Layers ***/
int
EnvironmentImpl::changeLayers( const std::vector<std::string>& categs,
			       const std::vector<std::string>& maps )
{
  if ( ! (categs.size() + maps.size()) )
    return 0;

  clearLayers();

  // STL maps with layer ids (unique) pointing to Map*s

  // categorical data
  std::vector<Map*> categ_layer_refs;
  for( unsigned int i = 0; i< categs.size(); i++ ) {

    categ_layer_refs.push_back(0);
  }

  loadLayers( categs, categ_layer_refs, 1 );

  // continuous data
  std::vector<Map*> cont_layer_refs;
  for( unsigned int i = 0; i< maps.size(); i++ ) {

    cont_layer_refs.push_back(0);
  }

  loadLayers( maps, cont_layer_refs, 0 );

  // Fill _layers attribute
  for( unsigned int i = 0; i< categs.size(); i++ ) {

    _layers.push_back( makeLayer(categs[i], categ_layer_refs[i]) );
  }

  for( unsigned int i = 0; i< maps.size(); i++ ) {

    _layers.push_back( makeLayer(maps[i], cont_layer_refs[i]) );
  }

  calcRegion();

  return categs.size() + maps.size();
}


/*******************/
/*** change Mask ***/
int
EnvironmentImpl::changeMask( const std::string& mask_file )
{
  int ret = 1;

  clearMask();

  // New mask
  if ( !mask_file.empty() ) {

    bool unfinished_loading = true;
  
    while ( unfinished_loading ) {

      unfinished_loading = false;

      try {

        _mask = makeLayer( mask_file, 0 );
      }
      catch ( RasterException& e ) {

        if ( e.getCode() == 1 ) {

          // mask is being downloaded by other process
          unfinished_loading = true;
          Sleep(5000);
        }
        else {

          throw;
        }
      }
    }

    if ( !_mask.second ) {

      ret = 0;
    }
  }

  calcRegion();

  return ret;
}


/******************************/
/*** num Categorical Layers ***/
size_t
EnvironmentImpl::numCategoricalLayers() const
{
  size_t size = 0;

  layers::const_iterator lay = _layers.begin();
  layers::const_iterator end = _layers.end();

  while ( lay != end && lay->second->isCategorical() ) {

    ++size;
    ++lay;
  }

  return size;
}


/*****************/
/*** get Type ***/
int
EnvironmentImpl::isCategorical( int i )
{
  return _layers[i].second->isCategorical();
}


/******************/
/*** get MinMax ***/
void 
EnvironmentImpl::getMinMax( Sample * min, Sample * max ) const
{
  int i = 0;

  layers::const_iterator lay = _layers.begin();
  layers::const_iterator end = _layers.end();

  while ( lay != end ) {

    Map *map = lay->second;

    Scalar mapMin, mapMax;
    map->getMinMax( &mapMin, &mapMax );
    (*min)[i] = mapMin;
    (*max)[i] = mapMax;

    ++lay;
    i++;
  }
}

/*****************/
/*** normalize ***/
void
EnvironmentImpl::normalize( Normalizer * normalizerPtr ) {

  if ( normalizerPtr ) {

    _normalizerPtr = normalizerPtr->getCopy();
  }
  else {

    resetNormalization();
  }
}


/***************************/
/*** reset Normalization ***/
void
EnvironmentImpl::resetNormalization() {

  if ( _normalizerPtr ) {

    delete _normalizerPtr;

    _normalizerPtr = 0;
  }
}

/*********************************/
/*** get Unnormalized Internal ***/
void
EnvironmentImpl::getUnnormalizedInternal( Sample *sample, Coord x, Coord y ) const
{
  // layers and the mask, if possible.
  if ( ! checkCoordinates( x, y ) ) {
#if defined(DEBUG_GET)
    Log::instance()->debug( "EnvironmentImpl::get() Coordinate (%f,%f) is not in common region\n",x,y);
#endif
    return;
  }

  // Create the return value.
  sample->resize( _layers.size() );

  // Read variables values from the layers.
  layers::const_iterator lay = _layers.begin();
  layers::const_iterator end = _layers.end();
  Sample::iterator s = sample->begin();

  while ( lay != end ) {

    if ( ! lay->second->get( x, y, s ) ) {
#if defined(DEBUG_GET)
      Log::instance()->debug( "EnvironmentImpl::get() Coordinate (%f,%f) does not have data in layer %s\n",x,y,lay->first.c_str());
#endif
      sample->resize(0);
      return;
    }
    ++lay;
    ++s;
  }
}

Sample
EnvironmentImpl::getUnnormalized( Coord x, Coord y ) const
{
  Sample sample;
  getUnnormalizedInternal( &sample, x, y );
  return sample;
}

Sample
EnvironmentImpl::getNormalized( Coord x, Coord y ) const
{
  Sample sample;
  getUnnormalizedInternal( &sample, x, y);

  sample.setCategoricalThreshold( numCategoricalLayers() );

  if ( _normalizerPtr ) {

    _normalizerPtr->normalize( &sample );
  }

  return sample;
}

Sample
EnvironmentImpl::get( Coord x, Coord y ) const
{
  if ( _normalizerPtr ) {

    return getNormalized(x,y);
  }
  else {

    return getUnnormalized(x,y);
  }
}

Sample
EnvironmentImpl::getRandom( Coord *xout, Coord *yout ) const
{
  Random myrand;
  Coord x, y;

  Sample s;

  int max_loop = 5000;

  int loop = 0;

  do {

    x = myrand( _xmin, _xmax );
    y = myrand( _ymin, _ymax );

    s = get( x, y );

    loop++;

  } while ( s.size() == 0 && loop < max_loop );

  if ( loop == max_loop ) {

    std::string msg = "Exceeded maximum number of attempts to generate pseudo point.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  if ( xout != 0 )
    *xout = x;
  if ( yout != 0 )
    *yout = y;

  return s;
}


/*************************/
/*** check Coordinates ***/
int
EnvironmentImpl::checkCoordinates( Coord x, Coord y ) const
{
  // Accept the point, regardless of mask, if
  // it falls in a common region among all layers.
  if ( x < _xmin || x > _xmax || y < _ymin || y > _ymax ) {
#if defined(DEBUG_GET)
    Log::instance()->debug( "EnvironmentImpl::checkCoordinates() Coordinate (%f,%f) not in extent of all regions\n",x,y);
#endif

    return 0;
  }

  // If there's no mask, accept the point.
  if ( ! _mask.second ) {

    return 1;
  }

  Scalar val;

  bool hasmaskevalue = ( _mask.second->get( x, y, &val ) > 0 );

  if ( ! hasmaskevalue ) {
#if defined(DEBUG_GET)
    Log::instance()->debug( "EnvironmentImpl::check() Coordinate (%f,%f) has no mask value\n",x,y);
#endif
  }

  return hasmaskevalue;
}


/******************/
/*** get Region ***/
int
EnvironmentImpl::getRegion( Coord *xmin, Coord *ymin, Coord *xmax, Coord *ymax ) const
{
  *xmin = _xmin;
  *ymin = _ymin;
  *xmax = _xmax;
  *ymax = _ymax;

  return 1;
}

int
EnvironmentImpl::getExtremes( Sample* min, Sample* max ) const
{
  int nlayers = _layers.size();
  min->resize( nlayers );
  max->resize( nlayers );

  layers::const_iterator map = _layers.begin();

  for ( int i = 0; i < nlayers; i++  ) {
    Scalar amin, amax;
    map->second->getMinMax( &amin, &amax );
    (*min)[i] = amin;
    (*max)[i] = amax;
    ++map;
  }

  if ( _normalizerPtr ) {

    _normalizerPtr->normalize( min );
    _normalizerPtr->normalize( max );
  }

  return 1;
}


/*******************/
/*** calc Region ***/
void
EnvironmentImpl::calcRegion()
{
  Coord xmin, ymin, xmax, ymax;

  _xmin = _ymin = -MAXFLOAT;
  _xmax = _ymax =  MAXFLOAT;

  // The mask region is the default.
  if ( _mask.second )
    _mask.second->getExtent( &_xmin, &_ymin, &_xmax, &_ymax );

  // Crop region to fit all layers.
  layers::const_iterator lay = _layers.begin();
  layers::const_iterator end = _layers.end();
  while ( lay != end ) {
    lay->second->getExtent( &xmin, &ymin, &xmax, &ymax );
    ++lay;

    if ( xmin > _xmin )
      _xmin = xmin;
    
    if ( ymin > _ymin )
      _ymin = ymin;
    
    if ( xmax < _xmax )
      _xmax = xmax;
    
    if ( ymax < _ymax )
      _ymax = ymax;
  }

  if ( (_xmin >= _xmax)  || ( _ymin >= _ymax ) ) {
    Log::instance()->warn( "Maps intersection is empty (boundaries: xmin=%f, xmax=%f, ymin=%f, ymax=%f)\n", _xmin, _xmax, _ymin, _ymax );
  }
}


/********************/
/*** remove Layer ***/
void
EnvironmentImpl::removeLayer(unsigned int index)
{
  if ( index >= numLayers() ) {

    return;
  }

  layers::iterator it = _layers.begin();

  unsigned int tmp = 0;

  while ( tmp < index ) {

    ++tmp;
    ++it;
  }

  _layers.erase( it );

  calcRegion();
}
