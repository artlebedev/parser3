#!/bin/sh
# httpd mode: the server runs the site with httpd class from etc/auto.p, see site/config.p

DIR=`pwd`
PARSER=`pwd`/../../src/targets/cgi/parser3
PORT=8100

(cd site && exec $PARSER -f config.p -p $PORT) &
server=$!
trap 'kill $server; exit 1' INT TERM
# wait for the server listens
for i in 1 2 3 4 5; do nc -z localhost $PORT 2>/dev/null && break; sleep 0.2; done

# the dates and the version of the server differ from run to run
{
for f in `ls ???.txt ???.curl | sort`; do
	echo "running test $f"
	case $f in
		*.txt) nc localhost $PORT < $f ;;
		*.curl) curl -s --config $f http://localhost:$PORT/form.html ;;
	esac
	echo
	echo "============="
done
} 2>&1 | sed -E \
	-e 's/^(Date|Last-Modified): .* GMT/\1: DATE GMT/' \
	-e 's/^Server: Parser3\.[[:print:]]*/Server: Parser3.X.X .../' \
	> tests.log

kill $server; wait $server 2>/dev/null

rm -f site/parser3.log
diff -u ok.log tests.log
