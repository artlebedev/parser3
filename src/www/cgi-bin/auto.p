@exception[origin;source;comment;type;code]
ERROR: $origin '$source' ${comment}. ^if($type){type=$type }^if($code){code=$code}

@auto[]
$limits[
	$post_max_size(10*0x400*0x400)
]	
$defaults[
	$content-type0[text/html]
	$content-type[
		$value[text/html]
		$charset[windows-1251]
	]
]
