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

    public static void main2(String argv[]) {
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

	AlgMetadata[] algmd;
	
	algmd = mod.availableAlgorithms();
	n = mod.numAvailableAlgorithms();
	for (i = 0; i < n; i++) {
	    // print info about one algorithm
	    System.out.println("ID:" + algmd[i].setId());
	    /*
	    System.out.println("Name: ");
	    System.out.println("Author: ");
	    System.out.println("Implementation Author: ");
	    System.out.println("Contact:");
	    System.out.println("");
	    */
	    System.out.println("");
	}
    }

    public static void printFooter() {
	System.out.println("Thanks for using openModeller!");
	System.out.println("Good bye!");
    }


    public static void main(String argv[]) {
	String animals[] = {"Cat","Dog","Cow","Goat"};
	om.print_args(animals);
	String args[] = om.get_args();
	for (int i=0; i<args.length; i++)
	    System.out.println(i + ":" + args[i]);
  }
}
