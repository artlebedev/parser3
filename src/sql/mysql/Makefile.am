#  $Id: Makefile.am,v 1.2 2001/04/17 19:00:48 paf Exp $

SUBDIRS = libltdl

INCLUDES = -I.. -I$(INCLTDL) -I$(MYSQL_INC)

lib_LTLIBRARIES = parser3mysql.la
parser3mysql_la_SOURCES = parser3mysql.C parser3mysql.h
parser3mysql_la_LIBADD = $(LIBLTDL)
parser3mysql_la_LDFLAGS = -module -avoid-version
