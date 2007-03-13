  /***************************************************************************
   *   Copyright (C) 2005 by Tim Sutton   *
   *   tim@linfiniti.com   *
   *                                                                         *
   *   This program is free software; you can redistribute it and/or modify  *
   *   it under the terms of the GNU General Public License as published by  *
   *   the Free Software Foundation; either version 2 of the License, or     *
   *   (at your option) any later version.                                   *
   *                                                                         *
   *   This program is distributed in the hope that it will be useful,       *
   *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
   *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
   *   GNU General Public License for more details.                          *
   *                                                                         *
   *   You should have received a copy of the GNU General Public License     *
   *   along with this program; if not, write to the                         *
   *   Free Software Foundation, Inc.,                                       *
   *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
   ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <openmodeller/om.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/AlgorithmFactory.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/MapIterator.hh>
#include "request_file.hh"

#include <string.h>
#include <utility>
#include <iostream>
#include <stdexcept>
  using namespace std;

  int main( int argc, char **argv )
  {
    try {

      // I like a noisy log file.
      Log::instance()->setLevel( Log::Debug );
    
    // Load the algorithm Dlls.  This test program really really
    // does not need to do this.
    AlgorithmFactory::searchDefaultDirs();
    
    // Here's my OpenModeller Object.
    OpenModeller om;


  RequestFile myRequestFile;
  cout << "Reading from request file " << argv[1] << std::endl;
  int myResult = myRequestFile.configure( &om, strdup(argv[0]));

  if ( myResult < 0 )
  {
    //notify user of eror here!
    printf ("om_dump : Error reading request file!");
    return 1;
   }

    /** @note this is the way to set up om using the xml format:
    cout << "Loading Test file " << argv[1] << endl;
    ConfigurationPtr c = Configuration::readXml( argv[1] );      
    // Configuring it.
    om.setConfiguration(c);
  */
      
    

    // This is the environment I want to dump.
    EnvironmentPtr e = om.getEnvironment();

    // Initialize the iterator.
    MapIterator it = e->getMask()->begin();

    // Initialize the terminal.
    MapIterator fin;


    // I'm not going to dump the whole stinking thing,
    // so I'm only going to dump 1000 locations.
    int i=0;
    while( it != fin ) {

      // Get the lon/lat coordinates from the iterator.
      pair<Coord,Coord> lonlat = *it;

      // Extract the environment sample at that point.
      Sample s = e->get( lonlat.first, lonlat.second );

      // Note:  s will have size() == 0 if the environment
      // does not have data for that location.  This can occurr
      // for a coupld of reasons.
      //   1.  The location is not in the mask.
      //   2.  One of the layers has value == NoVal at that location.
      if ( s.size() > 0 ) {
        cout << lonlat.first
             << " " << lonlat.second
             << " " << s
             << endl;
        ++i;
      }

      ++it;
      if ( i == 1000 ) 
        break;
    }


  }
  catch( exception& e ) {
    cerr << "Exception Caught" << endl;
    cerr << e.what() << endl;
  }

  return 0;

}
