#  $Id: Makefile.am,v 1.6 2001/05/18 12:58:07 parser Exp $

APACHE13_SRC=../../../../../../apache-1.3/src/

INCLUDES = -I../../../../include -I../../../../classes -I../../../../types $(INCLTDL) -I../../../../sql -I$(APACHE13_SRC)/include -I$(APACHE13_SRC)/os/unix

noinst_LIBRARIES = libmod_parser3.a
libmod_parser3_a_SOURCES = pa_pool.C pa_threads.C mod_parser3.C
