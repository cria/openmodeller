/**
 * Se an OpenModeller object reading parameters from a request file.
 * 
 * @file
 * @author Mauro E S Mu�oz (mauro@cria.org.br)
 * @date   2004-06-22
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

#ifndef _REQUEST_FILEHH_
#define _REQUEST_FILEHH_

#include  <om_algorithm_metadata.hh>

#include <om_occurrences.hh>

class OpenModeller;
class FileParser;
class AlgParameter;

/**************************************************************/
/************************ Request File ************************/

class RequestFile
{
public:

  RequestFile();
  ~RequestFile();

  /** Loads parameters from the request file.
   * 
   * @param om OpenModeller object to be configured.
   * @param request_file File from which the parameters will be read.
   * 
   * @return Number of parameters missing in the request file.
   */
  int configure( OpenModeller *om, char *request_file );

  int occurrencesSet() { return _occurrences_set; }
  int environmentSet() { return _environment_set; }
  int projectionSet()  { return _projection_set; }
  int algorithmSet()   { return _algorithm_set; }

  OccurrencesPtr getOccurrences() { return _occurrences; }

  void makeProjection( OpenModeller *om );

private:

  int setOccurrences( OpenModeller *om, FileParser &fp );
  int setEnvironment( OpenModeller *om, FileParser &fp );
  int setProjection ( OpenModeller *om, FileParser &fp );
  int setAlgorithm  ( OpenModeller *om, FileParser &fp );

  OccurrencesPtr readOccurrences( char *file, char *name,
                                char *coord_system );

  int readParameters( AlgParameter *result, AlgMetadata const *metadata,
                      int str_nparam, char **str_param );

  char *extractParameter( char const *name, int nvet, char **vet );

  int _occurrences_set;
  int _environment_set;
  int _projection_set;
  int _algorithm_set;

  OccurrencesPtr _occurrences;

  bool _nonNativeProjection;
  double _multiplier;
  int _ncat;
  char **_cat;
  int _nmap;
  char **_map;
  char *_mask;
  char *_file;

};


#endif

