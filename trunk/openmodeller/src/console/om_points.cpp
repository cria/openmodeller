#include <openmodeller/om.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"

#include <iostream>  // I/O 

#include <string>
#include <vector>

#include <stdexcept>

using namespace std;

int main( int argc, char **argv ) {

  char *args;
  int option;

  struct options opts[] = 
  {
    { 1, "log-level",  "Set the log level (debug, warn, info, error)", NULL, 1 },
    { 2, "version",    "Display version info",                         "v" , 0 },
    { 3, "list",       "List available formats",                       "l" , 0 },
    { 4, "source",     "Source where points are located",              "s" , 1 },
    { 5, "name",       "Name (label) to filter points",                "n" , 1 },
    { 6, "wkt",        "Spatial reference in WKT",                     "w" , 1 },
    { 7, "type",       "Output type",                                  "o" , 1 },
    { 0, NULL,         NULL,                                           NULL, 0 }
  };

  bool passed_params = false;

  std::string log_level("info");
  bool        list_formats = false;
  std::string source("");
  std::string label("");
  std::string wkt("GEOGCS[\"WGS84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"degree\",0.017453292519943295],AXIS[\"Longitude\",EAST],AXIS[\"Latitude\",NORTH]]");
  std::string format("TXT");

  while ( ( option = getopts( argc, argv, opts, &args ) ) != 0 ) {

    passed_params = true;

    switch ( option ) {

      // Special Case: Recognize options that we didn't set above.
      case -2: 
        printf( "Unknown option: %s\n", args );
        break;
      // Special Case: getopts() can't allocate memory.
      case -1:
        printf( "Unable to allocate memory from getopts().\n" );
        exit(-1);
        break;
      case 1:
        log_level = args;
        break;
      case 2:
        printf("om_points 0.1.0\n");
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 3:
        list_formats = true;
        break;
      case 4:
        source = args;
        break;
      case 5:
        label = args;
        break;
      case 6:
        wkt = args;
        break;
      case 7:
        format = args;
        break;
      default:
        break;
    }

    // This free() is required since getopts() automagically allocates space 
    // for "args" everytime it's called. */
    free( args );
  }

  if ( ! passed_params ) {

    // Display usage
    getopts_usage( "om_points", opts );
    exit(0);
  }

  // Log stuff

  Log::Level level_code = getLogLevel( log_level );

  Log::instance()->setLevel( level_code );

  // Real work

  try {

    if ( list_formats ) {

      // List drivers

      std::vector<std::string> driver_ids = OccurrencesFactory::instance().getRegisteredDrivers();

      printf( "Available formats (id: description (input/output))\n" );

      for ( size_t i = 0; i < driver_ids.size(); ++i ) {

        std::string description("?");
        std::string io("I");

        if ( driver_ids[i] == "GBIF" ) {

          description = "GBIF Web Service for occurrence points";
        }
        else if ( driver_ids[i] == "TAPIR" ) {

          description = "TAPIR Web Service using DarwinCore 1.4 and the geospatial extension";
        }
        else if ( driver_ids[i] == "TerraLib" ) {

          description = "Points stored in a TerraLib database";
        }
        else if ( driver_ids[i] == "TXT" ) {

          description = "Delimited text file (id, label, long, lat, abundance)";

          io = "I/O";
        }
        else if ( driver_ids[i] == "XML" ) {

          description = "openModeller serialized XML for occurrences";

          io = "I/O";
        }

        printf( "  %s: %s (%s)\n", driver_ids[i].c_str(), description.c_str(), io.c_str() );
      }

      return 0;
    }
    else {

      // Check requirements
      if ( source.empty() ) {

        printf( "Please specify a source to load points from.\n");
        exit(-1);
      }
      if ( label.empty() ) {

        printf( "Please specify a label to filter points.\n");
        exit(-1);
      }

      // Read occurrences
      OccurrencesReader * occ = OccurrencesFactory::instance().create( source.c_str(), wkt.c_str() );

      OccurrencesPtr presences = occ->getPresences( label.c_str() );

      OccurrencesPtr absences = occ->getAbsences( label.c_str() );

      delete occ;

      // Write occurrences
      std::cerr << flush;

      if ( format == "XML" ) {

        if ( presences ) {

          ConfigurationPtr cfg = presences->getConfiguration();
          cfg->setName( "Presence" );

          Configuration::writeXml( cfg, cout );
        }

        if ( absences && absences->numOccurrences() ) {

          ConfigurationPtr cfg = absences->getConfiguration();
          cfg->setName( "Absence" );

          Configuration::writeXml( cfg, cout );
        }
      }
      else {

        // Header
        cout << "#id\t" << "label\t" << "long\t" << "lat\t" << "abundance" << endl << flush;

        if ( presences ) {

          OccurrencesImpl::iterator it   = presences->begin();
          OccurrencesImpl::iterator last = presences->end();

          while ( it != last ) {

            cout << (*it)->id() << "\t" << label.c_str() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << (*it)->abundance() << endl << flush;
            ++it;
          }
	}

        if ( absences ) {

          OccurrencesImpl::iterator it   = absences->begin();
          OccurrencesImpl::iterator last = absences->end();

          while ( it != last ) {

            cout << (*it)->id() << "\t" << label.c_str() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << (*it)->abundance() << endl << flush;
            ++it;
          }
	}
      }
    }
  }
  catch ( runtime_error e ) {

    printf( "om_points: %s\n", e.what() );
    exit(-1);
  }

  return 0;
}
