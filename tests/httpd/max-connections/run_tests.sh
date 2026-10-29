#!/bin/sh
# $HTTPD.max-connections: the server rejects new connections above limit with 503, $status:connections

PARSER=`pwd`/../../../src/targets/cgi/parser3
PORT=8102

get() { # GET /?name
	curl -s -w ' [%{http_code}]' http://localhost:$PORT/?$1
	echo
}

run() { # httpd mode, max connections
	echo "=== $1, max-connections $2"
	rm -f site/parser3.log
	(cd site && TEST_HTTPD_MODE=$1 TEST_MAX_CONNECTIONS=$2 CGI_PARSER_CONFIG=`pwd`/config.p exec $PARSER -p $PORT) &
	server=$!
	# wait for the server listens
	for i in 1 2 3 4 5; do nc -z localhost $PORT 2>/dev/null && break; sleep 0.2; done

	get slow >slow1.out & slow1=$!
	sleep 0.1
	get slow >slow2.out & slow2=$!
	sleep 0.1
	echo "two connections are being served:"; get quick; get quick
	wait $slow1 $slow2
	cat slow1.out slow2.out
	echo "they have finished:"; get quick

	kill $server; wait $server 2>/dev/null
	grep -o "max-connections .*" site/parser3.log 2>/dev/null | sort -u
}

{
run parallel 0
run parallel 2
run threaded 0
run threaded 2
} >tests.log 2>&1

rm -f slow1.out slow2.out site/parser3.log
diff -u ok.log tests.log
