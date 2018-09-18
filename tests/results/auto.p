@conf[filespec]
$CHARSETS[
	$.windows-1251[../etc/parser3.charsets/windows-1251.cfg]
]

$CLASS_PATH[^table::create{path
/
/tests
}]

$NO_PROTOTYPE[
	$.[336.html][yes]
	$.[225.html][yes]
	$.[051.html][yes]
]

^if(def $NO_PROTOTYPE.[$env:PATH_TRANSLATED]){
	$OBJECT-PROTOTYPE(false)
}

^if($env:PATH_TRANSLATED eq '357.html'){
	$LIMITS[
		$.max_loop(50)
		$.max_recoursion(50)
	]
}
^if($env:PATH_TRANSLATED eq '358.html'){
	$LIMITS[
		$.max_loop(0)
		$.max_recoursion(0)
	]
}
^if($env:PATH_TRANSLATED eq '380.html' || $env:PATH_TRANSLATED eq '394-curl.html' ){
	$LIMITS[
		$.max_file_size(10000)
	]
}


@auto[]


@try-catch[jCode]
^try{$jCode}{
	$exception.handled(true)
	Type: $exception.type,
	Source: $exception.source,
	Comment: $exception.comment
}


@1251[]
^from-1251[]
^to-1251[]


@from-1251[]
$request:charset[windows-1251]


@to-1251[]
$response:charset[windows-1251]


# backward
@windows-1251[]
^1251[]

@os[win;unix][name]
$name[^env:OS.left(7)]
$result[^if(^name.lower[] eq 'windows'){$win}{$unix}]
