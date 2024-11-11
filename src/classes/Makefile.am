AM_CPPFLAGS = -I../types -I../sql -I../lib/gd -I../lib/sdbm/pa-include -Igd $(INCLTDL) @PCRE_INCLUDES@ @XML_INCLUDES@ -I../lib/md5 -I../lib/smtp -I../lib/json -I../lib/memcached -I../lib/curl

CLASSES_SOURCE_FILES = array.C bool.C curl.C date.C double.C file.C form.C hash.C hashfile.C image.C inet.C int.C json.C mail.C math.C memcached.C memory.C op.C reflection.C regex.C response.C string.C table.C void.C xnode.C xdoc.C

noinst_HEADERS = classes.h xnode.h
noinst_LTLIBRARIES = libclasses.la
libclasses_la_SOURCES = $(CLASSES_SOURCE_FILES) classes.C
EXTRA_DIST = classes.vcproj
