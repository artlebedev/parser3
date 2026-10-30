#!/bin/sh
# $response:timeout: the request which takes too long is interrupted, in parallel mode the process is killed if it does not stop

PARSER=`pwd`/../../../src/targets/cgi/parser3
PORT=8103

if command -v curl >/dev/null 2>&1; then
	get() { # GET /?name
		curl -s -m 6 -w ' [%{http_code}]' http://localhost:$PORT/?$1
		echo
	}
else
	get() { # GET /?name, fetch (FreeBSD) does not tell the status: [200] or nothing
		fetch -q -T 6 -o - http://localhost:$PORT/?$1 2>/dev/null && echo ' [200]' || echo ' [000]'
	}
fi

fast() { # GET /?name, must be answered at once
	start=`date +%s`
	get $1
	[ `date +%s` -lt $((start+4)) ] || echo "too slow"
}

alive() { # the process exists and is not a zombie
	pid=$1
	set -- `ps -o stat= -p $pid 2>/dev/null`
	[ -n "$1" ] && [ "${1#Z}" = "$1" ]
}

run() { # httpd mode
	echo "=== $1"
	rm -f site/parser3.log site/hang.pid site/hang-child.pid site/detach.pid
	# not in a subshell: on FreeBSD $! is the subshell then, the killed one is not the server
	cd site
	TEST_HTTPD_MODE=$1 CGI_PARSER_CONFIG=`pwd`/config.p $PARSER -p $PORT &
	server=$!
	cd ..
	# wait for the server listens
	for i in 1 2 3 4 5; do nc -z localhost $PORT 2>/dev/null && break; sleep 0.2; done

	echo "sleep:"; get sleep
	echo "loop:"; get loop
	echo "sleep in try:"; get try
	echo "loop in try:"; get try-loop
	echo "sleep in nested try with finally:"; get try-nested
	echo "timeout cancelled:"; get cancel
	echo "timeout restarted:"; get restart
	echo "hang in the executed program:"; get hang
	if [ "$1" = sequental ]; then
		# the server is busy with this request, the connection to it is accepted and not answered
		echo "http client waiting for the answer of the busy server itself:"; fast self
	fi
	echo "interruption swallowed in a libxml callback:"; fast xml-callback
	echo "the program and its child are killed:"
	for f in hang.pid hang-child.pid; do alive `cat site/$f` && echo "$f is alive"; done
	if [ "$1" = parallel ]; then
		# the program which has left holds the connection if it is inherited, the killed process can't close it
		echo "@unhandled_exception is slow, the process is killed, the connection is closed at once:"; fast slow-unhandled
	fi
	echo "the server goes on:"; get quick

	{ kill $server; wait $server; } 2>/dev/null
	# the program which has left lives on
	[ -f site/detach.pid ] && kill `cat site/detach.pid` 2>/dev/null
	grep -o "response timeout (1 s) exceeded, process killed" site/parser3.log 2>/dev/null | sort -u
}

{
run sequental
run parallel
run threaded
} >tests.log 2>&1

rm -f site/parser3.log site/hang.pid site/hang-child.pid site/detach.pid
diff -u ok.log tests.log
