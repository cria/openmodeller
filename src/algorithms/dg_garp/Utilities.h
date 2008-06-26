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

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
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
extern unsigned long Seed;						// seed for random number generator
extern unsigned long OrigSeed;					// original value for random seed

// macros for bitmap header generation
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

#define MAX_BYTE  255

//under mingw these defines are already defined in windef.h..
#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#endif
#ifndef MAKELONG
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#endif
#ifndef LOWORD
#define LOWORD(l)           ((WORD)(l))
#endif
#ifndef HIWORD
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#endif
#ifndef LOBYTE
#define LOBYTE(w)           ((BYTE)(w))
#endif
#ifndef HIBYTE
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

#define MAX(a, b)           ( (a > b)? a : b )
#define MIN(a, b)           ( (a < b)? a : b )

int getFileSize(char * strFilename);

// ===========================================================================
//  Declaration and Implementation of class GarpException
// ===========================================================================
class GarpException
{
public:
	int intCode;
	string strMessage;

	GarpException(int code, string message)
	{
		intCode = code;
		strMessage = message;
	}

	void getError(int& code, string& message)
	{
		code = intCode;
		message = strMessage;
	}
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
			OrigSeed = clock;
		}
		else
		{
			OrigSeed = iOrigSeedProvided;
		}

		Seed = OrigSeed;
	}

	// ------------------------------------------------
	static long getFileSize(char * strFilename)
	{
		// get the file size
		FILE * f;
		int intFileSize;

		f = fopen(strFilename, "rb");
		if (f)
		{
			fseek(f, 0, SEEK_END); 
			intFileSize = ftell(f);
			fclose(f);
		}
		else
			intFileSize = 0;
		
		return intFileSize;
	}

	// ------------------------------------------------
	static double readDoubleFromFile(FILE *f)
	{
		char S[255];
		double val;

		// read in into a string and then convert it into a double
		fscanf(f,"%s", S);

		// convert value
		val = atof(S);

		// return the value
		return val;
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


	// ------------------------------------------------
	//  XML Utilities
	// ------------------------------------------------
	static void addXMLTag(char * strXML)
	{
		char * strTemp = new char[strlen(strXML)];
		sprintf(strTemp, "<?xml version=\"1.0\"?>\n%s", strXML); 
		strcpy(strXML, strTemp); 
	}

	// ------------------------------------------------

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
//  Declaration of Generic Garp Classes
// ===========================================================================
// ===========================================================================

// ===========================================================================
//  Declaration of class: GarpGenericCoClass
// ===========================================================================
class GarpGenericCoClass
{
protected:
	// exception handling
	int intLastErrorCode;
	char strLastErrorMessage[CHAR_ARRAY_SIZE];

protected:
	// private methods
	// ------------------------------------------------------------
	void resetError()
	{
		intLastErrorCode = 0;
		strcpy(strLastErrorMessage, "No error has occurred");
	}
	// ------------------------------------------------------------

public:
	GarpGenericCoClass()   { resetError(); }
	virtual ~GarpGenericCoClass()  {}
};

/*
// ===========================================================================
//  Declaration of class: GarpProgressEvents
// ===========================================================================
class GarpProgressEvents
{
private:
	void * oCaller;
	double dblStep;

	void ( * callBackFunction ) (void * obj, int intPercentage, int intMethodId, int iObjectId);

public:
	// ------------------------------------------------------------
	static void defaultCallBackFunction (void * obj, int intPercentage, int intMethodId, int iObjectId)
	{ 
		int aux = intPercentage; 
	}

	// ------------------------------------------------------------
	void setDefaultCallBackFunction()
	{
		oCaller = NULL;
		callBackFunction = defaultCallBackFunction; 
	}

	// ------------------------------------------------------------
	GarpProgressEvents()
	{ 
		dblStep = 0.10;
		setDefaultCallBackFunction();
	}
	
	// ------------------------------------------------------------
	virtual ~GarpProgressEvents() {}

	// ------------------------------------------------------------
	double getStep()
	{ return dblStep; }

	// ------------------------------------------------------------
	void setStep(double dblNewStep)
	{ dblStep = dblNewStep; }

	// ------------------------------------------------------------
	void setCallBackFunction(void * obj, void (* cbFunction) (void *, int, int, int))
	{
		oCaller = obj;
		callBackFunction = cbFunction; 
	}

	// ------------------------------------------------------------
	void fireProcessEvent(int intPercent, int intMethodId, int intObjectId)
	{ 
		if (callBackFunction)
		{ 
		  //callBackFunction(oCaller, intPercent, intMethodId, intObjectId); 
		}
	}
};
*/
// ===========================================================================
// ===========================================================================
#endif
