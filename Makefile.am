SUBDIRS = src etc bin

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

EXTRA_DIST = operators.txt parser3.dsw 

update:
	cvs update
	$(MAKE)

commit:
	cvs commit -m "no message" -f aclocal.m4
	cvs commit

happy: update install-exec
