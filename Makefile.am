SUBDIRS = src etc bin

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

EXTRA_DIST = operators.txt parser3.dsw 

update:
	cvs update
	$(MAKE)

commit:
	cvs commit -m "no message" -f Makefile.am
	cvs commit -m "no message" -f Makefile.in
	cvs commit -m "no message" -f configure.in
	cvs commit -m "no message" -f configure
	cvs commit

happy: update install-exec
