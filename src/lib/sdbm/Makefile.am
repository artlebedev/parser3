SUBDIRS = apr-include

INCLUDES = -Iapr-include -I../../include -I../gc/include

noinst_HEADERS = sdbm_pair.h sdbm_private.h sdbm_tune.h
noinst_LIBRARIES = libsdbm.a
libsdbm_a_SOURCES = \
	sdbm.c sdbm_hash.c sdbm_lock.c sdbm_pair.c \
	apr_file_io.C apr_strings.C

EXTRA_DIST = sdbm.vcproj

