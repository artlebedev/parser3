SUBDIRS = libltdl

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck



INCLUDES = -I.. $(INCLTDL) -I$(ORACLEOCI_INC) -I$(ORACLEPUBLIC_INC)

lib_LTLIBRARIES = parser3oracle.la
parser3oracle_la_SOURCES = parser3oracle.C config_includes.h
parser3oracle_la_LIBADD = $(LIBLTDL)
parser3oracle_la_LDFLAGS = -module -avoid-version
