/**
 * Data used to test GarpRule::evaluate()
 * 
 * @file   test_rules_evaluate_data.cpp
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

struct HardcodedSample
{
  int resamples;
  int dimension;
  Scalar * samples;
};

#define NUM_SAMPLE_SETS 2

Scalar SampleSetValues0[84][3] =  
{   
 { +0.0,  -1.0000,  -1.0000 }, //   0
 { +0.0,  -1.0000,  -1.0000 }, //   1
 { +0.0,  -0.9000,  -0.9000 }, //   2
 { +0.0,  -0.9000,  -0.9000 }, //   3
 { +0.0,  -0.8000,  -0.8000 }, //   4
 { +0.0,  -0.8000,  -0.8000 }, //   5
 { +0.0,  -0.7000,  -0.7000 }, //   6
 { +0.0,  -0.7000,  -0.7000 }, //   7
 { +0.0,  -0.6000,  -0.6000 }, //   8
 { +0.0,  -0.6000,  -0.6000 }, //   9
 { +0.0,  -0.5000,  -0.5000 }, //  10
 { +0.0,  -0.5000,  -0.5000 }, //  11
 { +0.0,  -0.4000,  -0.4000 }, //  12
 { +0.0,  -0.4000,  -0.4000 }, //  13
 { +0.0,  -0.3000,  -0.3000 }, //  14
 { +0.0,  -0.3000,  -0.3000 }, //  15
 { +0.0,  -0.2000,  -0.2000 }, //  16
 { +0.0,  -0.2000,  -0.2000 }, //  17
 { +0.0,  -0.1000,  -0.1000 }, //  18
 { +0.0,  -0.1000,  -0.1000 }, //  19
 { +0.0,  -0.0000,  -0.0000 }, //  20
 { +0.0,  -0.0000,  -0.0000 }, //  21
 { +0.0,  +0.1000,  +0.1000 }, //  22
 { +0.0,  +0.1000,  +0.1000 }, //  23
 { +0.0,  +0.2000,  +0.2000 }, //  24
 { +0.0,  +0.2000,  +0.2000 }, //  25
 { +0.0,  +0.3000,  +0.3000 }, //  26
 { +0.0,  +0.3000,  +0.3000 }, //  27
 { +0.0,  +0.4000,  +0.4000 }, //  28
 { +0.0,  +0.4000,  +0.4000 }, //  29
 { +0.0,  +0.5000,  +0.5000 }, //  30
 { +0.0,  +0.5000,  +0.5000 }, //  31
 { +0.0,  +0.6000,  +0.6000 }, //  32
 { +0.0,  +0.6000,  +0.6000 }, //  33
 { +0.0,  +0.7000,  +0.7000 }, //  34
 { +0.0,  +0.7000,  +0.7000 }, //  35
 { +0.0,  +0.8000,  +0.8000 }, //  36
 { +0.0,  +0.8000,  +0.8000 }, //  37
 { +0.0,  +0.9000,  +0.9000 }, //  38
 { +0.0,  +0.9000,  +0.9000 }, //  39
 { +0.0,  +1.0000,  +1.0000 }, //  40
 { +0.0,  +1.0000,  +1.0000 }, //  41
 { +1.0,  -1.0000,  -1.0000 }, //  42
 { +1.0,  -1.0000,  -1.0000 }, //  43
 { +1.0,  -0.9000,  -0.9000 }, //  44
 { +1.0,  -0.9000,  -0.9000 }, //  45
 { +1.0,  -0.8000,  -0.8000 }, //  46
 { +1.0,  -0.8000,  -0.8000 }, //  47
 { +1.0,  -0.7000,  -0.7000 }, //  48
 { +1.0,  -0.7000,  -0.7000 }, //  49
 { +1.0,  -0.6000,  -0.6000 }, //  50
 { +1.0,  -0.6000,  -0.6000 }, //  51
 { +1.0,  -0.5000,  -0.5000 }, //  52
 { +1.0,  -0.5000,  -0.5000 }, //  53
 { +1.0,  -0.4000,  -0.4000 }, //  54
 { +1.0,  -0.4000,  -0.4000 }, //  55
 { +1.0,  -0.3000,  -0.3000 }, //  56
 { +1.0,  -0.3000,  -0.3000 }, //  57
 { +1.0,  -0.2000,  -0.2000 }, //  58
 { +1.0,  -0.2000,  -0.2000 }, //  59
 { +1.0,  -0.1000,  -0.1000 }, //  60
 { +1.0,  -0.1000,  -0.1000 }, //  61
 { +1.0,  -0.0000,  -0.0000 }, //  62
 { +1.0,  -0.0000,  -0.0000 }, //  63
 { +1.0,  +0.1000,  +0.1000 }, //  64
 { +1.0,  +0.1000,  +0.1000 }, //  65
 { +1.0,  +0.2000,  +0.2000 }, //  66
 { +1.0,  +0.2000,  +0.2000 }, //  67
 { +1.0,  +0.3000,  +0.3000 }, //  68
 { +1.0,  +0.3000,  +0.3000 }, //  69
 { +1.0,  +0.4000,  +0.4000 }, //  70
 { +1.0,  +0.4000,  +0.4000 }, //  71
 { +1.0,  +0.5000,  +0.5000 }, //  72
 { +1.0,  +0.5000,  +0.5000 }, //  73
 { +1.0,  +0.6000,  +0.6000 }, //  74
 { +1.0,  +0.6000,  +0.6000 }, //  75
 { +1.0,  +0.7000,  +0.7000 }, //  76
 { +1.0,  +0.7000,  +0.7000 }, //  77
 { +1.0,  +0.8000,  +0.8000 }, //  78
 { +1.0,  +0.8000,  +0.8000 }, //  79
 { +1.0,  +0.9000,  +0.9000 }, //  80
 { +1.0,  +0.9000,  +0.9000 }, //  81
 { +1.0,  +1.0000,  +1.0000 }, //  82
 { +1.0,  +1.0000,  +1.0000 }, //  83
}; 

// rules to be tested with this sample set
Scalar RulePred1_1 = 1.0; // range rule
Scalar RuleGenes1_1[4] = {-0.51, +0.51, -0.51, +0.51};

Scalar RulePerfs1_1[10] = {-1000,        
			   0.5238095238,  // pXs/n = 44/84
			   0.5000,        // pYs/n = 42/84
			   -1000,  
			   0.5238095238,  // no/n = pXs/n = 44/84
			   0.5000,        // pXYs/no = 22/44
			   -1000,
			   0.5238095238,  // no/n = 44/84
			   0.0,           // significance
 			   -1000
};

Scalar RulePred1_2 = 0.0; // range rule
Scalar RuleGenes1_2[4] = {-0.51, +0.51, -0.51, +0.51};

Scalar RulePerfs1_2[10] = {-1000,        
			   0.5238095238,  // pXs/n = 44/84
			   0.5000,        // pYs/n = 42/84
			   -1000,  
			   0.5238095238,  // no/n = pXs/n = 44/84
			   0.5000,        // pXYs/no = 22/44
			   -1000,
			   0.5238095238,  // no/n = 44/84
			   0.0,           // significance
 			   -1000
};

Scalar RulePred1_3 = 1.0; // negated range rule
Scalar RuleGenes1_3[4] = {-0.51, +0.51, -0.51, +0.51};

Scalar RulePerfs1_3[10] = {-1000,        
			   0.4761904762,  // pXs/n = 40/84
			   0.5000,        // pYs/n = 42/84
			   -1000,  
			   0.4761904762,  // no/n = pXs/n = 40/84
			   0.5000,        // pXYs/no = 20/40
			   -1000,
			   0.4761904762,  // no/n = 40/84
			   0.0,           // significance
 			   -1000
};

Scalar RulePred1_4 = 0.0; // negated range rule
Scalar RuleGenes1_4[4] = {-0.51, +0.51, -0.51, +0.51};

Scalar RulePerfs1_4[10] = {-1000,        
			   0.4761904762,  // pXs/n = 40/84
			   0.5000,        // pYs/n = 42/84
			   -1000,  
			   0.4761904762,  // no/n = pXs/n = 40/84
			   0.5000,        // pXYs/no = 20/40
			   -1000,
			   0.4761904762,  // no/n = 40/84
			   0.0,           // significance
 			   -1000
};



Scalar SampleSetValues1[84][3] =  
{   
 { +0.0,  -1.0000,  -1.0000 }, //   0
 { +0.0,  -0.9000,  -0.9000 }, //   1
 { +0.0,  -0.8000,  -0.8000 }, //   2
 { +0.0,  -0.7000,  -0.7000 }, //   3
 { +0.0,  -0.6000,  -0.6000 }, //   4
 { +0.0,  -0.5000,  -0.5000 }, //   5
 { +0.0,  -0.4000,  -0.4000 }, //   6
 { +0.0,  -0.3000,  -0.3000 }, //   7
 { +0.0,  -0.2000,  -0.2000 }, //   8
 { +0.0,  -0.1000,  -0.1000 }, //   9
 { +0.0,  -0.0000,  -0.0000 }, //  10
 { +0.0,  +0.1000,  +0.1000 }, //  11
 { +0.0,  +0.2000,  +0.2000 }, //  12
 { +0.0,  +0.3000,  +0.3000 }, //  13
 { +0.0,  +0.4000,  +0.4000 }, //  14
 { +0.0,  +0.5000,  +0.5000 }, //  15
 { +0.0,  +0.6000,  +0.6000 }, //  16
 { +0.0,  +0.7000,  +0.7000 }, //  17
 { +0.0,  +0.8000,  +0.8000 }, //  18
 { +0.0,  +0.9000,  +0.9000 }, //  19
 { +0.0,  +1.0000,  +1.0000 }, //  20
 { +1.0,  -1.0000,  -1.0000 }, //  21
 { +1.0,  -1.0000,  -1.0000 }, //  22
 { +1.0,  -0.9000,  -0.9000 }, //  23
 { +1.0,  -0.9000,  -0.9000 }, //  24
 { +1.0,  -0.8000,  -0.8000 }, //  25
 { +1.0,  -0.8000,  -0.8000 }, //  26
 { +1.0,  -0.7000,  -0.7000 }, //  27
 { +1.0,  -0.7000,  -0.7000 }, //  28
 { +1.0,  -0.6000,  -0.6000 }, //  29
 { +1.0,  -0.6000,  -0.6000 }, //  30
 { +1.0,  -0.5000,  -0.5000 }, //  31
 { +1.0,  -0.5000,  -0.5000 }, //  32
 { +1.0,  -0.4000,  -0.4000 }, //  33
 { +1.0,  -0.4000,  -0.4000 }, //  34
 { +1.0,  -0.3000,  -0.3000 }, //  35
 { +1.0,  -0.3000,  -0.3000 }, //  36
 { +1.0,  -0.2000,  -0.2000 }, //  37
 { +1.0,  -0.2000,  -0.2000 }, //  38
 { +1.0,  -0.1000,  -0.1000 }, //  39
 { +1.0,  -0.1000,  -0.1000 }, //  40
 { +1.0,  -0.0000,  -0.0000 }, //  41
 { +1.0,  -0.0000,  -0.0000 }, //  42
 { +1.0,  +0.1000,  +0.1000 }, //  43
 { +1.0,  +0.1000,  +0.1000 }, //  44
 { +1.0,  +0.2000,  +0.2000 }, //  45
 { +1.0,  +0.2000,  +0.2000 }, //  46
 { +1.0,  +0.3000,  +0.3000 }, //  47
 { +1.0,  +0.3000,  +0.3000 }, //  48
 { +1.0,  +0.4000,  +0.4000 }, //  49
 { +1.0,  +0.4000,  +0.4000 }, //  50
 { +1.0,  +0.5000,  +0.5000 }, //  51
 { +1.0,  +0.5000,  +0.5000 }, //  52
 { +1.0,  +0.6000,  +0.6000 }, //  53
 { +1.0,  +0.6000,  +0.6000 }, //  54
 { +1.0,  +0.7000,  +0.7000 }, //  55
 { +1.0,  +0.7000,  +0.7000 }, //  56
 { +1.0,  +0.8000,  +0.8000 }, //  57
 { +1.0,  +0.8000,  +0.8000 }, //  58
 { +1.0,  +0.9000,  +0.9000 }, //  59
 { +1.0,  +0.9000,  +0.9000 }, //  60
 { +1.0,  +1.0000,  +1.0000 }, //  61
 { +1.0,  +1.0000,  +1.0000 }  //  62
};

// rules to be tested with this sample set
Scalar RulePred2_1 = 1.0; // range rule
Scalar RuleGenes2_1[4] = {-0.51, +0.51, -0.51, +0.51};

Scalar RulePerfs2_1[10] = {-1000,        
			   0.5238095238,  // pXs/n = 33/63
			   0.6666666667,  // pYs/n = 42/63
			   -1000,  
			   0.5238095238,  // no/n = pXs/n = 33/63
			   0.6666666667,  // pXYs/no = 22/33
			   -1000,
			   0.5238095238,  // no/n = 44/84
			   0.0,           // significance
 			   -1000
};

Scalar RulePred2_2 = 0.0; // range rule
Scalar RuleGenes2_2[4] = {-0.51, +0.51, -0.51, +0.51};

Scalar RulePerfs2_2[10] = {-1000,        
			   0.5238095238,  // pXs/n = 33/63
			   0.3333333333,  // pYs/n = 21/63
			   -1000,  
			   0.5238095238,  // no/n = pXs/n = 33/63
			   0.3333333333,  // pXYs/no = 11/33
			   -1000,
			   0.5238095238,  // no/n = 44/84
			   0.0,           // significance
 			   -1000
};

// rules to be tested with this sample set
Scalar RulePred2_3 = 1.0; // range rule
Scalar RuleGenes2_3[4] = {-1.01, +1.01, -1.01, +1.01};

Scalar RulePerfs2_3[10] = {-1000,        
			   1.0000000000,  // pXs/n = 63/63
			   0.6666666667,  // pYs/n = 42/63
			   -1000,  
			   1.0000000000,  // no/n = pXs/n = 63/63
			   0.6666666667,  // pXYs/no = 42/63
			   -1000,
			   1.0000000000,  // no/n = 63/63
			   0.0,           // significance
 			   -1000
};

Scalar RulePred2_4 = 0.0; // range rule
Scalar RuleGenes2_4[4] = {-1.01, +1.01, -1.01, +1.01};

Scalar RulePerfs2_4[10] = {-1000,        
			   1.0000000000,  // pXs/n = 63/63
			   0.3333333333,  // pYs/n = 21/63
			   -1000,  
			   1.0000000000,  // no/n = pXs/n = 63/63
			   0.3333333333,  // pXYs/no = 21/63
			   -1000,
			   1.0000000000,  // no/n = 63/63
			   0.0,           // significance
 			   -1000
};



struct HardcodedSample SamplesSet[NUM_SAMPLE_SETS] = 
  {
    { 84, 2, (Scalar *) SampleSetValues0 },
    { 63, 2, (Scalar *) SampleSetValues1 }
  };

