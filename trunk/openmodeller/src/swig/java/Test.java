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

	System.out.println("");
    }
}
