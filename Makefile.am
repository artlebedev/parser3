SUBDIRS = src etc bin

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

EXTRA_DIST = operators.txt parser3.dsw 

update:
	cvs update
	$(MAKE)

happy: update install
