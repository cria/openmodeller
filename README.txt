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

The original motivation of openModeller was to help automating the tedious and
complex tasks involved in species' potential distribution modelling regarding 
georeferencing issues, resampling, existence of several raster formats and
algorithms, etc. This way:

a) One can spend time developing algorithms, testing, tunning or comparing
   results, instead of clipping rasters, converting between different raster formats 
   or using different programs to run different algorithms. The same input data, 
   sampling methodology and ideally the same computing environment should be used
   in a fair comparison between different algorithms.

b) Specialized components can help integrating data from distributed sources. For 
   example, one could look for species data in sources like the GBIF portal and for 
   environmental data in some remote source through WCS (OGC's Web Coverage Service). 
   Retrieving and integrating data from different sources using different protocols 
   and tools can be very complicated without using a single framework.

c) Different interfaces can be developed on top of the same modelling library.

d) Powerfull computers can be used to run heavy algorithms (like GARP) 
   in parallel (Cluster computing).


CURRENT STATUS
--------------

- Core modelling API implemented in ANSI C++ (compiles in GNU/Linux, Mac OSX and Windows).
- Algorithms dinamically loaded as plugins.
- Generic API to read & write raster data (GDAL and TerraLib drivers available).
- Generic API to load occurrence data (drivers available for tab-delimited text 
  files, serialized XML files, GBIF REST service and TAPIR/DarwinCore providers).
- Simple command-line/console interface.
- SOAP interface for remote modelling (server implementation available).
- Python binding.


DEPENDENCIES
------------

The following libraries are necessary to compile and run openModeller.
If you installed openModeller from a pre-built package, you don't
need to worry about this (most dependencies should already be included 
in the package).

* cmake (>= 2.6) if you need to compile the library.

* Expat XML parser
  http://expat.sourceforge.net/

* Proj.4 (Cartographic Projections Library)
  http://remotesensing.org/proj/

* GDAL (Geospatial Data Abstraction Library) version >= 1.1.9
  http://www.remotesensing.org/gdal

Optional libraries:

* TerraLib (>= 3.2.0) is needed to enable the TerraLib occurrence and raster drivers.
  http://www.terralib.org/

* libX11 is needed to run the command line viewer (om_viewer).
  http://x.org/

* GSL (GNU Scientific Library) version >= 1.4 is needed to run 
  the CSM (Climate Space Model) algorithm.
  http://www.gnu.org/software/gsl

* libcurl (>= 7.15.4) is needed to enable to TAPIR and GBIF occurrence drivers.
  http://curl.haxx.se/libcurl/


DOCUMENTATION
-------------

For compilation and installation instructions, please read:

http://openmodeller.sf.net/INSTALL.html

For code documentation, see: http://openmodeller.cria.org.br/doxygen/

Besides the library itself, openModeller comes with a series of 
command line tools. Please look at the README.txt file in the 
"examples" directory for more information about each tool.
Windows users must use the DOS shell to run command line tools.

------------ x -----------


