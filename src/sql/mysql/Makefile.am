#  $Id: Makefile.am,v 1.3 2001/04/18 15:55:41 paf Exp $

SUBDIRS = libltdl

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck



INCLUDES = -I.. $(INCLTDL) -I$(MYSQL_INC)

lib_LTLIBRARIES = parser3mysql.la
parser3mysql_la_SOURCES = parser3mysql.C parser3mysql.h
parser3mysql_la_LIBADD = $(LIBLTDL)
parser3mysql_la_LDFLAGS = -module -avoid-version
