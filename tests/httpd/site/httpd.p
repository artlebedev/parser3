@CLASS
httpd

@OPTIONS
partial

@config[cfg]
$result[
	$cfg
	^if($request:path eq "/index.html"){
		$.auth[ $.url[^^/index.html^$] $.login[auth] $.password[test] $.realm[auth test] ]
	}
	^if($request:path ne "/a b/"){
		$.404[/404.html]
	}
	^if(^request:path.left(7) eq "/broken"){
		$.deny[]
	}
]

@preprocess[return]
$response:server[Parser3]
$response:date[]
