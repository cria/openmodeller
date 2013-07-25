/**
 * openModeller Web service utils library.
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

#include "omH.h"

#include <openmodeller/FileParser.hh>

#include <fstream>
#include <sstream>
#include <string.h>
using namespace std;

#include "omws_utils.hh"

/***************************/
/**** getTicketFilePath ****/
string
getTicketFilePath( string dir, string prefix, string ticket )
{
  string filePath( dir );

  // Append slash if necessary
  if ( filePath.find_last_of( "/" ) != filePath.size() - 1 ) {

    filePath.append( "/" );
  }

  filePath.append( prefix );
  filePath.append( ticket );

  return filePath;
}

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

/********************/
/**** logMessage ****/
void 
logMessage( const string & msg, FILE * fdLog )
{
  printf( "%s\n", msg.c_str() );
  fputs( msg.c_str(), fdLog );
  fputs( "\n", fdLog );
  fflush( fdLog );
}

/*********************/
/**** getProgress ****/
int 
getProgress( const string & ticketDir, const string & ticket, bool checkDoneFile )
{
  string job_prog_file = getTicketFilePath( ticketDir, OMWS_JOB_PROGRESS_PREFIX, ticket );

  int progress = -1;

  if ( fileExists( job_prog_file.c_str() ) ) {

    // If file exists, get its content
    fstream fin;
    fin.open( job_prog_file.c_str(), ios::in );

    if ( fin.is_open() ) {

      // If file was opened, read the content
      ostringstream oss;
      string line;

      getline( fin, line );
      oss << line << endl;

      // Note: if the content is empty, atoi will return 0
      progress = atoi( oss.str().c_str() );

      // Make sure that everything is really done before returning 100%
      if ( progress == 100 ) {

        if ( checkDoneFile ) {

          // Finished flag
          string done_file = getTicketFilePath( ticketDir, OMWS_JOB_DONE_PREFIX, ticket );

          if ( ! fileExists( done_file.c_str() ) ) {

            progress = 99;
          }
        }
      }

      fin.close();
    }
    else {

      // This should never happen!
      throw OmwsException("Failed to read ticket data");
    }
  }
  else {

    // Is this an experiment?
    bool not_experiment = true;

    string metadata_file = getTicketFilePath( ticketDir, OMWS_JOB_METADATA_PREFIX, ticket );
    if ( fileExists( metadata_file.c_str() ) ) {

      FileParser metadata( metadata_file );
      string job_type = metadata.get("TYPE");

      if ( strcmp(job_type.c_str(), OMWS_EXPERIMENT) == 0 ) {

        not_experiment = false;

        string all_jobs = metadata.get( "JOBS" );
        vector<string> all_tickets = getTickets( all_jobs );

        int exp_progress = 0;
        bool all_queued = true;

        for ( vector<string>::iterator at = all_tickets.begin(); at != all_tickets.end(); ++at ) {

          string subjob_ticket = (*at);

          int job_progress = getProgress( ticketDir, subjob_ticket );

          if ( job_progress < -1 ) {

            all_queued = false;
            exp_progress = job_progress;
            break;
          }
          else if ( job_progress > -1 ) {

            all_queued = false;
            exp_progress += job_progress;
          }
        }

        if ( all_queued ) {

          exp_progress = -1;
        }
        else if ( exp_progress > -1 ) {

	    exp_progress /= all_tickets.size();
        }

        progress = exp_progress;
      }
    }

    if ( not_experiment ) {

      string ticket_file = getTicketFilePath( ticketDir, "", ticket );

      if ( ! fileExists( ticket_file.c_str() ) ) {

        progress = -4; // non-existing job
      }
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

/***********************/
/**** renameJobFile ****/
void
renameJobFile( const string & jobFullPath, const char * target, const char * replacement )
{
  string sub( target );

  size_t pos = jobFullPath.rfind( target );

  if ( pos != string::npos ) {

    string newname( jobFullPath );
    newname.replace( pos, sub.size(), replacement );

    if ( rename( jobFullPath.c_str() , newname.c_str() ) != 0 ) {

      throw OmwsException("Failed to update job status (2)");
    }
  }
  else {

    throw OmwsException("Failed to update job status (1)");
  }
}

/*******************/
/**** cancelJob ****/
bool cancelJob( const string & ticketDir, const string & ticket )
{
  // Read job metadata to get type
  string metadata_file = ticketDir + OMWS_JOB_METADATA_PREFIX + ticket;

  FileParser data( metadata_file );

  string type = data.get("TYPE");

  if ( type.compare(0, 3, OMWS_EXPERIMENT) == 0 ) {

    return stopExperiment( ticketDir, ticket, "", 0, "-3" );
  }

  // If job is pending, cancel it, otherwise let it finish behind the scenes
  if ( getProgress( ticketDir, ticket, false ) == -1 ) {

    string req_file = ticketDir + type + _REQUEST + ticket;

    try {

      if ( fileExists( req_file.c_str() ) ) {

        renameJobFile( req_file, _REQUEST, _PROCESSED_REQUEST );
      }

      string prog_file = ticketDir + OMWS_JOB_PROGRESS_PREFIX + ticket;

      updateProgress( prog_file, "-3" );
      string done_file = ticketDir + OMWS_JOB_DONE_PREFIX + ticket;
      createFile( done_file );

      return true;
    }
    catch (OmwsException& e) {

      printf( "Failed to cancel job: %s\n", e.what() );
      return false;
    }
  }

  return false;
}

/************************/
/**** stopExperiment ****/
bool
stopExperiment( const string & ticketDir, const string & expTicket, const string & jobTicketException, FILE * fdLog, const char* newStatus )
{
  string done_file = ticketDir + OMWS_JOB_DONE_PREFIX + expTicket;

  if ( fileExists( done_file.c_str() ) ) {

    return false;
  }

  if ( fdLog ) {

    fputs( "Stopping experiment\n", fdLog );
    fflush( fdLog );
  }

  // Are there other jobs to cancel?

  // Get all jobs in the experiment
  FileParser exp_data( ticketDir + OMWS_JOB_METADATA_PREFIX + expTicket );

  string all_jobs = exp_data.get( "JOBS" );

  vector<string> all_tickets = getTickets( all_jobs );

  bool cancelled = false;

  for ( vector<string>::iterator at = all_tickets.begin(); at != all_tickets.end(); ++at ) {

    if ( jobTicketException.size() == 6 && jobTicketException.compare(0, 6, (*at)) == 0 ) {

      continue; // Skip provided job!
    }

    if ( cancelJob( ticketDir, (*at) ) ) {

      cancelled = true;
    }
  }

  if ( cancelled ) {

    // Cancel experiment
    updateProgress( ticketDir + OMWS_JOB_PROGRESS_PREFIX + expTicket, newStatus );
    createFile( done_file );
    return true;
  }

  return false;
}

/************************/
/**** newSoapContext ****/
struct soap * 
newSoapContext()
{
  struct soap *ctx = soap_new1(SOAP_XML_STRICT);
  soap_init(ctx);
  soap_imode(ctx, SOAP_ENC_XML); // Set input mode
  soap_imode(ctx, SOAP_XML_IGNORENS);
  soap_begin(ctx); // start new (de)serialization phase

  return ctx;
}

/*********************/
/**** releaseSoap ****/
void 
releaseSoap(struct soap *ctx)
{
  soap_destroy(ctx); // remove deserialized class instances (C++ objects)
  soap_end(ctx);     // clean up and remove deserialized data
  soap_done(ctx);    // detach context (last use and no longer in scope)
}
