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

With OM_TESTS option on, tests will compile automatically
and you can run them using 'make test'.

Writing tests:

If you want to write your own tests for openModeller, take
om_test_template.h as a template. When you finish it, open 
CMakeLists.txt and type:

#yourtest Tests
SET (OM_TEST_YOURTEST_SRCS om_test_yourtest.cpp)
ADD_EXECUTABLE (om_test_yourtest ${OM_TEST_YOURTEST_SRCS})
TARGET_LINK_LIBRARIES(om_test_yourtest openmodeller)
ADD_TEST(om_test_yourtest ${EXECUTABLE_OUTPUT_PATH}/om_test_yourtest)

Now you need to type the following commands:

$cd your_path_to_openmodeller/test/ctest
$cxxtestgen --error-printer -o om_test_yourtest.cpp om_test_yourtest.h

If necessary, include the world parameter:
$cxxtestgen --error-printer -w "test_yourtest" -o om_test_yourtest.cpp om_test_yourtest.h

Add the same previous line to regenerate_tests.sh, which can be used to regenerate all tests when something changes in newer cxxtest versions.

Build with the following commands:

$cd your_path_to_openmodeller/build
$cmake ..
$make
$sudo make install

Run tests:

$make test

or

$ctest

or

$ctest -R om_test_occurrence

or

$ctest -R om_test_alg*

Add -V to increase verbosity




