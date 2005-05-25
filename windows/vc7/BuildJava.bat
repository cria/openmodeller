echo on
REM  Param1 is the base build directory ('%1')
REM  Param2 is the input interface file ('%2')
REM  Param3 is the output wrap file ('%3')

echo Creating Java package directory ..\%1\br\org\cria\OpenModeller

if not exist ..\%1                          mkdir ..\%1
if not exist ..\%1\br                       mkdir ..\%1\br
if not exist ..\%1\br\org                   mkdir ..\%1\br\org
if not exist ..\%1\br\org\cria              mkdir ..\%1\br\org\cria
if not exist ..\%1\br\org\cria\OpenModeller mkdir ..\%1\br\org\cria\OpenModeller

echo Deleting old wrap file: %3
del %3 

echo Creating SWIG interface for Java
%SWIG_HOME%\swig.exe -c++ -java -package br.org.cria.OpenModeller -outdir ..\%1\br\org\cria\OpenModeller -o %3 %2 

echo Patching SWIG generated file
C:\Cygwin\bin\patch.exe %3 swig_diff.txt