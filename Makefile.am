SUBDIRS = src etc bin

EXTRA_DIST = operators.txt parser3.sln gnu.vcproj buildall-with-xml buildall-without-xml acsite.m4

update:
	cvs update
	$(MAKE)

commit:
#	trick to make 'make' happy at check out time
#	without this aclocal.m4 never updates
#	and becomes older than it's prerequisities
#	thus causing redundant remaking:
#	aclocal+autoconf+automake

	cvs commit -m "$(m)" -f acsite.m4
	cvs commit -m "$(m)" -f acinclude.m4
	cvs commit -m "$(m)" -f aclocal.m4
	cvs commit -m "$(m)" -f Makefile.am
	cvs commit -m "$(m)" -f Makefile.in
	cvs commit -m "$(m)" -f configure.in
	cvs commit -m "$(m)" -f configure

	cvs commit

happy: update install-exec
