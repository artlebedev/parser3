# Parser has circular library dependencies, thus libmain.a is linked twice and --preserve-dup-deps libtool option is used
PA_LIBS = ../../main/libmain.a ../../classes/libclasses.a ../../types/libtypes.a ../../main/libmain.a \
	  ../../lib/gd/libgd.a ../../lib/cord/libcord.a ../../lib/md5/libmd5.a ../../lib/sdbm/libsdbm.a \
	  ../../lib/smtp/libsmtp.a ../../lib/json/libjson.a ../../lib/memcached/libmemcached.a

INCLUDES = -I../../classes -I../../types -I../../sql -I../../lib/gc/include -I../../lib/cord/include \
	   -I../../lib/md5 -I../../lib/json -I$(INCLTDL) @PCRE_INCLUDES@ @XML_INCLUDES@

# Automake 1.9 does not support LIBTOOLFLAGS
CXXLINK = $(LIBTOOL) --preserve-dup-deps --mode=link --tag=CXX $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

bin_PROGRAMS = parser3
noinst_HEADERS = getopt.h
parser3_DEPENDENCIES=Makefile $(PA_LIBS)
parser3_SOURCES = pa_threads.C parser3.C getopt.c
parser3_LDADD = $(PA_LIBS) $(LIBLTDL) @GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@
EXTRA_DIST = parser3.vcproj
