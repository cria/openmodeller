/* 
 *
 *
 *
 */

import br.org.cria.OpenModeller.*;

public class OMTest3 {

    static OpenModeller _mod;

    static {
	System.loadLibrary("omjava");

	_mod = new OpenModeller();
	//_mod.setLogLevel( Log.Debug );
	if (_mod == null) {
	    System.out.println("Could not load omjava library.");
	}
    }

    public static void main(String argv[]) {
	printHeader();
	printOmInfo();
	runGarpModel();
	printFooter();
    }

    public static void printHeader() {
	System.out.println("Welcome to openModeller!");
	System.out.println("This is the OM Java binding");
	System.out.println("");
    }

    public static void printOmInfo() {
	System.out.println("Version:              " + _mod.getVersion());
	System.out.println("Plugin Path:          " + _mod.getPluginPath());
	System.out.println("Algorithms loaded:    " + _mod.loadAlgorithms());
	System.out.println("Number of algorithms: " + _mod.numAvailableAlgorithms());
	System.out.println("");
    }

    public static void runGarpModel() {

	String maps[] = {"data/aspect", "data/dem", "data/slope", "data/dtr", "data/prec", "data/temp", "data/wet"};

	String params[][] = {
	    {"MaxGenerations",   "200"}, 
	    {"ConvergenceLimit", "0.0"},
	    {"PopulationSize",   "50"},
	    {"Resamples",        "2500"},
	    {"MutationRate",     "0.25"},
	    {"CrossoverRate",    "0.25"}};

	String wktsys = "GEOGCS[\"WGS84\", DATUM[\"WGS84\", SPHEROID[\"WGS84\", 6378137.0, 298.257223563]], PRIMEM[\"Greenwich\", 0.0], UNIT[\"degree\", 0.017453292519943295], AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]";
	String spfile = "acc_test_data.txt";
	String spname = "Strix varia";

	OccurrencesFile ocfile = new OccurrencesFile(spfile, wktsys);
	ocfile.tail();
	SWIGTYPE_p_Occurrences occurr = ocfile.get(spname);

	System.out.println("Starting GARP model");

	System.out.println("Reading occurrences:        " + 
			   _mod.setOccurrences(occurr, null));
	System.out.println("Setting Environment object: " + 
			   _mod.setEnvironment(0, null, 7, maps, "data/mask"));
	System.out.println("Setting algorithm params:   " + 
			   _mod.setAlgorithm("GARP", 6, params));
	System.out.println("Running algorithm:          " + 
			   _mod.run());
	System.out.println("Setting output format:      " + 
			   _mod.setOutputMap(255, "map.tif", "data/mask", "data/mask"));
	System.out.println("Creating output map:        " + 
			   _mod.createMap(_mod.getEnvironment(), "map.tif", "data/mask"));

	System.out.println("Finalizing algorithm");
    }

    public static void printFooter() {
	System.out.println("Thanks for using openModeller!");
	System.out.println("Good bye!");
    }
}
