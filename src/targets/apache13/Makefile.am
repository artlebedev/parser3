#@test why not INCLUDES=?
INCLUDES := $(INCLUDES) -I../../classes -I../../types $(INCLTDL) -I../../sql -I../../lib/pcre -I../../lib/md5 -I$(APACHE13)/src/include -I$(APACHE13)/src/os/unix -I$(APACHE13)/src/os/win32 @XML_INCLUDES@

noinst_LIBRARIES = libmod_parser3.a
libmod_parser3_a_SOURCES = pa_pool.C pa_threads.C mod_parser3.C pa_md5c.c
noinst_DATA = apache_build_instruction

apache_build_instruction:
	-chmod +x p3runConfigure
	-cp -f p3runConfigure $(APACHE13)
	@echo "******************************************************"
	@echo "to build apache with parser3 module:"
	@echo
	@echo "1. cd $(APACHE13)"
	@echo
	@echo "2. ./p3runConfigure [configure options]"
	@echo
	@echo "3. make"
	@echo "******************************************************"

EXTRA_DIST = ApacheModuleParser3.dsp
