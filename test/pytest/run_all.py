#!/usr/bin/env python
###############################################################################
# $Id$
#
# Project:  OM User Test Suit
# Purpose:  High level test executive ... it runs sub test scripts. 
#           Adapted from GDAL/OGR Test Suite
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

import sys
sys.path.append( 'pymod' )
import omtest

test_list = [ 'algs' ]

omtest.setup_run( 'om_user_test_all' )

omtest.run_all( test_list, omtest.parseOptions() )

omtest.summarize()
