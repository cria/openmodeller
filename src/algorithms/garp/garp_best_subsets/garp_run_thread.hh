/**
 * Definition of preprocessor symbols for thread management in GARPBS
 * 
 * @file   garp_run_thread.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-08-25
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 *
 * http://www.nhm.ku.edu
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
 * 
 */


#ifndef _GARP_RUN_THREAD_HH_
#define _GARP_RUN_THREAD_HH_

#ifdef _WINDOWS

// windows threading
#define THREAD_PROC_RETURN_TYPE        void
#define THREAD_PROC_RETURN_STATEMENT   return;

#define THREAD_START(threadProc, threadData) \
 _beginthread(threadProc, 0, (void *) threadData);

#define THREAD_END() _endthread()

#else

// posix threading (linux/unix/cygwin)
#define THREAD_PROC_RETURN_TYPE        void *
#define THREAD_PROC_RETURN_STATEMENT   return (NULL);

#define THREAD_START(threadProc, threadData) \
 pthread_t pthread; \
 pthread_create(&pthread, NULL threadProc, threadData);

#define THREAD_END() _endthread() \
 int status; \
 pthread_exit(&status);


#endif // _WINDOWS

#endif // _GARP_RUN_THREAD_HH_
