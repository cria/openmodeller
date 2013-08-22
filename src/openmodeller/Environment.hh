/**
 * Declaration of Environment class.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
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


#ifndef _ENVIRONMENTHH_
#define _ENVIRONMENTHH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/refcount.hh>
#include <openmodeller/Configurable.hh>
#include <openmodeller/Normalizable.hh>
#include <openmodeller/Normalizer.hh>
#include <openmodeller/Sample.hh>

#include <vector>
#include <string>
#include <utility>

class Map;
class SampledData;

class EnvironmentImpl;
typedef ReferenceCountedPointer<EnvironmentImpl> EnvironmentPtr;
typedef ReferenceCountedPointer<const EnvironmentImpl> ConstEnvironmentPtr;

/****************************************************************/
/************************* Environment **************************/

EnvironmentPtr dllexp createEnvironment( const std::vector<std::string>& categs,
				  const std::vector<std::string>& maps,
				  const std::string& mask_file );

EnvironmentPtr dllexp createEnvironment( const std::vector<std::string>& categs,
				  const std::vector<std::string>& maps );

EnvironmentPtr dllexp createEnvironment( const ConstConfigurationPtr& config );

EnvironmentPtr dllexp createEnvironment( );


/** 
 * Allow access to environmental variables by means of vectors
 * gathering data from all layers on the sampled points.
 */
class dllexp EnvironmentImpl : public Configurable, public Normalizable, private ReferenceCountedObject
{
  friend class ReferenceCountedPointer<EnvironmentImpl>;
  friend class ReferenceCountedPointer<const EnvironmentImpl>;

  friend EnvironmentPtr dllexp createEnvironment( const std::vector<std::string>& categs,
					   const std::vector<std::string>& maps,
					   const std::string& mask_file );
  
  friend EnvironmentPtr dllexp createEnvironment( const std::vector<std::string>& categs,
					   const std::vector<std::string>& maps );

  friend EnvironmentPtr dllexp createEnvironment( const ConstConfigurationPtr& config );

  friend EnvironmentPtr dllexp createEnvironment( );

  typedef std::pair<std::string, Map*> layer;
  typedef std::vector<layer> layers;

  EnvironmentImpl();

  /** Vector with file names that contain the variables to
   *  be used and also the mask layer.
   *
   * @param categs    Categorical layers (e.g. soil).
   * @param maps      Continuous layers (e.g. temperature).
   * @param mask_file Mask file.
   */
  EnvironmentImpl( const std::vector<std::string>& categs,
		   const std::vector<std::string>& maps, 
		   const std::string& mask_file = "" );

public:

  ~EnvironmentImpl();

  /** Deep copy of this. */
  EnvironmentImpl* clone() const;

  unsigned int numLayers() const { return _layers.size(); }

  size_t numCategoricalLayers() const;

  /** Returns 1 if the i-th variable is categorical.
   * else returns 0.
   */
  int isCategorical( int i );

  /** Indicate that all non categorical variable layers must
   *  be normalized according to the interval [min, max].
   */
  void getMinMax( Sample * min, Sample * max ) const;

  /** Set specific normalization parameters
   */
  void normalize( Normalizer * normalizerPtr );

  /** Reset normalization parameters
   */
  void resetNormalization();

  /** Read for vector 'sample' all values of environmental variables
   *  of coordinate (x,y).
   *  Returns a Sample of dim 0 if environment has a mask and point is
   *  outside its borders.
   *  If the environment has been normalized, the Sample returned will be
   *  normalized.
   */
  Sample get( Coord x, Coord y ) const;

  Sample getNormalized( Coord x, Coord y ) const;
  Sample getUnnormalized( Coord x, Coord y ) const;

  /** Read for 'sample' all values of environmental variables of a
   *  valid coordinate (inside the mask) randomly chosen
   *  returns coordinates (x,y) through pointer arguments.
   *  If the environment has been normalized, the Sample returned will be
   *  normalized.
   */
  Sample getRandom( Coord *x = 0, Coord *y = 0 ) const;

  /** Return 0 if (x,y) falls outside the mask. If there's no 
   *  mask, return != 0 always. */
  int check( Coord x, Coord y ) const;

  /** Get rectangle that contain all valid points.
   *  If there's a mask, use its rectangle. */
  int getRegion( Coord *xmin, Coord *ymin, Coord *xmax,
                 Coord *ymax ) const;

  /** Returns the minimum and maximum values of each layer.
   *  If the environment has been normalized, the values returned
   *  are normalized.
   */
  int getExtremes( Sample* min, Sample* max ) const;

  Map * getLayer(int index) const { return _layers[index].second; }

  /** Change the mask. */
  int changeMask( const std::string& mask_file );

  Map * getMask() const { return _mask.second; }

  const std::string& getLayerPath(int index) const { return _layers[index].first; }

  const std::string& getMaskPath() const { return _mask.first; }

  void removeLayer(unsigned int index);

  virtual ConfigurationPtr getConfiguration() const;

  virtual void setConfiguration( const ConstConfigurationPtr & );

private:

  void initialize( const std::vector<std::string>& categs,
		   const std::vector<std::string>& maps, 
		   const std::string& mask_file="" );

  /* utility function to extract unnormalized environment vector
   * Only needed to prevent unnecessary memory copy due to Sample
   * not having a reasonable copy constructor/temporary
   */
  void getUnnormalizedInternal( Sample *, Coord x, Coord y ) const;

  /* utility to clear the mask information.  Deallocates memory.  Does not computeRegion() */
  void clearMask();

  /* utility to clear the layer information, deallocates memory.  Does not computeRegion() */
  void clearLayers();

  /* utility to construct a ConfigurationPtr representation of a Layer.
     Does not properly set the Configuration name.
     Is a member function so it can access the private typedefs. */
  static ConfigurationPtr getLayerConfig( const layer& l, bool basicConfig=false );

  /* utility to construct a layer pair object.
     It is declared a member so it has access to the private typedef for layer */
  static layer makeLayer( const std::string& filename, int categ );
  static layer makeLayer( const ConstConfigurationPtr& config );

  /** Rebuild the layer representation. */
  int changeLayers( const std::vector<std::string>& categs, 
		    const std::vector<std::string>& maps );

  /** Calculate the widest region common to all layers. */
  void calcRegion();

  layers _layers; ///< Vector with all layers that describe the variables.
  layer _mask;   ///< Mask (can be 0).

  Coord _xmin; ///< Intersection of all layers.
  Coord _ymin; ///< Intersection of all layers.
  Coord _xmax; ///< Intersection of all layers.
  Coord _ymax; ///< Intersection of all layers.

  Normalizer * _normalizerPtr; ///< Normalize the environment
};


#endif
