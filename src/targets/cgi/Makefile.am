# Parser switches linker to gcc to avoid -lstdc++ to be dynamically linked by g++
CXXLD=$(CC)

# Parser has circular library dependencies, this libmain.a linked twice and --preserve-dup-deps libtool option used
PA_LIBS = ../../main/libmain.a ../../classes/libclasses.a ../../types/libtypes.a ../../main/libmain.a \
	  ../../lib/gd/libgd.a ../../lib/cord/libcord.a ../../lib/md5/libmd5.a ../../lib/sdbm/libsdbm.a \
	  ../../lib/smtp/libsmtp.a ../../lib/json/libjson.a ../../lib/memcached/libmemcached.a $(LIBLTDL)

INCLUDES = -I../../classes -I../../types -I../../sql -I../../lib/gc/include -I../../lib/cord/include \
	   -I../../lib/md5 -I../../lib/json -I$(INCLTDL) @PCRE_INCLUDES@ @XML_INCLUDES@

bin_PROGRAMS = parser3
noinst_HEADERS = getopt.h
parser3_DEPENDENCIES=Makefile $(PA_LIBS)
parser3_SOURCES = pa_threads.C parser3.C getopt.c
parser3_LDADD= $(PA_LIBS) $(LIBADD_DL) @GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@ @CPP_LIBS@
parser3_LIBTOOLFLAGS=--preserve-dup-deps
EXTRA_DIST = parser3.vcproj
