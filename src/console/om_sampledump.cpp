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
    Log::instance()->setLevel( Log::Info );
    AlgorithmFactory::searchDefaultDirs();

    if (argc != 3) {
      cout << "Usage " << argv[0] << " <request file> <output filename>" << endl;
      return -1;
    }

    unsigned int format = 1; // 0 = HTML table, 1 = TAB delimited

    OpenModeller om;
    // Configure the OpenModeller object from data read from a request file.
    RequestFile request;
    char *request_file = argv[1];
    request.configure( &om, request_file);

    SamplerPtr s = om.getSampler();

    if ( ! s ) {

      cout << "Could not load sampler (is there an algorithm in your request file?)" << endl;
      return -1;
    }

    ofstream fp_out;
    fp_out.open( argv[2], ios::out );

    int dim = s->numIndependent();
   
    if ( format == 0 ) {

      fp_out << "<table>\n<tr><th>id</th>";

      for ( int i = 0; i < dim; ++i ) {

        fp_out << "<th>attr" << i+1 << "</th>";
      }

      fp_out << "<th>abundance</th></tr>";
    }
    else if ( format == 1 ) {

      fp_out << "#id\tlabel\tlongitude\tlatitude\tabundance";

      for ( int i = 0; i < dim; ++i ) {

        fp_out << "\tattr" << i+1;
      }
    }

    ConstOccurrencesPtr p = s->getPresences();

    if ( p ) {
    
      OccurrencesImpl::const_iterator it = p->begin();
      OccurrencesImpl::const_iterator fin = p->end();

      while ( it != fin ) {

        cout << ((*it)->id()).c_str() << " (" << (*it)->x() << "," << (*it)->y() << ") " << (*it)->environment() << endl;

        if ( format == 0 ) {

          fp_out << "\n<tr><td>" << ((*it)->id()).c_str() << "</td>";

          Sample s = (*it)->environment();

          for ( int i = 0; i < dim; ++i ) {

            fp_out << "<td>" << s[i] << "</td>";
          }

          fp_out << "<td>" << (*it)->abundance() << "</td></tr>";
        }
        else if ( format == 1 ) {

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
    }

    ConstOccurrencesPtr a = s->getAbsences();

    if ( a ) {

      OccurrencesImpl::const_iterator it = a->begin();
      OccurrencesImpl::const_iterator fin = a->end();

      while ( it != fin ) {

        cout << ((*it)->id()).c_str() << " (" << (*it)->x() << "," << (*it)->y() << ") " << (*it)->environment() << endl;

        if ( format == 0 ) {

          fp_out << "\n<tr><td>" << ((*it)->id()).c_str() << "</td>";

          Sample s = (*it)->environment();

          for ( int i = 0; i < dim; ++i ) {

            fp_out << "<td>" << s[i] << "</td>";
          }

          fp_out << "<td>" << (*it)->abundance() << "</td></tr>";
        }
        else if ( format == 1 ) {

          fp_out << "\n" << ((*it)->id()).c_str() << "\t" << a->name() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << (*it)->abundance();

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
    }

    if ( format == 0 ) {

      fp_out << "\n</table>";
    }

    fp_out.close();

    return 0;
  }
  catch (runtime_error e) {
    cout<<"Exception: " << e.what() << endl;
  }
}