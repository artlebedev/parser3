#!/bin/sh
# ^cache under parallel requests (ticket #1153): the main suite is single process.
# The requests are page.html called as CGI from here, the site root; the cache file and the counter of runs are here too.

PARSER=`pwd`/../../src/targets/cgi/parser3
DIR=`pwd`
ROUNDS=3

request() { # the query string of page.html, prints the response body
	env -i GATEWAY_INTERFACE=CGI/1.1 QUERY_STRING="$1" REQUEST_METHOD=GET SCRIPT_NAME=/page.html PATH_INFO=/page.html DOCUMENT_ROOT="$DIR" PATH_TRANSLATED="$DIR/page.html" $PARSER | tr -d '\r' | sed '1,/^$/d'
}

parallel() { # count, query string: the requests start at once, prints how many got which answer
	i=1
	# not through request(): the pipes delay the start, and the requests must read the cache at the same time
	while [ $i -le $1 ]; do
		env -i GATEWAY_INTERFACE=CGI/1.1 QUERY_STRING="$2" REQUEST_METHOD=GET SCRIPT_NAME=/page.html PATH_INFO=/page.html DOCUMENT_ROOT="$DIR" PATH_TRANSLATED="$DIR/page.html" $PARSER > out.$i &
		i=$((i+1))
	done
	wait
	cat out.* | tr -d '\r' | grep -v '^Content-' | grep -v '^$' | sort | uniq -c | sed 's/^ *//'
	rm -f out.*
}

rounds() { # the state to start with [a file], count, query string: the same requests several times, one after another
	for round in `seq 1 $ROUNDS`; do
		rm -f cache.bin; [ -n "$1" ] && cp $1 cache.bin
		parallel $2 "$3"
	done
}

runs() { # how many times the code has run since the last call
	echo "the code ran: `cat runs.txt 2>/dev/null | wc -l | tr -d ' '`"
	rm -f runs.txt
}

holder() { # another process renews the cache under the lock, as the previous versions do: till release, or 3 seconds
	flock -x cache.bin sleep 3 &
	holder_pid=$!
	sleep 0.3
}

release() {
	kill $holder_pid 2>/dev/null
	wait $holder_pid 2>/dev/null
}

timed() { # the query string: the response, and if the request had to wait for the lock
	start=`date +%s`
	request "$1"
	[ $((`date +%s`-start)) -lt 2 ] && echo "did not wait" || echo "waited"
}

expired() { # the text: the cache with it which has just expired [no more than a second ago], kept in expired.bin
	rm -f cache.bin
	request "expires=1&sleep=0&text=$1" >/dev/null
	sleep 1.1
	mv cache.bin expired.bin
	rm -f runs.txt
}

{
echo "=== no cache yet, 20 requests at once, $ROUNDS times: the code runs once each time, the others wait for it"
rm -f runs.txt
rounds '' 20 'expires=60&sleep=0.5&text=first'
runs

echo "=== the cache has expired, 20 requests at once, $ROUNDS times: one renews it, the others get the old copy meanwhile"
expired old
rounds expired.bin 20 'expires=60&sleep=1&text=new'
runs

echo "=== the same with \$.extend(0): the others wait for the new copy"
rounds expired.bin 10 'expires=60&extend=0&sleep=1&text=new'
runs

echo "=== the cache has expired more than \$.extend seconds ago [the page is requested seldom]: the others wait for the new copy"
sleep 2
rounds expired.bin 10 'expires=60&extend=1&sleep=1&text=new'
runs

echo "=== the code fails when the cache is renewed: the old copy is given for \$.extend seconds, then the cache is renewed"
expired old
cp expired.bin cache.bin
request 'expires=60&extend=2&sleep=0&fail=1&text=new' | grep -o 'generation failed' | head -1
request 'expires=60&extend=2&sleep=0&text=new'
sleep 2.5
request 'expires=60&extend=2&sleep=0&text=new'
runs

echo "=== the code runs longer than \$.extend: the next request renews the cache too, the one who ends later writes the copy"
expired old
cp expired.bin cache.bin
request 'expires=60&extend=2&sleep=4&text=long' > long.txt &
long_pid=$!
sleep 0.3
request 'expires=60&extend=2&sleep=0&text=new'
sleep 2.5
request 'expires=60&extend=2&sleep=0&text=renewed'
wait $long_pid
cat long.txt; rm -f long.txt
request 'expires=60&extend=2&sleep=0&text=other'
runs

echo "=== the copy has expired, the other process renews it under the lock: the request waits for it, then renews the cache itself [the holder did not]"
expired old
cp expired.bin cache.bin
holder
timed 'expires=60&sleep=0&text=new'
release
runs

echo "=== the file is cut: not given"
rm -f cache.bin
request 'expires=60&sleep=0&text=original' >/dev/null
head -c 40 cache.bin > cut.bin; mv cut.bin cache.bin
rm -f runs.txt
request 'expires=60&sleep=0&text=renewed'
runs

echo "=== the new copy is written in place of the longer one: the file is cut to it"
expired `head -c 10000 /dev/zero | tr '\0' 'x'`
cp expired.bin cache.bin
request 'expires=60&sleep=0&text=short'
request 'expires=60&sleep=0&text=other'
[ `wc -c < cache.bin` -lt 1000 ] && echo "the file is cut" || echo "the file is not cut"
runs
rm -f cache.bin expired.bin runs.txt
} >tests.log 2>&1

diff -u ok.log tests.log
