noinst_PROGRAMS = dftables pcre_parser_ctype
dftables_SOURCES = dftables.c 
pcre_parser_ctype_SOURCES = pcre_parser_ctype.c

auto_part.p:   pcre_parser_ctype
	./pcre_parser_ctype Russian_Russia.1251 > auto_part.p

chartables.c:   dftables
	./dftables >chartables.c

CLEANFILES = auto_part.p chartables.c

pcre.c: chartables.c

noinst_LIBRARIES = libpcre.a
noinst_HEADERS = pcre.h internal.h
libpcre_a_SOURCES = get.c pcre.c study.c maketables.c

EXTRA_DIST = pcre.3.html pcre.3.txt pcre.dsp pcre_dftables.dsp pcre_parser_ctype.dsp
