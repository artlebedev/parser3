SUBDIRS = libltdl

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck



INCLUDES = -I.. $(INCLTDL) -I$(MYSQL_INC)

lib_LTLIBRARIES = parser3mysql.la
parser3mysql_la_SOURCES = parser3mysql.C parser3mysql.h
parser3mysql_la_LIBADD = $(LIBLTDL)
parser3mysql_la_LDFLAGS = -module -avoid-version
