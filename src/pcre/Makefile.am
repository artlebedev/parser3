#  $Id: Makefile.am,v 1.8 2001/05/21 08:16:13 parser Exp $

bin_PROGRAMS = dftables pcre_parser_ctype
dftables_SOURCES = dftables.c 
pcre_parser_ctype_SOURCES = pcre_parser_ctype.c

auto_part.p:   pcre_parser_ctype
	./pcre_parser_ctype Russian_Russia.1251 > auto_part.p

CHARTABLES_C=chartables.c

$(CHARTABLES_C):   dftables
	./dftables >$(CHARTABLES_C)

noinst_LIBRARIES = libpcre.a
libpcre_a_SOURCES = get.c pcre.c study.c maketables.c
CLEANFILES = $(CHARTABLES_C)