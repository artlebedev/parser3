SUBDIRS = src etc bin

EXTRA_DIST = operators.txt parser3.sln

update:
	cvs update
	$(MAKE)

commit:
#	trick to make 'make' happy at check out time
#	without this aclocal.m4 never updates
#	and becomes older than it's prerequisities
#	thus causing redundant remaking:
#	aclocal+autoconf+automake

	cvs commit -m "no message" -f acsite.m4
	cvs commit -m "no message" -f acinclude.m4
	cvs commit -m "no message" -f aclocal.m4
	cvs commit -m "no message" -f Makefile.am
	cvs commit -m "no message" -f Makefile.in
	cvs commit -m "no message" -f configure.in
	cvs commit -m "no message" -f configure

	cvs commit

happy: update install-exec
