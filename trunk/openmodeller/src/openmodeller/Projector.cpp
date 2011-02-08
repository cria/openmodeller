/**
 * Definition of Projector class
 * 
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

#include <openmodeller/CallbackWrapper.hh>
#include <openmodeller/Projector.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Model.hh>

#include <openmodeller/env_io/Header.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/Raster.hh>
#include <openmodeller/env_io/GeoTransform.hh>

#include <openmodeller/AreaStats.hh>

#include <openmodeller/Exceptions.hh>

#ifdef MPI_FOUND
#include "mpi.h"
#endif

#include <utility>
using std::pair;

#ifdef MPI_FOUND
#define N_X 10
#define size_block   30000
#endif

#ifndef MPI_FOUND
/******************/
/*** create Map ***/
bool
Projector::createMap( const Model& model,
		      const EnvironmentPtr& env,
		      Map *map,
		      AreaStats *areaStats,
		      CallbackWrapper *callbackWrapper )
{
  // Retrieve possible adjustments and/or additions made
  // on the effective header.
  Header hdr = map->getHeader();

#ifndef GEO_TRANSFORMATIONS_OFF
  if ( ! hdr.hasProj() ) {

    throw;
  }
#endif

  // Normalize the environment.
  model->setNormalization( env );
 
  if ( areaStats ) {

    areaStats->reset( areaStats->getPredictionThreshold() );
  }

  MapIterator fin;
  MapIterator it = map->begin();

  int pixels = 0;
  int pixelcount = hdr.ydim * hdr.xdim;
  int pixelstep = pixelcount/20;
  bool abort = false;

  Coord lg;
  Coord lt;
  Scalar val;
  int ret_put;

  while ( it != fin ) {

    // Call the abort callback function if it is set.
    if ( callbackWrapper && pixels%pixelstep == 0 ) {

      try {

        abort = callbackWrapper->abortionRequested();

        if ( abort ) {

          Log::instance()->info( "Projection aborted." );

          if ( ! map->deleteRaster() ) {

            Log::instance()->warn( "Could not delete map file." );
          }

          return false;
        }
      }
      catch( ... ) {}
    }
    
    pair<Coord,Coord> lonlat = *it;

    pixels++;
    ++it;

    lg = lonlat.first;
    lt = lonlat.second;

    Sample const &amb = env->get( lg, lt );

    // Read environmental values and find the output value.
    if ( amb.size() == 0 ) {

      // Write noval on the map.
      ret_put = map->put( lg, lt );

      val = -1; // could be used in a log
    }
    else {

      val = model->getValue( amb );

      if ( val < 0.0 || val > 1.0 ) {

        std::string msg = Log::format( "Probability for point (%f, %f) is outside the valid range: %f", lg, lt, val );
        throw AlgorithmException( msg.c_str() );
      }

      if ( areaStats ) {

	areaStats->addPrediction( val ); 
      }

      // Write value on map.
      ret_put = map->put( lg, lt, val );
    }

    // Call the callback function if it is set.
    if ( callbackWrapper && pixels%pixelstep == 0 ) {

      float progress = pixels/(float)pixelcount;

      if ( progress > 1.0 ) {

	progress = 1.0;
      }

      try {

        callbackWrapper->notifyModelProjectionProgress( progress );
      }
      catch( ... ) {}
    }
  }
  
  // Call the callback function if it is set.
  if ( callbackWrapper ) {

    try  {

      callbackWrapper->notifyModelProjectionProgress( 1.0 );
    }
    catch ( ... ) {}
  }

  map->finish();

  return true;
}

#else
/***************************/
/*** create Map Parallel ***/
bool
Projector::createMap( const Model& model,
                      const EnvironmentPtr& env,
                      Map *map,
                      AreaStats *areaStats,
                      CallbackWrapper *callbackWrapper )
{

  /*********************struct buff *************************************/
  typedef struct
  {
    double lt;
    double lg;
    Scalar val;
  } buff;
  /**********************************************************************/

  buff X[N_X][size_block];
  MPI_Status status;
  MPI_Request req_dado[N_X];
  MPI_Request inf_N[N_X];
  int ix;
  int n_elem[N_X];
  int env_dado[N_X],total_pixels;
  int pixel_next,interval_pixels[3];
  int count_tag,max_count_tag;
  int myrank,mysize;
  int j, N;

  // Retrieve possible adjustments and/or additions made
  // on the effective header.
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

  Header hdr = map->getHeader();

#ifndef GEO_TRANSFORMATIONS_OFF
  if ( ! hdr.hasProj() )
  {
    throw;
  }
#endif
  Log::instance()->debug( "Parallel version of createMap\n" );
  // Normalize the environment
  model->setNormalization( env );

  if ( areaStats ) {
    areaStats->reset();
  }

  MapIterator fin;
  MapIterator my_it = map->begin();

  j = 0;  //count the lines on struct array

  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&mysize);

  int pixels, init_pixel, lim_pixel;
  count_tag=10;
  total_pixels=hdr.ydim*hdr.xdim;

  MPI_Barrier(MPI_COMM_WORLD);

  if (myrank==0) { //sif rank 0

    max_count_tag=10+total_pixels/size_block;
    if ((total_pixels%size_block)!=0)
      max_count_tag++;

        while (count_tag<max_count_tag) {
      MPI_Recv(&N,1,MPI_INT,MPI_ANY_SOURCE,count_tag,MPI_COMM_WORLD,&status);
      MPI_Recv(X[0],N*sizeof(buff),MPI_UNSIGNED_CHAR,status.MPI_SOURCE,count_tag,MPI_COMM_WORLD,&status);

      for (j=0; j< N; j++) {
        if (X[0][j].val==100000)
          map->put( X[0][j].lg, X[0][j].lt);
        else {
          map->put(X[0][j].lg,X[0][j].lt,X[0][j].val );
        }
      }
count_tag++;
    }
  } //end if rank 0
  else
  if (myrank==mysize-1) {
    int k,req_pixels;
    pixel_next=0;
    interval_pixels[2]=10;  //count_tag

    while (pixel_next<(total_pixels)) {
      lim_pixel=pixel_next+size_block;

      if (lim_pixel>total_pixels)
        lim_pixel=total_pixels;

      interval_pixels[0]=pixel_next;
      interval_pixels[1]=lim_pixel;
      pixel_next=lim_pixel;
      MPI_Recv(&req_pixels,1,MPI_INT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,&status);
      MPI_Send(interval_pixels,3,MPI_INT,status.MPI_SOURCE,2,MPI_COMM_WORLD);
      interval_pixels[2]++;
    }

    for (k=1;k<(mysize-1);k++) {
      interval_pixels[0]=total_pixels;
      MPI_Recv(&req_pixels,1,MPI_INT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,&status);
      MPI_Send(interval_pixels,2,MPI_INT,status.MPI_SOURCE,2,MPI_COMM_WORLD);
    }
  }
  else {
        ix=0;
    for (int k=0;k<N_X;k++)
      env_dado[k]=0;
    MPI_Send(&ix,1,MPI_INT,(mysize-1),1,MPI_COMM_WORLD);
    MPI_Recv(interval_pixels,3,MPI_INT,(mysize-1),2,MPI_COMM_WORLD,&status);;
    init_pixel=interval_pixels[0];
    lim_pixel=interval_pixels[1];

while (init_pixel<(total_pixels)) {
      my_it.nextblock(init_pixel);

      j=0;

      for (pixels=init_pixel; pixels<lim_pixel; pixels++) {

        pair<Coord,Coord> lonlat = *my_it;
        ++my_it;
        Coord lg = lonlat.first;
        Coord lt = lonlat.second;

        { //begin block
          Sample const &amb = env->get( lg, lt );
          // TODO: use mask to check if pixel should contain prediction
          // Read environmental values and find the output value.
          if ( amb.size() == 0 ) {
            // Write noval on the map.

 /* map->put( lg, lt );********************************************************/
 /*substituir pela função, coloco lat, long, 255*******************************/
 /*****escrevo nada na struct, 255*********************************************/

            X[ix][j].lt=lt;
            X[ix][j].lg=lg;
            X[ix][j].val=100000;

          }
          else {
            Scalar val = model->getValue( amb );

            if ( val < 0.0 )
                      val = 0.0;
            else
                          if ( val > 1.0 )
                            val = 1.0;

                        if ( areaStats ) {
                  areaStats->addPrediction( val );
            }
            // Write value on map.
    /**********  map->put( lg, lt, val );substituir pela função***************/
            X[ix][j].lt=lt;
            X[ix][j].lg=lg;
            X[ix][j].val=val;
          }
        }  //end block
        j++;
      } //end do for

	  n_elem[ix]=j;

	  MPI_Isend(&n_elem[ix],1,MPI_INT,0,interval_pixels[2],MPI_COMM_WORLD,&inf_N[ix]);
      //MPI_Isend(&j,1,MPI_INT,0,interval_pixels[2],MPI_COMM_WORLD,&inf_N[ix]);
      MPI_Isend(X[ix],j*sizeof(buff),MPI_UNSIGNED_CHAR,0,interval_pixels[2],MPI_COMM_WORLD,&req_dado[ix]);
      env_dado[ix]=1;
      ix=(ix+1)%N_X;

      if (env_dado[ix]==1) {
        MPI_Wait(&inf_N[ix],&status);
        MPI_Wait(&req_dado[ix],&status);
        env_dado[ix]=0;
      }

      MPI_Send(&ix,1,MPI_INT,(mysize-1),1,MPI_COMM_WORLD);
      MPI_Recv(interval_pixels,3,MPI_INT,(mysize-1),2,MPI_COMM_WORLD,&status);;
      init_pixel=interval_pixels[0];
      lim_pixel=interval_pixels[1];
    } // end while
  } // end else myrank == 0

  MPI_Barrier(MPI_COMM_WORLD);

// Call the callback function if it is set.

    if ( callbackWrapper ) {

      try  {

        callbackWrapper->notifyModelProjectionProgress( 1.0 );
      }
      catch ( ... ) {}
    }


  return true;
}
#endif

