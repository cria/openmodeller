/**
 * openModeller Web service utils library.
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

#ifndef OMWS_UTILS_H
#define OMWS_UTILS_H

#include <stdexcept>
#include <string.h>
#include <map>
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
#define OMWS_PROJECTION_STATISTICS_PREFIX "stats."
#define OMWS_JOB_PROGRESS_PREFIX "prog."
#define OMWS_JOB_DONE_PREFIX "done."
#define OMWS_JOB_METADATA_PREFIX "job."

/******************/
/*** Exceptions ***/

class OmwsException : public std::runtime_error {
public:
  OmwsException( const std::string& msg ) :
    std::runtime_error( msg )
  {}

};

/*****************/
/*** Functions ***/

string getTicketFilePath( string dir, string prefix, string ticket );

bool fileExists( const char* fileName );

void logMessage( const string & msg, FILE * fdLog );

int getProgress( const string & ticketDir, const string & ticket, bool checkDoneFile=true );

bool updateProgress( const string & progFileName, const char * content );

vector<string> getTickets( const string & concatenatedValues );

map<string, string> getTicketsWithTask( const string & concatenatedValues );

void readFile( const char* filePath, ostringstream & oss );

bool createFile( const string & fileName );

void renameJobFile(  const string & jobFullPath, const char * target, const char * replacement );

bool cancelJob( const string & ticketDir, const string & ticket );

bool stopExperiment( const string & ticketDir, const string & expTicket, const string & jobTicketException, FILE * fdLog, const char* newStatus="-3" );

struct soap * newSoapContext();

void releaseSoap(struct soap *ctx);

#endif
