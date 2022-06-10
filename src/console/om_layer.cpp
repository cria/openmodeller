#include <openmodeller/om.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/os_specific.hh>

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"
#include "om_layer_utils.hh"

#include <fstream>   // file I/O for XML
#include <string>
#include <stdexcept>

using namespace std;

int main( int argc, char **argv ) {

  Options opts;
  int option;

  opts.addOption( "" , "log-level"  , "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "v", "version"    , "Display version info"                        , false );
  opts.addOption( "d", "scan-dir"   , "Directory to be scanned for layers"          , true );
  opts.addOption( "s", "result"     , "File to store scan result"                   , true );
  opts.addOption( "l", "check"      , "Layer to be checked"                         , true );
  opts.addOption( "c", "config-file", "Configuration file for openModeller"         , true );

  std::string log_level("info");
  std::string scan_dir;
  std::string layer_id;
  std::string config_file;
  std::string result_file;

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] ); 
    exit(0);
  }

  OpenModeller om;

  while ( ( option = opts.cycle() ) >= 0 ) {

    switch ( option ) {

      case 0:
        log_level = opts.getArgs( option );
        break;
      case 1:
        printf( "om_layer %s\n", om.getVersion().c_str() );
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 2:
        scan_dir = opts.getArgs( option );
        break;
      case 3:
        result_file = opts.getArgs( option );
        break;
      case 4:
        layer_id = opts.getArgs( option );
        break;
      case 5:
        config_file = opts.getArgs( option );
        break;
      default:
        break;
    }
  }

  // Check parameters

  if ( (scan_dir.empty() && layer_id.empty()) || (!scan_dir.empty() && !layer_id.empty()) ) {

    printf( "Please specify one and only one of the parameters: --check layer_id, --scan-dir directory\n");
    exit(1);
  }

  // Log stuff
  Log::Level level_code = getLogLevel( log_level );

  Log::instance()->setLevel( level_code );

  // Config file
  if ( ! config_file.empty() ) {

    Settings::loadConfig( config_file );
  }

  if ( layer_id.size() > 0 ) {

    try {

      Raster * r = RasterFactory::instance().create( layer_id, 0 );

      delete r;
    }
    catch ( std::runtime_error &e ) {

      printf( "Failed to process raster %s:\n\n%s\n", layer_id.c_str(), e.what() );
      exit(1);
    }
  }
  else {

    // Here we assume scan-dir was specified

    ostringstream oss;

    int seq = 1;

    if ( ! readDirectory( scan_dir.c_str(), "Layers", oss, 0, &seq ) ) {

      printf( "Failed to scan specified directory\n" );
      exit(1);
    }

    std::cerr << flush;

    // Write result to file, if requested
    if ( ! result_file.empty() ) {

      ofstream file( result_file.c_str() );
      file << oss.str();
      file.close();
    }
    else {

      // Otherwise send it to stdout
      std::cout << oss.str().c_str() << endl << flush;
    }
  }

  return 0;
}
