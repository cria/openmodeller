/**
 * Rule declarations for garp unit tests
 * 
 * @file   test_rules_defs.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-04-07
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
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


template <class T>
class ExtendedDummyRule : public T
{
public:
  ExtendedDummyRule(int numGenes = 0) : T (numGenes) { _prediction = 0.0; };
  ExtendedDummyRule(Scalar pred, int numGenes, 
		    Sample& ch1, Sample& ch2, double * perf) :
    T(pred, numGenes, ch1, ch2, perf) {};

  GarpRule * objFactory() const  { return new ExtendedDummyRule(); }
  double * getPerformanceArray() { return _performance; }
  void setPrediction(Scalar newPrediction) { _prediction = newPrediction; }

  void setGenes(Scalar * genes, int numGenes)
  { 
    //printf("Setting %3d genes:\n", numGenes);
    //printf(" Genes   | chrom1   | chrom2\n", numGenes);
    _numGenes = numGenes; 
    if (genes) 
      { 
	Scalar values1[_numGenes]; 
	Scalar values2[_numGenes]; 
	for (int i = 0; i < _numGenes; i++) 
	  { 
	    values1[i] = genes[i * 2]; 
	    values2[i] = genes[i * 2 + 1]; 

	    //printf(" %-7.3f |  %-7.3f |        \n", genes[i * 2], values1[i]);
	    //printf(" %-7.3f |          | %-7.3f \n", genes[i * 2 + 1], values2[i]);
	  } 
	_chrom1 = Sample(_numGenes, values1); 
	_chrom2 = Sample(_numGenes, values2); 
      } 
    //printf("=====\n");
  }
};
