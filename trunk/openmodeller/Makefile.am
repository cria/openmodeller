# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = \
	AUTHORS \
	README.txt \
	INSTALL \
	NEWS \
	ChangeLog \
	COPYING.txt \
	doc/openModeller.doxygen \
	doc/openModeller.xsd \
	examples/furcata_boliviana.txt \
	examples/rain_coolest.tif \
	examples/temp_avg.tif \
	examples/request.txt \
	examples/request_te.txt \
	examples/model_request.xml \
	examples/projection_request.xml \
	libopenmodeller.spec

#SUBDIRS = test src data
SUBDIRS = test src

release: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP_ENV) $(AMTAR)$(TAR) chozf $(distdir).tar.gz $(distdir)
	rm -f $(distdir)/libopenmodeller.spec
	sed '/^Source:/s/\.tar\.gz$$/\.tar\.bz2/' libopenmodeller.spec \
			> $(distdir)/libopenmodeller.spec
	BZIP2=$(BZIP2_ENV) $(AMTAR)$(TAR) --bzip2 -chof $(distdir).tar.bz2 $(distdir)
	-rm -rf $(distdir)

