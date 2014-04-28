#include <openmodeller/om.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/os_specific.hh>

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"

#include <iostream>  // I/O 

#include <string>
#include <vector>

#include <stdexcept>

using namespace std;

void writeOutput( ostream & stream, std::string format, OccurrencesPtr presences, OccurrencesPtr absences, std::string label );

int main( int argc, char **argv ) {

  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "" , "log-level"  , "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "v", "version"    , "Display version info"                        , false );
  opts.addOption( "l", "list"       , "List available formats"                      , false );
  opts.addOption( "s", "source"     , "Source where points are located"             , true );
  opts.addOption( "n", "name"       , "Name (label) to filter points"               , true );
  opts.addOption( "w", "wkt"        , "Spatial reference in WKT"                    , true );
  opts.addOption( "o", "type"       , "Output type"                                 , true );
  opts.addOption( "" , "split"      , "Split points using the specified proportion (0,1)"  , true );
  opts.addOption( "" , "file1"      , "File name to store 1st subset (used w/ param split)", true );
  opts.addOption( "" , "file2"      , "File name to store 2nd subset (used w/ param split)", true );
  opts.addOption( "c", "config-file", "Configuration file for openModeller"         , true );

  std::string log_level("info");
  bool        list_formats = false;
  std::string source("");
  std::string label("");
  std::string wkt("GEOGCS[\"WGS84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"degree\",0.017453292519943295],AXIS[\"Longitude\",EAST],AXIS[\"Latitude\",NORTH]]");
  std::string format("TXT");
  std::string split_prop_string;
  std::string file1;
  std::string file2;
  std::string config_file;

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] ); 
    exit(0);
  }

  // Set up any related external resources
  setupExternalResources();

  OpenModeller om;

  while ( ( option = opts.cycle() ) >= 0 ) {

    switch ( option ) {

      case 0:
        log_level = opts.getArgs( option );
        break;
      case 1:
        printf( "om_points %s\n", om.getVersion().c_str() );
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 2:
        list_formats = true;
        break;
      case 3:
        source = opts.getArgs( option );
        break;
      case 4:
        label = opts.getArgs( option );
        break;
      case 5:
        wkt = opts.getArgs( option );
        break;
      case 6:
        format = opts.getArgs( option );
        break;
      case 7:
        split_prop_string = opts.getArgs( option );
        break;
      case 8:
        file1 = opts.getArgs( option );
        break;
      case 9:
        file2 = opts.getArgs( option );
        break;
      case 10:
        config_file = opts.getArgs( option );
        break;
      default:
        break;
    }
  }

  // Log stuff

  Log::Level level_code = getLogLevel( log_level );

  Log::instance()->setLevel( level_code );

  // om configuration
  if ( ! config_file.empty() ) { 

    Settings::loadConfig( config_file );
  }

  // Real work

  try {

    if ( list_formats ) {

      // Process list drivers request

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


    // Check requirements
    if ( source.empty() ) {

      printf( "Please specify a source to load points from.\n");
      exit(-1);
    }
    if ( label.empty() ) {

      printf( "Please specify a name to filter points.\n");
      exit(-1);
    }

    // Check parameter split
    double split_prop = 0.0;

    if ( ! split_prop_string.empty() ) {

      if ( file1.empty() || file2.empty() ) {

        printf( "When splitting points, you need to specify file1 and file2.\n");
        exit(-1);
      }

      split_prop = atof( split_prop_string.c_str() );

      if ( split_prop <= 0.0 || split_prop >= 1.0 ) {

        printf( "Splitting proportion must be a value between 0 and 1.\n");
        exit(-1);
      }
    }

    // Read occurrences
    OccurrencesReader * occ = OccurrencesFactory::instance().create( source.c_str(), wkt.c_str() );

    OccurrencesPtr presences = occ->getPresences( label.c_str() );

    OccurrencesPtr absences = occ->getAbsences( label.c_str() );

    delete occ;

    // Split points if necessary
    if ( split_prop ) {

      OccurrencesPtr pres1, abs1, pres2, abs2;

      if ( presences ) {

        pres1 = new OccurrencesImpl( presences->label(), presences->coordSystem() );
        pres2 = new OccurrencesImpl( presences->label(), presences->coordSystem() );

        splitOccurrences( presences, pres1, pres2, split_prop );
      }

      if ( absences ) {

        abs1 = new OccurrencesImpl( absences->label(), absences->coordSystem() );
        abs2 = new OccurrencesImpl( absences->label(), absences->coordSystem() );

        splitOccurrences( absences, abs1, abs2, split_prop );
      }

      // Write result in two files

      ofstream outfile1( file1.c_str() );

      writeOutput( outfile1, format, pres1, abs1, label );

      outfile1.close();

      ofstream outfile2( file2.c_str() );

      writeOutput( outfile2, format, pres2, abs2, label );

      outfile2.close();

      return 0;
    }

    // Write result in cout

    std::cerr << flush;

    writeOutput( cout, format, presences, absences, label );
  }
  catch ( runtime_error e ) {

    printf( "om_points: %s\n", e.what() );
    exit(-1);
  }

  return 0;
}


// Function to output result
void writeOutput( ostream & stream, std::string format, OccurrencesPtr presences, OccurrencesPtr absences, std::string label ) {

    if ( format == "XML" ) {

      if ( presences ) {

        ConfigurationPtr cfg = presences->getConfiguration();
        cfg->setName( "Presence" );

        Configuration::writeXml( cfg, stream );
      }

      if ( absences && absences->numOccurrences() ) {

        ConfigurationPtr cfg = absences->getConfiguration();
        cfg->setName( "Absence" );

        Configuration::writeXml( cfg, stream );
      }
    }
    else {

      // Header
      stream << "#id\t" << "label\t" << "long\t" << "lat\t" << "abundance" << endl << flush;

      if ( presences ) {

        OccurrencesImpl::iterator it   = presences->begin();
        OccurrencesImpl::iterator last = presences->end();

        while ( it != last ) {

          stream << (*it)->id() << "\t" << label.c_str() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << (*it)->abundance() << endl << flush;
          ++it;
        }
      }

      if ( absences ) {

        OccurrencesImpl::iterator it   = absences->begin();
        OccurrencesImpl::iterator last = absences->end();

        while ( it != last ) {

          stream << (*it)->id() << "\t" << label.c_str() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << (*it)->abundance() << endl << flush;
          ++it;
        }
      }
    }
}
