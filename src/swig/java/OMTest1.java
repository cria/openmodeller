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

	SWIGTYPE_p_p_AlgMetadata algmd;
	AlgMetadata pAlgmd;
	
	algmd = mod.availableAlgorithms();
	pAlgmd = (AlgMetadata) algmd;
	n = mod.numAvailableAlgorithms();
	for (i = 0; i < n; i++) {
	    // print info about one algorithm
	    System.out.println("ID:" + pAlgmd.id());
	    /*
	    System.out.println("Name: ");
	    System.out.println("Author: ");
	    System.out.println("Implementation Author: ");
	    System.out.println("Contact:");
	    System.out.println("");
	    */
	    System.out.println("");
	    pAlgmd = om.NextAlgMetadata(pAlgmd);
	}
    }

    public static void printFooter() {
	System.out.println("Thanks for using openModeller!");
	System.out.println("Good bye!");
    }
}
