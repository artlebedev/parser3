@auto[]
$class_path[^table::set{path
d:\y\parser3\src\www\classes
}]

$limits[
	$.post_max_size(10*0x400*0x400)
]	
$defaults[
	$.content-type0[text/html]
	$.content-type[
		$.value[text/html]
		$.charset[windows-1251]
	]
]

$SQL[
	$.drivers[^table::set{protocol	driver	client
mysql	d:\Y\parser3\src\sql\mysql\debug\parser3mysql.dll	d:\y\parser3\src\sql\mysql\mySQL32\lib\opt\libmySQL.dll
odbc	d:\y\parser3\src\sql\odbc\Debug\parser3odbc.dll
pgsql	d:\y\parser3\src\sql\pgsql\Debug\parser3pgsql.dll	d:\Y\parser3\src\sql\pgsql\PgSQL32\lib\release\libpq.dll
oracle	d:\Y\parser3\src\sql\oracle\Debug\parser3oracle.dll
}]
]

#$ORIGINS(1)
$user-html[^table::set{user	html	comment
<<	&laquo^;	длинные user вперёд
>>	&raquo^;
\n\n	<p>	!эти есть в таблице по умолчанию
\n	<br>	!но т.к. заменяем, надо повторить.
<	&lt^;	!можно этим воспользоваться,
>	&gt^;	!и что-то сделать ДО обычных замен
"	&quot^;	!например из << и >>
&	&amp^;	!сделать ёлочки-кавычки.
_	&nbsp^;
^#AB	&laquo^;	windows коды ёлочек
^#BB	&raquo^;	windows коды ёлочек
(c)	&copy^;
^#A9	&copy^;	windows (c)
}]


#for ^file::load[name;user-name;mime-type << autodetection]
$MIME-TYPES[^table::set{ext	mime-type
zip	application/zip
doc	application/msword
xls	application/vnd.ms-excel
pdf	application/pdf
ppt	application/powerpoint
rtf	application/rtf
gif	image/gif
jpg	image/jpeg
png	image/png
tif	image/tiff
html	text/html
htm	text/html
txt	text/plain
mts	application/metastream
mid	audio/midi
midi	audio/midi
mp3	audio/mpeg
ram	audio/x-pn-realaudio
rpm	audio/x-pn-realaudio-plugin
ra	audio/x-realaudio
wav	audio/x-wav
au	audio/basic
mpg	video/mpeg
avi	video/x-msvideo
mov	video/quicktime
swf	application/x-shockwave-flash
}]

@exception[origin;source;comment;type;code]
ROOT_ERROR:
^if(def $source){$origin '$source'}
${comment}.
^if(def $type){type=$type}
^if(def $code){code=$code}
