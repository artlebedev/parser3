@CLASS
httpd

@OPTIONS
partial

@config[cfg]
$result[
	$cfg
	$.auth[ $.url[^^/index.html^$] $.login[auth] $.password[test] $.realm[auth test] ]
]

@preprocess[return]
$response:server[Parser3]
$response:date[]
