## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = no-dependencies foreign

noinst_HEADERS = ltdl.h
noinst_LTLIBRARIES = libltdlc.la

libltdlc_la_SOURCES = ltdl.c
libltdlc_la_LIBADD = $(LIBADD_DL)

## Because we do not have automatic dependency tracking:
ltdl.lo: ltdl.h config.h

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

## This allows us to install libltdl without using ln and without creating
## a world writeable directory.
## FIXME:  Remove this rule once automake can do this properly by itself.
local-install-files: $(DISTFILES)
	-rm -rf $(DESTDIR)$(datadir)/libtool/libltdl
	$(mkinstalldirs) $(DESTDIR)$(datadir)/libtool/libltdl
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -r $$d/$$file $(DESTDIR)$(datadir)/libtool/libltdl/$$file; \
	  else \
	    test -f $(DESTDIR)$(datadir)/libtool/libltdl/$$file \
	    || cp $$d/$$file $(DESTDIR)$(datadir)/libtool/libltdl/$$file || :; \
	  fi; \
	done
