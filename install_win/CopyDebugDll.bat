echo OFF
if not exist ..\bin\debug mkdir ..\bin\debug

echo Copying gdal15.dll
copy "%LIB_DIR%\bin\gdal15.dll" ..\bin\debug

echo Copying libexpat.dll
copy "%LIB_DIR%\bin\libexpat.dll" ..\bin\debug

echo Copying proj.dll
copy "%LIB_DIR%\bin\proj.dll" ..\bin\debug

REM echo Copying terralib dll's
REM copy %TERRALIB_HOME%\Debug\terralib_d.dll ..\..\bin\debug
REM copy %TERRALIB_HOME%\Debug\tiff_d.dll ..\..\bin\debug
REM copy %TERRALIB_HOME%\Debug\zlib1.dll ..\..\bin\debug

REM PAUSE