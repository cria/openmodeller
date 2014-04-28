#include <openmodeller/om.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/os_specific.hh>

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"

#include <string>
#include <iostream>  // I/O 

#include <stdexcept>
 
using namespace std;

int main( int argc, char **argv ) {

  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "" , "log-level"  , "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "v", "version"    , "Display version info", false );
  opts.addOption( "l", "list"       , "List available algorithms (id and name)", false );
  opts.addOption( "d", "dump-xml"   , "Dump algorithms' metadata in XML", false );
  opts.addOption( "i", "id"         , "Algorithm id", true );
  opts.addOption( "c", "config-file", "Configuration file for openModeller", true );

  std::string log_level("info");
  bool        list_algs = false;
  bool        dump_algs = false;
  std::string alg_id;
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
        printf( "om_algorithm %s\n", om.getVersion().c_str() );
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 2:
        list_algs = true;
        break;
      case 3:
        dump_algs = true;
        break;
      case 4:
        alg_id = opts.getArgs( option );
        break;
      case 5:
        config_file = opts.getArgs( option );
        break;
      default:
        break;
    }
  }

  // Check parameters

  if ( ( ! list_algs ) && ( ! dump_algs ) && alg_id.empty() ) {

    printf( "Please specify one of the parameters: --list, --dump-xml or --id\n");
    exit(1);
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

    AlgorithmFactory::searchDefaultDirs();

    if ( dump_algs ) {

      ConfigurationPtr cfg = AlgorithmFactory::getConfiguration();
      Configuration::writeXml( cfg, cout );

      return 0;
    }

    if ( list_algs ) {

      AlgMetadata const **availables = om.availableAlgorithms();

      if ( ! *availables ) {

        printf( "Could not find any algorithm.\n" );
        exit(1);
      }

      AlgMetadata const *metadata;

      printf( "Available algorithms (id: name)\n" );

      while ( ( metadata = *availables++ ) ) {

        printf( "  %s: %s\n", metadata->id.c_str(), metadata->name.c_str() );
      }

      return 0;
    }

    if ( ! alg_id.empty() ) {

      AlgMetadata const *alg_metadata = AlgorithmFactory::algorithmMetadata( alg_id.c_str() );

      printf( "Algorithm metadata:\n" );
      printf( "  Name: %s\n", alg_metadata->name.c_str() );
      printf( "  Version: %s\n", alg_metadata->version.c_str() );
      printf( "  Overview:\n\n%s\n\n", alg_metadata->overview.c_str() );
      printf( "  Description:\n\n%s\n\n", alg_metadata->description.c_str() );
      printf( "  Author(s): %s\n", alg_metadata->author.c_str() );
      printf( "  Bibliography:\n\n%s\n\n", alg_metadata->biblio.c_str() );
      printf( "  Developer(s): %s (%s)\n", alg_metadata->code_author.c_str(), alg_metadata->contact.c_str() );

      std::string const accepts_categorical = ( alg_metadata->categorical ) ? "yes" : "no";
      printf( "  Accepts categorical data: %s\n", accepts_categorical.c_str() );

      std::string const requires_absences = ( alg_metadata->absence ) ? "yes" : "no";
      printf( "  Requires absence points: %s\n", requires_absences.c_str() );

      AlgParamMetadata *param = alg_metadata->param;

      // Include parameters metadata
      for ( int i = 0 ; i < alg_metadata->nparam; param++, i++ ) {

        printf( "\n  Parameter: %s\n", param->id.c_str() );
        printf( "  Name: %s\n", param->name.c_str() );

        string datatype("?");

        if ( param->type == Integer ) {

          datatype = "Integer";
        }
        else if ( param->type == Real ) {

          datatype = "Real";
        }
        else if ( param->type == String ) {

          datatype = "String";
        }

        printf( "  Datatype: %s\n", datatype.c_str() );

        printf( "  Overview: %s\n", param->overview.c_str() );

        printf( "  Description: %s\n", param->description.c_str() );

        if ( param->has_min || param->has_max ) {

          if ( param->has_min ) {

            printf( "  Minimum value: %f\n", param->min_val );
          }
          if ( param->has_max ) {

            printf( "  Maximum value: %f\n", param->max_val );
          }
        }

        printf( "  Typical value: %s\n", param->typical.c_str() );
      }
    }
  }
  catch ( runtime_error e ) {

    printf( "om_algorithm: %s\n", e.what() );
    exit(1);
  }

  return 0;
}
