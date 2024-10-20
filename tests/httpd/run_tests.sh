#!/bin/sh

rm -f tests.log

for f in ???.txt
do
    echo "running test $f" | tee -a tests.log
    cat $f |netcat localhost 8100 >>tests.log
    echo "\n=============" >>tests.log

done

diff -u ok.log tests.log
