INCLUDES = -I../types -I../classes -I../sql -I../pcre $(INCLTDL)

noinst_LIBRARIES = libmain.a
libmain_a_SOURCES = compile.C compile.tab.C compile_tools.C execute.C pa_array.C pa_common.C pa_exception.C pa_globals.C pa_hash.C pa_pool.C pa_request.C pa_string.C pa_table.C untaint.C pa_dir.C pa_exec.C pa_socks.C pa_sql_driver_manager.C pa_dictionary.C

compile.tab.C: compile.y
	bison -v $< -o $@
