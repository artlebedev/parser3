#  $Id: Makefile.am,v 1.7 2001/05/21 07:35:29 parser Exp $

bin_PROGRAMS = dftables pcre_parser_ctype
dftables_SOURCES = dftables.c 
pcre_parser_ctype_SOURCES = pcre_parser_ctype.c

auto_part.p:   pcre_parser_ctype
	./pcre_parser_ctype Russian_Russia.1251 > auto_part.p

CHARTABLES_C=chartables.c

$(CHARTABLES_C):   dftables
	./dftables >$(CHARTABLES_C)

noinst_LIBRARIES = libpcre.a
libpcre_a_SOURCES = get.c pcre.c study.c  chartables.c maketables.c
CLEANFILES = $(CHARTABLES_C)