#!/bin/sh

if [ $1 = "cgi" ]
then
	echo "content-type: $2
"
fi


if [ -n "$4" ]
then
	echo "$4"
fi

cat -s $3

