#include <openmodeller/om.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/os_specific.hh>
#include <openmodeller/FileParser.hh>

#include "request_file.hh"

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"

#include <istream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>  // I/O 
#include <fstream>   // file I/O

#include <stdexcept>
 
using namespace std;

void printOccurrences( ostream & stream, ConstOccurrencesPtr occ );

int main( int argc, char **argv ) {

  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "v", "version"    , "Display version info"                        , false );
  opts.addOption( "s", "source"     , "Source with references to points and layers" , true );
  opts.addOption( "e", "dump-env"   , "Dump environment data for a specified cell range", false );
  opts.addOption( "" , "cell-start" , "Cell position to start environment dumping (default 0)"  , true );
  opts.addOption( "" , "cell-end"   , "Cell position to end environment dumping (default 1000)" , true );
  opts.addOption( "" , "log-level"  , "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "c", "config-file", "Configuration file for openModeller"         , true );

  std::string log_level("info");
  std::string source("");
  bool        dump_env = false;
  std::string start_string("0");
  std::string end_string("1000");
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
        printf( "om_sampler %s\n", om.getVersion().c_str() );
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 1:
        source = opts.getArgs( option );
        break;
      case 2:
        dump_env = true;
        break;
      case 3:
        start_string = opts.getArgs( option );
        break;
      case 4:
        end_string = opts.getArgs( option );
        break;
      case 5:
        log_level = opts.getArgs( option );
        break;
      case 6:
        config_file = opts.getArgs( option );
        break;
      default:
        break;
    }
  }

  // Check requirements
  if ( source.empty() ) {

    printf( "Please specify a source (request file in txt or xml) with references to points and layers.\n");
    exit(-1);
  }

  // om configuration
  if ( ! config_file.empty() ) { 

    Settings::loadConfig( config_file );
  }

  int cell_start = 0;
  int cell_end = 0;

  if ( dump_env ) {

    cell_start = atoi( start_string.c_str() );

    cell_end = atoi( end_string.c_str() );
  }

  // Log stuff

  Log::Level level_code = getLogLevel( log_level );

  Log::instance()->setLevel( level_code );

  // Real work

  try {

    // Read source

    bool is_xml = false;

    // Try to open file and check if first char is "<". In this case we will
    // assume that it's an XML file.
    std::ifstream fs( source.c_str(), std::ios_base::in );

    if ( fs.fail() ) {

      printf( "Could not open source file.\n");
      exit(-1);
    }

    std::string line("");

    while ( getline( fs, line ) ) {

      for ( unsigned int i = 0; i < line.size(); ++i ) {    

        // Skip carriage returns, line feeds and spaces
        if ( line[i] == '\r' || line[i] == '\n' || line[i] == ' ' ) {

          continue;
        }

        // Looks like an XML file
        if ( line[i] == '<' ) {

          is_xml = true;
        }

        break;
      }
    }

    fs.close();

    SamplerPtr sampler;

    if ( is_xml ) {

      ConfigurationPtr config = Configuration::readXml( source.c_str() );

      sampler = createSampler( config->getSubsection( "Sampler" ) );
    }
    else {

      // Must be a txt request file, so parse it

      FileParser fp( source.c_str() );

      // Load points

      // Obtain the Well Known Text string for the localities
      // coordinate system.
      std::string oc_cs = fp.get( "WKT coord system" );

      // Get the name of the file containing localities
      std::string oc_file = fp.get( "Occurrences source" );

      // Get the label
      std::string oc_name = fp.get( "Occurrences group" );

      // When a model needs to be created, 'WKT coord system' and 
      // 'Species file' are mandatory parameters
      if ( oc_cs.empty() ) {

        printf( "'WKT coord system' keyword not specified in the request file!\n" );
        exit(-1);
      }

      if ( oc_file.empty() ) {

        printf( "'Occurrences source' keyword not specified in the request file!\n" );
        exit(-1);
      }

      // Populate the occurences list from the localities file
      OccurrencesReader* oc_reader = OccurrencesFactory::instance().create( oc_file.c_str(), oc_cs.c_str() );

      OccurrencesPtr presences = oc_reader->getPresences( oc_name.c_str() );

      OccurrencesPtr absences = oc_reader->getAbsences( oc_name.c_str() );

      delete oc_reader;

      // Load layers

      // Mask to select the desired species occurrence points
      std::string input_mask = fp.get( "Mask" );

      // Initiate the environment with all maps.
      std::vector<std::string> categ_map = fp.getAll( "Categorical map" );
      std::vector<std::string> cont_map = fp.getAll( "Map" );

      // When a model needs to be created, there should be at least one input map
      if ( ! (categ_map.size() + cont_map.size()) ) {

        printf( "At least one 'Map' or 'Categorical map' needs to be specified in the request file!\n" );
        exit(-1);
      }

      // If Mask was not specified, use the first layer
      if ( input_mask.empty() ) {

        if ( cont_map.size() ) {

          input_mask = cont_map[0];
        }
        else {

          input_mask = categ_map[0];
        }
      }

      EnvironmentPtr env = createEnvironment( categ_map, cont_map, input_mask );

      sampler = createSampler( env, presences, absences );

      std::string spatially_unique = fp.get( "Spatially unique" );
      if ( spatially_unique == "true" ) {

        sampler->spatiallyUnique();
      }

      std::string environmentally_unique = fp.get( "Environmentally unique" );
      if ( environmentally_unique == "true" ) {

        sampler->environmentallyUnique();
      }
    }

    // Print output

    int dim = sampler->numIndependent();

    // Header
    cout << "#id\tlabel\tlongitude\tlatitude\tabundance";

    for ( int i = 0; i < dim; ++i ) {

      cout << "\tattr" << i+1;
    }

    if ( dump_env ) {

      // Dump environment data

      EnvironmentPtr e = sampler->getEnvironment();

      // Initialize the iterator
      MapIterator it = e->getMask()->begin();

      // Initialize the terminal
      MapIterator fin;

      int cnt = 0;

      while( it != fin ) {

        if ( cnt < cell_start) {

          ++it;
          ++cnt;
          continue;
        }

        // Get the lon/lat coordinates from the iterator
        pair<Coord,Coord> lonlat = *it;

        // Extract the environment sample at that point
        Sample s = e->get( lonlat.first, lonlat.second );

        cout << "\n" << cnt << "\t" << "env data" << "\t" << lonlat.first << "\t" << lonlat.second << "\t" << "-";

        // Note:  s will have size() == 0 if the environment
        // does not have data for that location. This can happen
        // for a couple of reasons.
        //   1.  The location is not in the mask.
        //   2.  One of the layers has value == NoVal at that location.
        if ( s.size() > 0 ) {

          for ( int i = 0; i < dim; ++i ) {

            cout << "\t" << s[i];
          }
        }
        else {

            cout << "\t" << "nodata";
	}

        ++cnt;
        ++it;

        if ( cnt > cell_end ) {

          break;
        }
      }

      return 0;
    }

    // Default action: dump locality samples

    ConstOccurrencesPtr p = sampler->getPresences();

    if ( p ) {
    
      printOccurrences( cout, p );
    }

    ConstOccurrencesPtr a = sampler->getAbsences();

    if ( a ) {

      printOccurrences( cout, a );
    }

    return 0;
  }
  catch ( runtime_error e ) {

    printf( "om_sampler: %s\n", e.what() );
    exit(-1);
  }
}

// Print occurrences
void printOccurrences( ostream & stream, ConstOccurrencesPtr occ ) {

  int dim = occ->dimension();

  OccurrencesImpl::const_iterator it = occ->begin();
  OccurrencesImpl::const_iterator fin = occ->end();

  while ( it != fin ) {

    stream << "\n" << ((*it)->id()).c_str() << "\t" << occ->label() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << (*it)->abundance();

    Sample s = (*it)->environment();

    for ( int i = 0; i < dim; ++i ) {

      stream << "\t" << s[i];
    }

    it++;
  }
}
