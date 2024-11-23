#!/bin/sh

rm -f tests.log

for f in $(ls ???.txt ???.curl | sort)
do
    echo "running test $f" | tee -a tests.log
    case "$f" in
        *.txt)
            cat "$f" |netcat localhost 8100 >>tests.log
            ;;
        *.curl)
            curl -s --config "$f" http://localhost:8100/form.html >> tests.log
            ;;
    esac
    echo "\n=============" >>tests.log
done

diff -u ok.log tests.log
