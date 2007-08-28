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
#  Revision 1.2  2005/05/13 15:14:18  scachett
#  Changed python tests to accept command line options to restrict tests to
#  be executed.
#
#  Fixed type in reference to Bioclim parameter in request.txt. (was Standad)
#
#  Revision 1.1  2005/04/19 14:24:23  scachett
#  Reimplemented user tests using GDAL/OGR autotest as a template.
#  Main changes:
#
#  - Code is clearer.
#  - It is simpler to add new tests. Just create a new directory and copy the sample test from ./samples directory.
#  - HTML generation is now separated from test coding by using the Python htmltmpl templating engine.
#
#  Also removed some obsolete Python test scripts.
#
#  Revision 1.2  2003/06/10 14:27:09  warmerda
#  comment
#
#  Revision 1.1  2003/03/05 05:05:10  warmerda
#  New
#

import os
import sys

sys.path.append( '../pymod' )

import omtest
import om

###############################################################################

def check_algorithm(args, options):

    alg_id = args

    if not omtest.checkArguments(options, "algorithm", alg_id):
        return ('skip', None)

    mod = om.OpenModeller()
    try:
        algMetadata = mod.algorithmMetadata(alg_id)
        return ('success', None)
    
    except:
        return ('fail', None)


###############################################################################

omtest_list = [ 
    ( check_algorithm, "Bioclim", "Bioclim" ),
    ( check_algorithm, "BioclimDistance", "BioclimDistance" ),
    ( check_algorithm, "DistanceToAverage", "DistanceToAverage" ),
    ( check_algorithm, "MinimumDistance", "MinimumDistance" ),
    ( check_algorithm, "CSMBS", "CSMBS" ),
    ( check_algorithm, "GARP", "GARP" ),
    ( check_algorithm, "DG_GARP", "DG_GARP" ),
    ( check_algorithm, "GARP_BS", "GARP_BS" ),
    ( check_algorithm, "DG_GARP_BS", "DG_GARP_BS" )
    ]


if __name__ == '__main__':

    omtest.setup_run( 'algorithm_presence_test' )

    omtest.run_tests( omtest_list, omtest.parseOptions() )

    omtest.summarize()

