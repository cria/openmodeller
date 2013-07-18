/**
 * openModeller Web service manager for experiments.
 * 
 * @author Renato De Giovanni (renato [at] cria [dot] org [dot] br)
 * $Id: $
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2013 by CRIA -
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

#include "openModeller.nsmap"
#include "omH.h"

#include <openmodeller/FileParser.hh>
#include "console/getopts/getopts.h"

#include <openmodeller/om.hh>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string.h>
using namespace std;

#define OMWS_MODEL "model"
#define OMWS_TEST "test"
#define OMWS_PROJECTION "proj"
#define OMWS_EVALUATE "eval"
#define OMWS_SAMPLING "samp"
#define OMWS_EXPERIMENT "exp"
#define _REQUEST "_req."
#define _PROCESSED_REQUEST "_proc."
#define _RESPONSE "_resp."
#define _PENDING_REQUEST "_pend."
#define OMWS_JOB_PROGRESS_PREFIX "prog."
#define OMWS_JOB_DONE_PREFIX "done."
#define OMWS_JOB_METADATA_PREFIX "job."

// Forward declarations
bool fileExists( const char* fileName );
int  getProgress( const string & progFileName );
bool updateProgress( const string & progFileName, const char * content );
vector<string> getTickets( const string & concatenatedValues );
map<string, string> getTicketsWithTask( const string & concatenatedValues );
bool createFile( const string & fileName );
void cancelExperiment( const string & exp_metadata_file, const string & exp_prog_file, const string & exp_done_file, const string & ticket_dir, const string & job_ticket, FILE * fd_log );
void releaseSoap(struct soap *ctx1, struct soap *ctx2);

int main(int argc, char **argv)
{
  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "v", "version", "Display version info"    , false );
  opts.addOption( "c", "config" , "OMWS configuration file" , true );
  opts.addOption( "t", "ticket" , "Job ticket"              , true );

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] ); 
    exit(0);
  }

  OpenModeller om;

  std::string config_file;
  std::string job_ticket;

  while ( ( option = opts.cycle() ) >= 0 ) {

    switch ( option ) {

      case 0:
        printf( "omws_manager %s\n", om.getVersion().c_str() );
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 1:
        config_file = opts.getArgs( option );
        break;
      case 2:
        job_ticket = opts.getArgs( option );
        break;
      default:
        break;
    }
  }

  // Check parameters
  if ( config_file.empty() ) {

    printf( "Please specify the omws configuration file\n");
    exit(1);
  }

  if ( job_ticket.empty() ) {

    printf( "Please specify the job ticket\n");
    exit(1);
  }

  // OMWS configuration
  FileParser omws_config( config_file );

  string ticket_dir( omws_config.get( "TICKET_DIRECTORY" ) );

  if ( ticket_dir.empty() ) {

    printf("Missing ticket directory configuration\n");
    exit(1);
  }

  // Append slash if necessary
  if ( ticket_dir.find_last_of( "/" ) != ticket_dir.size() - 1 ) {

    ticket_dir.append( "/" );
  }

  // Job data
  string job_metadata_file = ticket_dir + OMWS_JOB_METADATA_PREFIX + job_ticket;

  FileParser job_data( job_metadata_file );

  string exp_ticket( job_data.get( "EXP" ) );

  if ( exp_ticket.empty() ) {

    printf("Missing experiment ticket\n");
    exit(1);
  }

  string exp_file = ticket_dir + exp_ticket;

  // Lock experiment file
  struct flock fl;
  fl.l_type   = F_RDLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start  = 0;
  fl.l_len    = 0;
  fl.l_pid    = getpid();

  int fd = open( exp_file.c_str(), O_RDONLY );

  fcntl( fd, F_SETLKW, &fl ); // Wait for lock if necessary

  // Open experiment log file
  string log_file = ticket_dir + exp_ticket;
  FILE *fd_log = fopen( log_file.c_str(), "a" );

  bool start = true;

  if ( fd_log == NULL ) {

    printf("Cannot open experiment log file\n");
    start = false;
  }
  else {

    string msg = "\nJob " + job_ticket + " finished. Managing workflow...\n";

    if ( fputs( msg.c_str(), fd_log ) < 0 ) {

      printf("Could not write to experiment log file %s\n", log_file.c_str());
      start = false;
    }
  }

  // Main stuff
  while ( start ) {

    string next_jobs = job_data.get( "NEXT" );

    string exp_done_file = ticket_dir + OMWS_JOB_DONE_PREFIX + exp_ticket;
    string exp_prog_file = ticket_dir + OMWS_JOB_PROGRESS_PREFIX + exp_ticket;
    string exp_metadata_file = ticket_dir + OMWS_JOB_METADATA_PREFIX + exp_ticket;

    if ( fileExists( exp_done_file.c_str() ) ) {

      // Some other process already did all necessary stuff!
      // Here we assume that the existence of this file means that the experiment
      // finished - successfully or not!
      fputs( "Experiment already finished by other job. Aborting.\n", fd_log );
      break;
    }

    // Read job progress
    string job_prog_file = ticket_dir + OMWS_JOB_PROGRESS_PREFIX + job_ticket;

    if ( getProgress( job_prog_file ) == 100 ) {

      // Job finished successfully!
      fputs( "Job finished successfuly!\n", fd_log );

      // Are there other jobs to be triggered?

      if ( next_jobs.empty() ) {

        // There are no subsequent jobs to be triggered
        fputs( "No subsequent jobs.\n", fd_log );

        // Get all jobs in the experiment to check if they all finished
        FileParser exp_data( exp_metadata_file );

        string all_jobs = exp_data.get( "JOBS" );

        vector<string> all_tickets = getTickets( all_jobs );

        bool finish_experiment = true;

        // For each job in the experiment
        for ( vector<string>::iterator at = all_tickets.begin(); at != all_tickets.end(); ++at ) {

          if ( job_ticket.compare(0, 6, (*at)) == 0 ) {

            continue; // Skip current job!
          }

          string done_file = ticket_dir + OMWS_JOB_DONE_PREFIX + (*at);

          // If job is finished
          // Note: here we assume that if the job finished, it finished OK, otherwise it 
          //       would have already aborted the experiment!
          if ( ! fileExists( done_file.c_str() ) ) {

            string msg = "Another job (" + (*at) + ") is still pending.\n";
            fputs( msg.c_str(), fd_log );
            finish_experiment = false;
            break;
          }
        }

        // Only finish experiment if all jobs finished OK
        if ( finish_experiment ) {      

          fputs( "All jobs finished OK. Finishing experiment.\n", fd_log );
          updateProgress( exp_prog_file, "100" );
          createFile( exp_done_file );
        }
      }
      else {

        // There are subsequent jobs to be triggered
        fputs( "Checking next jobs...\n", fd_log );

        // Check if they can be started
        vector<string> next_tickets = getTickets( next_jobs );

        // For each of the next jobs
        for ( vector<string>::iterator nt = next_tickets.begin(); nt != next_tickets.end(); ++nt ) {

          string msg = "Ticket " + (*nt) + "\n";
          fputs( msg.c_str(), fd_log );

          // Get dependencies from metadata
          string metadata_file = ticket_dir + OMWS_JOB_METADATA_PREFIX + (*nt);

          FileParser data( metadata_file );

          string dep_jobs = data.get("PREV");

          map<string, string> dep_tickets = getTicketsWithTask( dep_jobs );

          bool can_start = true;

          map<string, string>::iterator dt;

          // For each dependency, check that it finished
          for ( dt = dep_tickets.begin(); dt != dep_tickets.end(); ++dt ) {

            if ( job_ticket.compare(0, 6, (*dt).first) == 0 ) {

              continue; // Skip current job!
            }

            string done_file = ticket_dir + OMWS_JOB_DONE_PREFIX + (*dt).first;

            // If job did not finish
            // Note: here we assume that if the job finished, it finished OK, otherwise it 
            //       would have already aborted the experiment!
            if ( ! fileExists( done_file.c_str() ) ) {

              string msg = "Dependent job " + (*dt).first + " still pending.\n";
              fputs( msg.c_str(), fd_log );
              can_start = false;
              break;
            }
          }

          if ( can_start ) {

            fputs( "All dependencies met! Starting job.\n", fd_log );

            string type = data.get("TYPE");

            // gSOAP context to parse next jobs XML
            struct soap *ctx_next = soap_new1(SOAP_XML_STRICT);
            soap_init(ctx_next);
            soap_imode(ctx_next, SOAP_ENC_XML); // Set input mode
            soap_imode(ctx_next, SOAP_XML_IGNORENS);
            soap_begin(ctx_next); // start new (de)serialization phase

            // gSOAP context to parse dependent jobs XML
            struct soap *ctx_dep = soap_new1(SOAP_XML_STRICT);
            soap_init(ctx_dep);
            soap_imode(ctx_dep, SOAP_ENC_XML); // Set input mode
            soap_imode(ctx_dep, SOAP_XML_IGNORENS);
            soap_begin(ctx_dep); // start new (de)serialization phase

            ifstream fs_in;
            ofstream fs_out;

            // Pending jobs can only be of type model, test or proj

            ///////////////////////// MODEL ///////////////////////////
            ///////////////////////////////////////////////////////////
            if ( strcmp(type.c_str(), OMWS_MODEL) == 0 ) {

              string pend_file = ticket_dir + OMWS_MODEL + _PENDING_REQUEST + (*nt);

              // This file must exist!
              if ( ! fileExists( pend_file.c_str() ) ) {

                string msg = "File does not exist: " + pend_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }

              fs_in.open( pend_file.c_str() );
              ctx_next->is = &fs_in;

              om::_om__ModelParameters mp;

              // Parsing must succeed! (file was created using the same lib)
              if ( soap_read_om__ModelParametersType( ctx_next, &mp ) != SOAP_OK ) {

                string msg = "Could not deserialize: " + pend_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }

              // Get result from each dependency
              for ( dt = dep_tickets.begin(); dt != dep_tickets.end(); ++dt ) {

                string dep_ticket = (*dt).first;
                string dep_place = (*dt).second;

                if ( strcmp(dep_place.c_str(), "presence") == 0 ) {

                  string result_file = ticket_dir + OMWS_SAMPLING + _PROCESSED_REQUEST + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    string msg = "File does not exist: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  fs_in.open( result_file.c_str() );

                  om::_om__Sampler samp;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SamplerType( ctx_dep, &samp ) != SOAP_OK ) {

                    string msg = "Could not deserialize: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  mp.Sampler->Presence = samp.Presence;
                }
                else if ( strcmp(dep_place.c_str(), "absence") == 0 ) {

                  string result_file = ticket_dir + OMWS_SAMPLING + _PROCESSED_REQUEST + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    string msg = "File does not exist: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  fs_in.open( result_file.c_str() );

                  om::_om__Sampler samp;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SamplerType( ctx_dep, &samp ) != SOAP_OK ) {

                    string msg = "Could not deserialize: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  mp.Sampler->Absence = samp.Absence;
                }
                else {

                  string msg = "Ignoring unknown dependency type: " + dep_place;
                  fputs( msg.c_str(), fd_log );
                }
              }

              // Write request to file
              string req_file = ticket_dir + OMWS_MODEL + _REQUEST + (*nt);

              fs_out.open( req_file.c_str() );
              ctx_next->os = &fs_out;
              soap_set_omode(ctx_next, SOAP_XML_CANONICAL);
              soap_set_omode(ctx_next, SOAP_XML_INDENT);
              soap_set_omode(ctx_next, SOAP_XML_TREE);
              soap_set_omode(ctx_next, SOAP_XML_NOTYPE);

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__ModelParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              if ( ( mp.soap_serialize(ctx_next), soap_begin_send(ctx_next) || mp.soap_put(ctx_next, "om:ModelParameters", NULL) || soap_end_send(ctx_next), ctx_next->error ) != SOAP_OK ) {

                string msg = "Failed to serialize new job request: " + req_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }
            }
            ///////////////////////// TEST ////////////////////////////
            ///////////////////////////////////////////////////////////
            else if ( strcmp(type.c_str(), OMWS_TEST) == 0 ) {

              string pend_file = ticket_dir + OMWS_TEST + _PENDING_REQUEST + (*nt);

              // This file must exist!
              if ( ! fileExists( pend_file.c_str() ) ) {

                string msg = "File does not exist: " + pend_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }

              fs_in.open( pend_file.c_str() );
              ctx_next->is = &fs_in;

              om::_om__TestParameters tp;

              // Parsing must succeed! (file was created using the same lib)
              if ( soap_read_om__TestParametersType( ctx_next, &tp ) != SOAP_OK ) {

                string msg = "Could not deserialize: " + pend_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }

              // Get result from each dependency
              for ( dt = dep_tickets.begin(); dt != dep_tickets.end(); ++dt ) {

                string dep_ticket = (*dt).first;
                string dep_place = (*dt).second;

                if ( strcmp(dep_place.c_str(), "presence") == 0 ) {

                  string result_file = ticket_dir + OMWS_SAMPLING + _PROCESSED_REQUEST + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    string msg = "File does not exist: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  fs_in.open( result_file.c_str() );

                  om::_om__Sampler samp;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SamplerType( ctx_dep, &samp ) != SOAP_OK ) {

                    string msg = "Could not deserialize: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  tp.Sampler->Presence = samp.Presence;
                }
                else if ( strcmp(dep_place.c_str(), "absence") == 0 ) {

                  string result_file = ticket_dir + OMWS_SAMPLING + _PROCESSED_REQUEST + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    string msg = "File does not exist: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  fs_in.open( result_file.c_str() );

                  om::_om__Sampler samp;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SamplerType( ctx_dep, &samp ) != SOAP_OK ) {

                    string msg = "Could not deserialize: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  tp.Sampler->Absence = samp.Absence;
                }
                else if ( strcmp(dep_place.c_str(), "model") == 0 ) {

                  string result_file = ticket_dir + OMWS_MODEL + _PROCESSED_REQUEST + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    string msg = "File does not exist: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  fs_in.open( result_file.c_str() );

                  om::om__ModelEnvelopeType model;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__ModelEnvelopeType( ctx_dep, &model ) != SOAP_OK ) {

                    string msg = "Could not deserialize: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  tp.Algorithm = model.SerializedModel->Algorithm;
                }
                else {

                  string msg = "Ignoring unknown dependency type: " + dep_place;
                  fputs( msg.c_str(), fd_log );
                }
              }

              // Write request to file
              string req_file = ticket_dir + OMWS_TEST + _REQUEST + (*nt);

              fs_out.open( req_file.c_str() );
              ctx_next->os = &fs_out;
              soap_set_omode(ctx_next, SOAP_XML_CANONICAL);
              soap_set_omode(ctx_next, SOAP_XML_INDENT);
              soap_set_omode(ctx_next, SOAP_XML_TREE);
              soap_set_omode(ctx_next, SOAP_XML_NOTYPE);

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__ModelParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              if ( ( tp.soap_serialize(ctx_next), soap_begin_send(ctx_next) || tp.soap_put(ctx_next, "om:TestParameters", NULL) || soap_end_send(ctx_next), ctx_next->error ) != SOAP_OK ) {

                string msg = "Failed to serialize new job request: " + req_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }
            }
            ////////////////////// PROJECTION /////////////////////////
            ///////////////////////////////////////////////////////////
            else if ( strcmp(type.c_str(), "proj") == 0 ) {

              string pend_file = ticket_dir + OMWS_PROJECTION + _PENDING_REQUEST + (*nt);

              // This file must exist!
              if ( ! fileExists( pend_file.c_str() ) ) {

                string msg = "File does not exist: " + pend_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }

              fs_in.open( pend_file.c_str() );
              ctx_next->is = &fs_in;

              om::_om__ProjectionParameters pp;

              // Parsing must succeed! (file was created using the same lib)
              if ( soap_read_om__ProjectionParametersType( ctx_next, &pp ) != SOAP_OK ) {

                string msg = "Could not deserialize: " + pend_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }

              // Get result from each dependency
              for ( dt = dep_tickets.begin(); dt != dep_tickets.end(); ++dt ) {

                string dep_ticket = (*dt).first;
                string dep_place = (*dt).second;

                if ( strcmp(dep_place.c_str(), "model") == 0 ) {

                  string result_file = ticket_dir + OMWS_MODEL + _PROCESSED_REQUEST + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    string msg = "File does not exist: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  fs_in.open( result_file.c_str() );

                  om::om__ModelEnvelopeType model;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__ModelEnvelopeType( ctx_dep, &model ) != SOAP_OK ) {

                    string msg = "Could not deserialize: " + result_file;
                    fputs( msg.c_str(), fd_log );
                    cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                    releaseSoap(ctx_next, ctx_dep);
                    break;
                  }

                  pp.Algorithm = model.SerializedModel->Algorithm;
                }
                else {

                  string msg = "Ignoring unknown dependency type: " + dep_place;
                  fputs( msg.c_str(), fd_log );
                }
              }

              // Write request to file
              string req_file = ticket_dir + OMWS_TEST + _REQUEST + (*nt);

              fs_out.open( req_file.c_str() );
              ctx_next->os = &fs_out;
              soap_set_omode(ctx_next, SOAP_XML_CANONICAL);
              soap_set_omode(ctx_next, SOAP_XML_INDENT);
              soap_set_omode(ctx_next, SOAP_XML_TREE);
              soap_set_omode(ctx_next, SOAP_XML_NOTYPE);

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__ModelParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              if ( ( pp.soap_serialize(ctx_next), soap_begin_send(ctx_next) || pp.soap_put(ctx_next, "om:TestParameters", NULL) || soap_end_send(ctx_next), ctx_next->error ) != SOAP_OK ) {

                string msg = "Failed to serialize new job request: " + req_file;
                fputs( msg.c_str(), fd_log );
                cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
                releaseSoap(ctx_next, ctx_dep);
                break;
              }
            }

            releaseSoap( ctx_next, ctx_dep );
          }
        }
      }
    }
    else {

      // Job didn't finish successfully, so cancel the experiment and all pending jobs
      fputs( "Job didn't finish successfully.\n", fd_log );
      cancelExperiment( exp_metadata_file, exp_prog_file, exp_done_file, ticket_dir, job_ticket, fd_log );
    }

    break;
  }

  fputs( "Finished management trigger\n", fd_log );
  fclose( fd_log );

  // Unlock experiment file
  fl.l_type = F_UNLCK;
  fcntl( fd, F_SETLK, &fl );

  return 0; 
}


////////////////////////////////////////////////////
//                                                //
//                Helper functions                //
//                                                //
////////////////////////////////////////////////////

/********************/
/**** fileExists ****/
bool 
fileExists( const char* fileName )
{ 
  FILE * file = fopen( fileName, "r" );

  if ( file != NULL ) {

    fclose( file );

    return true;
  }

  return false;
}

/*********************/
/**** getProgress ****/
int 
getProgress( const string & progFileName )
{
  int progress = -1;

  if ( fileExists( progFileName.c_str() ) ) {

    // If file exists, get its content
    fstream fin;
    fin.open( progFileName.c_str(), ios::in );

    if ( fin.is_open() ) {

      // If file was opened, read the content
      ostringstream oss;
      string line;

      getline( fin, line );
      oss << line << endl;

      // Note: if the content is empty, atoi will return 0
      progress = atoi( oss.str().c_str() );

      fin.close();
    }
    else {

      // This should never happen. Report job abortion.
      progress = -2;
    }
  }

  return progress;
}

/************************/
/**** updateProgress ****/
bool 
updateProgress( const string & progFileName, const char * content )
{
  FILE *fd;

  if ( fileExists( progFileName.c_str() ) ) {

    fd = fopen( progFileName.c_str(), "w" );
  }
  else {

    fd = fopen( progFileName.c_str(), "a" );
  }

  if ( fd == NULL ) {

    printf("Could not create/open progress file %s\n", progFileName.c_str());
    return false;
  }

  if ( fputs( content, fd ) < 0 ) {

    printf("Could not write to progress file %s\n", progFileName.c_str());
    return false;
  }

  fclose( fd );

  return true;
}

/********************/
/**** getTickets ****/
vector<string>
getTickets( const string & concatenatedValues )
{
  vector<string> tickets;

  // Split tokens and process each one
  stringstream ss( concatenatedValues );
  string ticket;
  while ( getline( ss, ticket, ',' ) ) {

    tickets.push_back( ticket );
  }

  return tickets;
}


/********************/
/**** getTickets ****/
map<string, string>
getTicketsWithTask( const string & concatenatedValues )
{
  map<string, string> tickets;

  // Split tokens and process each one
  stringstream ss( concatenatedValues );
  string task_ticket;
  while ( getline( ss, task_ticket, ',' ) ) {

    size_t pos = task_ticket.find( "_" );

    if ( pos != string::npos ) {

      string task = task_ticket.substr( 0, pos );
      string ticket = task_ticket.substr( pos+1, 6 );

      tickets.insert( pair< string, string >( ticket, task ) );
    }
  }

  return tickets;
}

/********************/
/**** createFile ****/
bool 
createFile( const string & fileName )
{
  FILE *fd = fopen( fileName.c_str(), "w" );

  if ( fd == NULL ) {

    printf("Could not create file %s\n", fileName.c_str());
    return false;
  }

  fclose( fd );

  return true;
}


/**************************/
/**** cancelExperiment ****/
void
cancelExperiment( const string & exp_metadata_file, const string & exp_prog_file, const string & exp_done_file, const string & ticket_dir, const string & job_ticket, FILE * fd_log )
{
  fputs( "Cancelling experiment\n", fd_log );

  // Are there other jobs to cancel?

  // Get all jobs in the experiment
  FileParser exp_data( exp_metadata_file );

  string all_jobs = exp_data.get( "JOBS" );

  vector<string> all_tickets = getTickets( all_jobs );

  for ( vector<string>::iterator at = all_tickets.begin(); at != all_tickets.end(); ++at ) {

    if ( job_ticket.compare(0, 6, (*at)) == 0 ) {

      continue; // Skip current job!
    }

    // Read job metadata to get type
    string metadata_file = ticket_dir + OMWS_JOB_METADATA_PREFIX + (*at);

    FileParser data( metadata_file );

    string type = data.get("TYPE");

    // Build pending request file name
    string pending_file = ticket_dir + type + _PENDING_REQUEST + (*at);

    // If job is pending, cancel it, otherwise let it finish behind the scenes
    if ( fileExists( pending_file.c_str() ) ) {

      string prog_file = ticket_dir + OMWS_JOB_PROGRESS_PREFIX + (*at);

      updateProgress( prog_file, "-3" );
      string done_file = ticket_dir + OMWS_JOB_DONE_PREFIX + (*at);
      createFile( done_file );
    }
  }

  // Cancel experiment
  updateProgress( exp_prog_file, "-2" );
  createFile( exp_done_file );
}


/*********************/
/**** releaseSoap ****/
void 
releaseSoap(struct soap *ctx1, struct soap *ctx2)
{
  soap_destroy(ctx1); // remove deserialized class instances (C++ objects)
  soap_end(ctx1);     // clean up and remove deserialized data
  soap_done(ctx1);    // detach context (last use and no longer in scope)

  soap_destroy(ctx2);
  soap_end(ctx2);
  soap_done(ctx2);
}
