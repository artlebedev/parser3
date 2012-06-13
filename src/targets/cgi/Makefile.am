# Parser has circular library dependencies, thus libmain.a is linked twice and --preserve-dup-deps libtool option is used
PA_LIBS = ../../main/.libs/libmain.a ../../classes/.libs/libclasses.a ../../types/.libs/libtypes.a ../../main/.libs/libmain.a \
	  ../../lib/gd/libgd.la ../../lib/cord/libcord.la ../../lib/md5/libmd5.la ../../lib/sdbm/libsdbm.la \
	  ../../lib/smtp/libsmtp.la ../../lib/json/libjson.la ../../lib/memcached/libmemcached.la

INCLUDES = -I../../classes -I../../types -I../../lib/gc/include -I../../lib/cord/include @XML_INCLUDES@

# Automake 1.9 does not support LIBTOOLFLAGS
CXXLINK = $(LIBTOOL) --preserve-dup-deps --mode=link --tag=CXX $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

bin_PROGRAMS = parser3
noinst_HEADERS = getopt.h
parser3_DEPENDENCIES=Makefile $(PA_LIBS)
parser3_SOURCES = pa_threads.C parser3.C getopt.c
parser3_LDADD = $(PA_LIBS) $(LIBLTDL) @GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@
EXTRA_DIST = parser3.vcproj
