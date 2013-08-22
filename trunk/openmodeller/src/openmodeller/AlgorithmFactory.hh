/**
 * Declaration of AlgorithmFactory class.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
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


#ifndef _ALGORITHM_FACTORYHH_
#define _ALGORITHM_FACTORYHH_

#include <openmodeller/os_specific.hh>
#include <openmodeller/AlgMetadata.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Algorithm.hh>

#include <openmodeller/Configuration.hh>

#include <string>
#include <list>

class AlgParameter;

/****************************************************************/
/*********************** Algorithm Factory **********************/

/** 
 * Responsable for instantiate algorithms.
 *
 * Obs: Besides instantiate the algorithms it does not sets the
 *  Sampler object of the algorithms. So, before use an algorithm
 *  returned by this classe one needs to calls the method
 *  Algorithm::setSample().
 * 
 */
class dllexp AlgorithmFactory
{

public:

  ~AlgorithmFactory();

  /** Set a default directory to search for algorithms.
   * @param dir Default directory
   */
  static void setDefaultAlgDir( std::string const dir );

  /** Return the default directory to search for algorithms.
   * @return Default directory
   */
  static std::string getDefaultAlgDir();

  /** Search for all DLLs in default directories.
   *
   * The default search algorithm is platform specific.
   *
   * @return Number of DLLs loaded.
   */
  static int searchDefaultDirs();

  /** Search for all DLLs in some directories.
   * @param dirs null terminated string representing a directory to scan
   * @return Number of DLLs found.
   */
  static int addDir( const std::string& dirs );

  /** Add a single file to the DLL search list.
   * @param file name of dll file to attempt to load
   * @return true for success.
   */
  static bool addDll( const std::string& file );

  /** Finds the system available algorithms.
   * 
   * Note that the returned algorithms can not run because they
   * are not initialized with "Sampler" and parameters!
   * To do so, use the method newAlgorithm().
   * 
   * The pointer to the array must be deallocated by the caller
   * using delete []. However, the individual AlgMetadata*'s
   * in the array must not be deallocated. For instance:
   *
   * AlgMetadata const **algorithms = om->availableAlgorithms();
   * // do something
   * delete [] algorithms;
   *
   * @return a null terminated list of available algorithms.
   */
  static AlgMetadata const**availableAlgorithms();

  /** Number of available algorithms.
   * If the algorithms are not already searched in the system,
   * searchs them first.
   *
   * @return Number of available algorithms.
   */
  static int numAvailableAlgorithms();

  /** Returns an specific algorithm metadata
   * @param algorithm_id Identifier of the algorithm.
   * @return Algorithm's metadata or zero if there algorithm
   *  was not found.
   */
  static AlgMetadata const *algorithmMetadata( std::string const algorithm_id );

  /** Instantiate a new algorithm object.
   *
   * @param id Identifier of the algorithm to be instantiated.
   * 
   * @return A pointer to the new instantiated algorithm or
   *  null if an algorithm with id was not found.
   */
  static AlgorithmPtr newAlgorithm( std::string const id );

  static AlgorithmPtr newAlgorithm( const ConstConfigurationPtr& );

  /*
   * Method to serialize the available algorithms
   *
   * Note that this class does not implement "Configurable" 
   * because it cannot be instantiated from outside (singleton 
   * pattern). Also the counterpart setConfiguration method is not
   * implemented in this case because it makes no sense to load
   * algorithms from XML - they can only be dynamically loaded
   * from the respective libraries.
   */
  static ConfigurationPtr getConfiguration();

  /** Returns the current algorithm id in case it matches one of the previous
   * ids of an algorithm. This method was created to allow changes in algorithm
   * ids without breaking compatibility with serialized data.
   * @param algorithm_id Identifier of an algorithm.
   * @return Current algorithm identifier when the parameter matches a known 
   * previous identifier, otherwise it returns the parameter.
   */
  static std::string const getCurrentId( std::string const algorithm_id );

private:

  AlgorithmFactory();

  static AlgorithmFactory& getInstance();

  // Default directory to search for algorithms
  std::string _default_alg_dir;

  friend class testDLLId;
  class DLL;
  typedef ReferenceCountedPointer<DLL> DLLPtr;
  typedef std::list<DLLPtr> ListDLL;
  typedef std::list<std::string> PluginPath;

  ListDLL _dlls;
  PluginPath _pluginpath;

  /** Implementation of directory scan.
   * @param dir Directory to scan.
   * @return Number of libraries found.
   */
  int p_addDir( const std::string& dir );

  /** Implementation of Dll load.
   * @param file Name of file to laod.
   * @return True if successfully loaded.
   */
  bool p_addDll( const std::string& file );

  /** Manages an algorithm with its DLL file. */
  class DLL : private ReferenceCountedObject
  {
    friend class ReferenceCountedPointer<DLL>;
  public:
    DLL();
    DLL( const std::string& file );
    ~DLL();

    operator bool() const;

    /** Returns a new instantiated algorithm object. **/
    AlgorithmPtr newAlgorithm();

    AlgMetadata const *getMetadata();

  private:
    DLLHandle _handle;
    TAlgFactory _factory;
    TAlgMetadata _metadata;
    std::string _file;
  };


};


#endif
