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

// EnvCell.cpp: implementation of the EnvCell class.
//
//////////////////////////////////////////////////////////////////////

#include "EnvCell.h"
#include "Utilities.h"

#include <stdio.h>

// ============================================================================
EnvCell::EnvCell() 
{
	intCount = 0;
	values = NULL;

	intPos = -1;
	x = y = 0.0;
}

// ============================================================================
EnvCell::EnvCell(int ct, BYTE * val)
{
	intCount = ct;
	values = val;

	intPos = -1;
	x = y = 0.0;
}

// ============================================================================
EnvCell::EnvCell(int ct, BYTE * val, int pos, double px, double py)
{
	intCount = ct;
	values = val;

	intPos = pos;
	x = px;
	y = py;
}

// ============================================================================
EnvCell::~EnvCell() 
{
}

// ============================================================================
void EnvCell::setSize(int newSize)
{ 
	intCount = newSize; 
}

// ============================================================================
int EnvCell::size()
{ 
	return intCount; 
}

// ============================================================================
void EnvCell::setValues(BYTE * newValues)
{ 
	values = newValues; 
}

// ============================================================================
BYTE * EnvCell::getValues()
{ 
	return values; 
}

// ============================================================================
bool EnvCell::operator==(EnvCell oCell)
{
	int i;

	// if the sizes are different than they are not equal
	if (intCount != oCell.intCount)
		return false;

	// check if the values in both cells are identical
	i = 0;
	while ((values[i] == oCell.values[i]) && (i < intCount))
		i++;

	// if (i == intCount) then all values are equal
	// otherwise, the values at index [i] are the first difference found
	return (i == intCount);
}

// ============================================================================
bool EnvCell::operator!=(EnvCell oCell)
{
	return !(operator==(oCell));
}

// ============================================================================
char * EnvCell::toXML()
{
	const int iStringSize = 1024;

	char * strXML = new char[iStringSize];
	char strAux[256];

	sprintf(strXML, "<EnvCell Pos=\"%d\" X=\"%f\" Y=\"%f\">", intPos, x, y);

	for (int i = 0; i < intCount; i++)
	{
		sprintf(strAux, "%d ", values[i]);
		strcat(strXML, strAux);
	}

	strcat(strXML, "</EnvCell>\n");

	if (strlen(strXML) > static_cast<int> (iStringSize))
		throw GarpException(82, "String size exceeded in EnvCell::toXML()");

	return strXML;
}

// ============================================================================
// ============================================================================
