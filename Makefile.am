SUBDIRS = src etc

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

EXTRA_DIST = asm.txt form.txt lang.txt operators.txt sql.txt types.txt parser3.dsw 

cvsupdate:
	cvs update
	$(MAKE)