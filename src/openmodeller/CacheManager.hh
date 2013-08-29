/**
 * Declaration of CacheManager class.
 * 
 * @author Renato De Giovanni (renato (at) cria . org . br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2012 by CRIA -
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

#ifndef _CACHE_MANAGER_HH_
#define _CACHE_MANAGER_HH_

#include <string>
#include <sstream>

#include <openmodeller/os_specific.hh>

/************************************************************/
/*********************** Cache Manager **********************/

/**
 * This class was initially created to help handling openModeller WCS rasters.
 * It is used by env_io/WcsProxyRaster, which receives an openModeller WCS raster
 * identifier in the form wcs>wcs_service_url>coverage_id and uses the cache manager
 * to convert the identifier into a safe MD5 hash string that can be used as a 
 * file name to store the WCS XML content expected by GDAL. In the future it may
 * be used for other things, such as caching background points or caching entire 
 * rasters. TODO: write flush method (so far the cache directories tend to keep
 * growing).
 */
class dllexp CacheManager {

public:

  ~CacheManager();

  /** Set the cache directory. When not specified, try the OM_CACHE_PATH
   *  environment variable. If undefined, use current_dir/cache.
   * @param dir Cache directory
   */
  static void initialize( const std::string dir="" );

  /** Indicates if the specified content identified by id is cached.
   * @param id Content identifier.
   * @param subdir Subdirectory inside cache dir. Optional.
   * @return Yes or no
   */
  static bool isCached( const std::string id, const std::string subdir="" );

  /** Indicates if the specified content identified by id is cached. Id is converted
   *  to an MD5 hash.
   * @param id Content identifier.
   * @param subdir Subdirectory inside cache dir. Optional.
   * @return Yes or no
   */
  static bool isCachedMd5( const std::string id, const std::string subdir="" );

  /** Cache the specified content identified by id.
   * @param id Content identifier.
   * @param content Content.
   * @param subdir Subdirectory inside cache dir. Optional.
   */
  static void cache( const std::string id, const std::ostringstream& content, const std::string subdir="" );

  /** Cache the specified content identified by id. Id is converted
   *  to an MD5 hash.
   * @param id Content identifier.
   * @param content Content.
   * @param subdir Subdirectory inside cache dir. Optional.
   */
  static void cacheMd5( const std::string id, const std::ostringstream& content, const std::string subdir="" );

  /** Return the location of cached content.
   * @param id Content identifier.
   * @param subdir Subdirectory inside cache dir. Optional.
   * @return Location of cached content (file path).
   */
  static std::string getContentLocation( const std::string id, const std::string subdir="" );

  /** Return the location of cached content. Id is converted
   *  to an MD5 hash.
   * @param id Content identifier.
   * @param subdir Subdirectory inside cache dir. Optional.
   * @return Location of cached content (file path).
   */
  static std::string getContentLocationMd5( const std::string id, const std::string subdir="" );

  /** Return the local id of cached content. Id is converted to an MD5 hash.
   * @param id Content identifier.
   * @return Local id of cached content.
   */
  static std::string getContentIdMd5( const std::string id ); 

private:

  CacheManager();

  static CacheManager& _getInstance();

  static void _ensureInitialized();

  // Cache directory
  std::string _cacheDir;
};

#endif
