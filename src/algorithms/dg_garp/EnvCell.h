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

// ============================================================================
// EnvCell.h: interface for the EnvCell class.
//
// ============================================================================

#ifndef __ENVCELL_H__
#define __ENVCELL_H__

#include "Utilities.h"

class EnvCell  
{
public:
	int intCount;
	BYTE * values;

	// coordinates
	int intPos;
	double x, y;

public:

	EnvCell();
	EnvCell(int ct, BYTE * val);
	EnvCell(int ct, BYTE * val, int pos, double x, double y);
	virtual ~EnvCell();

	void setSize(int newSize);
	int size();
	void setValues(BYTE * newValues);
	BYTE * getValues();

	char * toXML();

	bool operator==(EnvCell oCell);
	bool operator!=(EnvCell oCell);
};

// ============================================================================
#endif // __ENVCELL_H__

