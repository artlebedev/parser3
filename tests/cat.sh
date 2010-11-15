#!/bin/sh

if [ $1 = "halt" ]
then
	exit $2
fi

if [ $1 = "stdin" ]
then
	cat
	exit 0
fi

if [ $1 = "cgi" ]
then
	echo "content-type: $2
"
fi

if [ -n "$5" ]
then
	echo "$5"
fi

cat -s $3

if [ -n "$4" ]
then
	exit $4
fi

