@exception[origin;source;comment;type;code]
SITE_ERROR:
^if(def $source){$origin '$source'}
${comment}.
^if(def $type){type=$type}
^if(def $code){code=$code}


@auto[]
$site_test(345)

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
