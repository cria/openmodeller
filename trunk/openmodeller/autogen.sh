#!/bin/sh
echo Configuring build environment for openModeller
aclocal  \
  && libtoolize --force --copy \
  && automake --add-missing --foreign --copy \
  && autoconf --force \
  && echo Now running configure to configure openModeller \
	&& ./configure
