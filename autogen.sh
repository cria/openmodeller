#!/bin/sh
echo Configuring build environment for openmodeller
aclocal  \
  && libtoolize --force --copy \
  && automake --add-missing --foreign --copy \
  && autoconf --force \
  && echo Now running configure to configure openmodeller \
  && ./configure $@
