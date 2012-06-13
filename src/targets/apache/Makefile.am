INCLUDES = $(APACHE_INC) -I./ -I../../classes -I../../types -I../../lib/gc/include -I../../lib/cord/include @XML_INCLUDES@

PA_LIBS = ../../classes/libclasses.la ../../types/libtypes.la ../../main/libmain.la ../../lib/gd/libgd.la ../../lib/cord/libcord.la \
	  ../../lib/md5/libmd5.la ../../lib/sdbm/libsdbm.la ../../lib/smtp/libsmtp.la ../../lib/json/libjson.la  ../../lib/memcached/libmemcached.la

# Automake 1.9 does not support LIBTOOLFLAGS
libmod_parser3_la_LINK = $(LIBTOOL) --tag=CXX --tag=disable-static --mode=link $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(libmod_parser3_la_LDFLAGS) $(LDFLAGS) -o $@

noinst_HEADERS = pa_httpd.h

lib_LTLIBRARIES = libmod_parser3.la

libmod_parser3_la_SOURCES = pa_threads.C mod_parser3_core.C mod_parser3.c
libmod_parser3_la_DEPENDENCIES = Makefile $(PA_LIBS)
libmod_parser3_la_LDFLAGS = -module -avoid-version
 
libmod_parser3_la_LIBADD = $(PA_LIBS) @GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@ $(LIBLTDL)
