#!/bin/sh
# the memcached class: the pages are run by the console parser3 against the memcached on localhost:11211,

PARSER=`pwd`/../../src/targets/cgi/parser3

{
for page in *.html; do
	printf '\n=== %s\n' $page
	$PARSER -f config.p $page
done
} >tests.log 2>&1

diff -u ok.log tests.log
