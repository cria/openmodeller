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
	README \
	INSTALL \
	NEWS \
	ChangeLog \
	COPYING \
	doc/classes.dia \
	doc/classes.png \
	doc/openModeller.doxygen \
	doc/overview.dia \
	examples/furcata_boliviana.txt \
	examples/request.txt

SUBDIRS = test src algorithms

release: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP_ENV) $(AMTAR)$(TAR) chozf $(distdir).tar.gz $(distdir)
	rm -f $(distdir)/openmodeller.spec
	sed '/^Source:/s/\.tar\.gz$$/\.tar\.bz2/' openmodeller.spec \
			> $(distdir)/openmodeller.spec
	BZIP2=$(BZIP2_ENV) $(AMTAR)$(TAR) --bzip2 -chof $(distdir).tar.bz2 $(distdir)
	-rm -rf $(distdir)

