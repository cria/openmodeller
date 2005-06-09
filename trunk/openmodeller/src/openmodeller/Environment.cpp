/**
 * Definition of Environment class.
 * 
 * @file
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


#include <environment.hh>

#include <om_log.hh>
#include <env_io/map.hh>
#include <env_io/raster.hh>
#include <env_io/geo_transform.hh>
#include <random.hh>
#include <configuration.hh>
#include <occurrence.hh>
#include <Exceptions.hh>

#ifdef HAVE_VALUES_H
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
EnvironmentPtr createEnvironment( std::vector<std::string> categs,
                                  std::vector<std::string> maps,
                                  std::string mask_file )
{
  return EnvironmentPtr( new EnvironmentImpl( categs, maps, mask_file ) );
}

EnvironmentPtr createEnvironment( std::vector<std::string> categs,
                                  std::vector<std::string> maps )
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
EnvironmentImpl::getLayerConfig( const layer& l ) {
  ConfigurationPtr cfg( new ConfigurationImpl() );

  cfg->addNameValue("Filename", l.first );
  cfg->addNameValue("Categorical", l.second->isCategorical());

  if ( l.second->hasMinMax() ) {
    Scalar min;
    Scalar max;
    l.second->getMinMax( &min, &max );
    cfg->addNameValue( "Min", min );
    cfg->addNameValue( "Max", max );
  }

  return cfg;

}

EnvironmentImpl::layer
EnvironmentImpl::makeLayer( const ConstConfigurationPtr& config ) {
  string filename = config->getAttribute( "Filename" );
  int categ = config->getAttributeAsInt( "Categorical", 0 );

  layer l = makeLayer( filename, categ );
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
  }

  return l;

}

EnvironmentImpl::layer
EnvironmentImpl::makeLayer( const string& filename, int categ ) {
  layer l;
  Map *map = new Map( new Raster( filename, categ ) );
  if ( !map ) {
    g_log.warn( "Cannot read environment file: '%s'\n", filename.c_str() );
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
  _normalize( false ),
  _scales(),
  _offsets()
{
}

EnvironmentImpl::EnvironmentImpl( std::vector<std::string> categs,
				  std::vector<std::string> maps, 
				  std::string mask )
{
  initialize( categs, maps, mask );
}

void
EnvironmentImpl::initialize( std::vector<std::string> categs,
			     std::vector<std::string> maps, 
			     std::string mask )
{
  _normalize = false;

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
}

void
EnvironmentImpl::clearLayers() {
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

/******************/
/*** configuration ***/

ConfigurationPtr
EnvironmentImpl::getConfiguration() const
{

  ConfigurationPtr config( new ConfigurationImpl("Environment") );

  config->addNameValue( "Layers", (int) _layers.size() );

  layers::const_iterator l = _layers.begin();
  layers::const_iterator end = _layers.end();
  for( ; l != end; ++l ) {
    ConfigurationPtr cfg( getLayerConfig( *l ) );
    cfg->setName("Map");
    config->addSubsection( cfg );
  }

  if ( _mask.second ) {
    ConfigurationPtr maskcfg( getLayerConfig( _mask ) );
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

  // Suck in all the filenames.
  Configuration::subsection_list subs = config->getAllSubsections();
  Configuration::subsection_list::const_iterator it = subs.begin();
  while( it != subs.end() ) {

    string subname = (*it)->getName();
    string filename = (*it)->getAttribute( "Filename" );
    int categ = (*it)->getAttributeAsInt( "Categorical", 0 );
    
    layer l = makeLayer( filename, categ );

    if ( subname == "Mask" ) {
      _mask = l;
    }
    else if ( l.second ) {
      _layers.push_back( l );
    }

    ++it;
  }

  calcRegion();

}

/*********************/
/*** change Layers ***/
int
EnvironmentImpl::changeLayers( std::vector<std::string> categs,
			       std::vector<std::string> maps )
{
  if ( ! (categs.size() + maps.size()) )
    return 0;

  clearLayers();

  // Categorical maps.
  for( int i = 0; i< categs.size(); i++ ) {
    _layers.push_back( makeLayer( categs[i], 1 ) );
  }

  // Copy continuos maps.
  for( int i = 0; i< maps.size(); i++ ) {
    _layers.push_back( makeLayer( maps[i], 0 ) );
  }

  calcRegion();

  return categs.size() + maps.size();
}


/*******************/
/*** change Mask ***/
int
EnvironmentImpl::changeMask( std::string mask_file )
{
  int ret = 1;

  clearMask();

  // New mask
  if ( !mask_file.empty() ) {
    _mask = makeLayer( mask_file, 0 );
    if ( !_mask.second ) {
      ret = 0;
    }
  }

  calcRegion();

  return ret;
}


/*****************/
/*** get Type ***/
int
EnvironmentImpl::isCategorical( int i )
{
  return _layers[i].second->isCategorical();
}


/*****************/
/*** normalize ***/
void 
EnvironmentImpl::getMinMax( Sample * min, Sample * max ) const
{
  int nlayers = _layers.size();

  int i = 0;

  layers::const_iterator lay = _layers.begin();
  layers::const_iterator end = _layers.end();

  while ( lay != end ) {
    Map *map = lay->second;

    if ( !map->isCategorical() ) {
      Scalar mapMin, mapMax;
      map->getMinMax( &mapMin, &mapMax );
      (*min)[i] = mapMin;
      (*max)[i] = mapMax;
    }

    ++lay;
    i++;
  }
}

void
EnvironmentImpl::normalize( bool use_norm, const Sample& offsets, const Sample& scales ) {
  _normalize = use_norm;
  _offsets = offsets;
  _scales = scales;
}

Sample
EnvironmentImpl::get( Coord x, Coord y ) const
{
  // Make sure that (x,y) belong to a common region among all
  // layers and the mask, if possible.
  if ( ! check( x, y ) ) {
#if defined(DEBUG_GET)
    g_log.debug( "EnvironmentImpl::get() Coordinate (%f,%f) is not in common region\n",x,y);
#endif
    return Sample();
  }

  // Create the return value.
  Sample sample( _layers.size() );

  // Read variables values from the layers.
  layers::const_iterator lay = _layers.begin();
  layers::const_iterator end = _layers.end();
  Sample::iterator s = sample.begin();

  while ( lay != end ) {
    if ( ! lay->second->get( x, y, s ) ) {
#if defined(DEBUG_GET)
      g_log.debug( "EnvironmentImpl::get() Coordinate (%f,%f) does not have data in layer %s\n",x,y,lay->first.c_str());
#endif
      return Sample();
    }
    ++lay;
    ++s;
  }

  if ( _normalize ) {
    sample *= _scales;
    sample += _offsets;
  }

  return sample;
}

Sample
EnvironmentImpl::getRandom( Coord *xout, Coord *yout) const
{
  Random rand;
  Coord x, y;

  Sample s;

  do
    {
      x = rand( _xmin, _xmax );
      y = rand( _ymin, _ymax );

      s = get(x,y);

    } while ( s.size() == 0 );


  if ( xout != 0 )
    *xout = x;
  if ( yout != 0 )
    *yout = y;

  return s;
}


/*************/
/*** check ***/
int
EnvironmentImpl::check( Coord x, Coord y ) const
{
  // Accept the point, regardless of mask, if
  // it falls in a common region among all layers.
  if ( x < _xmin || x > _xmax || y < _ymin || y > _ymax ) {
#if defined(DEBUG_GET)
    g_log.debug( "EnvironmentImpl::check() Coordinate (%f,%f) not in extent of all regions\n",x,y);
#endif
    return 0;
  }

  // If there's no mask, accept the point.
  if ( ! _mask.second )
    return 1;

  Scalar val;

  bool hasmaskevalue = _mask.second->get(x,y,&val);

  if (!hasmaskevalue) {
#if defined(DEBUG_GET)
    g_log.debug( "EnvironmentImpl::check() Coordinate (%f,%f) has no mask value\n",x,y);
#endif
  }

  if ( !val ) {
#if defined(DEBUG_GET)
    g_log.debug( "EnvironmentImpl::check() Coordinate (%f,%f) has mask value of Zero\n",x,y);
#endif
  }

  return hasmaskevalue && val;
}


/******************/
/*** get Region ***/
int
EnvironmentImpl::getRegion( Coord *xmin, Coord *ymin,
			Coord *xmax, Coord *ymax ) const
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

  if ( _normalize ) {
    *min *= _scales;
    *min += _offsets;
    *max *= _scales;
    *max += _offsets;
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
    _mask.second->getRegion( &_xmin, &_ymin, &_xmax, &_ymax );

  // Crop region to fit all layers.
  layers::const_iterator lay = _layers.begin();
  layers::const_iterator end = _layers.end();
  while ( lay != end ) {
    lay->second->getRegion( &xmin, &ymin, &xmax, &ymax );
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
    g_log.warn( "Maps intersection is empty!!!\n" );
  }

}
