REM  Param1 is the base build directory ('%1')
REM  Param2 is the from directory ('%2')
REM  Param3 is the output file base name without extension ('%3')

if not exist ..\%1 mkdir ..\%1

echo Installing library %2\%3.dll and additional files to .\%1
copy %2\%3.dll  ..\%1
copy %2\%3.exp  ..\%1
copy %2\%3.lib  ..\%1