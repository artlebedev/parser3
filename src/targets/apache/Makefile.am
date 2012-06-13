INCLUDES = $(APACHE_INC) -I./ -I../../classes -I../../types -I../../lib/gc/include -I../../lib/cord/include @XML_INCLUDES@

PA_LIBS = ../../classes/libclasses.a ../../types/libtypes.a ../../main/libmain.a ../../lib/gd/libgd.a ../../lib/cord/libcord.a \
	  ../../lib/md5/libmd5.a ../../lib/sdbm/libsdbm.a ../../lib/smtp/libsmtp.a ../../lib/json/libjson.a  ../../lib/memcached/libmemcached.a

noinst_HEADERS = pa_httpd.h

lib_LTLIBRARIES = libmod_parser3.la

libmod_parser3_la_SOURCES = pa_threads.C mod_parser3_core.C mod_parser3.c
libmod_parser3_la_DEPENDENCIES = Makefile $(PA_LIBS)
libmod_parser3_la_LDFLAGS = -module -avoid-version
 
libmod_parser3_la_LIBADD = $(PA_LIBS) @GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@ $(LIBLTDL)
