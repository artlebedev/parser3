#  $Id: Makefile.am,v 1.8 2001/03/21 14:06:45 paf Exp $

INCLUDES = -I../include -I../types -I../classes

noinst_LIBRARIES = libmain.a
libmain_a_SOURCES = compile.C compile.tab.C compile_tools.C execute.C pa_array.C pa_common.C pa_exception.C pa_globals.C pa_hash.C pa_pool.C pa_request.C pa_string.C pa_table.C untaint.C

compile.tab.C: compile.y
	bison -v $<
	bison -v $< -o $@
