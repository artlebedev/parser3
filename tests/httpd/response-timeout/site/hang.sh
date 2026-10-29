#!/bin/sh
echo $$ >hang.pid
sleep 5 &
echo $! >hang-child.pid
wait
