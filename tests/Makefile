ifdef COMSPEC
	PARSER_DIR=../src/targets/cgi/release
	PARSER_EXECUTABLE=$(PARSER_DIR)/parser3.exe
	PARSER_CONFIG=$(PARSER_DIR)/auto.p
else
	PARSER_DIR=../src/targets/cgi
	PARSER_EXECUTABLE=$(PARSER_DIR)/parser3
	PARSER_CONFIG=$(PARSER_DIR)/auto.p
endif

RUN_PARSER=./run_parser.sh
DIFF=diff -b
ECHO=echo
RM=rm

#

OUTPUTS=$(patsubst %.html,outputs/%.processed,$(wildcard *.html))
RESULTS=$(patsubst %.html,results/%.processed,$(wildcard *.html))

outputs/%.processed: %.html
# Makefile $(RUN_PARSER) $(PARSER_EXECUTABLE) $(PARSER_CONFIG)
	$(RUN_PARSER) $(PARSER_EXECUTABLE) $< > $@

.PHONY: FORCE all tests clean install install-exec install-data

results/%.processed: outputs/%.processed FORCE
	-$(DIFF) $< $@

tests: outputs compares

outputs: $(OUTPUTS)

compares: $(RESULTS)
	@$(ECHO) DONE

clean:
	$(RM) -f outputs/*.processed

install:
install-exec:
install-data:
