ifdef COMSPEC
	PARSER_DIR=../src/targets/cgi/release
	PARSER_EXECUTABLE=$(PARSER_DIR)/parser3.exe
else
	PARSER_DIR=../src/targets/cgi
	PARSER_EXECUTABLE=$(PARSER_DIR)/parser3
endif
PARSER_CONFIG=results/auto.p

RUN_PARSER=./run_parser.sh
DIFF=diff -b
RM=rm

#

OUTPUTS=$(patsubst %.html,outputs/%.processed,$(wildcard *.html))
RESULTS=$(patsubst %.html,results/%.processed,$(wildcard *.html))

outputs/%.processed: %.html
	$(RUN_PARSER) $(PARSER_EXECUTABLE) $< $(PARSER_CONFIG) > $@

.PHONY: FORCE all tests clean install install-exec install-data

results/%.processed: outputs/%.processed FORCE
	-$(DIFF) $< $@

tests: outputs compares

outputs: $(OUTPUTS)

compares: $(RESULTS)

clean:
	$(RM) -f outputs/*.processed

install:
install-exec:
install-data:
