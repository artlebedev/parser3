#  $Id: Makefile.am,v 1.2 2001/04/24 14:43:25 paf Exp $

INCLUDES = -I../../../../include -I../../../../classes -I../../../../types -I$(INCLTDL) -I../../../../sql

lib_LTLIBRARIES = mod_parser3.la
mod_parser3_la_SOURCES = pa_pool.C mod_parser3.C
mod_parser3_la_LIBADD=../../../../main/libmain.a ../../../../classes/libclasses.a ../../../../main/libmain.a ../../../../types/libtypes.a $(LIBLTDL) ../../../../classes/gd/libgd.a ../../../../pcre/libpcre.a $(WIN32_ADDITIONAL_LIBS)
