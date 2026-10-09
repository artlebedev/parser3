# the memcached on localhost, see run_tests.sh

@auto[]
$server[127.0.0.1:11211]

@open_memcached[]
$result[^memcached::open[$server]]

# the value as one-line json, to see the types
@show[value][s]
$s[^json:string[$value]]
$result[^s.match[\n][g]{}]

# the exception of the code, which is expected
@failed[code]
^try{$code}{$exception.handled(true)$exception.type: $exception.comment}

@unhandled_exception[exception;stack]
$exception.type: $exception.comment
