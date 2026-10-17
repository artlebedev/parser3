#!/bin/sh
# the WebSocket demo: run the server and open http://localhost:8100/ in the browser

cd "$(dirname "$0")"
../../../src/targets/cgi/parser3 -f config.p -p 8100
