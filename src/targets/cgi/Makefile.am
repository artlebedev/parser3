#this read configure.in comment on stdc++ about next line
if DO_NOT_LINK_LIBSTDCPP
CXXLD=$(CC)
else
CXXLD=$(CXX)
endif

INCLUDES = -I../../classes -I../../types -I../../sql -I../../lib/pcre -I../../lib/md5 -I$(INCLTDL) @XML_INCLUDES@

bin_PROGRAMS = parser3
noinst_HEADERS = pool_storage.h
parser3_DEPENDENCIES=Makefile
parser3_SOURCES = pa_pool.C pa_threads.C parser3.C getopt.c
parser3_LDADD=../../main/libmain.a ../../classes/libclasses.a ../../main/libmain.a ../../types/libtypes.a ../../classes/gd/libgd.a ../../lib/pcre/libpcre.a ../../lib/md5/libmd5.a $(LIBLTDL) $(LIBADD_DL) @XML_LIBS@ @MIME_LIBS@

EXTRA_DIST = parser3.dsp getopt.h
