INCLUDES = -I../types -I../classes -I../sql -I../lib/pcre -I../lib/gc/include -I../lib/cord/include $(INCLTDL) @XML_INCLUDES@

noinst_HEADERS = compile_tools.h utf8-to-lower.inc utf8-to-upper.inc
noinst_LIBRARIES = libmain.a
libmain_a_SOURCES = pa_pool.C pa_os.C pa_common.C compile.tab.C compile.C compile_tools.C execute.C pa_exception.C pa_xml_exception.C pa_globals.C pa_xml_io.C pa_memory.C pa_request.C pa_string.C pa_table.C untaint.C pa_dir.C pa_exec.C pa_socks.C pa_sql_driver_manager.C pa_dictionary.C pa_stylesheet_manager.C pa_cache_managers.C pa_charset.C pa_charsets.C pa_uue.C

compile.tab.C: compile.y
	bison -v $< -o $@

EXTRA_DIST = compile.y main.vcproj
