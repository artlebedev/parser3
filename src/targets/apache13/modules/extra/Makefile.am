INCLUDES = -I../../../../classes -I../../../../types $(INCLTDL) -I../../../../sql -I$(APACHE13_SRC)/include -I$(APACHE13_SRC)/os/unix

noinst_LIBRARIES = libmod_parser3.a libpostbuildmessage.a
libmod_parser3_a_SOURCES = pa_pool.C pa_threads.C mod_parser3.C

libpostbuildmessage.a:
	@echo "******************************************************"
	@echo "0. go to your Apache sources directory"
	@echo
	@echo "1. add these lines to Configuration file"
	@echo
	@echo "PARSER_SRC={path to your parser3 sources}"
	@echo "PARSER_LIBS=\$\(PARSER_SRC)/main/libmain.a \$\(PARSER_SRC)/classes/libclasses.a \$\(PARSER_SRC)/main/libmain.a \$\(PARSER_SRC)/types/libtypes.a \$\(PARSER_SRC)/libltdl/.libs/libltdlc.al -ldl \$\(PARSER_SRC)/classes/gd/libgd.a \$\(PARSER_SRC)/pcre/libpcre.a \$\(PARSER_SRC)/targets/apache13/modules/extra/libmod_parser3.a"
	@echo "EXTRA_LIBS=\$\(PARSER_LIBS) \$\(PARSER_LIBS)"
	@echo "AddModule modules/extra/mod_parser3.o"
	@echo
	@echo "2. create empty file modules/extra/mod_parser3.c"
	@echo
	@echo "touch modules/extra/mod_parser3.c"
	@echo
	@echo "3. ./configure"
	@echo "4. make"
	@echo "******************************************************"
