echo on
REM  Param1 is the base build directory ('%1')
REM  Param2 is the input interface file ('%2')
REM  Param3 is the output wrap file ('%3')

echo Creating Python binding

echo Deleting old wrap file: %3
del %3 

echo Creating SWIG interface for Python
%SWIG_HOME%\swig.exe -c++ -python -o %3 %2

echo Patching SWIG generated file
C:\Cygwin\bin\patch.exe %3 swig_diff.txt