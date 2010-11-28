SUBDIRS = pa-include

INCLUDES = -Ipa-include -I../../include -I../gc/include

noinst_HEADERS = sdbm_pair.h sdbm_private.h sdbm_tune.h
noinst_LIBRARIES = libsdbm.a
libsdbm_a_SOURCES = \
	sdbm.c sdbm_hash.c sdbm_lock.c sdbm_pair.c \
	pa_file_io.C pa_strings.C

EXTRA_DIST = sdbm.vcproj

