#!c:\cygwin\bin\sh.exe

if [ $1 = "halt" ]
then
	exit $2
fi

if [ $1 = "stdin" ]
then
	c:/cygwin/bin/cat.exe
	exit 0
fi

if [ $1 = "args" ]
then
	for arg in "$@"; do
		echo -n "\"$arg\" "
	done
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

c:/cygwin/bin/cat.exe -s $3

if [ -n "$4" ]
then
	exit $4
fi

