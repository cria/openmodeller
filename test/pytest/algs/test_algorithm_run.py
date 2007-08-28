#!/usr/bin/env python
###############################################################################
# $Id$
#
# Project:  OM User Test Suite
# Authors:  Frank Warmerdam (original version from GDAL/OGR
#           Ricardo Scachetti Pereira (OM version)
# 
###############################################################################
# Copyright (c) 2003, Frank Warmerdam <warmerdam@pobox.com>
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
###############################################################################
# 
#  $Log$
#  Revision 1.8  2006/11/27 13:27:20  rdg
#  Updated serialization method names.
#
#  Revision 1.7  2005/06/09 02:14:43  scachett
#  Merged branch-sampler-matrix-input2 to CVS HEAD.
#  This set of changes allow OM to take Occurrences objects already
#  filled with Sample objects. In this case OM doesn't need an Environment
#  object to get env data samples from.
#  This feature allows users to gather data from several different env datasets
#  (time-educated sampling) or from a table (depending on client implementation).
#
#

import os
import sys

sys.path.append( '../pymod' )

import omtest
import om

###############################################################################
def get_env_list(filename):

    return env_list


###############################################################################

def algorithm_run(args, options):
    nat_env_file = args[0]
    nat_env_name = args[1]
    alg_id = args[2]
    occ_file = args[3]
    spp_name = args[4]

    if not omtest.checkArguments(options, "algorithm", alg_id):
        return ('skip', None)

    coordSys = "GEOGCS[\"WGS84\", DATUM[\"WGS84\", SPHEROID[\"WGS84\", 6378137.0, 298.257223563]], PRIMEM[\"Greenwich\", 0.0], UNIT[\"degree\", 0.017453292519943295], AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]"

    (nat_env_list, nat_env_mask) = omtest.get_env_list(nat_env_file,
                                                       nat_env_name,
                                                       "../data")

    (presences, absences) = omtest.get_occurrences(occ_file, spp_name,
                                                   coordSys, 0, 1, 2, None)

    alg_params = omtest.get_alg_default_params(alg_id)

    filename_prefix = "run_%s_%s" % (alg_id, spp_name.replace(" ", "_"))
    bmp_filename = filename_prefix + ".bmp"
    xml_filename = filename_prefix + ".xml"
    expected_filename = filename_prefix + "_expected.bmp"

    mod = om.OpenModeller()
    mod.setOccurrences(presences, absences)
    mod.setEnvironment([], nat_env_list, nat_env_mask)
    mod.setAlgorithm(alg_id, alg_params)
    mod.run()

    cfg = mod.getModelConfiguration();
    om.Configuration.writeXml( cfg, xml_filename )

    mf = om.MapFormat()
    mf.setFormat( om.MapFormat.GreyBMP )
    mod.projectNativeRange(bmp_filename, mf)

    result = omtest.get_map_results_obj( mod, None, presences, absences,
                                         "algs/" + bmp_filename,
                                         "algs/" + expected_filename,
                                         "algs/" + xml_filename )

    return ('success', result)

    
###############################################################################

experiment_list = [
    ( "environmental_datasets.cfg", "CRU10",
      "../data/species/occ_data.csv", "peromyscus maniculatus" ),

    ( "environmental_datasets.cfg", "CRU10",
      "../data/species/occ_data.csv", "leucaena leucocephala" ),

    ( "environmental_datasets.cfg", "CRU10",
      "../data/species/occ_data.csv", "lathyrus japonicus" ),

    ( "environmental_datasets.cfg", "CRU10",
      "../data/species/occ_data.csv", "senna obtusifolia" ),

    ( "environmental_datasets.cfg", "CRU10",
      "../data/species/occ_data.csv", "strix varia" ),

    ( "environmental_datasets.cfg", "CRU10",
      "../data/species/occ_data.csv", "mephitis mephitis" ),

    ( "environmental_datasets.cfg", "CRU10",
      "../data/species/occ_data.csv", "ursus americanus" ) ]

    
mod = om.OpenModeller()
algmd = mod.availableAlgorithms()
num = mod.numAvailableAlgorithms()

omtest_list = []

# run every experiment for every algorithm available
for i in range(0, num):
    alg_id = algmd[i].id
    for exp in experiment_list:
        # append a list with function to be executed, test name
        # and parameters to be passed to function
        omtest_list.append(
            (algorithm_run, "%s: %s" % (alg_id, exp[3].capitalize() ),
             [ exp[0], exp[1], alg_id, exp[2], exp[3] ] ) )



if __name__ == '__main__':

    omtest.setup_run( 'algorithm_run_test' )

    omtest.run_tests( omtest_list, omtest.parseOptions() )

    omtest.summarize()

