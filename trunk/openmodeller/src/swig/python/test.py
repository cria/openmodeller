import om

# You'll need this instance for sure...
mod = om.OpenModeller()

# Some basic information
print "\nWelcome to the openModeller Python Binding test\n"
print "openModeller version", mod.getVersion()

# Loop over algorithms to collect their names
algList = mod.availableAlgorithms()

strAlgList = ""

for i in range(0, len(algList)):
    alg = algList[i]
    if i > 0:
        strAlgList += ", "
    strAlgList += alg.name

print "\n", mod.numAvailableAlgorithms(), "algorithm(s) available: ", strAlgList

# Display all metadata for a particular algorithm

algId = 'GARP'
print "\nMetadata of a particular algorithm:\n"

alg = mod.algorithmMetadata(algId)

acceptsCategoricalMaps = "no"

if (alg.categorical):
    acceptsCategoricalMaps = "yes"

needsAbsencePoints = "no"

if (alg.absence):
    needsAbsencePoints = "yes"

print "Id:", alg.id
print "Name:", alg.name
print "Version:", alg.version
print "Algorithm designer(s):", alg.author
print "Developer(s):", alg.code_author
print "Developer(s) contact:", alg.contact
print "Bibliography:", alg.biblio
print "Overview:", alg.overview
print "Description:", alg.description
print "Accepts categorical maps:", acceptsCategoricalMaps
print "Needs absence points:", needsAbsencePoints

print "\nParameters:"

paramList = alg.getParameterList()
for j in range(0, len(paramList)):
    param = paramList[j]
    
    if (param.has_min):
        min = param.min_val
    else:
        min = "unbounded"

    if (param.has_max):
        max = param.max_val
    else:
        max = "unbounded"

    print "\nParameter Id:", param.id
    print "Name:", param.name
    print "Overview:", param.overview
    print "Description:", param.description
    print "Type:", param.type
    print "Min:", min
    print "Max:", max
    print "Default:", param.typical

print "\nSample experiment:"

print "\nSetting occurrences..."

wktsys = "GEOGCS[\"WGS84\", DATUM[\"WGS84\", SPHEROID[\"WGS84\", 6378137.0, 298.257223563]], PRIMEM[\"Greenwich\", 0.0], UNIT[\"degree\", 0.017453292519943295], AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]"
spfile = "../../../examples/furcata_boliviana.txt"
spname = "Furcata boliviana"

presences = om.readPresences(spfile, wktsys, spname)
#absences = om.readAbsences(spfile, wktsys, spname)
mod.setOccurrences(presences)
#mod.setOccurrences(presences, absences)

print "Setting Environment..."

layers_dir = "../../../examples/"

maps = [layers_dir+"rain_coolest.tif", layers_dir+"temp_avg.tif"]

inputMask = layers_dir+"rain_coolest.tif"

mod.setEnvironment([], maps, inputMask)

print "Setting Algorithm..."

params = [["MaxGenerations",   "25"], 
          ["ConvergenceLimit", "0.0"],
          ["PopulationSize",   "50"],
          ["Resamples",        "2500"]]

mod.setAlgorithm( "GARP", params)

print "Creating model..."

# Create Model
mod.createModel()

# Display some statistics about the model
matrix = mod.getConfusionMatrix();

roc = mod.getRocCurve();

print "\nModel statistics"
print "Accuracy:          ", matrix.getAccuracy() * 100, "%"
print "Omission error:    ", matrix.getOmissionError() * 100, "%"

commission = matrix.getCommissionError()

if ( commission != -1 ):
    print "Commission error:  ", commission * 100, "%"
    
print "AUC:               ", roc.getTotalArea()

print "Projecting model..."

# Project Map

# note1: native projection - automatically get same layers of model creation)
# note2: no mask specified - automatically takes first map as a mask)
# note3: no output format specified (automatically takes mask format - cell size & projection)
# note4: no output file type specified (automatically creates ERDAS imagine map)

mod.createMap("map.img")

# If you want to specify an output format (a map file used as a template from where
# cell size and projection will be taken, and that can also specify the output file type),
# use the following commands instead:

#outputFormat = om.MapFormat(layers_dir+"temp_avg.tif")
#outputFormat.setFormat(0) # FloatingTiff = 0, GreyTiff = 1, GreyBMP = 2, FloatingHFA = 3
#mod.createMap("map.tif", outputFormat)

# If you want to specify a different projection scenario (same input variables, in the same order
# but related to a different geographic region or to a different period) use the following
# commands instead:

# note: Here you can also specify a different output mask

#outputFormat = om.MapFormat(layers_dir+"temp_avg.tif")
#outputFormat.setFormat(1) # FloatingTiff = 0, GreyTiff = 1, GreyBMP = 2, FloatingHFA = 3
#projMaps = [layers_dir+"rain_coolest.tif", layers_dir+"temp_avg.tif"] # You should change this
#outputMask = layers_dir+"temp_avg.tif"
#projEnv = om.makeEnvironment([], projMaps, outputMask)
#mod.createMap(projEnv, "map.tif", outputFormat)

# Another projection option without specifying output format
#mod.createMap(projEnv, "map.tif")

print "Done!\n"

