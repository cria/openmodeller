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


#define EXTENDED_DUMMY_RULE( name ) \
class Ext##name : public name \
{ \
public: \
  Ext##name() {}; \
  GarpRule * objFactory()   { return new Ext##name; } \
  Scalar * getGenes()       { return _genes; }; \
  double * getPerformanceArray() { return _performance; } \
  void setPrediction(Scalar newPrediction) { _prediction = newPrediction; } \
  void initialize(GarpCustomSampler * sampler) { name::initialize(sampler); } \
  void setGenes(Scalar * genes, int numGenes); \
}; \
void Ext##name::setGenes(Scalar * genes, int numGenes) \
{ \
  int i; \
  _numGenes = numGenes; \
  if (_genes) { delete _genes; } \
  if (genes) \
    { \
      _genes = new Scalar[_numGenes * 2]; \
      g_log("Setting genes: "); \
      for (i = 0; i < _numGenes * 2; i++) \
	{ \
	  _genes[i] = genes[i]; \
	  g_log("%+8.4f ", _genes[i]); \
	} \
      g_log("\n"); \
    } \
}
