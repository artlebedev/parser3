#!/bin/sh
echo $$ >hang.pid
sleep 8 &
echo $! >hang-child.pid
wait
