#  $Id: Makefile.am,v 1.2 2001/04/17 19:00:43 paf Exp $

bin_PROGRAMS = dftables
dftables_SOURCES = dftables.c

chartables.c:   dftables
	./dftables >chartables.c


noinst_LIBRARIES = libpcre.a
libpcre_a_SOURCES = get.c pcre.c study.c  chartables.c maketables.c


