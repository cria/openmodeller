REM  Param1 is the base build directory ('%1')

if not exist ..\%1 mkdir ..\%1
if not exist ..\%1\lib mkdir ..\%1\lib

echo Copying gdal12.dll to .\%1\lib
copy %GDAL_HOME%\bin\gdal12.dll ..\%1\lib