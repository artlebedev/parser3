#  $Id: Makefile.am,v 1.3 2001/04/24 14:49:42 paf Exp $

INCLUDES = -I../../../../include -I../../../../classes -I../../../../types -I$(INCLTDL) -I../../../../sql

noinst_LIBRARIES = mod_parser3.a
mod_parser3_a_SOURCES = pa_pool.C mod_parser3.C
mod_parser3_a_LIBADD=../../../../main/libmain.a ../../../../classes/libclasses.a ../../../../main/libmain.a ../../../../types/libtypes.a $(LIBLTDL) ../../../../classes/gd/libgd.a ../../../../pcre/libpcre.a $(WIN32_ADDITIONAL_LIBS)
