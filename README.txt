openModeller - Open Source Spatial Distribution Modeller
--------------------------------------------------------

* Coordination:

  CRIA - Centro de Referencia em Informacao Ambiental (SP - Brazil)
  (Reference Center on Environmental Information)
  http://www.cria.org.br/

* Funding:

  FAPESP - Fundacao de Amparo a Pesquisa do Estado de SP
  http://www.fapesp.br/

* Website: 

  http://openmodeller.sf.net/


OVERVIEW
--------

The original motivation of openModeller was to automate the tedious and
complex tasks involved in species' potential distribution modelling regarding 
map file formats, georeferencing issues, resamples, map interpolations, etc.
Some consequences arise from this automation:

a) One can spend time developing algorithms, testing, tunning or comparing
   results. To compare two or more different algorithms we need the same 
   input data, the same sample methodology, etc.

b) Different tasks can be performed by independent components in distributed
   frameworks. A framework could look for species data (in sources like 
   the GBIF portal) and for environmental data. These data are hard to find
   (with high quality) and some times are enormous.

c) Powerfull computers can be used to run heavy algorithms (like GARP) 
   and that could also be done in parallel (clustering).


CURRENT STATUS
--------------

- Core modelling interface implemented in ANSI C++ (control interface).
- Algorithms dinamically loaded as separate C++ libraries (algorithm
  interface).
- Simple command-line (console) interface.
- SOAP interface.
- Desktop interface (available in a separate package).


LICENSE
-------

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details:
 
http://www.gnu.org/copyleft/gpl.html


REQUIREMENTS
------------

* Proj.4 (Cartographic Projections Library)
  http://remotesensing.org/proj/

* GDAL (Geospatial Data Abstraction Library) version >= 1.1.9
  http://www.remotesensing.org/gdal

* To run the simple map viewer (om_viewer), libX11 is needed
  (tested with version 6.2)

* To run the CSM (Climate Space Model) algorithm, the GSL (GNU
  Scientific Library) version >= 1.4 is needed
  http://www.gnu.org/software/gsl


DOCUMENTATION
-------------

For now only Doxygen documentation is available.

Download Doxygen from: http://www.doxygen.org

In the main directory, run: make doc

Then have a look at the "html" or "latex" directories that
will be created.

------------ x -----------


