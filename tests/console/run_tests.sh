#!/bin/sh
# tests for what the main suite cannot set: console arguments, the config name, DOCUMENT_ROOT.
# A test is a directory NNN, the site root: parser3 is called from there.

PARSER=`pwd`/../../src/targets/cgi/parser3

{
cd 001
DIR=`pwd`
BDIR=$(pwd | tr / '\\') # the same with backslashes

printf '\n=== 001 console\n'
$PARSER -f config.p page.html

printf '\n=== 001 console, the config given as ./config.p\n'
$PARSER -f ./config.p page.html

printf '\n=== 001 console, the config, log and file names with backslashes\n'
$PARSER -f "$BDIR\config.p" -l "$BDIR\my.log" "$BDIR\page.html"

printf '\n=== 001 CGI without DOCUMENT_ROOT, as IIS and fcgiwrap call it\n'
env -i GATEWAY_INTERFACE=CGI/1.1 CGI_PARSER_CONFIG=./config.p REQUEST_URI=/page.html SCRIPT_NAME=/page.html PATH_INFO=/page.html PATH_TRANSLATED="$DIR/page.html" $PARSER

printf '\n=== 001 CGI, DOCUMENT_ROOT, PATH_TRANSLATED and a relative CGI_PARSER_LOG with backslashes, the root with a trailing one\n'
env -i GATEWAY_INTERFACE=CGI/1.1 CGI_PARSER_CONFIG=./config.p CGI_PARSER_LOG="log\my.log" REQUEST_URI=/page.html SCRIPT_NAME=/page.html PATH_INFO=/page.html DOCUMENT_ROOT="$DIR\\" PATH_TRANSLATED="$DIR\page.html" $PARSER
cd ..

cd 002
printf '\n=== 002 the config which is the /auto.p too: same path, loaded once\n'
$PARSER -f auto.p page.html

printf '\n=== 002 the same config given as ./auto.p: loaded again as /auto.p [a known limitation]\n'
$PARSER -f ./auto.p page.html

printf '\n=== 002 the page given as ./page.html: auto.p is loaded again as /./auto.p [a known limitation]\n'
$PARSER -f auto.p ./page.html
cd ..
} >tests.log 2>&1

diff -u ok.log tests.log
