import om;

mod = om.OpenModeller();
print "Welcome to openModeller Python Binding test.\n";
print "This script loads all available OM algorithms and list all of their metadata.\n\n";

print "Using openModeller version:     ", mod.getVersion();
print "Loading algorithms from:        ", mod.getPluginPath();
print "Algorithms loaded:              ", mod.loadAlgorithms();
print "Number of available algorithms: ", mod.numAvailableAlgorithms();

algList = mod.availableAlgorithms();

for i in range(0, len(algList)):
    md = algList[i];

    print "==================================================";
    print "\nAlgorithm info: ";
    print "Id:             ", md.id;
    print "Name:           ", md.name;
    print "Version:        ", md.version;
    print "Author:         ", md.author;
    print "Contact:        ", md.contact;

    paramList = om.getParameterList(md);

    for j in range(0, len(paramList)):
        param = paramList[j];

        if (param.has_min):
            min = param.min
        else:
            min = "unbounded";

        if (param.has_max):
            max = param.max
        else:
            max = "unbounded";

        print "\n-------------";
        print "Parameter:   ", param.id;
        print "Name         ", param.name;
        print "Description: ", param.description;
        print "Type:        ", param.type;
        print "Min:         ", min;
        print "Max:         ", max;
        print "Default:     ", param.typical;


print "==================================================";
print "\nThanks for using openModeller.";
print "Good bye!\n";

