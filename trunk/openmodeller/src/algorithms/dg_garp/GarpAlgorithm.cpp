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

// Algorithm.cpp : Implementation of CGarpAlgorithm

#ifdef WIN32
// avoid warnings caused by problems in VC headers
#define _SCL_SECURE_NO_DEPRECATE
#endif

#include "Rule.h"
#include "GarpAlgorithm.h"
#include "Utilities.h"
#include "EnvCellSet.h"

#include <openmodeller/om.hh>
#include <openmodeller/Random.hh>
#include <openmodeller/ScaleNormalizer.hh>

#include <time.h>

#include <string>

#define NUM_PARAM 4

/****************************************************************/
/*** Algorithm parameter metadata *******************************/

static AlgParamMetadata parameters[NUM_PARAM] = 
{
  // Metadata of the first parameter.
  {
    "MaxGenerations",              // Id.
    "Max generations",             // Name.
    Integer,                       // Type.

    // Overview.
    "Maximum number of iterations run by the Genetic Algorithm.",

    // Description.
    "Maximum number of iterations (generations) run by the Genetic Algorithm.",

    1,      // Not zero if the parameter has lower limit.
    1,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    "400"   // Parameter's typical (default) value.
  },

  {
    "ConvergenceLimit",        // Id.
    "Convergence limit",       // Name.
    Real,                      // Type.

    // Overview.
    "Defines the convergence value that makes the algorithm stop (before reaching MaxGenerations).",

    // Description.
    "Defines the convergence value that makes the algorithm stop (before reaching MaxGenerations).",

    1,     // Not zero if the parameter has lower limit.
    0.0,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    1.0,   // Parameter's upper limit.
    "0.01"  // Parameter's typical (default) value.
  },

  {
    "PopulationSize",        // Id.
    "Population size",       // Name.
    Integer,                 // Type.

    "Maximum number of rules to be kept in solution.", // Overview.
    "Maximum number of rules to be kept in solution.", // Description

    1,     // Not zero if the parameter has lower limit.
    1,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    500,   // Parameter's upper limit.
    "50"  // Parameter's typical (default) value.
  },

  {
    "Resamples",      // Id.
    "Resamples",      // Name.
    Integer,          // Type.

    // Overview.
    "Number of points sampled (with replacement) used to test rules.",

    // Description.
    "Number of points sampled (with replacement) used to test rules.",

    1,     // Not zero if the parameter has lower limit.
    1,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    100000,   // Parameter's upper limit.
    "2500"  // Parameter's typical (default) value.
  }
};


/*****************************************************************/
/*** Algorithm's general metadata ********************************/

static AlgMetadata metadata = {
  
  "DG_GARP",                                         // Id.
  "GARP (single run) - DesktopGARP implementation",  // Name.
  "1.1 alpha",                                       // Version.

  // Overview.
  "This is the 2nd implementation of GARP algorithm, based on the \
original C code by David Stockwell. This version correspondss to \
the version in use by the DesktopGarp modeling package, with \
modifications to use OpenModeller base data access objects.",

  // Description.
  "GARP is a genetic algorithm that creates ecological niche \
models for species. The models describe environmental conditions \
under which the species should be able to maintain populations. \
For input, GARP uses a set of point localities where the species \
is known to occur and a set of geographic layers representing \
the environmental parameters that might limit the species' \
capabilities to survive.",

  // Author
  "Stockwell, D. R. B., modified by Ricardo Scachetti Pereira",  

  // Bibliography.
  "Stockwell, D. R. B. 1999. Genetic algorithms II. \
Pages 123-144 in A. H. Fielding, editor. \
Machine learning methods for ecological applications. \
Kluwer Academic Publishers, Boston.\
\n\
Stockwell, D. R. B., and D. P. Peters. 1999. \
The GARP modelling system: Problems and solutions to automated \
spatial prediction. International Journal of Geographic \
Information Systems 13:143-158.\
\n\
Stockwell, D. R. B., and I. R. Noble. 1992. \
Induction of sets of rules from animal distribution data: \
A robust and informative method of analysis. Mathematics and \
Computers in Simulation 33:385-390.",

  "Ricardo Scachetti Pereira",  // Code author.
  "rpereira [at] ku.edu",       // Code author's contact.

  0,  // Does not accept categorical data.
  1,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/
#ifndef DONT_EXPORT_GARP_FACTORY
OM_ALG_DLL_EXPORT 
AlgorithmImpl * 
algorithmFactory()
{
  return new GarpAlgorithm();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}
#endif


// ==========================================================================
// ==========================================================================
//  GarpAlgorithm implementation
// ==========================================================================
GarpAlgorithm::GarpAlgorithm()
  : AlgorithmImpl(& metadata)
{
        _normalizerPtr = new ScaleNormalizer( 1.0, 253.0, true );
       
	GarpUtil::randomize(0);
	srand((unsigned)time(NULL));

	// initialize properties to default values
	initializeProperties(); 

	// data points used for training
	objTrainSet = NULL;
	//Log::instance()->info("GarpAlgorithm::GarpAlgorithm() at %x\n", this );
}

// ==========================================================================
GarpAlgorithm::~GarpAlgorithm()
{
  //objBest.log();

  if (objTrainSet)
    delete objTrainSet;

  //Log::instance()->info("GarpAlgorithm::~GarpAlgorithm() at %x\n", this );
}


// ==========================================================================
int GarpAlgorithm::initialize()
{
  if (!getParameter("MaxGenerations",   &Totalgens)) {
    Log::instance()->error("Parameter MaxGenerations not set properly.");
    return 0;
  }

  if (!getParameter("ConvergenceLimit", &Conv_limit)) {
    Log::instance()->error("Parameter ConvergenceLimit not set properly.");
    return 0;
  }

  if (!getParameter("PopulationSize",   &Popsize)) {
    Log::instance()->error("Parameter PopulationSize not set properly.");
    return 0;
  }

  if (!getParameter("Resamples",        &Resamples)) {
    Log::instance()->error("Parameter Resamples not set properly.");
    return 0;
  }

  //Log::instance()->debug("MaxGenerations set to:   %d\n", _max_gen);
  //Log::instance()->debug("ConvergenceLimit set to: %.4f\n", _conv_limit);
  //Log::instance()->debug("PopulationSize set to:   %d\n", _popsize);
  //Log::instance()->debug("Resamples set to:        %d\n", _resamples);

  int dim = _samp->numIndependent();
  iActiveGenes = dim + 1;
  objBest.setDimension(dim + 1);
  objNew.setDimension(dim + 1);

  //printf("Dimensions: %d\n", dim);

  //printf("Presences: %d\nAbsences:  %d\n", _samp->numPresence(), _samp->numAbsence());

  objTrainSet = new EnvCellSet;
  objTrainSet->initialize(Resamples);
  for (int i = 0; i < Resamples; i++)
    {
      OccurrencePtr oc = _samp->getOneSample();
      Sample sample = (*oc).environment();
      Scalar dep = ( (*oc).abundance() > 0.0 ) ? 1.0 : 0.0;

      // transfer values to cell
      BYTE * values = new BYTE[dim + 2];
      EnvCell * cell = new EnvCell(dim + 2, values);
      values[0] = (BYTE) dep;
      //printf("%5d]: dep=[%d] ", i, values[0]);
      for (int j = 0; j < dim; j++)
	{ 
	  values[j + 1] = (BYTE) sample[j]; 
	  //printf("%3d (%8.3f) ", values[j + 1], sample[j]);
	}
      // Always initialize last element (added by rdg 2009-01-09)
      values[dim+1] = (BYTE)0;

      //printf("\n");
      objTrainSet->add(cell);
    }
  
  // create histograms of occurrence of each layer value for bioclim/range rules
  objTrainSet->createBioclimHistogram();
	
  // get initial model
  getInitialModel(Popsize, objTrainSet);

  return 1;
}


/****************************************************************/
/****************** getProgress *********************************/

float GarpAlgorithm::getProgress() const
{
  if (done())
    { return 1.0; }
  else
    { 
      float byIterations  = ( Gen / (float) Totalgens );
      float byConvergence = (float)( Conv_limit / Convergence );
      float progress = (byIterations > byConvergence) ? byIterations : byConvergence; 

      if (progress > _maxProgress)
	{ _maxProgress = progress; }
      return _maxProgress;
    } 
}


/****************************************************************/
/****************** getConvergence ******************************/

int GarpAlgorithm::getConvergence( Scalar * const val )
{
  *val = Convergence;
  return 0;
}


/****************************************************************/
/****************** getValue ************************************/

Scalar GarpAlgorithm::getValue( const Sample& x ) const
{
  return objBest.getValue(x);
}
  
// ==========================================================================
void GarpAlgorithm::initializeProperties()
{
	int i, j;

	lVersion = 0;

	iCPUTime = 0;

	Improvements = 0;

	Resamples = 2500;
	Accuracylimit = 0.0; // The minimium post prob of a rule
	MinUsage = 0.00;
	Mortality = 0.9;

	Experiment = 0;		// generations per experiment
	Totalgens = 2560;		// generations per experiment
	Totaltrials = 0;			// trials per experiment               
	Popsize = 50;				// population size
	C_rate = 0.25;       // crossover rate                      
	M_rate = 0.50;       // mutation rate                       
	J_rate = 0.25;       // join rate
	I_rate = 0.0;
	Gapsize = 0.8;		// fraction of pop generated from archive
	Maxspin = 2;	        // max gens without evals
	Resampling_f = 1.0 ;
	Significance = 2.70 ;    // the minimum level for inc in best    
	Conv_limit = 0.10 ;     // the fractional addition of rules     
	Cutval = 0.0;
	Trials = 0;

	// data collection and loop control variables
	Ave_current_perf = 0.0;   // ave perf in current generation       
	Best_current_perf = 0.0;  // best perf in current generation     
	Worst_current_perf = 0.0; // worst perf in current generation   
	Best = 0.0;         // best performance seen so far         
	Worst = 0.0;        // worst performance seen so far        
	Best_guy = 0;           // index of best_current_perf           

	Conv = 0;            // number of partially coverged genes   
	Doneflag = false;// set when termination conditions hold 
	Gen = 0;         // generation counter                   
	Lost = 0;            // number of totally coverged positions 
	Spin = 0;        // number of gens since eval occurred   
	Convergence = 1.0;        // the stability of rule set 
	Improvements = 0;// the number of times the best set has been 
					// improved by addition or alteration 
	Resample=1;

	for (i = 0; i < 2; i++)
		for (j = 0; j < 5; j++)
			Heuristic[i][j] = 0;        // successes of heuristic operators 

	// flags
	Sigflag  = 1;
	Postflag = 0;
	Compflag = 0;
	Adjustflag = 1;

	BioclimOnlyFlag = 0;
	LogitOnlyFlag = 0;  
	RangeRuleFlag = 1;  
	NegatedRuleFlag = 1;
	AtomicRuleFlag = 1; 
	LogitRuleFlag = 1;  


	// reset gene selection
	// gene 0 is always active (it is the species gene)
	iActiveGenes = 1;
	iGeneIndex[0] = 0;
	bGeneIsActive[0] = true;
	for (i = 1; i < MAX_ENV_LAYERS; i++)
	{
		bGeneIsActive[i] = true;
		iGeneIndex[i] = i;
	}

	_maxProgress = 0.0;
}

// ==========================================================================
char * GarpAlgorithm::getParameter2(char * strParamName)
{
	const int iStringLen = 512;
	char * strResult = new char[iStringLen];

	strcpy(strResult, "");

	// most used parameters
	if		(strcmp(strParamName, "Gen"				   ) == 0) sprintf(strResult, "%d", Gen);

	// flags
	else if (strcmp(strParamName, "Sigflag"            ) == 0) sprintf(strResult, "%d", Sigflag);
	else if (strcmp(strParamName, "Postflag"           ) == 0) sprintf(strResult, "%d", Postflag);
	else if (strcmp(strParamName, "Compflag"           ) == 0) sprintf(strResult, "%d", Compflag);
	else if (strcmp(strParamName, "Adjustflag"         ) == 0) sprintf(strResult, "%d", Adjustflag);
	else if (strcmp(strParamName, "BioclimOnlyFlag"    ) == 0) sprintf(strResult, "%d", BioclimOnlyFlag);
	else if (strcmp(strParamName, "LogitOnlyFlag"      ) == 0) sprintf(strResult, "%d", LogitOnlyFlag);
	else if (strcmp(strParamName, "RangeRuleFlag"      ) == 0) sprintf(strResult, "%d", RangeRuleFlag);
	else if (strcmp(strParamName, "NegatedRuleFlag"    ) == 0) sprintf(strResult, "%d", NegatedRuleFlag);
	else if (strcmp(strParamName, "AtomicRuleFlag"     ) == 0) sprintf(strResult, "%d", AtomicRuleFlag);
	else if (strcmp(strParamName, "LogitRuleFlag"      ) == 0) sprintf(strResult, "%d", LogitRuleFlag);

	// int parameters
	else if (strcmp(strParamName, "Version"            ) == 0) sprintf(strResult, "%d", static_cast<int>(lVersion));
	else if (strcmp(strParamName, "CPUTime"            ) == 0) sprintf(strResult, "%d", iCPUTime);
	else if (strcmp(strParamName, "Resamples"          ) == 0) sprintf(strResult, "%d", Resamples);
	else if (strcmp(strParamName, "Totalgens"          ) == 0) sprintf(strResult, "%d", Totalgens);
	else if (strcmp(strParamName, "Totaltrials"        ) == 0) sprintf(strResult, "%d", Totaltrials);
	else if (strcmp(strParamName, "Popsize"            ) == 0) sprintf(strResult, "%d", Popsize);
	else if (strcmp(strParamName, "Maxspin"            ) == 0) sprintf(strResult, "%d", Maxspin);
	else if (strcmp(strParamName, "Best_guy"           ) == 0) sprintf(strResult, "%d", Best_guy);
	else if (strcmp(strParamName, "Resample"           ) == 0) sprintf(strResult, "%d", Resample);
	else if (strcmp(strParamName, "Conv"			   ) == 0) sprintf(strResult, "%d", Conv);
	else if (strcmp(strParamName, "Trials"			   ) == 0) sprintf(strResult, "%d", Trials);
	else if (strcmp(strParamName, "Experiment"         ) == 0) sprintf(strResult, "%d", Experiment);
	else if (strcmp(strParamName, "Lost"			   ) == 0) sprintf(strResult, "%d", Lost);
	else if (strcmp(strParamName, "Spin"			   ) == 0) sprintf(strResult, "%d", Spin);
	else if (strcmp(strParamName, "Improvements"       ) == 0) sprintf(strResult, "%d", Improvements);
	else if (strcmp(strParamName, "Doneflag"           ) == 0) sprintf(strResult, "%d", (int) Doneflag);
	else if (strcmp(strParamName, "Heuristic_0"        ) == 0) sprintf(strResult, "%d", Heuristic[0][0]);
	else if (strcmp(strParamName, "Heuristic_1"        ) == 0) sprintf(strResult, "%d", Heuristic[0][1]);
	else if (strcmp(strParamName, "Heuristic_2"        ) == 0) sprintf(strResult, "%d", Heuristic[0][2]);
	else if (strcmp(strParamName, "Heuristic_3"        ) == 0) sprintf(strResult, "%d", Heuristic[0][3]);
	else if (strcmp(strParamName, "Heuristic_4"        ) == 0) sprintf(strResult, "%d", Heuristic[0][4]);

	// double parameters
	else if (strcmp(strParamName, "Accuracylimit"      ) == 0) sprintf(strResult, "%f", Accuracylimit);
	else if (strcmp(strParamName, "MinUsage"           ) == 0) sprintf(strResult, "%f", MinUsage);
	else if (strcmp(strParamName, "Mortality"          ) == 0) sprintf(strResult, "%f", Mortality);
	else if (strcmp(strParamName, "C_rate"             ) == 0) sprintf(strResult, "%f", C_rate);
	else if (strcmp(strParamName, "M_rate"             ) == 0) sprintf(strResult, "%f", M_rate);
	else if (strcmp(strParamName, "J_rate"             ) == 0) sprintf(strResult, "%f", J_rate);
	else if (strcmp(strParamName, "I_rate"             ) == 0) sprintf(strResult, "%f", I_rate);
	else if (strcmp(strParamName, "Gapsize"            ) == 0) sprintf(strResult, "%f", Gapsize);
	else if (strcmp(strParamName, "Resampling_f"       ) == 0) sprintf(strResult, "%f", Resampling_f);
	else if (strcmp(strParamName, "Significance"       ) == 0) sprintf(strResult, "%f", Significance);
	else if (strcmp(strParamName, "Conv_limit"         ) == 0) sprintf(strResult, "%f", Conv_limit);
	else if (strcmp(strParamName, "Cutval"             ) == 0) sprintf(strResult, "%f", Cutval);
	else if (strcmp(strParamName, "Ave_current_perf"   ) == 0) sprintf(strResult, "%f", Ave_current_perf);
	else if (strcmp(strParamName, "Best_current_perf"  ) == 0) sprintf(strResult, "%f", Best_current_perf);
	else if (strcmp(strParamName, "Worst_current_perf" ) == 0) sprintf(strResult, "%f", Worst_current_perf);
	else if (strcmp(strParamName, "Best"               ) == 0) sprintf(strResult, "%f", Best);
	else if (strcmp(strParamName, "Worst"              ) == 0) sprintf(strResult, "%f", Worst);
	else if (strcmp(strParamName, "Convergence"        ) == 0) sprintf(strResult, "%f", Convergence);

	// special parameters
	else if (strcmp(strParamName, "SelectedLayers"     ) == 0) 
	{
		char * strAux = getSelectedLayersAsString();
		sprintf(strResult, "%s", strAux);
		delete strAux;
	}

	if (strlen(strResult) > static_cast<unsigned int>(iStringLen))
		throw GarpException(82, "String size exceeded in getParameter::parametersToXML()");

	return strResult;
}

// ==========================================================================
void GarpAlgorithm::setParameter(char * strParamName, char * strParamValue)
{
	// flags
	if      (strcmp(strParamName, "Sigflag"            ) == 0) Sigflag = atoi(strParamValue);
	else if (strcmp(strParamName, "Postflag"           ) == 0) Postflag = atoi(strParamValue);
	else if (strcmp(strParamName, "Compflag"           ) == 0) Compflag = atoi(strParamValue);
	else if (strcmp(strParamName, "Adjustflag"         ) == 0) Adjustflag = atoi(strParamValue);
	else if (strcmp(strParamName, "BioclimOnlyFlag"    ) == 0) BioclimOnlyFlag = atoi(strParamValue);
	else if (strcmp(strParamName, "LogitOnlyFlag"      ) == 0) LogitOnlyFlag = atoi(strParamValue);
	else if (strcmp(strParamName, "RangeRuleFlag"      ) == 0) RangeRuleFlag = atoi(strParamValue);
	else if (strcmp(strParamName, "NegatedRuleFlag"    ) == 0) NegatedRuleFlag = atoi(strParamValue);
	else if (strcmp(strParamName, "AtomicRuleFlag"     ) == 0) AtomicRuleFlag = atoi(strParamValue);
	else if (strcmp(strParamName, "LogitRuleFlag"      ) == 0) LogitRuleFlag = atoi(strParamValue);

	// int parameters
	else if (strcmp(strParamName, "Version"            ) == 0) lVersion = atoi(strParamValue);
	else if (strcmp(strParamName, "CPUTime"            ) == 0) iCPUTime = atoi(strParamValue);
	else if (strcmp(strParamName, "Resamples"          ) == 0) Resamples = atoi(strParamValue);
	else if (strcmp(strParamName, "Totalgens"          ) == 0) Totalgens = atoi(strParamValue);
	else if (strcmp(strParamName, "Totaltrials"        ) == 0) Totaltrials = atoi(strParamValue);
	else if (strcmp(strParamName, "Popsize"            ) == 0) Popsize = atoi(strParamValue);
	else if (strcmp(strParamName, "Maxspin"            ) == 0) Maxspin = atoi(strParamValue);
	else if (strcmp(strParamName, "Best_guy"           ) == 0) Best_guy = atoi(strParamValue);
	else if (strcmp(strParamName, "Resample"           ) == 0) Resample = atoi(strParamValue);
	else if (strcmp(strParamName, "Conv"			   ) == 0) Conv = atoi(strParamValue);
	else if (strcmp(strParamName, "Trials"			   ) == 0) Trials = atoi(strParamValue);
	else if (strcmp(strParamName, "Experiment"         ) == 0) Experiment = atoi(strParamValue);
	else if (strcmp(strParamName, "Gen"				   ) == 0) Gen = atoi(strParamValue);
	else if (strcmp(strParamName, "Lost"			   ) == 0) Lost = atoi(strParamValue);
	else if (strcmp(strParamName, "Spin"			   ) == 0) Spin = atoi(strParamValue);
	else if (strcmp(strParamName, "Improvements"       ) == 0) Improvements = atoi(strParamValue);
	else if (strcmp(strParamName, "Doneflag"           ) == 0) Doneflag = ( atoi(strParamValue) ) ? true : false;
	else if (strcmp(strParamName, "Heuristic_0"        ) == 0) Heuristic[0][0] = atoi(strParamValue);
	else if (strcmp(strParamName, "Heuristic_1"        ) == 0) Heuristic[0][1] = atoi(strParamValue);
	else if (strcmp(strParamName, "Heuristic_2"        ) == 0) Heuristic[0][2] = atoi(strParamValue);
	else if (strcmp(strParamName, "Heuristic_3"        ) == 0) Heuristic[0][3] = atoi(strParamValue);
	else if (strcmp(strParamName, "Heuristic_4"        ) == 0) Heuristic[0][4] = atoi(strParamValue);

	// double parameters
	else if (strcmp(strParamName, "Accuracylimit"      ) == 0) Accuracylimit = atof(strParamValue);
	else if (strcmp(strParamName, "MinUsage"           ) == 0) MinUsage = atof(strParamValue);
	else if (strcmp(strParamName, "Mortality"          ) == 0) Mortality = atof(strParamValue);
	else if (strcmp(strParamName, "C_rate"             ) == 0) C_rate = atof(strParamValue);
	else if (strcmp(strParamName, "M_rate"             ) == 0) M_rate = atof(strParamValue);
	else if (strcmp(strParamName, "J_rate"             ) == 0) J_rate = atof(strParamValue);
	else if (strcmp(strParamName, "I_rate"             ) == 0) I_rate = atof(strParamValue);
	else if (strcmp(strParamName, "Gapsize"            ) == 0) Gapsize = atof(strParamValue);
	else if (strcmp(strParamName, "Resampling_f"       ) == 0) Resampling_f = atof(strParamValue);
	else if (strcmp(strParamName, "Significance"       ) == 0) Significance = atof(strParamValue);
	else if (strcmp(strParamName, "Conv_limit"         ) == 0) Conv_limit = atof(strParamValue);
	else if (strcmp(strParamName, "Cutval"             ) == 0) Cutval = atof(strParamValue);
	else if (strcmp(strParamName, "Ave_current_perf"   ) == 0) Ave_current_perf = atof(strParamValue);
	else if (strcmp(strParamName, "Best_current_perf"  ) == 0) Best_current_perf = atof(strParamValue);
	else if (strcmp(strParamName, "Worst_current_perf" ) == 0) Worst_current_perf = atof(strParamValue);
	else if (strcmp(strParamName, "Best"               ) == 0) Best = atof(strParamValue);
	else if (strcmp(strParamName, "Worst"              ) == 0) Worst = atof(strParamValue);
	else if (strcmp(strParamName, "Convergence"        ) == 0) Convergence = atof(strParamValue);

	// special parameters
	else if (strcmp(strParamName, "SelectedLayers"     ) == 0) setSelectedLayers(strParamValue);
}

// ==========================================================================
char * GarpAlgorithm::getSelectedLayersAsString()
{
	const int iStringSize = 1024;
	char strSelectedLayers[iStringSize], strNextGene[16], * strResult;

	strcpy(strSelectedLayers, "");
	for (int i = 1; i < iActiveGenes; i++)
	{
		sprintf(strNextGene, ";%d", iGeneIndex[i] - 1);
		strcat(strSelectedLayers, strNextGene);
	}

	if (strlen(strSelectedLayers) > static_cast<unsigned int>(iStringSize))
		throw GarpException(82, "String size exceeded in getParameter::parametersToXML()");

	strResult = new char[strlen(strSelectedLayers) + 2];
	// get rid of the first colon (;) if there is at least one gene active
	if (iActiveGenes > 1)
		strcpy(strResult, strSelectedLayers + 1);
	else
		strcpy(strResult, strSelectedLayers);

	return strResult;
}

// ==========================================================================
void GarpAlgorithm::setSelectedLayers(char * strParamValue)
{
	// set active genes, given a string with the gene numbers separeted by colons
	char strAux[1024], * strToken;
	int iCurrentGene;

	iActiveGenes = 1;
	iGeneIndex[0] = 0;
	bGeneIsActive[0] = true;
	for (int i = 1; i < MAX_ENV_LAYERS; i++)
		bGeneIsActive[i] = false;

	strcpy(strAux, strParamValue);
	strToken = strtok(strAux, ";");
	while (strToken != NULL)
	{ 
		// get the gene number
		iCurrentGene = 1 + atoi(strToken);
		bGeneIsActive[iCurrentGene] = true;
		iGeneIndex[iActiveGenes] = iCurrentGene;
		iActiveGenes++;

		strToken = strtok(NULL, ";"); 
	}
}

// ==========================================================================
int GarpAlgorithm::iterate()
{ 
  if (done())
    return 1;
  
  generate(objTrainSet);

  return 1;
}

// ==========================================================================
int GarpAlgorithm::done() const
{
  return Doneflag = ( Doneflag ||
		  (Gen >= Totalgens) || 
		  (Spin >= Maxspin) ||
		  (Convergence < Conv_limit) );
}

// ==========================================================================
void GarpAlgorithm::generate(EnvCellSet * objTrainSet)
{
	int i, iNewSize;

	// create a new population
	Spin++;

	if (!Gen || Resample) 
		objTrainSet->resampleInPlace();

	// evaluate the newly formed population
	evaluate(&objNew, objTrainSet);

	// put rule into archive
	iNewSize = objNew.size();
	for(i = 0; i < iNewSize; i++) 
		Conv += saveRule(i);

	objBest.trim(Popsize);

	// gather performance statistics
	measure();

	// check termination condition for this experiment
	Doneflag = ( done() ) ? true : false;

	//objBest.gatherRuleSetStats(Gen);
	//objNew.gatherRuleSetStats(-Gen);

	if (Doneflag)
    {
		// sort rules by usage
		//objBest.sort(9);

		// discard rules which performance is worse than the specified significance
		objBest.discardRules(8, Significance);
    }
	else
	{
		if (objBest.size()) 
			select();
      
		// Fill the rest with new rules */  
		colonize(&objNew, objTrainSet, Popsize - objNew.size());

		// Kill off proportion of archive
		objBest.trim((int) ((double) objBest.size() * Mortality));

		// apply heuristic operators
		mutate();
		crossover();
		join();
	}

	// one more generation has been computed
	Gen++;	
}

// ==========================================================================
void GarpAlgorithm::measure()
{
	double performance, ch;
	int i, iBestSize;

	iBestSize = objBest.size();

	// update current statistics
	if (iBestSize > 0)
	{
		performance = objBest.get(0)->dblPerformance[0];
		Best_current_perf = performance;
		Worst_current_perf = performance;
		Ave_current_perf = performance;
		Best_guy = 0;
	}

	for (i = 1; i < iBestSize; i++)
	{
		// update current statistics
		performance = objBest.get(i)->dblPerformance[0];
	  
		Ave_current_perf += performance;
		if (BETTER(performance, Best_current_perf))
		{
			Best_current_perf = performance;
			Best_guy = i;
		}
		
		if (BETTER(Worst_current_perf, performance))
			Worst_current_perf = performance;
	}

	Ave_current_perf /= Popsize;

	// Adjuct heuristic operators
	if (Adjustflag) 
	{
		ch = (double) (Heuristic[1][0] - Heuristic[0][0]);
		
		if (ch < 1) 
			I_rate = Gapsize = M_rate = 0.1;
		else 
		{
			I_rate = ch / (double) objBest.size();
			Gapsize = 0.5 * (Heuristic[1][1] - Heuristic[0][1]) / ch + 0.1;
			M_rate =  0.5 * (Heuristic[1][2] - Heuristic[0][2]) / ch + 0.1;
			C_rate = C_rate ? 0.5 * (Heuristic[1][3] - Heuristic[0][3]) / ch + 0.1 : 0;
			J_rate = J_rate ? 0.5 * (Heuristic[1][4] - Heuristic[0][4]) / ch + 0.1 : 0;
		}

		for (i = 0; i < 5; i++) 
			Heuristic[0][i] = Heuristic[1][i];

		//printf("gs=%4.2f ", Gapsize);
	}

	// update overall performance measures
	Convergence = converge();

	//DisplayStatus();
}

// ==========================================================================
void GarpAlgorithm::DisplayStatus()
{
	FILE * fp = fopen("status.txt", "a");

	double perf = objBest.getOveralPerformance(8, 5);
	fprintf(fp, "%5d %f\n", Gen, perf);

	/*
	int i;

  fprintf(fp,"Max generations %d", Totalgens);
	fprintf(fp,"\nGens \tTrials \tConv \t\tData\tResamp\tBest \tAverage");
	fprintf(fp,"\n%4d \t%6d \t%4f \t%4d \t%4d \t%5.2f \t%5.2f",
	  Gen, Trials, Convergence, objTrainSet->count(), objTrainSet->count(),  
	  Best, Ave_current_perf);
	fprintf(fp,"\nHeuristic:	Rules	Improve	Random	Mutate	Cross	Join");
	fprintf(fp,"\nNo\t\t%d", objBest.size());
	for (i=0; i<5; i++) 
		fprintf(fp,"\t%d", Heuristic[1][i]);

	fprintf(fp,"\nRate\t\t\t%4.2f\t%4.2f\t%4.2f\t%4.2f\t%4.2f", I_rate, Gapsize, M_rate, C_rate, J_rate);

	if (objBest.size() > 0)
	{
		//fprintf(fp,"\n<RuleSet>\n");
		for (i = 0; i < objBest.size(); i++)
		{
			Rule * pRule = objBest.objRules[i];

			if (pRule->dblPerformance[8] > Significance)
				fprintf(fp,"%f %5d %f %d %f %f %d\n", pRule->_pXYs, pRule->_no, pRule->_dA, -1, pRule->_dSig, pRule->dblPerformance[8], pRule->Gene[0] == pRule->intConclusion);
				//fprintf(fp,"%s", objBest.objRules[i]->headerToXML());
		}

		//fprintf(fp,"</RuleSet>");
	}

	fprintf(fp,"\nPerformance: %5.3f",objBest.objRules[0]->dblPerformance[0]);

	double * Utility;

	Utility = objBest.objRules[0]->dblPerformance;
	fprintf(fp,"\n\n\t\tStrength\tCertainty\tError\nAll");
	for (i=1; i<4; i++)
		fprintf(fp,"\t\t%5.3f",Utility[i]);
	fprintf(fp,"\nSelect");
	for (i=4; i<7; i++)
		fprintf(fp,"\t\t%5.3f",Utility[i]);
	fprintf(fp,"\n\n\t\tno/n\t\tSignificance\tError\n");
	for (i=7; i<10; i++)
		fprintf(fp,"\t\t%5.3f",Utility[i]);
	*/

	fclose(fp);
}

// ==========================================================================
void GarpAlgorithm::updateHeuOpPerformance(char chrType)
{
	Improvements++;

	Heuristic[1][0] = Improvements;

	switch (chrType)
	{
	case 'r': Heuristic[1][1]++; break;
	case 'm': Heuristic[1][2]++; break;
	case 'c': Heuristic[1][3]++; break;
	case 'j': Heuristic[1][4]++; break;
	}
}

// ==========================================================================
void GarpAlgorithm::colonize(RuleSet * objRules, EnvCellSet * objTrainSet, int intNewRules)
{
	int i, p, t;
	Rule * newRule=0;

	// number of available rule types
	int iRuleIndex[4];
	int iRuleTypes;

	iRuleTypes = 0;
	if (RangeRuleFlag)   iRuleIndex[iRuleTypes++] = 0;
	if (NegatedRuleFlag) iRuleIndex[iRuleTypes++] = 1;
	if (AtomicRuleFlag)  iRuleIndex[iRuleTypes++] = 2;
	if (LogitRuleFlag)   iRuleIndex[iRuleTypes++] = 3;
		
	for (i = 0; i < intNewRules; i++)
	{
		// pick the next rule to be generate
		p = i % iRuleTypes;
		t = iRuleIndex[p];

		switch (t)
		{
		case 0: 
			newRule = new RangeRule(); 
			break;

		case 1: 
			newRule = new NegatedRangeRule(); 
			break;

		case 2: 
			newRule = new AtomicRule(); 
			break;

		case 3: 
			newRule = new LogitRule(); 
			break;
		}

		//printf("Active Genes: %3d\n", iActiveGenes);

		newRule->initialize(objTrainSet, objRules, bGeneIsActive, iGeneIndex, iActiveGenes);
		newRule->lId = Gen * 1000 + i;
		newRule->iOrigGen = Gen;

		objRules->add(newRule);
	}
}

// ==========================================================================
void GarpAlgorithm::evaluate(RuleSet * objRules, EnvCellSet * objTrainSet)
{
	Rule * pRule;
	register double performance=0.0;
	register int i, n;

	Conv = 0;

	n = objRules->size();
	for (i = 0; i < n; i++)
    {
		pRule = objRules->get(i);

		if (pRule->needsEvaluation())
		{
			performance = pRule->testWithData(objTrainSet);

			/* Make not rule if significance is decreased */
			/*if (Speciesflag && performance<0) 
			{
				New[i]->Perf[0] = -New[i]->Perf[0];
				New[i]->Perf[8] = -New[i]->Perf[8];
				New[i]->Type = '!';
			}*/

			pRule->intGens += 1;
			pRule->intTrials += 1;
			pRule->blnNeedsEvaluation = false;
			Trials++;
		}

		Spin = 0;			/* we're making progress */
      
		if (Trials == 1) 
			Best = performance;

		if (BETTER(performance, Best)) 
			Best = performance;
	} 
}

// ==========================================================================
int GarpAlgorithm::saveRule(int iIndex)
{
	//  Save the ith structure in current population
	//  if it is one of the Popsize best seen so far

	int j, l;			// loop control var
	int found, ind, iLength, iBestSize;
	char cNewType;
	Rule * Temp, * oNewRule;

	// Set criteria for optimizing
	ind = 0;

	// get best size
	iBestSize = objBest.size();

	// get rule from objNew being inserted into objBest
	oNewRule = objNew.get(iIndex);
	cNewType = oNewRule->type();

	// get Gene length
	iLength = oNewRule->intGenes * 2;

	// Check if an identical or more general structure is already there
	for (j = 0, found = 0; j < iBestSize && (!found); j++)
    {
		if (cNewType == objBest.get(j)->type())
			found = oNewRule->similar(objBest.get(j));
    }

	if (found)
    {
		j--;

		//Rule * oRuleToBeReplaced = objBest.get(j);

		if (oNewRule->dblPerformance[ind] > objBest.objRules[j]->dblPerformance[ind])
		// Replace if better
		{
			delete objBest.objRules[j];
			objBest.objRules[j] = oNewRule->clone();

			/*
			int k;
			for (k = 0; k < iLength; k++)
				oRuleToBeReplaced->Gene[k] = oNewRule->Gene[k];

			for (k = 0; k < 10; k++)
				oRuleToBeReplaced->dblPerformance[k] = oNewRule->dblPerformance[k];
	  
			oRuleToBeReplaced->intGens = oNewRule->intGens;
			oRuleToBeReplaced->chrOrigin = oNewRule->chrOrigin;
			oRuleToBeReplaced->intTrials++;
			*/

			objBest.objRules[j]->intTrials++;
			updateHeuOpPerformance(oNewRule->chrOrigin);
		}
      
		return 1;
    }


	// No similar structure found
	// allocate new structure at the end if room
	
	Rule * oRuleBeingInserted;
	if (iBestSize < Popsize )
    {
		// create dummy rule and insert it into the best rule set
		oRuleBeingInserted = oNewRule->clone();
		oRuleBeingInserted->dblPerformance[ind] = 0.0;
		objBest.add(oRuleBeingInserted);
    }

	// get best size again
	iBestSize = objBest.size();

	// find insertion point and sort j
	for (j = 0; (iBestSize > 1 && j < iBestSize - 2) && 
			    (objBest.get(j)->dblPerformance[ind] > oNewRule->dblPerformance[ind]); j++) 
	{
		if (objBest.get(j)->dblPerformance[ind] < objBest.get(j + 1)->dblPerformance[ind]) 
		{
			Temp = objBest.objRules[j];
			objBest.objRules[j] = objBest.objRules[j + 1];
			objBest.objRules[j + 1] = Temp;
		}
	}
     
	if (j >= Popsize)
		return 0;

	// Inserting new rule in j
	// Shift rules down
	for (l = iBestSize - 1; (l >= j); l--)
    {
		objBest.objRules[l + 1] = objBest.objRules[l];
    }

	objBest.objRules[j] = oNewRule->clone();
	objBest.intRules++;

	updateHeuOpPerformance(oNewRule->chrOrigin);

	return 1;
}

// ==========================================================================
void GarpAlgorithm::saveBestRules(RuleSet * toRuleSet, RuleSet * fromRuleSet)
{
	// deprecated!!!
	throw GarpException(1, "GarpAlgorithm::saveBestRules() method is deprecated");

	/*
	// put marks on both sets so it is easy to tell where each rule came from
	toRuleSet->setPad(' ');
	fromRuleSet->setPad('n');

	// concatenate the rulesets, replacing the similar rules and adding the different ones
	concatenateRuleSets(toRuleSet, fromRuleSet);

	// sort the result set
	toRuleSet->sort(0);

	// trim it to the max number of rules
	toRuleSet->trim(Popsize);

	// count how many of the remaining rules came from the new set
	updateHeuOpPerformance(toRuleSet, 'n');
	*/
}

// ==========================================================================
void GarpAlgorithm::concatenateRuleSets(RuleSet * toRuleSet, RuleSet * fromRuleSet)
{
	// deprecated!!!
	throw GarpException(1, "GarpAlgorithm::concatenateRuleSets() method is deprecated");

	
	Rule * fromRule;
	int i, j, nf;
	bool found;

	//nt = toRuleSet->size();
	nf = fromRuleSet->size();

	for (i = 0; i < nf; i++)
	{
		fromRule = fromRuleSet->get(i);

		// Check if an identical or more general structure is already there
		for (j = 0, found = false; j < toRuleSet->size() && (!found); j++)
		{
			if (fromRule->type() == toRuleSet->get(j)->type())
				found = fromRule->similar(toRuleSet->get(j));
		}
		
		j--;

		if (found)
			toRuleSet->get(j)->copy(fromRule); 
		else
			toRuleSet->add(fromRule->clone());
	}
}

// ==========================================================================
void GarpAlgorithm::select()
{
	double expected;		// expected number of offspring		
	double factor;			// normalizer for expected value        
	double ptr;				// determines fractional selection	
	double sum;				// control for selection loop           

	int i, j, k, n, temp;

	int Sample[MAX_RULES];

	n = objBest.size();
	for (i = 0; i < MAX_RULES; i++)
		Sample[i] = i % n;

	// normalizer for proportional selection probabilities 
	if (Ave_current_perf - Worst) 
		factor = Maxflag ? 1.0 / (Ave_current_perf - Worst) :	1.0/(Worst - Ave_current_perf);
	else 
		factor=1.0;


	// Stochastic universal sampling algorithm by James E. Baker 
	k=0;						// index of next Selected structure 
	ptr = GarpUtil::random();		// spin the wheel one time 

	for (sum = i = 0; i < objBest.size(); i++)
	{
		if (Maxflag) 
		{
			if (objBest.get(i)->dblPerformance[0] > Worst)
				expected = (objBest.get(i)->dblPerformance[0] - Worst) * factor;
			else 
				expected = 0.0;
		}
		
		else 
		{
			if (objBest.get(i)->dblPerformance[0] < Worst)			  
				expected = (Worst - objBest.get(i)->dblPerformance[0]) * factor;			
			else 
				expected = 0.0;
		}

		for (sum += expected; (sum > ptr) && (k<=Popsize); ptr++)	
			Sample[k++] = i;
	}

	// randomly shuffle pointers to new structures 
	for (i = 0; i < Popsize; i++)
	{
		j = GarpUtil::randint (i, Popsize - 1);
		temp = Sample[j];
		Sample[j] = Sample[i];
		Sample[i] = temp;
	}

	// finally, form the new population 
	// Gapsize giving the proportion contribution
	// to the new population from the objBest archive set 
	Rule * oRuleBeingInserted;
	double dSize;

	objNew.clear();
	dSize = ((double) Popsize) * Gapsize;

	for (i = 0; i < dSize; i++)
	{
		oRuleBeingInserted = objBest.objRules[Sample[i]]->clone();
		oRuleBeingInserted->blnNeedsEvaluation = true;
		objNew.add(oRuleBeingInserted);
    }
}
 
// ==========================================================================
double GarpAlgorithm::converge()
{
	if (Heuristic[1][0] != 0) 
		Convergence = (Convergence + ((double)Conv)/Improvements)/2;
	else
		Convergence = 1.0;

	return Convergence;
}
 
// ==========================================================================
void GarpAlgorithm::join()
{
  return;

	register int mom, dad;	/* participants in the crossover */
	register int xpoint1;	/* first crossover point w.r.t. structure */
	register int xpoint2;	/* second crossover point w.r.t. structure */
	register int i;		/* loop control variable */
	register BYTE temp;		/* used for swapping alleles */
	static double last;		/* last element to undergo Crossover */
	int diff;			/* set if parents differ from offspring */
	BYTE  *kid1;			/* pointers to the offspring */
	BYTE  *kid2;

	last = Popsize * J_rate;

	if (J_rate > 0.0) 
	{
		for (mom = Popsize - 1; mom > Popsize-last ; mom -= 2)
		{
			/* diff wasn't beeing initialized in original Stockwell's code */
			diff = 0;

			dad = mom - 1;

			/* kids start as identical copies of parents */
			kid1 = objNew.get(mom)->Gene;
			kid2 = objNew.get(dad)->Gene;

			/* choose two Crossover points */
			xpoint1 = GarpUtil::randint(0, objNew.get(mom)->intLength);
			xpoint2 = GarpUtil::randint(0, objNew.get(mom)->intLength);


			/* perform crossover */
			for (i=xpoint1 ; i % objNew.get(mom)->intLength == xpoint2; i++)
			{
				temp = kid1[i];
				if (kid1[i] == MAX_BYTE) kid1[i] = kid2[i];
				if (temp != MAX_BYTE) kid2[i] = temp;
				diff += (kid1[i] != kid2[i]);
			}

			if (diff)		/* kids differ from parents */
			{
				/* set evaluation flags */
				objNew.get(mom)->blnNeedsEvaluation = true;
				objNew.get(mom)->intGens = 0;
				objNew.get(mom)->chrOrigin = 'j';
			}
		}
	}
}

// ==========================================================================
void GarpAlgorithm::mutate()
{
	int	i;
	int Temperature = MAX_MUTATION_TEMPERATURE;

	if (Gen) Temperature = (int)(MAX_MUTATION_TEMPERATURE/(double)Gen);

	for (i = 0; i < Popsize; i++)
		objNew.get(i)->mutate(Temperature);
	
	return;
}
 
// ==========================================================================
void GarpAlgorithm::crossover()
{
  return;

	register int mom, dad;	/* participants in the crossover */
	register int xpoint1;		/* first crossover point w.r.t. structure */
	register int xpoint2;		/* second crossover point w.r.t. structure */
	register int i;		/* loop control variable */
	register char temp;		/* used for swapping alleles */
	static double last;		/* last element to undergo Crossover */
	int diff;			/* set if parents differ from offspring */
	BYTE *kid1;			/* pointers to the offspring */
	BYTE *kid2;

	last = C_rate * Popsize;

	if (C_rate > 0.0)
	{
		for (mom=0; mom < last ; mom += 2)
		{
			/* diff wasn't beeing initialized in original Stockwell's code */
			diff = 0;

			dad = mom + 1;

			/* kids start as identical copies of parents */
			kid1 = objNew.get(mom)->Gene;
			kid2 = objNew.get(dad)->Gene;

			/* choose two Crossover points */
			xpoint1 = GarpUtil::randint(0,objNew.get(mom)->intLength);
			xpoint2 = GarpUtil::randint(0,objNew.get(mom)->intLength);


			/* perform crossover */
			for (i=xpoint1 ; i%(objNew.get(mom)->intLength)==xpoint2; i++)
			{
				temp = kid1[i];
				kid1[i] = kid2[i];
				kid2[i] = temp;
				diff += (kid1[i] != kid2[i]);
			}

			if (diff)		/* kids differ from parents */
			{
				/* set evaluation flags */
				objNew.get(mom)->blnNeedsEvaluation = true;
				objNew.get(dad)->blnNeedsEvaluation = true;
				objNew.get(mom)->intGens = 0;
				objNew.get(dad)->intGens = 0;
				objNew.get(mom)->chrOrigin = 'c';
				objNew.get(dad)->chrOrigin = 'c';
			}
		}
	}
}

// ==========================================================================
void GarpAlgorithm::getInitialModel(int intSize, EnvCellSet * objTrainSet)
{
	Popsize = intSize;
	colonize(&objNew, objTrainSet, Popsize);
}

// ==========================================================================

/****************************************************************/
/****************** configuration *******************************/
void
GarpAlgorithm::_getConfiguration( ConfigurationPtr& config ) const
{

  // Only get the Model portion.  Since the parameter portion is
  // handled by the base class AlgorithmImpl.
  if ( !done() )
    return;

  // Const hack.
  // To accomodate other's const-incorrectness.
  RuleSet* rs = const_cast<RuleSet*>( & objBest );

  ConfigurationPtr model_config ( new ConfigurationImpl("Garp") );
  config->addSubsection( model_config );

  const int rule_count = rs->size();

  model_config->addNameValue( "ActiveGenes", iActiveGenes );
  model_config->addNameValue( "RuleCount", rule_count );
  
  for ( int i=0; i<rule_count; i++ ) {

    //Log::instance()->debug( "Gettting Rule %d\n",i );

    Rule *rule = rs->get(i);

    ConfigurationPtr rule_config ( new ConfigurationImpl("Rule") );
    model_config->addSubsection( rule_config );

    char type[2];
    sprintf(type, "%c", rule->type() );
    rule_config->addNameValue( "Type", type );

    rule_config->addNameValue( "Performance", rule->dblPerformance, 10 );

    rule_config->addNameValue( "Genes", rule->Gene, rule->intLength );

  }
  

}

void
GarpAlgorithm::_setConfiguration( const ConstConfigurationPtr& config )
{

  ConstConfigurationPtr model_config = config->getSubsection( "Garp", false );

  if (!model_config)
    return;

  Doneflag = true;
  //
  // Clear out the rule set.
  objBest.clear();

  iActiveGenes = model_config->getAttributeAsInt( "ActiveGenes", 0 );

  objBest.setDimension( iActiveGenes );

  Configuration::subsection_list::const_iterator ss;
  for( ss = model_config->getAllSubsections().begin();
       ss != model_config->getAllSubsections().end();
       ++ss ) {

    const ConstConfigurationPtr& c(*ss);

    std::string type = c->getAttribute( "Type" );
    
    double *perf;
    c->getAttributeAsDoubleArray( "Performance", &perf, 0 );

    int n_genes;
    unsigned char *p_genes;
    c->getAttributeAsByteArray( "Genes", &p_genes, &n_genes );

    Rule * newRule=0;
    switch( type [0] ) {

    case 'a':
      newRule = new AtomicRule();
      newRule->RestoreRule( perf, p_genes, n_genes, iGeneIndex );
      break;

    case 'd':
      newRule = new RangeRule();
      newRule->RestoreRule( perf, p_genes, n_genes, iGeneIndex );
      break;

    case 'r':
      newRule = new LogitRule();
      newRule->RestoreRule( perf, p_genes, n_genes, iGeneIndex );
      break;

    case '!':
      newRule = new NegatedRangeRule();
      newRule->RestoreRule( perf, p_genes, n_genes, iGeneIndex );
      break;

    }

    objBest.add(newRule);
    delete [] p_genes;
    delete [] perf;
  }

  return;

}
