@auto[]
$user-html[^table:set{user	html	comment
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
#$if(!($SQL is hash)){$SQL[$z[z]]}
$SQL[
	$drivers[^table:set{protocol	driver	client
mysql	Y:\parser3\src\sql\mysql\Debug\parser3mysql.dll	Y:\parser3\src\sql\mysql\mySQL32\lib\opt\libmySQL.dll
}]
]
$SQL.connect-string[mysql://test:test@localhost/test/cp1251_koi8]
$SQL.connect-string[mysql://test:test@[/a/b]/test/cp1251_koi8]
#for ^file:load[name;user-name;mime-type << autodetection]
$MIME-TYPES[^table:set{ext	mime-type
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

@auto_test[]
^BASE.auto_test[]
<li>DR

@main[]
/auto.p main
