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

// Utilities.h: interface for the GarpUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <openmodeller/Exceptions.hh>

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
using std::string;

#ifndef __GARPWIN
typedef unsigned char BYTE;
#else
#include <windows.h>
#endif

// Comparison of two performance values 
#define Maxflag 1
#define BETTER(X,Y)     (Maxflag ? (X) > (Y) : (X) < (Y))

const int CHAR_ARRAY_SIZE = 256;
const int FILE_CHUNK = 1024;

const double EPS = 0.000001;
const int MASK = 2147483647;
const int PRIME = 65539;
const double SCALE = 0.4656612875e-9;

const int MAX_ENV_LAYERS      = 256;
const int MAX_PRESENCE_POINTS = 100000;
const int MAX_RULES           = 256;

const int MIN_SCALED_VALUE   = 1;
const int MAX_SCALED_VALUE   = 254;
const int SCALED_VALUE_RANGE = MAX_SCALED_VALUE - MIN_SCALED_VALUE;

const int MAX_MUTATION_TEMPERATURE = (int) SCALED_VALUE_RANGE;

const int MIN_SIG_NO = 4;

const BYTE MASK_VALUE    = 255;
const BYTE MISSING_VALUE = 254;
const BYTE PRESENCE      =   1;
const BYTE ABSENCE       =   0;

// used in random number generator below
extern unsigned long Seed;     // seed for random number generator
extern unsigned long OrigSeed; // original value for random seed

// macros for bitmap header generation
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

#define MAX_BYTE  255

// these defines are already present in windef.h
#ifndef WIN32
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

/*
 * Generic definitions like this should be avoided as, at any time, may
 * conflict with some system or other included header.
 */
#if defined(WIN32) || defined(__APPLE__)
#define MAX(a, b)           ( (a > b)? a : b )
#define MIN(a, b)           ( (a < b)? a : b )
#endif

int getFileSize(char * strFilename);

// ===========================================================================
//  Declaration and Implementation of class GarpException
// ===========================================================================
class GarpException : public AlgorithmException 
{
public:

  GarpException(int code, string message) :
     AlgorithmException( message ),
     intCode( code ),
     strMessage( message)
  { }

  ~GarpException() throw() {}

  void getError(int& code, string& message)
  {
    code = intCode;
    message = strMessage;
  }

private:

  int intCode;
  string strMessage;
};

// ===========================================================================
// Declaration of class GarpUtil
// ===========================================================================
class GarpUtil
{

public:
	GarpUtil() {};

	// ------------------------------------------------
	static void randomize(unsigned long iOrigSeedProvided = 0)
	{
		time_t clock;

		if (iOrigSeedProvided == 0)
		{
			time(&clock);
			OrigSeed = (unsigned long)clock;
		}
		else
		{
			OrigSeed = iOrigSeedProvided;
		}

		Seed = OrigSeed;
	}

	// ==================================================
	//  Method:		bool between(x, y, z)
	// --------------------------------------------------
	//  Purpose: Check if a number is in between two others
	//  Output:  Return 1 if x is between y and z
	//		     Return 0 otherwise
	// --------------------------------------------------
	static bool between(double x, double y, double z)
	{
		if (y < z && x > y && x < z) 
			return true;
		
		else if (y > z && x < y && x > z) 
			return true;

		else 
			return false;
	}

	// ==================================================
	//  Method:		bool notBetween(x, y, z)
	// --------------------------------------------------
	//  Purpose: Check if a number is in not between two others
	//  Output:  Return 1 if x is between y and z
	//		     Return 0 otherwise
	// --------------------------------------------------
	inline static int notBetween(double x, double y, double z)
	{
		if (y <= z && x >= y && x <= z) 
			return 0;
		
		else if (y >= z && x <= y && x >= z) 
			return 0;

		else 
			return 1;
	}

	// --------------------------------------------------
	inline static bool equalEps(double x, double y, double eps)
	{ 
		return (fabs(x - y) < eps); 
	}

	// --------------------------------------------------
	inline static double random()
	{ 
		double r;

		Seed = (Seed * PRIME) & MASK;
		r = Seed * SCALE;
		return r;
	}

	// --------------------------------------------------
	inline static int randint(int low, int high) 
	{ 
		int p;
		double r;

		r = random();
		p = (int) (low + (high - low) * r);
		
		return p; 
	}

	// ------------------------------------------------
	static int isInArray(long array[], int n, int value)
	{
		int i;

		// position array is sorted!
		for (i = 0; i < n; i++)
			if (array[i] == value)
				// value found
				return 1;

			else if (array[i] > value)
				// cannot be found anymore (array is sorted)
				return 0;

		return 0;
	}

	// ------------------------------------------------
	static int membership(BYTE rval1, BYTE rval2, BYTE value) 
	{
	  if (rval1 == 0 && rval2 == 255) 
	    return 255; 
	  else if (value < rval1 || value > rval2 ) 
	    return 0; 
	  else
	    return 1;
	}

};

// ===========================================================================
//  Declaration and Implementation of class BioclimHistogram
// ===========================================================================
class BioclimHistogram
{
public:
	BYTE matrix[2][MAX_ENV_LAYERS][256];

	BioclimHistogram()
	{ reset(); }

	void reset()
	{
		// reset values
		for (int k = 0; k < 2; k++)
			for (int i = 0; i < MAX_ENV_LAYERS; i++)
				for (int j = 0; j < 256; j++)
					matrix[k][i][j] = (BYTE) 0;
	}
};

// ===========================================================================
// ===========================================================================
#endif
