echo OFF
if not exist ..\..\bin\debug mkdir ..\..\bin\debug

echo Copying gdal15.dll
copy %GDAL_HOME%\bin\gdal15.dll ..\..\bin\debug

echo Copying libexpat.dll
copy "%EXPAT_HOME%\Bin\libexpat.dll" ..\..\bin\debug

echo Copying proj.dll
copy "%PROJ4_HOME%\proj_fw.dll" ..\..\bin\debug\proj.dll

echo Copying terralib dll's
copy %TERRALIB_HOME%\Debug\terralib_d.dll ..\..\bin\debug
copy %TERRALIB_HOME%\Debug\tiff_d.dll ..\..\bin\debug
copy %TERRALIB_HOME%\Debug\zlib1.dll ..\..\bin\debug

PAUSE