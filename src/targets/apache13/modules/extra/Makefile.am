#  $Id: Makefile.am,v 1.5 2001/04/24 15:50:48 paf Exp $

APACHE13_SRC=../../../../../../apache-1.3/src/

INCLUDES = -I../../../../include -I../../../../classes -I../../../../types $(INCLTDL) -I../../../../sql -I$(APACHE13_SRC)/include -I$(APACHE13_SRC)/os/unix

noinst_LIBRARIES = libmod_parser3.a
libmod_parser3_a_SOURCES = pa_pool.C mod_parser3.C
