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

COMMON=$(sort $(wildcard ???.html))
OUTPUTS_COMMON=$(patsubst %.html,outputs/%.processed,$(COMMON))
RESULTS_COMMON=$(patsubst %.html,results/%.processed,$(COMMON))

CURL=$(sort $(wildcard *-curl.html))
OUTPUTS_CURL=$(patsubst %.html,outputs/%.processed,$(CURL))
RESULTS_CURL=$(patsubst %.html,results/%.processed,$(CURL))

SQL=$(sort $(wildcard *-sql.html))
OUTPUTS_SQL=$(patsubst %.html,outputs/%.processed,$(SQL))
RESULTS_SQL=$(patsubst %.html,results/%.processed,$(SQL))

outputs/%.processed: %.html
	$(RUN_PARSER) $(PARSER_EXECUTABLE) $< $(PARSER_CONFIG) > $@

.PHONY: FORCE tests tests-curl tests-sql all clean

results/%.processed: outputs/%.processed FORCE
	-$(DIFF) $@ $<

tests: $(OUTPUTS_COMMON) $(RESULTS_COMMON)

tests-curl: $(OUTPUTS_CURL) $(RESULTS_CURL)

tests-sql: $(OUTPUTS_SQL) $(RESULTS_SQL)

all: tests tests-curl tests-sql

clean:
	$(RM) -f outputs/*.processed
