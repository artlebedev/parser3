SUBDIRS = libltdl

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck



INCLUDES = -I.. $(INCLTDL) -I$(PGSQL_INC)

lib_LTLIBRARIES = parser3pgsql.la
parser3pgsql_la_SOURCES = parser3pgsql.C config_includes.h
parser3pgsql_la_LIBADD = $(LIBLTDL)
parser3pgsql_la_LDFLAGS = -module -avoid-version
