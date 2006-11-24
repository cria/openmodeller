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

occurr = om.readOccurrences(spfile, wktsys, spname)
mod.setOccurrences(occurr)

print "Setting Environment..."

maps = ["../../../examples/rain_coolest.tif", "../../../examples/temp_avg.tif"]

inputMask = "../../../examples/rain_coolest.tif"

mod.setEnvironment([], maps, inputMask)

print "Setting Algorithm..."

params = [["MaxGenerations",   "25"], 
          ["ConvergenceLimit", "0.0"],
          ["PopulationSize",   "50"],
          ["Resamples",        "2500"]]

mod.setAlgorithm(alg.id, params)

# Create Model
mod.createModel()

# Project Map
mod.createMap(mod.getEnvironment(), "map.img")

print "Done!\n"

