#include <openmodeller/om.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"

#include <string>
#include <iostream>  // I/O 

#include <stdexcept>
 
using namespace std;

int main( int argc, char **argv ) {

  char *args;
  int option;

  struct options opts[] = 
  {
    { 1, "log-level", "Set the log level (debug, warn, info, error)", NULL, 1 },
    { 2, "version",   "Display version info",                          "v", 0 },
    { 3, "list",      "List available algorithms (id and name)",       "l", 0 },
    { 4, "dump-xml",  "Dump algorithms' metadata in XML",              "d", 0 },
    { 5, "id",        "Algorithm id",                                  "i", 1 },
    { 0, NULL,        NULL,                                           NULL, 0 }
  };

  bool passed_params = false;

  std::string log_level("info");
  bool        list_algs = false;
  bool        dump_algs = false;
  std::string alg_id;

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
        printf("om_algorithm 0.1.0\n");
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 3:
        list_algs = true;
        break;
      case 4:
        dump_algs = true;
        break;
      case 5:
        alg_id = args;
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
    getopts_usage( "om_algorithm", opts );
    exit(0);
  }

  // Check parameters

  if ( ( ! list_algs ) && ( ! dump_algs ) && alg_id.empty() ) {

    printf( "Please specify one of the parameters: --list, --dump-xml or --id\n");
    exit(-1);
  }

  // Log stuff

  Log::Level level_code = getLogLevel( log_level );

  Log::instance()->setLevel( level_code );

  // Real work

  try {

    AlgorithmFactory::searchDefaultDirs();

    if ( dump_algs ) {

      ConfigurationPtr cfg = AlgorithmFactory::getConfiguration();
      Configuration::writeXml( cfg, cout );

      return 0;
    }

    OpenModeller om;

    if ( list_algs ) {

      AlgMetadata const **availables = om.availableAlgorithms();

      if ( ! *availables ) {

        printf( "Could not find any algorithm.\n" );
        exit(-1);
      }

      AlgMetadata const *metadata;

      printf( "Available algorithms (id: name)\n" );

      while ( ( metadata = *availables++ ) ) {

        printf( "  %s: %s\n", metadata->id, metadata->name );
      }

      return 0;
    }

    if ( ! alg_id.empty() ) {

      AlgMetadata const *alg_metadata = AlgorithmFactory::algorithmMetadata( alg_id.c_str() );

      printf( "Algorithm metadata:\n" );
      printf( "  Name: %s\n", alg_metadata->name );
      printf( "  Version: %s\n", alg_metadata->version );
      printf( "  Overview:\n\n%s\n\n", alg_metadata->overview );
      printf( "  Description:\n\n%s\n\n", alg_metadata->description );
      printf( "  Author(s): %s\n", alg_metadata->author );
      printf( "  Bibliography:\n\n%s\n\n", alg_metadata->biblio );
      printf( "  Developer(s): %s (%s)\n", alg_metadata->code_author, alg_metadata->contact );

      char const *accepts_categorical = ( alg_metadata->categorical ) ? "yes" : "no";
      printf( "  Accepts categorical data: %s\n", accepts_categorical );

      char const *requires_absences = ( alg_metadata->absence ) ? "yes" : "no";
      printf( "  Requires absence points: %s\n", requires_absences );

      AlgParamMetadata *param = alg_metadata->param;

      // Include parameters metadata
      for ( int i = 0 ; i < alg_metadata->nparam; param++, i++ ) {

        printf( "\n  Parameter: %s\n", param->id );
        printf( "  Name: %s\n", param->name );

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

        printf( "  Overview: %s\n", param->overview );

        printf( "  Description: %s\n", param->description );

        if ( param->has_min || param->has_max ) {

          if ( param->has_min ) {

            printf( "  Minimum value: %f\n", param->min_val );
          }
          if ( param->has_max ) {

            printf( "  Maximum value: %f\n", param->max_val );
          }
        }

        printf( "  Typical value: %s\n", param->typical );
      }
    }
  }
  catch ( runtime_error e ) {

    printf( "om_algorithm: %s\n", e.what() );
    exit(-1);
  }

  return 0;
}
