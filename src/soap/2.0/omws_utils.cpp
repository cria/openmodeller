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
logMessage( const string & msg, FILE * fd_log )
{
  printf( "%s\n", msg.c_str() );
  fputs( msg.c_str(), fd_log );
  fputs( "\n", fd_log );
  fflush( fd_log );
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
