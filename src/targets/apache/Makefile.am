INCLUDES := $(INCLUDES) $(APACHE_INC) -I./ -I../../classes -I../../types -I../../lib/gc/include -I../../lib/cord/include @XML_INCLUDES@

AM_CFLAGS := $(AM_CFLAGS) $(APACHE_CFLAGS)

PA_LIBS = ../../classes/libclasses.a ../../types/libtypes.a ../../main/libmain.a ../../lib/gd/libgd.a ../../lib/cord/libcord.a \
	  ../../lib/md5/libmd5.a ../../lib/sdbm/libsdbm.a ../../lib/smtp/libsmtp.a ../../lib/json/libjson.a $(LIBLTDL)

noinst_LIBRARIES = libmod_parser3.a

libmod_parser3_a_DEPENDENCIES=Makefile $(PA_LIBS)
libmod_parser3_a_SOURCES = pa_threads.C mod_parser3_core.C mod_parser3.c

EXTRA_DIST = pa_httpd.h

all-local: parser3module

parser3module:
	$(CC) -shared -o mod_parser3.so -Wl,--whole-archive libmod_parser3.a $(PA_LIBS) -Wl,--no-whole-archive \
	@GC_LIBS@ @PCRE_LIBS@ @XML_LIBS@ @MIME_LIBS@ $(LIBADD_DL) @CPP_LIBS@
