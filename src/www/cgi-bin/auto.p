@exception[origin;source;comment;type;code]
ERROR: $origin '$source' ${comment}. type=$type code=$code

@auto[]
$limits[
	$post_max_size(10*0x400*0x400)
]	
$defaults[
	$content-type[
		$value[text/html]
		
	]
]
