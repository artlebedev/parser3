#  $Id: Makefile.am,v 1.3 2001/04/17 19:00:59 paf Exp $

INCLUDES = -I../../include -I../../classes -I../../types -I$(INCLTDL) -I../../sql

bin_PROGRAMS = parser3
parser3_SOURCES = pa_pool.C parser3.C

parser_common_libs=../../main/libmain.a ../../classes/libclasses.a ../../main/libmain.a ../../types/libtypes.a $(LIBLTDL) ../../classes/gd/libgd.a ../../pcre/libpcre.a 

ifeq (${host_os},"cygwin")
parser_libs = $(parser_common_libs) /lib/libwsock32.a 
endif

parser3_LDADD = $(parser_libs)
