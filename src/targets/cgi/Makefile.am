#  $Id: Makefile.am,v 1.4 2001/04/18 15:55:48 paf Exp $

INCLUDES = -I../../include -I../../classes -I../../types -I$(INCLTDL) -I../../sql

bin_PROGRAMS = parser3
parser3_SOURCES = pa_pool.C parser3.C
parser3_LDADD = ../../main/libmain.a ../../classes/libclasses.a ../../main/libmain.a ../../types/libtypes.a $(LIBLTDL) ../../classes/gd/libgd.a ../../pcre/libpcre.a 
