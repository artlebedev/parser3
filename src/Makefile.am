INCLUDES = $(INCLUDES) -I$(APACHE13_SRC)/include -I$(APACHE13_SRC)/os/unix 
INCLUDES = $(INCLUDES) -I$(XML_SRC)/xml-xerces/c/src -I$(XML_SRC)/xml-xalan/c/src

SUBDIRS = classes types main libltdl pcre targets

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

