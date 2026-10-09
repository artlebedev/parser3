#!/bin/sh
# the redis class: the pages are run by the console parser3 against a temporal redis-server

PARSER=`pwd`/../../src/targets/cgi/parser3
DIR=`pwd`

PORT=6399
TLS_PORT=6400

# a ca with the server and a client certificate, and another ca with a client certificate
TLS=$DIR/tls
mkdir -p $TLS
for ca in ca other-ca; do
	openssl req -x509 -newkey rsa:2048 -nodes -days 1 -subj /CN=$ca -keyout $TLS/$ca.key -out $TLS/$ca.crt 2>/dev/null
done
for cert in ca/server ca/client other-ca/other-client; do
	ca=${cert%/*} name=${cert#*/}
	openssl req -newkey rsa:2048 -nodes -subj /CN=localhost -keyout $TLS/$name.key -out $TLS/$name.csr 2>/dev/null
	openssl x509 -req -days 1 -in $TLS/$name.csr -CA $TLS/$ca.crt -CAkey $TLS/$ca.key -CAcreateserial -out $TLS/$name.crt 2>/dev/null
done

redis-server --port $PORT --unixsocket $DIR/redis.sock --unixsocketperm 700 --requirepass test \
	--tls-port $TLS_PORT --tls-cert-file $TLS/server.crt --tls-key-file $TLS/server.key \
	--tls-ca-cert-file $TLS/ca.crt --tls-auth-clients optional \
	--save '' --appendonly no --dir $DIR --logfile $DIR/redis.log &
server=$!

# wait for the server listens
for i in 1 2 3 4 5; do nc -z localhost $PORT 2>/dev/null && break; sleep 0.2; done

{
for page in *.html; do
	printf '\n=== %s\n' $page
	REDIS_SOCKET=$DIR/redis.sock REDIS_TLS=$TLS $PARSER -f config.p $page
done
} >tests.log 2>&1

kill $server; wait $server 2>/dev/null
rm -rf redis.log redis.sock $TLS
diff -u ok.log tests.log
