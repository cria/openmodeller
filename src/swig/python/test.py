import om;

ctrl = om.ControlInterface();
print "Welcome to openModeller Python Binding test.\n";
print "Using openModeller version:     ", ctrl.getVersion();
print "Loading algorithms from:        ", ctrl.getPluginPath();
print "Algorithms loaded:              ", ctrl.loadAlgorithms();
print "Number of available algorithms: ", ctrl.numAvailableAlgorithms();

md = ctrl.algorithmMetadata("GARP v3.0.1 alpha");
#md = ctrl.algorithmMetadata("CSM v0.1 alpha");

print "Algorithm info: ";
print "Id:             ", md.id;
print "Name:           ", md.name;
print "Version:        ", md.version;
print "Author:         ", md.author;
print "Contact:        ", md.contact;

maps = ["aspect", "dem", "slope", "dtr", "prec", "temp", "wet"];

params = [["a","b"], ["c","d"], ["e","f"]];

#params = om.AlgParameter();
#params.setName("MaxGenerations");
#params.setValue(100);

hdr = om.Header();

hdr.xdim = 360;
hdr.ydim = 180;
hdr.xmin = -180;
hdr.ymin = -90;
hdr.xmax = 180;
hdr.ymax = 90;
hdr.noval = 0;
hdr.nband = 1;

print "\nInitializing algorithm.";
print "Setting Environment object. ", ctrl.setEnvironment(0, [], 7, maps, "mask");
print "Setting Output format.      ", ctrl.setOutputMap("map.tif", "mask", 255);
#print "Setting Output format.      ", ctrl.setOutputMap_Header("map.tif", hdr, 255);
print "Setting algorithm params.   ", ctrl.setAlgorithm(md.id, 0, params);
print "Reading occurrences.        ", 0;
print "Running algorithm.          ", 0;

print "Finalizing algorithm";

print "\nThanks for using openModeller.";
print "Good bye!\n";

