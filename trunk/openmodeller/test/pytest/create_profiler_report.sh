#bash

for lib in /usr/lib/libopenmodeller.so /usr/lib/openmodeller/libom*.so
do
  echo ==================================================================
  echo  Profile report for lib: $lib
  echo ==================================================================
  opreport --demangle=smart -l $lib 
done
