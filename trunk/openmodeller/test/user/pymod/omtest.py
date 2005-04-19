###############################################################################
# $Id$
# 
# Project:  OM User Test Suit
# Purpose:  Python Library supporting OM User Test Suite
# Authors:  Frank Warmerdam (original version from GDAL/OGR
#           Ricardo Scachetti Pereira (OM version)
# 
###############################################################################
# Copyright (c) 2003, Frank Warmerdam <warmerdam@pobox.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
###############################################################################
# 
# $Log$
# Revision 1.1  2005/04/19 14:24:23  scachett
# Reimplemented user tests using GDAL/OGR autotest as a template.
# Main changes:
#
# - Code is clearer.
# - It is simpler to add new tests. Just create a new directory and copy the sample test from ./samples directory.
# - HTML generation is now separated from test coding by using the Python htmltmpl templating engine.
#
# Also removed some obsolete Python test scripts.
#
#
#

import sys
import os
import string
import om
import csv
import ConfigParser

from htmltmpl import TemplateManager, TemplateProcessor


cur_name = 'default'

success_counter = 0
failure_counter = 0
blow_counter = 0
skip_counter = 0

reason = None

report_entry_set = []
map_stats_entry_set = []


def setup_run( name ):
    global success_counter, failure_counter, blow_counter, skip_counter
    global cur_name
    global report_entry_set, map_stats_entry_set
    
    cur_name = name


def run_tests( test_list ):
    global success_counter, failure_counter, blow_counter, skip_counter
    global cur_name, reason
    global report_entry_set, map_stats_entry_set

    for test_item in test_list:
        if test_item is None:
            continue

        try:
            (func, name, param) = test_item
            test_name = func.__name__ + ': ' + name
	except:
            func  = test_item
            name  = func.__name__
            param = None
	    test_name = name

        sys.stdout.write( '  TEST: ' + test_name + ' ... ' )
        sys.stdout.flush()
            
        reason = None
        map_stats = None
        try:
            (result, map_stats) = func(param)
            print result
        except:
            result = 'blowup'
            print result
            
            import traceback
            traceback.print_exc()


        if reason is not None:
            print '    ' + reason

        if result == 'success':
            success_counter = success_counter + 1
        elif result == 'fail':
            failure_counter = failure_counter + 1
        elif result == 'skip':
            skip_counter = skip_counter + 1
        else:
            blow_counter = blow_counter + 1

        # add one entry to the report summary
        links = (map_stats is not None)
        report_entry = get_report_entry( test_name, result,
                                         links, success_counter )

        if map_stats is None:
            map_stats = {}
            map_stats["exists"] = 0
        else:
            map_stats["TestName"] = test_name
            map_stats["Id"] = success_counter
            report_entry["ModelFilename"] = map_stats["ModelFilename"]

        report_entry_set.append( report_entry )
        map_stats_entry_set.append( map_stats )


def post_reason( msg ):
    global reason

    reason = msg


def summarize():
    global success_counter, failure_counter, blow_counter, skip_counter
    global cur_name
    global report_entry_set, map_stats_entry_set
    
    print
    print 'Test Script: %s' % cur_name
    print 'Succeeded: %d' % success_counter
    print 'Failed:    %d (%d blew exceptions)' \
          % (failure_counter+blow_counter, blow_counter)
    print 'Skipped:   %d' % skip_counter
    print


    # Compile or load already precompiled template.
    template = TemplateManager().prepare("SummaryReport.tmpl")
    tproc = TemplateProcessor()

    # Set the title.
    mod = om.OpenModeller()
    tproc.set("om_version", mod.getVersion())
    tproc.set("om_num_algs", mod.numAvailableAlgorithms())

    # htmltmpl requires var name starting with uppercase (!?!)
    Entries = report_entry_set
    tproc.set("Entries", Entries)
    
    # get list of algorithms
    Algorithms = []
    algList = mod.availableAlgorithms()
    for i in range(0, len(algList)):
        alg = {}
        alg["class"] = "a"
        alg["id"] = algList[i].id
        alg["name"] = algList[i].name
        Algorithms.append(alg)

    tproc.set("Algorithms", Algorithms)
    
    # write the processed template to disk
    summary_file = file("SummaryReport.html", "w")
    summary_file.write( tproc.process(template) )
    summary_file.close()
    
    # Compile or load already precompiled template.
    template = TemplateManager().prepare("MapReport.tmpl")
    tproc = TemplateProcessor()

    # htmltmpl requires var name starting with uppercase (!?!)
    Mapstats = map_stats_entry_set
    tproc.set("Mapstats", Mapstats)
    
    # write the processed template to disk
    map_file = file("MapReport.html", "w")
    map_file.write( tproc.process(template) )
    map_file.close()
    
    
###############################################################################

def run_all( dirlist, option_list ):
    global report_entry_set

    for dir_name in dirlist:
        files = os.listdir(dir_name)

        old_path = sys.path
        sys.path.append('.')
        
        for file in files:
            if not file[-3:] == '.py':
                continue

            module = file[:-3]
            try:
                wd = os.getcwd()
                os.chdir( dir_name )
                
                exec "import " + module
                try:
                    exec "test_list = " + module + ".omtest_list"

                    print '\nRunning tests from %s/%s' % (dir_name,file)
                    report_entry_set.append( get_report_separator('%s/%s' % (dir_name, file) ) )
                    run_tests( test_list )
                except:
                    pass
                
                os.chdir( wd )

            except:
                os.chdir( wd )
                print '... failed to load %s ... skipping.' % file

                import traceback
                traceback.print_exc()


        # We only add the tool directory to the python path long enough
        # to load the tool files.
        sys.path = old_path

def approx_equal( a, b ):
    a = float(a)
    b = float(b)
    if a == 0 and b != 0:
        return 0

    if abs(b/a - 1.0) > .00000000001:
        return 0
    else:
        return 1
    

def get_report_separator( group_name ):
    report_entry = {}
    report_entry["group"] = 1
    report_entry["name"] = group_name
    return report_entry


def get_report_entry( test_name, result, links, id ):
    report_entry = {}
    report_entry["group"] = 0
    report_entry["name"] = test_name
    report_entry["result"] = result
    report_entry["links"] = links
    report_entry["id"] = id
    if result == 'success':
        report_entry["class"] = "a"
    elif result == 'skip':
        report_entry["class"] = "g"
    else:
        report_entry["class"] = "r"
    
    return report_entry


def get_map_results_obj( mod, presences, absences,
                         actual_filename, expected_filename, model_filename ):
    
    result = {}
    
    result["exists"] = 1
    
    result["Act_MapFilename"] = actual_filename
    result["Exp_MapFilename"] = expected_filename
    result["ModelFilename"] = model_filename
    
    # get map statistics
    act = mod.getActualAreaStats()
    est = mod.getEstimatedAreaStats(0.01)

    result["Act_TotalArea"] = str(act.getTotalArea())
    result["Est_TotalArea"] = str(est.getTotalArea())
    result["Act_AreaPresent"] = str(act.getAreaPredictedPresent())
    result["Est_AreaPresent"] = str(est.getAreaPredictedPresent())
    result["Act_PercPresent"] = str(round(100.0 * act.getAreaPredictedPresent() / act.getTotalArea(), 2))
    result["Est_PercPresent"] = str(round(100.0 * est.getAreaPredictedPresent() / est.getTotalArea(), 2))
    result["Act_AreaAbsent"] = str(act.getAreaPredictedAbsent())
    result["Est_AreaAbsent"] = str(est.getAreaPredictedAbsent())
    result["Act_PercAbsent"] = str(round(100.0 * act.getAreaPredictedAbsent() / act.getTotalArea(), 2))
    result["Est_PercAbsent"] = str(round(100.0 * est.getAreaPredictedAbsent() / est.getTotalArea(), 2))
    result["Act_AreaNonPredicted"] = str(act.getAreaNotPredicted())
    result["Est_AreaNonPredicted"] = str(est.getAreaNotPredicted())
    result["Act_PercNonPredicted"] = str(round(100.0 * act.getAreaNotPredicted() / act.getTotalArea(), 2))
    result["Est_PercNonPredicted"] = str(round(100.0 * est.getAreaNotPredicted() / est.getTotalArea(), 2))

    # get conf matrix
    matrix = om.ConfusionMatrix()
    matrix.calculate(mod.getEnvironment(),
                     mod.getAlgorithm().getModel(),
                     presences, absences)
    
    result["Matrix11"] = str(matrix.getValue(1, 1))
    result["Matrix10"] = str(matrix.getValue(1, 0))
    result["Matrix01"] = str(matrix.getValue(0, 1))
    result["Matrix00"] = str(matrix.getValue(0, 0))
    result["Accuracy"] = str(round(matrix.getAccuracy() * 100, 2))
    result["Omission"] = str(round(matrix.getOmissionError() * 100, 2))
    result["Commission"] = str(round(matrix.getCommissionError() * 100, 2))
    
    return result



def get_occurrences(filename, sppName, coordSys, sppNameColumn,
                    xCoordColumn, yCoordColumn, abundanceColumn):
    
    file = open(filename)
    csv_parser = csv.parser()
    
    # get point data from open file
    line = file.readline()

    presence = om.makeOccurrences(sppName, coordSys)
    absence = om.makeOccurrences(sppName, coordSys)

    while line:

        row = csv_parser.parse(string.lower(line))

        if row:
            validRow = False
            currSppName = row[sppNameColumn]

            try:
                x = float(row[xCoordColumn])
                y = float(row[yCoordColumn])

                if (abundanceColumn >= 0):
                    abundance = float(row[abundanceColumn])
                else:
                    abundance = 1.0
                    
                validRow = True

            except:
                pass

                
            if (validRow):
                
                # check whether it is the first row of a new species
                if (sppName == currSppName):

                    if (abundance == 0.0):
                        absence.createOccurrence(x, y, 0, 0.0, \
                                                 0, None, 0, None)
                    else:
                        presence.createOccurrence(x, y, 0, 1.0, \
                                                  0, None, 0, None)
                        
                 
        line = file.readline()
            
            
    file.close()

    #print "Total Number of Points: (" + \
    #      str(presence.numOccurrences()) + ", " + \
    #      str(absence.numOccurrences()) + ")"
    
    return (presence, absence)



def get_env_list(filename, section, base_dir):

    config = ConfigParser.ConfigParser()
    config.read(filename)

    # loop through the options in this section to
    # gather all Maps
    maps = []
    options = []
    values = []
    done = False
    currMapNumber = 1
    for optionName in config.options(section):
        optionValue = config.get(section, optionName)
        options.append(optionName)
        values.append(optionValue)

    while not done:
        currOption = "map " + str(currMapNumber)

        try:
            index = options.index(currOption)
        except:
            done = True

        if (not done):
            maps.append(os.path.join(base_dir, values[index]))
            currMapNumber = currMapNumber + 1
            

    mask = os.path.join(base_dir, config.get(section, "Mask"))

    return (maps, mask)


def get_alg_default_params(alg_id):

    mod = om.OpenModeller()

    #algParams = mod.algorithmMetadata(alg_id).getParameterList();

    algParamList = [];
    algList = mod.availableAlgorithms()
    for i in range(0, len(algList) - 1):
        if algList[i].id == alg_id:

            algParams = algList[i].getParameterList()
            for param in algParams:
                algParamList.append([param.id, param.typical])
    
            return algParamList

    return None
