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

	AlgMetadata algmd_array[], algmd;
	
	algmd_array = mod.availableAlgorithms();
	n = mod.numAvailableAlgorithms();
	for (i = 0; i < n; i++) {
	    // print info about one algorithm
	    algmd = algmd_array[i];
	    System.out.println("ID:" + algmd.getId());
	    System.out.println("");
	}
    }

    public static void printFooter() {
	System.out.println("Thanks for using openModeller!");
	System.out.println("Good bye!");
    }
}
