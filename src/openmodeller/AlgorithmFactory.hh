/**
 * Declaration of AlgorithmFactory class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2004-03-19
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

#include <list.hh>
#include <os_specific.hh>
#include <om_algorithm_metadata.hh>

class Algorithm;
class AlgParameter;
class Sampler;


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
class AlgorithmFactory
{
  /** Manages an algorithm with its DLL file. */
  class DLL
  {
  public:
    DLL( char *file );
    ~DLL();

    Algorithm *load();

    /**
     * Returns an algorithm that cannot be deleted.
     * It is a reference object.
     */
    Algorithm *getAlgorithm()  { return _alg; }

    /** Returns a new instantiated algorithm object. **/
    Algorithm *newAlgorithm();


  private:
    DLLHandle _handle;
    Algorithm *_alg;
    char      *_file;
  };


public:

  /** @search_dirs Null terminated list of directory paths to
   * search for algorithms.
   */
  AlgorithmFactory( char **search_dirs );
  ~AlgorithmFactory();

  /** Set directories that will be searched when trying to load algorithms
   */
  void setDirs(char ** dirs);

  /** Load the system available algorithms. If there are
   * algorithm already loaded they are unloaded.
   * 
   * Warning:
   * 
   * The pointers to old algorithms are invalid after a call to
   * this method.
   * 
   * @return Number of loaded algorithms.
   */
  int loadAlgorithms();

  /** Finds the system available algorithms.
   * 
   * Note that the returned algorithms can not run because they
   * are not initialized with "Sampler" and parameters!
   * To do so, use the method newAlgorithm().
   * 
   * The pointer returned are copied from an internal storage of
   * algorithms. So they can not be deallocated.
   * Another point is that the Algorithms will be reallocated
   * the next time this method is called.
   *
   * @return a null terminated list of available algorithms.
   */
  AlgMetadata **availableAlgorithms();

  /** Number of available algorithms.
   * If the algorithms are not already searched in the system,
   * searchs them first.
   *
   * @return Number of available algorithms.
   */
  int numAvailableAlgorithms();

  /** Returns an specific algorithm metadata
   * @param algorithm_id Identifier of the algorithm.
   * @return Algorithm's metadata or zero if there algorithm
   *  was not found.
   */
  AlgMetadata *algorithmMetadata( char *algorithm_id );

  /** Instantiate a new algorithm object.
   *
   * @param samp Sampler object.
   * @param id Identifier of the algorithm to be instantiated.
   * @param nparam Number of parameters.
   * @param param Vector with all parameters. The address 'param'
   *  points to must exists while the returned algorithm is used.
   * 
   * @return A pointer to the new instantiated algorithm or
   *  null if an algorithm with id was not found.
   */
  Algorithm *newAlgorithm( Sampler *samp, char *id, int nparam,
                           AlgParameter *param );


private:


  typedef List<DLL *> ListDLL;


  /** Search for all DLLs in some directories.
   * @param lst Filled with the DLLs found.
   * @param dirs Null terminated array of directories to search.
   * @return Number of DLLs found.
   */
  int loadDLLs( char **dirs );

  void cleanDLLs();


  /** Scans a directory for dynamic link libraries.
   * @param dir Directory to scan.
   * @param lst Stores the dynamic link libraries found.
   * @return Number of libraries found.
   */
  int scanDir( char *dir, ListDLL * ldll );

  static ListDLL * _lstDLL; ///< Algorithms' DLLs paths list.

  char    **_dirs;
};


#endif
