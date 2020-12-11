# Parser has circular library dependencies, thus libmain.a is linked twice and --preserve-dup-deps libtool option is used
PA_LIBS = ../../main/.libs/libmain.a ../../classes/.libs/libclasses.a ../../types/.libs/libtypes.a ../../main/.libs/libmain.a \
	  ../../lib/gd/libgd.la ../../lib/cord/libcord.la ../../lib/md5/libmd5.la ../../lib/sdbm/libsdbm.la \
	  ../../lib/pcre/libpcre_internal.la ../../lib/smtp/libsmtp.la ../../lib/json/libjson.la \
	  ../../lib/punycode/libpunycode.la ../../lib/memcached/libmemcached.la

AM_CPPFLAGS = -I../../classes -I../../types @PCRE_INCLUDES@ @XML_INCLUDES@

# Automake 1.9 does not support LIBTOOLFLAGS
CXXLINK = $(LIBTOOL) --preserve-dup-deps --mode=link --tag=CXX $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

noinst_HEADERS = pa_sapi_info.h

bin_PROGRAMS = parser3
parser3_DEPENDENCIES=Makefile $(PA_LIBS)
parser3_SOURCES = parser3.C
parser3_LDADD = $(PA_LIBS) $(LIBLTDL) @GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@
EXTRA_DIST = parser3.vcproj
