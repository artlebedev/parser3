# the memcached on localhost, see run_tests.sh

@auto[]
$server[127.0.0.1]

# the value as one-line json, to see the types
@show[value]
$result[^json:string[$value; $.one-line(true) ]]

# the exception of the code, which is expected
@failed[code]
^try{$code}{$exception.handled(true)$exception.type: $exception.comment}

@unhandled_exception[exception;stack]
$exception.type: $exception.comment
