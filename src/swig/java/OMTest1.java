/* 
 *
 *
 *
 */

import br.org.cria.OpenModeller.*;

public class OMTest1 {

    static OpenModeller mod;

    static {
	System.loadLibrary("omjava");

	mod = new OpenModeller();
	if (mod == null) {
	    System.out.println("Could not load omjava library.");
	}
    }

    public static void main(String argv[]) {
	printHeader();
	printOmInfo();
	printAlgInfo();
	printFooter();
    }

    public static void printHeader() {
	System.out.println("Welcome to openModeller!");
	System.out.println("This is the OM Java binding");
	System.out.println("");
    }

    public static void printOmInfo() {
	System.out.println("Version:              " + mod.getVersion());
	System.out.println("Plugin Path:          " + mod.getPluginPath());
	System.out.println("Algorithms loaded:    " + mod.loadAlgorithms());
	System.out.println("Number of algorithms: " + mod.numAvailableAlgorithms());
	System.out.println("");
    }

    public static void printAlgInfo() {
	int i, j, n;

	AlgMetadata algmd[];
	
	algmd = mod.availableAlgorithms();
	n = mod.numAvailableAlgorithms();
	for (i = 0; i < n; i++) {
	    // print info about one algorithm
	    System.out.println("===============================");
	    System.out.println("ID:         " + algmd[i].getId());
	    System.out.println("Name:       " + algmd[i].getName());
	    System.out.println("Version:    " + algmd[i].getVersion());
	    System.out.println("Overview:   " + algmd[i].getOverview());

	    AlgMetadata algmd2 = mod.algorithmMetadata(algmd[i].getId());

	    AlgParamMetadata[] parammdArray = om.getParameterList(algmd2);

	    System.out.println("Parameters: " + parammdArray.length);

	    for (j = 0; j < parammdArray.length; j++) {
		AlgParamMetadata parammd = parammdArray[j];
		System.out.println(" - - - - - - - - - - - -");
		System.out.println(" Parameter " + (j + 1) + ": " + parammd.getId());
		System.out.println(" Name:        " + parammd.getName());
		System.out.println(" Type:        " + parammd.getType());
		System.out.println(" Overview:    " + parammd.getOverview());
		System.out.println(" Description: " + parammd.getDescription());

		if (parammd.getHas_min() == 0) {
		    System.out.println(" Lower bound:  " + parammd.getMin());
		}
		else {
		    System.out.println(" No lower bound. ");
		}

		if (parammd.getHas_max() == 0) {
		    System.out.println(" Upper bound: " + parammd.getMax());
		}
		else {
		    System.out.println(" No upper bound. ");
		}
	    }

	    System.out.println("===============================");
	}
    }

    public static void printFooter() {
	System.out.println("Thanks for using openModeller!");
	System.out.println("Good bye!");
    }
}
