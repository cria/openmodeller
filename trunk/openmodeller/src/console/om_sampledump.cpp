#include <openmodeller/om.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>
#include "request_file.hh"
#include "request_file.hh"
#include "file_parser.hh"

#include <istream>

#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>  // I/O 
#include <fstream>   // file I/O

#include <stdexcept>
 
using namespace std;

int main( int argc, char **argv ) {
#ifdef BUILD_TERRALIB
  USE_TERRALIB_IO
#endif

  try {
    //Debug, Info, Warn, Error
    Log::instance()->setLevel( Log::Error );
    AlgorithmFactory::searchDefaultDirs();

    if (argc != 3) {
      cout << "Usagex " << argv[0] << " <request file> <output filename>" << endl;
      return -1;
    }

    char *format = "occ_input"; // html_table or occ_input

    OpenModeller om;
    // Configure the OpenModeller object from data read from the
    // request file.
    RequestFile request;
    char *request_file = argv[1];
    request.configure( &om, request_file);

    //cout << "Reading configuration from: " << argv[1] << endl;    
    //ConfigurationPtr c = Configuration::readXml( argv[1] );
    //om.setConfiguration(c);
    //OutputFormat myOutputFormat = MapFormat( format.c_str() );
    // hard coded for now: 8-bit grey tiffs
    //myOutputFormat.setFormat( MapFormat::GreyTiff );
    //om->createMap( argv[1], _outputFormat );

    SamplerPtr s = om.getSampler();

    if ( ! s ) {

      cout << "Could not load sampler (is there an algorithm in your request file?)" << endl;
      return -1;
    }
   
    ConstOccurrencesPtr p = s->getPresences();
    
    OccurrencesImpl::const_iterator it = p->begin();
    OccurrencesImpl::const_iterator fin = p->end();

    ofstream fp_out;
    fp_out.open(argv[2], ios::out);

    int dim = p->dimension();

    if ( ! strcasecmp( format, "html_table" ) ) {

      fp_out << "<table>\n<tr><th>id</th>";

      for ( int i = 0; i < dim; ++i )
      {
        fp_out << "<th>attr" << i+1 << "</th>";
      }

      fp_out << "<th>abundance</th></tr>";
    }
    else if ( ! strcasecmp( format, "occ_input" ) ) {

      fp_out << "#id\tlabel\tlongitude\tlatitude\tabundance";

      for ( int i = 0; i < dim; ++i )
      {
        fp_out << "\tattr" << i+1;
      }
    }

    while ( it != fin ) {

      cout << ((*it)->id()).c_str() << " (" << (*it)->x() << "," << (*it)->y() << ") " << (*it)->environment() << endl;

      if ( ! strcasecmp( format, "html_table" ) ) {

        fp_out << "\n<tr><td>" << ((*it)->id()).c_str() << "</td>";

        Sample s = (*it)->environment();

        for ( int i = 0; i < dim; ++i ) {

          fp_out << "<td>" << s[i] << "</td>";
        }

        fp_out << "<td>" << (*it)->abundance() << "</td></tr>";
      }
      else if ( ! strcasecmp( format, "occ_input" ) ) {

        fp_out << "\n" << ((*it)->id()).c_str() << "\t" << p->name() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << (*it)->abundance();

        Sample s = (*it)->environment();

        for ( int i = 0; i < dim; ++i ) {

          fp_out << "\t" << s[i];
        }
      }
      else {

        fp_out<<(*it)->environment()<<endl;
      }
      it++;
    }

    ConstOccurrencesPtr a = s->getAbsences();

    if ( ! a ) {

      // No absences. Just quit.
      return 0;
    }
    
    it = a->begin();
    fin = a->end();

    while ( it != fin ) {

      cout << ((*it)->id()).c_str() << " (" << (*it)->x() << "," << (*it)->y() << ") " << (*it)->environment() << endl;

      if ( ! strcasecmp( format, "html_table" ) ) {

        fp_out << "\n<tr><td>" << ((*it)->id()).c_str() << "</td>";

        Sample s = (*it)->environment();

        for ( int i = 0; i < dim; ++i ) {

          fp_out << "<td>" << s[i] << "</td>";
        }

        fp_out << "<td>" << (*it)->abundance() << "</td></tr>";
      }
      else if ( ! strcasecmp( format, "occ_input" ) ) {

        fp_out << "\n" << ((*it)->id()).c_str() << "\t" << p->name() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << (*it)->abundance();

        Sample s = (*it)->environment();

        for ( int i = 0; i < dim; ++i ) {

          fp_out << "\t" << s[i];
        }
      }
      else {

        fp_out<<(*it)->environment()<<endl;
      }

      it++;
    }

    if ( ! strcasecmp( format, "html_table" ) ) {

      fp_out << "\n</table>";
    }

    fp_out.close();
    return 0;
  }
  catch (runtime_error e) {
    cout<<"Exception: " << e.what() << endl;
  }
}
