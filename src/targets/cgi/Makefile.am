#this read configure.in comment on stdc++ about next line
if DO_NOT_LINK_LIBSTDCPP
CXXLDtrue=$(CC)
else
CXXLDfalse=$(CXX)
endif

CXXLD=$(CXXLDtrue) $(CXXLDfalse)

INCLUDES = -I../../classes -I../../types -I../../sql -I../../lib/pcre -I../../lib/md5 -I$(INCLTDL) @XML_INCLUDES@
PA_LIBS = ../../main/libmain.a ../../classes/libclasses.a ../../main/libmain.a ../../types/libtypes.a ../../classes/gd/libgd.a ../../lib/pcre/libpcre.a ../../lib/md5/libmd5.a $(LIBLTDL)

bin_PROGRAMS = parser3
noinst_HEADERS = pool_storage.h getopt.h
parser3_DEPENDENCIES=Makefile $(PA_LIBS)
parser3_SOURCES = pa_pool.C pa_threads.C parser3.C getopt.c
parser3_LDADD= $(PA_LIBS) $(LIBADD_DL) @XML_LIBS@ @MIME_LIBS@
EXTRA_DIST = parser3.dsp
