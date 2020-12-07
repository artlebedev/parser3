SUBDIRS = src etc
ACLOCAL_AMFLAGS = -I src/lib/ltdl/m4

EXTRA_DIST = operators.txt parser3.sln gnu.vcproj buildall acsite.m4

commit:
#       trick to make 'make' happy at check out time
#       and avoid redundant remaking: aclocal+autoconf+automake

	cvs commit -m "no message" -f configure.ac acsite.m4 aclocal.m4 Makefile.am Makefile.in configure src/include/pa_config_auto.h.in
