import om;

mod = om.OpenModeller();
print "Welcome to openModeller Python Binding test.\n";
print "Using openModeller version:     ", mod.getVersion();
print "Loading algorithms from:        ", mod.getPluginPath();
print "Algorithms loaded:              ", mod.loadAlgorithms();
print "Number of available algorithms: ", mod.numAvailableAlgorithms();

#md = mod.algorithmMetadata("MinimumDistance v0.1");
md = mod.algorithmMetadata("GARP v3.0.1 alpha");
#md = mod.algorithmMetadata("CSM v0.1 alpha");

print "Algorithm info: ";
print "Id:             ", md.id;
print "Name:           ", md.name;
print "Version:        ", md.version;
print "Author:         ", md.author;
print "Contact:        ", md.contact;

maps = ["aspect", "dem", "slope", "dtr", "prec", "temp", "wet"];

params = [["MaxGenerations",   "50"], 
          ["ConvergenceLimit", "0.0"],
          ["PopulationSize",   "50"],
          ["Resamples",        "2500"],
          ["MutationRate",     "0.25"],
          ["CrossoverRate",    "0.25"]];

hdr = om.Header();
#hdr = om.Header_Params(360, 180, -180, -90, 180, 90, 0, 1);

hdr.xdim = 360;
hdr.ydim = 180;
hdr.xmin = -180;
hdr.ymin = -90;
hdr.xmax = 180;
hdr.ymax = 90;
hdr.noval = 0;
hdr.nband = 1;

wktsys = "GEOGCS[\"WGS84\", DATUM[\"WGS84\", SPHEROID[\"WGS84\", 6378137.0, 298.257223563]], PRIMEM[\"Greenwich\", 0.0], UNIT[\"degree\", 0.017453292519943295], AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]";
spfile = "Strix_varia.txt";
spname = "Strix varia";

ocfile = om.OccurrencesFile(spfile, wktsys);
ocfile.tail();
occurr = ocfile.remove(spname);

print "\nInitializing algorithm.";
print "Setting Environment object. ", mod.setEnvironment(0, [], 7, maps, "mask");
print "Setting Output format.      ", mod.setOutputMap("map.tif", "mask", 255);
#print "Setting Output format.      ", mod.setOutputMap_Header("map.tif", hdr, 255);
print "Setting algorithm params.   ", mod.setAlgorithm(md.id, 6, params);
print "Reading occurrences.        ", mod.setOccurrences(occurr);
print "Running algorithm.          ", mod.run();

print "Finalizing algorithm";

print "\nThanks for using openModeller.";
print "Good bye!\n";

