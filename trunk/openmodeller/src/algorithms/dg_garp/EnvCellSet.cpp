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

// EnvCellSet.cpp: implementation of the EnvCellSet class.

#include "EnvCell.h"
#include "EnvCellSet.h"
#include "Utilities.h"

// ============================================================================
EnvCellSet::EnvCellSet()
{
	intCount = 0;
	intSize = 0;
	objCells = NULL;

	histogram = NULL;

	resampled = false;
}

// ============================================================================
EnvCellSet::EnvCellSet(int size)
{ 
	intCount = 0;
	intSize = 0;
	objCells = NULL;

	histogram = NULL;

	resampled = false;

	initialize(size); 
}

// ============================================================================
EnvCellSet::~EnvCellSet()
{ 
	if (histogram)
		delete histogram;

	if (objCells != NULL)
	{
		if (!resampled)
		{
			// not resampled:
			// it is not a copy of another CellSet
			// can delete all the EnvCells
			for (int i = 0; i < intCount; i++)
			{
				// EnvCellSet takes care of EnvCell property alocation and dealocation
				// possible bug!!!
				if (objCells[i])
				{
					delete[] objCells[i]->values;
					delete objCells[i];
				}
			}
		}

		delete[] objCells;
	}
}

// ============================================================================
void EnvCellSet::resampleInPlace()
{
	int p, q;
	EnvCell * oAuxCell;

	// shuffle pointers to cells
	for (int i = 0; i < intSize; i++)
	{
		// pick two random numbers
		p = GarpUtil::randint(0, intSize - 1);
		q = GarpUtil::randint(0, intSize - 1);

		// change places between cells <p> and <q>
		oAuxCell = objCells[p];
		objCells[p] = objCells[q];
		objCells[q] = oAuxCell;
	}
}

// ============================================================================
EnvCellSet * EnvCellSet::resample()
{
	// create an EnvCellSet to hold shuffled pointers to the original set
	// it is a resampled set so the cells won't be dealocate upon destruction
	// of the resampled object
	EnvCellSet * objResampled = new EnvCellSet(intSize);
	objResampled->resampled = true;

	// point to random data
	for (int i = 0; i < intSize; i++)
		objResampled->add(objCells[GarpUtil::randint(0, intSize - 1)]);

	return objResampled;
}

// ============================================================================
void EnvCellSet::initialize(int size)
{
	intCount = 0;
	intSize = size;
	objCells = new EnvCell*[intSize];
}

// ============================================================================
int EnvCellSet::genes()
{
	if (intCount)
		return objCells[0]->size();
	else
		return 0;
}

// ============================================================================
int EnvCellSet::size()
{ 
	return intSize; 
}

// ============================================================================
int EnvCellSet::count()
{ 
	return intCount; 
}

// ============================================================================
EnvCell * EnvCellSet::get(int index)
{ 
	if (index < intCount)
		return objCells[index]; 
	else
		throw GarpException(1, "EnvCellSet::get(): index out of bounds");
}

// ============================================================================
void EnvCellSet::add(EnvCell * cell)
{ 
	if (intCount < intSize)
		objCells[intCount++] = cell; 
	else
		throw GarpException(1, "EnvCellSet::add(): cell set already full");
}

// ==========================================================================
BioclimHistogram * EnvCellSet::getBioclimHistogram()
{ return histogram; }

// ==========================================================================
void EnvCellSet::createBioclimHistogram()
{
	//EnvCell * cell;
	int i, j, nc, ng;
	BYTE pred, val;

	if (!histogram)
		histogram = new BioclimHistogram;

	histogram->reset();

	// number of genes
	ng = genes();

	// number of cells
	nc = intCount;

	// calculate occurrence of value for each gene
	for (i = 0; i < ng; i++)
	{
		// take into account the occurrence of each value for each cell in train set
		for (j = 0; j < nc; j++)
		{
			// cell = objCells[j];
			
			// get presence or absence
			pred = objCells[j]->values[0];
			val  = objCells[j]->values[i];
			histogram->matrix[pred][i][val]++;
		}
	}
}

// ============================================================================
// ============================================================================
// ============================================================================
