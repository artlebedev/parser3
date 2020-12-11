AM_CPPFLAGS = -I../types -I../classes -I../sql -I../lib/punycode $(INCLTDL) @PCRE_INCLUDES@ @XML_INCLUDES@

noinst_HEADERS = compile_tools.h utf8-to-lower.inc utf8-to-upper.inc
noinst_LTLIBRARIES = libmain.la
libmain_la_SOURCES = pa_pool.C pa_os.C pa_threads.C pa_common.C pa_base64.C compile.tab.C compile.C compile_tools.C execute.C pa_cache_managers.C pa_exception.C pa_xml_exception.C pa_globals.C pa_xml_io.C pa_memory.C pa_request.C pa_string.C pa_table.C untaint.C pa_dir.C pa_exec.C pa_socks.C pa_dictionary.C pa_charset.C pa_charsets.C pa_uue.C pa_sql_driver_manager.C pa_stylesheet_manager.C pa_stylesheet_connection.C pa_http.C pa_random.C pa_symbols.C

compile.tab.C: compile.y
	bison -v compile.y -o $@

EXTRA_DIST = compile.y main.vcproj
