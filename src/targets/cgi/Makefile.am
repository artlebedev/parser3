#by default c++ when running linker adds -lstdc++ (and -lm) which overrides 
#our [default] setting of linking that that lib should be linked statically
#so we're switching linker to gcc, which does not add anything
CXXLD=$(CC)

INCLUDES = -I../../classes -I../../types -I../../sql -I../../lib/gc/include -I../../lib/cord/include -I../../lib/pcre -I../../lib/md5 -I$(INCLTDL) @XML_INCLUDES@
PA_LIBS = ../../main/libmain.a ../../classes/libclasses.a ../../types/libtypes.a ../../main/libmain.a ../../lib/gd/libgd.a ../../lib/cord/libcord.a ../../lib/pcre/libpcre.a ../../lib/md5/libmd5.a ../../lib/sdbm/libsdbm.a ../../lib/smtp/libsmtp.a $(LIBLTDL)

bin_PROGRAMS = parser3
noinst_HEADERS = getopt.h
parser3_DEPENDENCIES=Makefile $(PA_LIBS)
parser3_SOURCES = pa_threads.C parser3.C getopt.c
parser3_LDADD= $(PA_LIBS) $(LIBADD_DL) @GC_LIBS@ @XML_LIBS@ @MIME_LIBS@ @CPP_LIBS@
EXTRA_DIST = parser3.vcproj pp3.cmd
