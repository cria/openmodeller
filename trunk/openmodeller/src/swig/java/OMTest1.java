/* 
 *
 *
 *
 */

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
	int i, n;

	AlgMetadata algmd[];
	
	algmd = mod.availableAlgorithms();
	n = mod.numAvailableAlgorithms();
	for (i = 0; i < n; i++) {
	    // print info about one algorithm
	    System.out.println("ID:         " + algmd[i].getId());
	    System.out.println("Name:       " + algmd[i].getName());
	    System.out.println("Version:    " + algmd[i].getVersion());
	    System.out.println("Overview:   " + algmd[i].getOverview());
	    System.out.println("Parameters: " + algmd[i].getNparam());

	    AlgParamMetadata parammd = algmd[i].getParam();
	    if (parammd != null)
		System.out.println("First Param ID: " + parammd.getId());
	    
	    AlgMetadata algmd2 = mod.algorithmMetadata(algmd[i].getId());
	    
	    System.out.println("*ID:         " + algmd2.getId());
	    System.out.println("*Name:       " + algmd2.getName());
	    System.out.println("*Version:    " + algmd2.getVersion());
	    System.out.println("*Overview:   " + algmd2.getOverview());
	    System.out.println("*Parameters: " + algmd2.getNparam());

	    parammd = algmd2.getParam();
	    if (parammd != null)
		System.out.println("*First Param ID: " + parammd.getId());
	    
	}
    }

    public static void printFooter() {
	System.out.println("Thanks for using openModeller!");
	System.out.println("Good bye!");
    }
}
