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

// EnvCellSet.h : Declaration of the CEnvCellSet

#ifndef __ENVCELLSET_H_
#define __ENVCELLSET_H_

#include "EnvCell.h"
#include "Utilities.h"

// ============================================================================
// EnvCellSet
// ============================================================================

class EnvCellSet
{
private:
	int intCount;
	int intSize;
	EnvCell ** objCells;

	bool resampled;

	// bioclim histograms
	BioclimHistogram * histogram;

public:
	EnvCellSet();
	EnvCellSet(int size);
	virtual ~EnvCellSet();

	void initialize(int size);

	void createBioclimHistogram();
	BioclimHistogram * getBioclimHistogram();

	EnvCellSet * resample();
	void resampleInPlace();

	int size();
	int count();
	int genes();

	void add(EnvCell * cell);
	EnvCell * get(int index);
};

// ============================================================================
#endif 
