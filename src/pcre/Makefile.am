#  $Id: Makefile.am,v 1.6 2001/05/21 07:33:53 parser Exp $

bin_PROGRAMS = dftables pcre_parser_ctype
dftables_SOURCES = dftables.c 
pcre_parser_ctype_SOURCES = pcre_parser_ctype.c

auto_part.p:   pcre_parser_ctype
	./pcre_parser_ctype Russian_Russia.1251 > auto_part.p

chartables.c:   dftables
	./dftables >chartables.c

noinst_LIBRARIES = libpcre.a
libpcre_a_SOURCES = get.c pcre.c study.c  chartables.c maketables.c

