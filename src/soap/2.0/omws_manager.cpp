/**
 * openModeller Web service manager for experiments.
 * 
 * @author Renato De Giovanni (renato [at] cria [dot] org [dot] br)
 * $Id$
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

#include "om.nsmap"
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
#include <string.h>
#include <sstream>
using namespace std;

#include "omws_utils.hh"

int main(int argc, char **argv)
{
  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "v", "version", "Display version info"    , false );
  opts.addOption( "c", "config" , "OMWS configuration file" , true );
  opts.addOption( "t", "ticket" , "Job ticket"              , true );
  // The following option was included so that omws_manager can be used by HTCondor. If
  // the new request file is created as _req, it may be incorrectly processed as an
  // individual job, rather than part of an experiment. So this option allows the
  // new request to be directly written as _proc.
  opts.addOption( "s", "skip-request", "Write new request in proc instead of req file", false );
  // The following option was included so that omws_manager can be used by HTCondor.
  // It tells the program to create the done.ticket file in the end, instead of relying on 
  // an external command.
  opts.addOption( "d", "create-done", "Write done.ticket file in the end", false );

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] ); 
    exit(0);
  }

  OpenModeller om;

  std::string config_file;
  std::string job_ticket;

  bool skip_request = false;
  bool create_done = false;

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
      case 3:
        skip_request = true;
        break;
      case 4:
        create_done = true;
        break;
      default:
        break;
    }
  }

  // Check parameters
  if ( config_file.empty() ) {

    printf( "Please specify the OMWS configuration file\n");
    exit(1);
  }

  if ( job_ticket.empty() ) {

    printf( "Please specify a job ticket that finished\n");
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

  printf("Processing job %s\n", job_ticket.c_str());

  string exp_file = ticket_dir + exp_ticket;

  // Lock experiment file, which is also the log file
  struct flock fl;
  fl.l_type   = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start  = 0;
  fl.l_len    = 0;
  fl.l_pid    = getpid();

  FILE *fd_log = fopen( exp_file.c_str(), "a" );

  fcntl( fileno(fd_log), F_SETLKW, &fl ); // Wait for lock if necessary

  printf("Acquired lock\n");

  // Open experiment log file

  bool start = true;
  string msg;

  if ( fd_log == NULL ) {

    printf("Cannot open experiment log file\n");
    start = false;
  }
  else {

    msg = "\nManaging workflow for job " + job_ticket + "\n\n";
    printf( "%s", msg.c_str() );
    if ( fputs( msg.c_str(), fd_log ) < 0 ) {

      printf("Could not write to experiment log file %s\n", exp_file.c_str());
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
      logMessage( "Experiment already finished by other job. Aborting.", fd_log );
      break;
    }

    logMessage( "Checked experiment status.", fd_log );

    // Read job progress
    int progress = -1;

    try {

      progress = getProgress( ticket_dir, job_ticket, false );
    }
    catch (OmwsException& e) {

      logMessage( "Cannot read job progress data. Aborting.", fd_log );
      break;
    }

    if ( progress == 100 ) {

      // Job finished successfully!
      logMessage( "Job finished successfuly!", fd_log );

      // Are there other jobs to be triggered?

      if ( next_jobs.empty() ) {

        // There are no subsequent jobs to be triggered
        logMessage( "No subsequent jobs.", fd_log );

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

            logMessage( "Another job (" + (*at) + ") is still pending.", fd_log );
            finish_experiment = false;
            break;
          }
        }

        // Only finish experiment if all jobs finished OK
        if ( finish_experiment ) {      

          logMessage( "All jobs finished OK. Finishing experiment.", fd_log );
          updateProgress( exp_prog_file, "100" );
          createFile( exp_done_file );
        }
      }
      else {

        // There are subsequent jobs to be triggered
        logMessage( msg = "Checking next jobs...", fd_log );

        // Check if they can be started
        vector<string> next_tickets = getTickets( next_jobs );

        // For each of the next jobs
        for ( vector<string>::iterator nt = next_tickets.begin(); nt != next_tickets.end(); ++nt ) {

          logMessage( "Ticket " + (*nt), fd_log );

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

              logMessage( "Dependent job " + (*dt).first + " still pending.", fd_log );
              can_start = false;
              break;
            }
          }

          if ( can_start ) {

            logMessage( "All dependencies met! Starting job.", fd_log );

            string type = data.get("TYPE");

            // gSOAP context to parse next jobs XML
            struct soap *ctx = newSoapContext();
            soap_set_omode(ctx, SOAP_XML_CANONICAL);
            soap_set_omode(ctx, SOAP_XML_INDENT);
            soap_set_omode(ctx, SOAP_XML_TREE);
            soap_set_omode(ctx, SOAP_XML_NOTYPE);

            // Pending jobs can only be of type model, test or proj

            ///////////////////////// MODEL ///////////////////////////
            ///////////////////////////////////////////////////////////
            if ( strcmp(type.c_str(), OMWS_MODEL) == 0 ) {

              logMessage( "Pending job is a model.", fd_log );

              string pend_file = ticket_dir + OMWS_MODEL + _PENDING_REQUEST + (*nt);

              // This file must exist!
              if ( ! fileExists( pend_file.c_str() ) ) {

                logMessage( "File does not exist: " + pend_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              ifstream fs_in( pend_file.c_str() );
              ctx->is = &fs_in;

              om::_om__ModelParameters mp;

              // Parsing must succeed! (file was created using the same lib)
              if ( soap_read_om__ModelParametersType( ctx, &mp ) != SOAP_OK ) {

                logMessage( "Could not deserialize: " + pend_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              // Get result from each dependency
              for ( dt = dep_tickets.begin(); dt != dep_tickets.end(); ++dt ) {

                string dep_ticket = (*dt).first;
                string dep_place = (*dt).second;

                if ( strcmp(dep_place.c_str(), "presence") == 0 ) {

                  string result_file = ticket_dir + OMWS_SAMPLING + _RESPONSE + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  om::_om__Sampler samp;

                  struct soap *ctx1 = newSoapContext();

                  ifstream fs1( result_file.c_str() );
                  ctx1->is = &fs1;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SamplerType( ctx1, &samp ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx1);
                    releaseSoap(ctx);
                    break;
                  }

                  mp.Sampler->Presence = samp.Presence;
                }
                else if ( strcmp(dep_place.c_str(), "absence") == 0 ) {

                  string result_file = ticket_dir + OMWS_SAMPLING + _RESPONSE + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  om::_om__Sampler samp;

                  struct soap *ctx2 = newSoapContext();

                  ifstream fs2( result_file.c_str() );
                  ctx2->is = &fs2;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SamplerType( ctx2, &samp ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx2);
                    releaseSoap(ctx);
                    break;
                  }

                  mp.Sampler->Absence = samp.Absence;
                }
                else {

                  logMessage( "Ignoring unknown dependency type: " + dep_place, fd_log );
                }
              }

              // Write request to file
              string req_file;

              if ( skip_request ) {

                req_file = ticket_dir + OMWS_MODEL + _PROCESSED_REQUEST + (*nt);
              }
              else {

                req_file = ticket_dir + OMWS_MODEL + _REQUEST + (*nt);
              }

              ofstream fs_out( req_file.c_str() );
              ctx->os = &fs_out;

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__ModelParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              if ( ( mp.soap_serialize(ctx), soap_begin_send(ctx) || mp.soap_put(ctx, "om:ModelParameters", NULL) || soap_end_send(ctx), ctx->error ) != SOAP_OK ) {

                logMessage( "Failed to serialize new job request: " + req_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }
            }
            ///////////////////////// TEST ////////////////////////////
            ///////////////////////////////////////////////////////////
            else if ( strcmp(type.c_str(), OMWS_TEST) == 0 ) {

              logMessage( "Pending job is a test.", fd_log );

              string pend_file = ticket_dir + OMWS_TEST + _PENDING_REQUEST + (*nt);

              // This file must exist!
              if ( ! fileExists( pend_file.c_str() ) ) {

                logMessage( "File does not exist: " + pend_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              ifstream fs_in( pend_file.c_str() );
              ctx->is = &fs_in;

              om::_om__TestParameters tp;

              // Parsing must succeed! (file was created using the same lib)
              if ( soap_read_om__TestParametersType( ctx, &tp ) != SOAP_OK ) {

                logMessage( "Could not deserialize: " + pend_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              logMessage( "Completed pending job deserialization.", fd_log );

              // Get result from each dependency
              for ( dt = dep_tickets.begin(); dt != dep_tickets.end(); ++dt ) {

                string dep_ticket = (*dt).first;
                string dep_place = (*dt).second;

                if ( strcmp(dep_place.c_str(), "presence") == 0 ) {

                  logMessage( "Getting presence from previous job.", fd_log );

                  string result_file = ticket_dir + OMWS_SAMPLING + _RESPONSE + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  om::_om__Sampler samp;

                  struct soap *ctx1 = newSoapContext();

                  ifstream fs1( result_file.c_str() );
                  ctx1->is = &fs1;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SamplerType( ctx1, &samp ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx1);
                    releaseSoap(ctx);
                    break;
                  }

                  tp.Sampler->Presence = samp.Presence;
                }
                else if ( strcmp(dep_place.c_str(), "absence") == 0 ) {

                  logMessage( "Getting absence from previous job.", fd_log );

                  string result_file = ticket_dir + OMWS_SAMPLING + _RESPONSE + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  om::_om__Sampler samp;

                  struct soap *ctx2 = newSoapContext();

                  ifstream fs2( result_file.c_str() );
                  ctx2->is = &fs2;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SamplerType( ctx2, &samp ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx2);
                    releaseSoap(ctx);
                    break;
                  }

                  tp.Sampler->Absence = samp.Absence;
                }
                else if ( strcmp(dep_place.c_str(), "model") == 0 ) {

                  string result_file = ticket_dir + OMWS_MODEL + _RESPONSE + dep_ticket;

                  logMessage( "Getting model from previous job in " + result_file, fd_log );

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  om::om__SerializedModelType model;

                  struct soap *ctx3 = newSoapContext();

                  ifstream fs3( result_file.c_str() );
                  ctx3->is = &fs3;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SerializedModelType( ctx3, &model ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx3);
                    releaseSoap(ctx);
                    break;
                  }

                  logMessage( "Managed to deserialize previous job.", fd_log );

                  tp.Algorithm = model.Algorithm;
                }
                else if ( strcmp(dep_place.c_str(), "lpt") == 0 ) {

                  string result_file = ticket_dir + OMWS_EVALUATE + _RESPONSE + dep_ticket;

                  logMessage( "Getting LPT from previous job in " + result_file, fd_log );

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  om::_om__ModelEvaluation eval;

                  struct soap *ctx1 = newSoapContext();

                  ostringstream eval_oss;
                  readFile( result_file.c_str(), eval_oss );

                  string content( "<ModelEvaluation>" );
                  content.append( eval_oss.str() ).append("</ModelEvaluation>");
                  istringstream iss( content );

                  ctx1->is = &iss;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__ModelEvaluationResultType( ctx1, &eval ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx1);
                    releaseSoap(ctx);
                    break;
                  }

                  // Determine LPT
                  string lpt = "1";
                  double d_lpt = 1.0;
                  stringstream values( eval.Values.V, ios::in );

                  string buf;
                  while ( values >> buf ) {

                    double temp = ::atof( buf.c_str() );

                    if ( temp < d_lpt && temp > 0.0000000000001 ) {

                      d_lpt = temp;
                      lpt = string( buf );
                    }
                  }

                  logMessage( "Determined LPT: " + lpt, fd_log );

                  // Replace last decimal with smaller digit
                  size_t dot_pos = lpt.find( "." );

                  if ( dot_pos != string::npos ) {

                    size_t num_decimals = lpt.size() -dot_pos -1;

                    if ( num_decimals > 3 ) {

                      d_lpt = d_lpt - (1.0/pow(10.0,(double)num_decimals));

                      ostringstream lpt_oss;
                      lpt_oss << d_lpt;

                      lpt = string( lpt_oss.str() );

                      logMessage( "Adjusted LPT to: " + lpt, fd_log );
                    }
                  }

                  tp.Statistics->ConfusionMatrix->Threshold = new string( lpt );
                }
                else {

                  logMessage( "Ignoring unknown dependency type: " + dep_place, fd_log );
                }
              }

              // Write request to file
              logMessage( "Writing new request file.", fd_log );

              string req_file;

              if ( skip_request ) {

                req_file = ticket_dir + OMWS_TEST + _PROCESSED_REQUEST + (*nt);
              }
              else {

                req_file = ticket_dir + OMWS_TEST + _REQUEST + (*nt);
              }

              ofstream fs_out( req_file.c_str() );
              ctx->os = &fs_out;

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__TestParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              if ( ( tp.soap_serialize(ctx), soap_begin_send(ctx) || tp.soap_put(ctx, "om:TestParameters", NULL) || soap_end_send(ctx), ctx->error ) != SOAP_OK ) {

                logMessage( "Failed to serialize new job request: " + req_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              logMessage( "Done.", fd_log );
            }
            ////////////////////// PROJECTION /////////////////////////
            ///////////////////////////////////////////////////////////
            else if ( strcmp(type.c_str(), "proj") == 0 ) {

              logMessage( "Pending job is a projection.", fd_log );

              string pend_file = ticket_dir + OMWS_PROJECTION + _PENDING_REQUEST + (*nt);

              // This file must exist!
              if ( ! fileExists( pend_file.c_str() ) ) {

                logMessage( "File does not exist: " + pend_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              ifstream fs_in( pend_file.c_str() );
              ctx->is = &fs_in;

              om::_om__ProjectionParameters pp;

              // Parsing must succeed! (file was created using the same lib)
              if ( soap_read_om__ProjectionParametersType( ctx, &pp ) != SOAP_OK ) {

                logMessage( "Could not deserialize: " + pend_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              // Get result from each dependency
              for ( dt = dep_tickets.begin(); dt != dep_tickets.end(); ++dt ) {

                string dep_ticket = (*dt).first;
                string dep_place = (*dt).second;

                if ( strcmp(dep_place.c_str(), "model") == 0 ) {

                  string result_file = ticket_dir + OMWS_MODEL + _RESPONSE + dep_ticket;

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  om::om__SerializedModelType model;

                  struct soap *ctx1 = newSoapContext();

                  ifstream fs1( result_file.c_str() );
                  ctx1->is = &fs1;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SerializedModelType( ctx1, &model ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx1);
                    releaseSoap(ctx);
                    break;
                  }

                  pp.Algorithm = model.Algorithm;
                }
                else if ( strcmp(dep_place.c_str(), "lpt") == 0 ) {

                  string result_file = ticket_dir + OMWS_EVALUATE + _RESPONSE + dep_ticket;

                  logMessage( "Getting LPT from previous job in " + result_file, fd_log );

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  om::_om__ModelEvaluation eval;

                  struct soap *ctx1 = newSoapContext();

                  ostringstream eval_oss;
                  readFile( result_file.c_str(), eval_oss );

                  string content( "<ModelEvaluation>" );
                  content.append( eval_oss.str() ).append("</ModelEvaluation>");
                  istringstream iss( content );

                  ctx1->is = &iss;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__ModelEvaluationResultType( ctx1, &eval ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx1);
                    releaseSoap(ctx);
                    break;
                  }

                  // Determine LPT
                  string lpt = "1";
                  double d_lpt = 1.0;
                  stringstream values( eval.Values.V, ios::in );

                  string buf;
                  while ( values >> buf ) {

                    double temp = ::atof( buf.c_str() );

                    if ( temp < d_lpt && temp > 0.0000000000001 ) {

                      d_lpt = temp;
                      lpt = string( buf );
                    }
                  }

                  logMessage( "Determined LPT: " + lpt, fd_log );

                  // Replace last decimal with smaller digit
                  size_t dot_pos = lpt.find( "." );

                  if ( dot_pos != string::npos ) {

                    size_t num_decimals = lpt.size() -dot_pos -1;

                    if ( num_decimals > 3 ) {

                      d_lpt = d_lpt - (1.0/pow(10.0,(double)num_decimals));

                      ostringstream lpt_oss;
                      lpt_oss << d_lpt;

                      lpt = string( lpt_oss.str() );

                      logMessage( "Adjusted LPT to: " + lpt, fd_log );
                    }
                  }

                  pp.Statistics->AreaStatistics->PredictionThreshold = lpt.c_str();
                }
                else {

                  logMessage( "Ignoring unknown dependency type: " + dep_place, fd_log );
                }
              }

              // Write request to file
              string req_file;

              if ( skip_request ) {

                req_file = ticket_dir + OMWS_PROJECTION + _PROCESSED_REQUEST + (*nt);
              }
              else {

                req_file = ticket_dir + OMWS_PROJECTION + _REQUEST + (*nt);
              }

              ofstream fs_out( req_file.c_str() );
              ctx->os = &fs_out;

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__ProjectionParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              if ( ( pp.soap_serialize(ctx), soap_begin_send(ctx) || pp.soap_put(ctx, "om:ProjectionParameters", NULL) || soap_end_send(ctx), ctx->error ) != SOAP_OK ) {

                logMessage( "Failed to serialize new job request: " + req_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }
            }
            ////////////////////// EVALUATION /////////////////////////
            ///////////////////////////////////////////////////////////
            else if ( strcmp(type.c_str(), OMWS_EVALUATE) == 0 ) {

              logMessage( "Pending job is an evaluation.", fd_log );

              string pend_file = ticket_dir + OMWS_EVALUATE + _PENDING_REQUEST + (*nt);

              // This file must exist!
              if ( ! fileExists( pend_file.c_str() ) ) {

                logMessage( "File does not exist: " + pend_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              ifstream fs_in( pend_file.c_str() );
              ctx->is = &fs_in;

              om::_om__ModelEvaluationParameters me;

              // Parsing must succeed! (file was created using the same lib)
              if ( soap_read_om__ModelEvaluationParametersType( ctx, &me ) != SOAP_OK ) {

                logMessage( "Could not deserialize: " + pend_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              logMessage( "Completed pending job deserialization.", fd_log );

              // Get result from each dependency
              for ( dt = dep_tickets.begin(); dt != dep_tickets.end(); ++dt ) {

                string dep_ticket = (*dt).first;
                string dep_place = (*dt).second;

                if ( strcmp(dep_place.c_str(), "model") == 0 ) {

                  string result_file = ticket_dir + OMWS_MODEL + _RESPONSE + dep_ticket;

                  logMessage( "Getting model from previous job in " + result_file, fd_log );

                  // This file must exist!
                  if ( ! fileExists( result_file.c_str() ) ) {

                    logMessage( "File does not exist: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx);
                    break;
                  }

                  // Get model
                  om::om__SerializedModelType model;

                  struct soap *ctx3 = newSoapContext();

                  ifstream fs3( result_file.c_str() );
                  ctx3->is = &fs3;

                  // Parsing must succeed! (job is expected to be finished successfully)
                  if ( soap_read_om__SerializedModelType( ctx3, &model ) != SOAP_OK ) {

                    logMessage( "Could not deserialize: " + result_file, fd_log );
                    stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                    releaseSoap(ctx3);
                    releaseSoap(ctx);
                    break;
                  }

                  logMessage( "Managed to deserialize previous job.", fd_log );

                  me.Algorithm = model.Algorithm;

                  // Get training points
                  me.Sampler->Environment = model.Sampler->Environment;
                  me.Sampler->Presence = model.Sampler->Presence;
                }
                else {

                  logMessage( "Ignoring unknown dependency type: " + dep_place, fd_log );
                }
              }

              // Write request to file
              logMessage( "Writing new request file.", fd_log );

              string req_file;

              if ( skip_request ) {

                req_file = ticket_dir + OMWS_EVALUATE + _PROCESSED_REQUEST + (*nt);
              }
              else {

                req_file = ticket_dir + OMWS_EVALUATE + _REQUEST + (*nt);
              }

              ofstream fs_out( req_file.c_str() );
              ctx->os = &fs_out;

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__ModelEvaluationParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              if ( ( me.soap_serialize(ctx), soap_begin_send(ctx) || me.soap_put(ctx, "om:ModelEvaluationParameters", NULL) || soap_end_send(ctx), ctx->error ) != SOAP_OK ) {

                logMessage( "Failed to serialize new job request: " + req_file, fd_log );
                stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
                releaseSoap(ctx);
                break;
              }

              logMessage( "Done.", fd_log );
            }

            releaseSoap(ctx);
          }
        }
      }
    }
    else {

      // Job didn't finish successfully, so cancel the experiment and all pending jobs
      ostringstream oss;
      oss << "Job didn't finish successfully (" << progress << ").";
      logMessage( oss.str(), fd_log );
      stopExperiment( ticket_dir, exp_ticket, job_ticket, fd_log, "-2" );
    }

    break;
  }

  if ( create_done ) {

    string job_done_file = ticket_dir + OMWS_JOB_DONE_PREFIX + job_ticket;
    createFile( job_done_file );
  }

  logMessage( "Finished management trigger.", fd_log );
  fclose( fd_log );

  // Unlock experiment file
  fl.l_type = F_UNLCK;
  fcntl( fileno(fd_log), F_SETLK, &fl );

  return 0; 
}
