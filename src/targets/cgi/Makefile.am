INCLUDES = -I../../include -I../../classes -I../../types -I$(INCLTDL) -I../../sql

bin_PROGRAMS = parser3
parser3_SOURCES = pa_pool.C pa_threads.C parser3.C
parser3_LDADD=../../main/libmain.a ../../classes/libclasses.a ../../main/libmain.a ../../types/libtypes.a $(LIBLTDL) ../../classes/gd/libgd.a ../../pcre/libpcre.a $(WIN32_ADDITIONAL_LIBS)
