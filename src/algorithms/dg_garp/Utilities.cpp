/* **************************************
 *  GARP Modeling Package
 *
 * **************************************
 *
 * Copyright (c), The Center for Research, University of Kansas, 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright (C), David R.B. Stockwell of Symbiotik Pty. Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license that is distributed with the software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * license.txt file included with this software for more details.
 */

// Utilities.cpp

unsigned long Seed;						// seed for random number generator
unsigned long OrigSeed;					// original value for random seed

/**
 * Returns the first element child that matches the given name. Includes
 * the parent in search. Remember that XML names are case-sensitive.
 *
 * @return the first element that matches the given name, NULL if not
 * found.
 */

#include <string.h>

#ifdef __GARPWIN
#include <io.h>
#else
#include <sys/stat.h>
#endif 

int getFileSize(char * strFilename)
{
#ifdef __GARPWIN

  _finddata_t fileInfo;
  _findfirst(strFilename, &fileInfo);
  return (int) fileInfo.size;

#else

  struct stat stat_p;
  stat (strFilename, &stat_p);
  return stat_p.st_size;

#endif
}
