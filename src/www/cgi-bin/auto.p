@main[]
^untaint[as-is]{^process[$request:query]}
^$request:query=$request:query<BR>
^$request:uri=$request:uri<BR>

^rem{$response:content-type[test]}

@exception[origin;source;comment;type;code]
ERROR: $origin '$source' ${comment}. type=$type code=$code

@auto[]
$limits[
	$post_max_size(10*0x400*0x400)
]	
^rem{$defaults[
	$content-type[text/html-z]
]}
