#  $Id: Makefile.am,v 1.6 2001/04/23 16:58:22 paf Exp $

INCLUDES = -I../../include -I../../classes -I../../types -I$(INCLTDL) -I../../sql

bin_PROGRAMS = parser3
parser3_SOURCES = pa_pool.C parser3.C

parser_common_libs=../../main/libmain.a ../../classes/libclasses.a ../../main/libmain.a ../../types/libtypes.a $(LIBLTDL) ../../classes/gd/libgd.a ../../pcre/libpcre.a 
parser_libs = 
if WIN32
	parser_libs = $(parser_common_libs) /lib/libwsock32.a 
else
	parser_libs = $(parser_common_libs)
endif
parser3_LDADD = $(parser_libs)
