import om;

params = [["MaxGenerations",   "50"], 
          ["ConvergenceLimit", "0.0"],
          ["PopulationSize",   "100"],
          ["Resamples",        "2500"],
          ["MutationRate",     "0.25"],
          ["CrossoverRate",    "0.25"]];

om.print_alg_params(6, params);
