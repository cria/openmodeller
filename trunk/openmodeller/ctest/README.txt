     Building Unit Tests for libopenmodeller
    Tim Sutton and Albert Massayuki Kuniyoshi
                   July 2007
   -------------------------------------------

Getting cxxtest:

Download from http://cxxtest.sourceforge.net/ see below
for installation notes.

Compiling:

Building with unit tests requires you to build with cmake.
When configuring cmake using ccmake or cmakesetup, be sure
to turn the OM_TESTS option on.
You need to have the cxxtest headers on your system. For
mac and linux users cmake will automatically find them if
they exist under /usr/local/include/cxxtest or 
/usr/include/cxxtest. Under windows cmake will look for 
them under c:\msys\include

Running:

With OM_TESTS option on tests will compile automatically
and you can run them using 'make test'

Writing tests:

If you want to write your own tests for openModeller, take
om_test_template.h as a template. When you finish it, run
the following command:

$./cxxtestgen.pl --error-printer -o om_test_yourtest.cpp om_test_yourtest.h





