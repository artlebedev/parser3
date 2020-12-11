PA_LIBS = ../../classes/libclasses.la ../../types/libtypes.la ../../main/libmain.la ../../lib/gd/libgd.la \
	  ../../lib/cord/libcord.la ../../lib/md5/libmd5.la ../../lib/sdbm/libsdbm.la ../../lib/smtp/libsmtp.la \
	  ../../lib/pcre/libpcre_internal.la ../../lib/json/libjson.la  ../../lib/memcached/libmemcached.la \
	  ../../lib/punycode/libpunycode.la

AM_CPPFLAGS = $(APACHE_INC) -I./ -I../../classes -I../../types @XML_INCLUDES@

# Automake 1.9 does not support LIBTOOLFLAGS
mod_parser3_la_LINK = $(LIBTOOL) --tag=CXX --tag=disable-static --mode=link $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(mod_parser3_la_LDFLAGS) $(LDFLAGS) -o $@

noinst_HEADERS = pa_httpd.h

lib_LTLIBRARIES = mod_parser3.la

mod_parser3_la_SOURCES = mod_parser3_core.C mod_parser3.c
mod_parser3_la_DEPENDENCIES = Makefile $(PA_LIBS)
mod_parser3_la_LDFLAGS = -module -avoid-version
mod_parser3_la_CFLAGS = $(APACHE_CFLAGS)

mod_parser3_la_LIBADD = $(PA_LIBS) @GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@ $(LIBLTDL)
