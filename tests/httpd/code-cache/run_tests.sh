#!/bin/sh
# the code caching for httpd mode records and then replays the compilation using the cached methods (feature #1302)

PARSER=`pwd`/../../../src/targets/cgi/parser3
PORT=8101

class_file() { # file, class, method, text
	printf '@CLASS\n%s\n\n@%s[]\n%s\n' "$2" "$3" "$4" > "$1"
}

get() { # the response body of the test @name in site/tests.p
	printf 'GET /?%s HTTP/1.0\r\n\r\n' "$1" | nc localhost $PORT | tr -d '\r' | sed '1,/^$/d'
	echo
}

run() { # httpd mode, code cache: on or off
	echo "=== $1, code cache $2"
	class_file site/changing.p Changing version before
	no_cache=; [ $2 = off ] && no_cache=1

	(cd site && TEST_HTTPD_MODE=$1 TEST_NO_CODE_CACHE=$no_cache CGI_PARSER_CONFIG=`pwd`/config.p exec $PARSER -p $PORT) &
	server=$!
	# wait for the server listens
	for i in 1 2 3 4 5; do nc -z localhost $PORT 2>/dev/null && break; sleep 0.2; done

	echo "fields are fresh in each request:"; get fresh; get fresh
	echo "an error in the cached code:"; get failed
	echo "^use of a cached file:"; get use_cached
	echo "a cached partial class extended by the file named in ^process[...][$.file[...]] of the config:"; get extended
	echo "a file compiled into a cached class takes its options:"; get options
	echo "a cached class already defined:"; get defined
	echo "a cached partial class extending one not marked as partial:"; get not_partial
	echo "a cached class with the undefined base:"; get no_base
	echo "a cached file changed after the start:"; get changing
	class_file site/changing.p Changing version after; get changing
	if [ $2 = on ]; then
		chmod a-r site/cached.p
		echo "a cached file unreadable after the start:"; get use_cached
		chmod a+r site/cached.p
	fi

	kill $server; wait $server 2>/dev/null
}

{
run sequental on
run parallel on
run threaded on
run sequental off
} >tests.log 2>&1

rm -f site/changing.p site/parser3.log
diff -u ok.log tests.log
