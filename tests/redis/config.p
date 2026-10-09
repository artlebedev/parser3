# the redis-server started by run_tests.sh

@auto[]
$options[
	$.port(6399)
	$.password[test]
]

@open_redis[extra][o]
$o[^hash::create[$options]]
^if($extra is hash){^o.add[$extra]}
$result[^redis::open[$o]]

# the value as one-line json, to see the types
@show[value][s]
$s[^json:string[$value]]
$result[^s.match[\n][g]{}]

# the exception of the code, which is expected
@failed[code]
^try{$code}{$exception.handled(true)$exception.type: $exception.comment}

@unhandled_exception[exception;stack]
$exception.type: $exception.comment
