#@test why not INCLUDES=?
INCLUDES := $(INCLUDES) -I../apache13 -I../../classes -I../../types $(INCLTDL) -I../../sql -I../../lib/cord/include -I../../lib/pcre -I../../lib/md5 -I$(APACHE13)/src/include -I$(APACHE13)/src/os/unix -I$(APACHE13)/src/os/win32 @XML_INCLUDES@

noinst_LIBRARIES = libmod_parser3core.a
libmod_parser3core_a_DEPENDENCIES=Makefile ../apache13/pa_httpd.h
libmod_parser3core_a_SOURCES = pa_threads.C mod_parser3.C

EXTRA_DIST = ApacheModuleParser3Core.dsp
