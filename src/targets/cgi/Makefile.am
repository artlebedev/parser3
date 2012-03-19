#by default c++ when running linker adds -lstdc++ (and -lm) which overrides 
#our [default] setting of linking that that lib should be linked statically
#so we're switching linker to gcc, which does not add anything
CXXLD=$(CC)

INCLUDES = -I../../classes -I../../types -I../../sql -I../../lib/gc/include -I../../lib/cord/include -I../../lib/md5 -I../../lib/json -I$(INCLTDL) @PCRE_INCLUDES@ @XML_INCLUDES@
PA_LIBS = ../../main/libmain.a ../../classes/libclasses.a ../../types/libtypes.a ../../main/libmain.a ../../lib/gd/libgd.a ../../lib/cord/libcord.a ../../lib/md5/libmd5.a ../../lib/sdbm/libsdbm.a ../../lib/smtp/libsmtp.a ../../lib/json/libjson.a ../../lib/memcached/libmemcached.a $(LIBLTDL)

bin_PROGRAMS = parser3
noinst_HEADERS = getopt.h
parser3_DEPENDENCIES=Makefile $(PA_LIBS)
parser3_SOURCES = pa_threads.C parser3.C getopt.c
parser3_LDADD= $(PA_LIBS) $(LIBADD_DL) @GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@ @CPP_LIBS@ -lmemcached
EXTRA_DIST = parser3.vcproj pp3.cmd
