//import OpenModeller.ControlInterface;

public class Test
{
    public static void main(String argv[])
    {
	System.loadLibrary("omjava");

	ControlInterface mod = new ControlInterface();
	System.out.println("Welcome to openModeller Java Binding test.");
	System.out.println("Using openModeller version:     " + mod.getVersion());
	System.out.println("Loading algorithms from:        " + mod.getPluginPath());
	System.out.println("Algorithms loaded:              " + mod.loadAlgorithms());
	System.out.println("Number of available algorithms: " + mod.numAvailableAlgorithms());

	AlgMetadata md = mod.algorithmMetadata("GARP v3.0.1 alpha");
        //AlgMetadata md = mod.algorithmMetadata("CSM v0.1 alpha");

	System.out.println("Algorithm info: ");
	System.out.println("Id:             " + md.getId());
	System.out.println("Name:           " + md.getName());
	System.out.println("Version:        " + md.getVersion());
	System.out.println("Author:         " + md.getAuthor());
	System.out.println("Contact:        " + md.getContact());

	String categs[] = {};
	String maps[] = {"aspect", "dem", "slope", "dtr", "prec", "temp", "wet"};

	Header hdr = new Header();
	
	String wktsys = "GEOGCS[\"WGS84\", DATUM[\"WGS84\", SPHEROID[\"WGS84\", 6378137.0, 298.257223563]], PRIMEM[\"Greenwich\", 0.0], UNIT[\"degree\", 0.017453292519943295], AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]";
	String spfile = "Strix_varia.txt";
	String spname = "Strix varia";
	
	//OccurrencesFile ocfile = new OccurrencesFile(spfile, wktsys);
	//ocfile.tail();
	//SWIGTYPE_p_Occurrences occurr = ocfile.remove(spname);
	
	System.out.println("\nInitializing algorithm.");
	//System.out.println("Setting Environment object. " + mod.setEnvironment(0, categs, 7, maps, "mask"));
	//System.out.println("Setting Output format.      " + mod.setOutputMap("map.tif", "mask", 255));
	//System.out.println("Setting algorithm params.   " + mod.setAlgorithm(md.id, 6, params));
	//System.out.println("Reading occurrences.        " + mod.setOccurrences(occurr, null));
	//System.out.println("Running algorithm.          " + mod.run());

	System.out.println("Finalizing algorithm");

	System.out.println("\nThanks for using openModeller.");
	System.out.println("Good bye!\n");



	System.out.println("");
    }
}
