/**
 * Definition of OpenModeller class (former ControlInterface class).
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-25
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

#include "om_control.hh"

#include <env_io/map.hh>
#include <env_io/header.hh>
#include <env_io/raster_file.hh>
#include <env_io/geo_transform.hh>

#include <om_defs.hh>
#include <om_log.hh>
#include <om_algorithm.hh>
#include <om_alg_parameter.hh>
#include <om_sampler.hh>
#include <om_occurrences.hh>
#include <map_format.hh>
#include <occurrence.hh>

#include <algorithm_factory.hh>
#include <environment.hh>

#include <serialization/om_serializer.hh>
#include <serialization/om_deserializer.hh>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

// Directories to search for dynamic libraries.
// Fixme: read this from some configuration file.
static char *g_search_dirs[] = {
  PLUGINPATH, 
  0
};

const char * g_config_file = CONFIG_FILE;


/****************************************************************/
/************************* Open Modeller ************************/

/********************/
/*** constructors ***/

OpenModeller::OpenModeller()
{
  _env = 0;
  _alg = 0;
  _samp = 0;

  _alg_id     = 0;
  _alg_param  = 0;
  _alg_nparam = 0;

  _presence = 0;
  _absence  = 0;

  _output_file   = 0;
  _output_mask   = 0;
  _output_header = 0;

  _model_callback       = 0;
  _model_callback_param = 0;
  _map_callback         = 0;
  _map_callback_param   = 0;

  _error[0] = '\0';

  // sets _plugin_path with path from config file or hardcoded constant
  _plugin_path = NULL;
  _factory = NULL;
  resetPluginPath();

  _factory = new AlgorithmFactory( _plugin_path );
}


/******************/
/*** destructor ***/

OpenModeller::~OpenModeller()
{
  if ( _samp )     delete _samp;
  if ( _alg )      delete _alg;
  if ( _env )      delete _env;
  if ( _alg_id )   delete _alg_id;
  if ( _presence ) delete _presence;
  if ( _absence )  delete _absence;

  if ( _output_file )   delete _output_file;
  if ( _output_mask )   delete _output_mask;
  if ( _output_header ) delete _output_header;

  if (_plugin_path) deleteStringArray(_plugin_path);

  delete _factory;
}

void OpenModeller::setLogLevel(Log::Level level)
{ g_log.setLevel(level); }


/*******************/
/*** get Version ***/
char *
OpenModeller::getVersion()
{
  // Defined in "om_defs.hh".
  return OM_VERSION;
}

/****************************/
/*** get Config File name ***/
char *
OpenModeller::getConfigFileName()
{
  return (char *) g_config_file;
}


/*************************/
/*** deleteStringArray ***/
void
OpenModeller::deleteStringArray(char ** array)
{
  //g_log.debug("Deallocating existing string array\n");

  char ** curr_dir;

  // delete existing plugin_path and strings it points to first
  if (array)
    {
      curr_dir = array;
      while (*curr_dir)
	{ delete[] *curr_dir++; }
    }
  delete[] array;
}

/***********************/
/*** set Plugin Path ***/
void 
OpenModeller::setPluginPath(char ** new_plugin_path)
{
  //g_log.debug("Setting new plugin path\n");

  // pointer to current dir in plugin path passed as argument (read only)
  char ** curr_arg_dir;      

  // pointer to current dir in instance variable _plugin_path (being set)
  char ** curr_plugin_dir;

  // get number of directories
  curr_arg_dir = new_plugin_path;
  while (*curr_arg_dir++);
  //{ g_log.debug("Visiting directory %s\n", *curr_arg_dir++); }
  int num_dirs = curr_arg_dir - new_plugin_path;
  //g_log.debug("Total directories found: %d\n", num_dirs);

  deleteStringArray(_plugin_path);

  // create new array and copy values from argument
  _plugin_path = new char*[num_dirs + 1];
  curr_plugin_dir = _plugin_path;
  curr_arg_dir = new_plugin_path;
  while (*curr_arg_dir)
    {
      //g_log.debug("Copying directory name %s to path\n", *curr_arg_dir);
      *curr_plugin_dir = new char[strlen(*curr_arg_dir) + 1];
      strcpy(*curr_plugin_dir++, *curr_arg_dir++);
    }
  *curr_plugin_dir = NULL;

  if (_factory)
    _factory->setDirs(_plugin_path);
}

/*************************/
/*** reset Plugin Path ***/
void
OpenModeller::resetPluginPath()
{
  //g_log.debug("Resetting plugin path to default value\n");

  FILE * conf_file = fopen(g_config_file, "r");

  if (conf_file)
    {
      //g_log.debug("Loading plugin path from configuration file\n");

      // config file found: read the search path from it
      const int size = 1024;
      char line[size];

      // get number of dirs in path
      int num_dirs = 0;
      while (fgets(line, size, conf_file))
	{ num_dirs++; }

      deleteStringArray(_plugin_path);
      _plugin_path = new char*[num_dirs + 1];
      rewind(conf_file); 
      char ** curr_plugin_dir = _plugin_path;
      while (fgets(line, size, conf_file))
	{ 
	  // get rid of new lines
	  char * new_line;
	  if (new_line = strchr( line, '\n'))
	    *new_line = '\0';
	  *curr_plugin_dir = new char[strlen(line) + 1];
	  strcpy(*curr_plugin_dir++, line);
	}

      *curr_plugin_dir = NULL;

      if (_factory)
	_factory->setDirs(_plugin_path);

      fclose(conf_file);
    }
  else
    {
      //g_log.debug("Loading plugin path from hardcoded default\n");

      // config file not found, set plugin path to hardcoded default
      setPluginPath(g_search_dirs);
    }
}


/***********************/
/*** get Plugin Path ***/
char * 
OpenModeller::getPluginPath()
{
  int length = 0;
  char ** curr_plugin_dir = _plugin_path;

  while (*curr_plugin_dir)
    { length += strlen(*curr_plugin_dir++) + 2; }

  char * path = new char[length];
  strcpy(path, "");
  curr_plugin_dir = _plugin_path;
  while (*curr_plugin_dir)
    { 
      if (curr_plugin_dir != _plugin_path)
	{ strcat(path, ", "); }
      strcat(path, *curr_plugin_dir++);
    }

  return path;
}


/***********************/
/*** load Algorithms ***/
int 
OpenModeller::loadAlgorithms()
{
  return _factory->loadAlgorithms();
}


/****************************/
/*** available Algorithms ***/
AlgMetadata **
OpenModeller::availableAlgorithms()
{
  return _factory->availableAlgorithms();
}


/**************************/
/*** algorithm Metadata ***/
AlgMetadata *
OpenModeller::algorithmMetadata( char *algorithm_id )
{
  return _factory->algorithmMetadata( algorithm_id );
}


/********************************/
/*** num Available Algorithms ***/
int
OpenModeller::numAvailableAlgorithms()
{
  return _factory->numAvailableAlgorithms();
}


/***********************/
/*** set Environment ***/
int
OpenModeller::setEnvironment( int num_categ,
			      char **categ_map,
			      int num_continuous,
			      char **continuous_map,
			      char *mask )
{
  // set up environmental variables.

  if ( _env )
    delete _env;

  _env = new Environment( GeoTransform::cs_default,
                          num_categ, categ_map,
                          num_continuous, continuous_map, mask );

  g_log( "Environment initialized.\n" );

  return 1;
}


/*********************/
/*** set Algorithm ***/
int
OpenModeller::setAlgorithm( char *id, int nparam,
                            AlgParameter *param )
{
  AlgMetadata *meta = algorithmMetadata( id );

  // Check the parameters.
  if ( ! meta || meta->nparam != nparam )
    {
      if ( ! meta )
	g_log( "Can't get metadata for algorithm %s", id );

      if ( meta->nparam != nparam )
	g_log( "Number of parameters provided (%d) does not match required parameters (%d)",
               nparam, meta->nparam);

      return 0;
    }

  // Check if _env is initialized
  if ( ! _env )
    {
      g_log("Environmental variables not initialized.");
      return 0;
    }

  // Check if occurrence data is initialized
  if ( ! _presence )
    {
      g_log( "Occurrence data not initialized." );
      return 0;
    }

  // filter presences and absences that are masked out
  filterMaskedOccurrences(_presence);
  filterMaskedOccurrences(_absence);

  stringCopy( &_alg_id, id );

  // Reallocate '_alg_param' to stores 'nparam' parameters.
  _alg_nparam = nparam;
  if ( _alg_param )
    delete[] _alg_param;
  _alg_param = new AlgParameter[ _alg_nparam ];

  // Copy 'param' to '_alg_param'.
  AlgParameter *dst = _alg_param;
  AlgParameter *end = _alg_param + _alg_nparam;
  while ( dst < end )
      *dst++ = *param++;

  // Sampler and algorithm.
  if ( _samp )
    delete _samp;

  _samp = new Sampler( _env, _presence, _absence );

  if ( _alg )
    delete _alg;

  _alg = _factory->newAlgorithm( _samp, _alg_id, _alg_nparam,
                                 _alg_param );
  if ( ! _alg )
    {
      sprintf( _error, "Could not find (%s) algorithm.", _alg_id );
      return 0;
    }

  return 1;
}


/*******************************/
/*** filter masked occurrences */
void OpenModeller::filterMaskedOccurrences(Occurrences * occur)
{
  int i = 0, j = 0;
  Scalar * indep = new Scalar[_env->numLayers()];

  if (occur)
  {
    Occurrence * oc; 
    occur->head();
    while ( oc = occur->get() )
    {
      if (!_env->get( oc->x(), oc->y(), indep))
      { delete occur->remove(); j++; }
      
      occur->next(); i++;
    }
  occur->head();
  }

  delete[] indep;
}

/***********************/
/*** set Occurrences ***/
int
OpenModeller::setOccurrences( Occurrences *presence,
                              Occurrences *absence )
{
  _presence = presence;
  _absence  = absence;

  return 1;
}


/********************/
/*** create Model ***/
int
OpenModeller::createModel()
{
  char *error = parameterModelCheck();
  if ( error )
    {
      strcpy( _error, error );
      return 0;
    }

  g_log( "Modelling occurrences of: %s.\n", _presence->name() );

  // Check if the algorithm needs normalized variables.
  Scalar min, max;
  if ( _alg->needNormalization( &min, &max ) )
    {
      g_log( "Normalizing environment variables.\n" );
      _env->normalize( min, max );
    }

  g_log( "Creating the model\n" );

  // Initialize algorithm.  
  if ( ! _alg->initialize() )
    {
      sprintf( _error, "Algorithm (%s) could not be initialized.",
	       _alg->getID() );
      return 0;
    }

  // Generate model.
  int ncycle = 0;
  while ( _alg->iterate() && ! _alg->done() )
    {
      ncycle++;
      if ( _model_callback )
        (*_model_callback)( ncycle, _model_callback_param );
    }

  // Algorithm terminated with error.
  if ( ! _alg->done() )
    {
      sprintf( _error, "Algorithm (%s) iteraction error.",
	       _alg->getID() );
      return 0;
    }

  // Finalise algorithm.  
  if ( ! _alg->finalize() )
    {
      sprintf( _error, "Algorithm (%s) could not be finalized.",
	       _alg->getID() );
      return 0;
    }

  return 1;
}


/*******************/
/*** string Copy ***/
void
OpenModeller::stringCopy( char **dst, char *src )
{
  if ( *dst )
    delete *dst;

  if ( src )
    {
      *dst = new char[1 + strlen( src )];
      strcpy( *dst, src );
    }
  else
    *dst = 0;
}


/*****************************/
/*** parameter Model Check ***/
char *
OpenModeller::parameterModelCheck()
{
  // Presence occurrence points.
  if ( ! _presence )
    return "Presence occurrences points were not set.";

  // Sampler
  if ( ! _samp )
    return "Sampler not specified.";

  // Environmental data.
  if ( ! _env )
    return "Environmental variables not specified.";


  // Algorithm.
  if ( ! _alg_id || ! _alg )
    return "Modeling algorithm not specified.";

  return 0;
}


/**********************/
/*** set Output Map ***/
int
OpenModeller::setOutputMap( Scalar mult, char *output_file,
                            char *mask, char *file_with_format )
{
  _output_mult = mult;
  stringCopy( &_output_file, output_file );
  stringCopy( &_output_mask, mask );

  // Output file header.
  RasterFile map( file_with_format );
  _output_header = new Header( map.header() );

  return 1;
}


/**********************/
/*** set Output Map ***/
int
OpenModeller::setOutputMap( Scalar mult, char *output_file,
                            char *mask, MapFormat *format )
{
  _output_mult = mult;
  stringCopy( &_output_file, output_file );
  stringCopy( &_output_mask, mask );

  // Output file header.
  _output_header = new Header( format->getWidth(),
                               format->getHeight(),
                               format->getXMin(),
                               format->getYMin(),
                               format->getXMax(),
                               format->getYMax(),
                               format->getNoDataValue() );

  _output_header->setProj( format->getProjection() );

  return 1;
}


/******************/
/*** create Map ***/
int
OpenModeller::createMap( Environment *env, char *output_file,
                         char *output_mask )
{
  if ( ! output_mask )
    output_mask = _output_mask;

  if ( ! output_file )
    output_file = _output_file;

  return createMap( env, output_file, _output_mult,
                    output_mask, _output_header );
}


/******************/
/*** create Map ***/
int
OpenModeller::createMap( Environment *env, char *file, Scalar mult, 
			 char * mask, Header *hdr )
{
  if ( !_alg )
    {
      sprintf( _error,
               "Algorithm object is empty!\n\n",
               file );
      return 0;
    }
  else if ( !_alg->done() )
    {
      sprintf( _error,
               "Algorithm model is not finished yet!\n\n",
               file );
      return 0;
    }

  if ( ! hdr->hasProj() )
    {
      sprintf( _error, 
	       "Output map (%s) without a coordinate system!\n\n",
	       file );
      return 0;
    }

  // check if env object is original one (used to create model) or
  // is a different one (caller wants to project model onto it)
  if ( ! env || env == _env )
    {
      env = _env;
      g_log("Native range projection (using original env object).\n");
    }

  // env objects are not the same, so copy normalization
  // parameters from original source and procced with
  // projection
  else
    {
      g_log("Preparing target environment object for projection.\n");

      Scalar min, max;
      if ( _alg->needNormalization( &min, &max ) )
	{
	  g_log( "Normalizing environment variables on projection Environment object.\n" );
	  env->copyNormalizationParams(_env);
	}
    }

  // Force noval = 0
  hdr->noval = 0.0;
 
  // Create map on disc.
  RasterFile rst( file, *hdr );
  Map map( &rst , env->getCoordinateSystem() );

  // Retrieve possible adjustments and/or additions made
  // on the the effective header.
  *hdr = rst.header();


  // Transformer used by the resulting map.
  GeoTransform *gt = map.getGT();

  // Dimension of environment space.
  int dim = env->numLayers();


  // Debug: minimum and maximum.
  Scalar min = 1e10;
  Scalar max = -1e10;


  // Fill the map with probabilities given by the model.
  Scalar *amb; 
  amb = new Scalar[dim];
  *amb = 0.0;

  Scalar val;
  Coord lg, lt;
  Coord y0 = hdr->ymin; // + 0.5 * hdr.ycel;
  Coord x0 = hdr->xmin; // + 0.5 * hdr.xcel;

  float progress = 0.0;
  float progress_step = hdr->ycel / (hdr->ymax - y0);

  int row = 0;
  for ( float y = y0; y < hdr->ymax; y += hdr->ycel )
    {
      int col = 0;
      Scalar sum = 0.0;
      for ( float x = x0; x < hdr->xmax; x += hdr->xcel )
	{
	  // Transform coordinates (x,y) that are in the resulting
	  // map system, in (lat, long) according to the system 
	  // accepted by the environment (env).
	  gt->transfOut( &lg, &lt, x, y );


	  // TODO: use mask to check if pixel should contain prediction
	  // Read environmental values and find the output value.
	  if ( ! env->get( lg, lt, amb ) )
	    val = hdr->noval;
	  else
	    {
	      val = _alg->getValue( amb );
	      if ( val < 0.0 ) val = 0.0;
	      if ( val > 1.0 ) val = 1.0;
	      val *= mult;
	    }

	  // Write value on map.
	  map.put( lg, lt, &val );

	  // Sum of line values.
	  sum += val;
	}

      // Call the callback function if it is set.
      if ( _map_callback )
        {
          if ( (progress += progress_step) > 1.0 )
            progress = 1.0;
          (*_map_callback)( progress, _map_callback_param );
        }

    }
  g_log( "\n" );

  delete amb;
  return 1;
}


/**********************************/
/******* serialize ****************/
int 
OpenModeller::serialize(Serializer * ser)
{
  int i;
  Map * layer;
  char * sectionName, * layerFilename;

  ser->writeStartSection("OpenModeller");

  // environment object and related information
  ser->writeStartSection("Environment");
  ser->writeString("CoordinateSystem", _env->getCoordinateSystem());
  int nlayers = _env->numLayers();
  ser->writeStartSection("Layers", nlayers);
  for (i = 0; i <= nlayers; i++)
    {
      if (i != nlayers)
	{
	  // deal with regular layers
	  layer = _env->getLayer(i);
	  layerFilename = _env->getLayerFilename(i);
	  sectionName = "Map";
	}
      else
	{
	  layer = _env->getMask();
	  layerFilename = _env->getMaskFilename();
	  sectionName = "Mask";
	}

      ser->writeStartSection(sectionName);
      ser->writeString("Filename", layerFilename);
      ser->writeString("CoordinateSystem", layer->getCoordSystem());
      ser->writeInt("Categorical", layer->isCategorical());
      ser->writeInt("Normalized", layer->isNormalized());
      ser->writeDouble("Offset", layer->offset());
      ser->writeDouble("Scale", layer->scale());
      ser->writeEndSection(sectionName);
    }
  ser->writeEndSection("Layers");
  ser->writeEndSection("Environment");

  // species occurrences and related info
  ser->writeStartSection("SpeciesOccurrences");
  ser->writeString("SpeciesName", _presence->name());
  ser->writeString("CoordinateSystem", _presence->coordSystem());

  int numOccur = _presence->numOccurrences();
  ser->writeStartSection("Presences", numOccur);
  Occurrences * occurrences = _presence;
  Occurrence * oc;
  for (oc = NULL; oc = occurrences->get(); occurrences->next())
    {
      ser->writeStartSection("Point");
      ser->writeDouble("X", oc->x());
      ser->writeDouble("Y", oc->y());
      ser->writeEndSection("Point");
    }
  ser->writeEndSection("Presences");
  
  numOccur = (_absence)? _absence->numOccurrences() : 0;
  ser->writeStartSection("Absences", numOccur);
  if (_absence)
    {
      Occurrences * occurrences = _absence;
      Occurrence * oc;
      for (oc = NULL; oc = occurrences->get(); occurrences->next())
	{
	  ser->writeStartSection("Point");
	  ser->writeDouble("X", oc->x());
	  ser->writeDouble("Y", oc->y());
	  ser->writeEndSection("Point");
	}
    }
  ser->writeEndSection("Absences");

  ser->writeEndSection("SpeciesOccurrences");

  // algorithm metadata, parameters and model
  ser->writeStartSection("Algorithm");

  // algorithm metadata
  AlgMetadata * meta = _alg->getMetadata();
  ser->writeStartSection("AlgorithmMetadata");
  ser->writeString("Id", meta->id);
  ser->writeString("Name", meta->name);
  ser->writeString("Version", meta->version);
  ser->writeString("Overview", meta->overview);
  ser->writeString("Author", meta->author);
  ser->writeString("CodeAuthor", meta->code_author);
  ser->writeString("Contact", meta->contact);
  ser->writeEndSection("AlgorithmMetadata");

  // algorithm parameters
  ser->writeStartSection("AlgorithmParameters", _alg_nparam);
  for (i = 0; i < _alg_nparam; i++)
    {
      ser->writeStartSection("AlgorithmParameter");
      ser->writeString("Id", _alg_param[i].id());
      ser->writeString("Value", _alg_param[i].value());
      ser->writeEndSection("AlgorithmParameter");
    }
  ser->writeEndSection("AlgorithmParameters");

  // model
  ser->writeObject(_alg);

  ser->writeEndSection("Algorithm");

  ser->writeEndSection("OpenModeller");

  return 1;
}

/**********************************/
/******* deserialize **************/
int 
OpenModeller::deserialize(Deserializer * des)
{
  int i, k;

  des->readStartSection("OpenModeller");

  // environment object and related information
  des->readStartSection("Environment");
  des->readString("CoordinateSystem");

  int nlayers = des->readStartSection("Layers");
  char * mask, ** layers;
  int ncont = 0;
  int ncateg = 0;
  layers = new char*[nlayers];
  for (i = 0; i <= nlayers; i++)
    {
      des->readStartSection("Map");
      char * filename = des->readString("Filename");
      des->readString("CoordinateSystem");
      int categorical = des->readInt("Categorical");
      des->readInt("Normalized");
      des->readDouble("Offset");
      des->readDouble("Scale");
      des->readEndSection("Map");

      char * copy = new char[strlen(filename) + 1];
      strcpy(copy, filename);
      if (i < nlayers)
	{
	  ncateg += categorical;
	  ncont += 1 - categorical;
	  layers[i] = copy; 
	}
      else
	{ mask = copy; }
    }
  des->readEndSection("Layers");
  des->readEndSection("Environment");

  // serialization assumes first layers are categorical 
  // and last one is the mask
  setEnvironment(ncateg, layers, ncont, layers + ncateg, mask); 

  for (i = 0; i < nlayers; i++)
    delete[] layers[i];
  delete[] layers;
  delete[] mask;

  // species occurrences and related info
  des->readStartSection("SpeciesOccurrences");
  char * sp_name = des->readString("SpeciesName");
  char * cs = des->readString("CoordinateSystem");

  for (i = 0; i < 2; i++)
    {
      int numOccur;
      char * sectionName;
      Occurrences * occs;
      if (i == 0)
	{
	  // presences are mandatory
	  occs = _presence = new Occurrences(sp_name, cs);
	  sectionName = "Presences";
	  numOccur = des->readStartSection(sectionName);
	}
      else
	{
	  // absences are optional
	  sectionName = "Absences";
	  numOccur = des->readStartSection(sectionName);
	  if (numOccur)
	    { occs = _absence = new Occurrences(sp_name, cs); }
	  else
	    { occs = _absence = NULL; }
	}


      for (k = 0; k < numOccur; k++)
	{
	  des->readStartSection("Point");
	  double x = des->readDouble("X");
	  double y = des->readDouble("Y");
	  des->readEndSection("Point");

	  occs->insert(x, y, 0.0, 1.0);
	}

      des->readEndSection(sectionName);
    }

  des->readEndSection("SpeciesOccurrences");

  // algorithm metadata, parameters and model
  des->readStartSection("Algorithm");

  // algorithm metadata
  des->readStartSection("AlgorithmMetadata");
  char * alg_id = des->readString("Id");
  char * alg_name = des->readString("Name");
  char * alg_ver = des->readString("Version");
  des->readString("Overview");
  des->readString("Author");
  des->readString("CodeAuthor");
  des->readString("Contact");
  des->readEndSection("AlgorithmMetadata");

  // algorithm parameters
  int nparam = des->readStartSection("AlgorithmParameters");
  AlgParameter * param = new AlgParameter[ nparam ];
  for (i = 0; i < nparam; i++)
    {
      des->readStartSection("AlgorithmParameter");
      char * param_id = des->readString("Id");
      char * param_value = des->readString("Value");
      des->readEndSection("AlgorithmParameter");

      param[i].setId(param_id);
      param[i].setValue(param_value);
    }
  des->readEndSection("AlgorithmParameters");

  // model
  setAlgorithm(alg_id, nparam, param);
  des->readObject(_alg);

  des->readEndSection("Algorithm");

  des->readEndSection("OpenModeller");

  // Check if the algorithm needs normalized variables.
  Scalar min, max;
  if ( _alg->needNormalization( &min, &max ) )
    {
      g_log( "Normalizing environment variables.\n" );
      _env->normalize( min, max );
    }

  delete[] param;

  return 1;
}
