#!/bin/sh
# the program leaves its own session and lives on, it must not hold the connection of the request
if command -v setsid >/dev/null 2>&1; then
	setsid sleep 8 >/dev/null 2>&1 </dev/null &
else
	sleep 8 >/dev/null 2>&1 </dev/null & # old systems have no setsid: the program is killed with its group
fi
echo $! >detach.pid
