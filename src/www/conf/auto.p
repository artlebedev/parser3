@auto[]
$user-html[^table::set{user	html	comment
<<	&laquo^;	длинные user вперЄд
>>	&raquo^;
\n\n	<p>	!эти есть в таблице по умолчанию
\n	<br>	!но т.к. замен€ем, надо повторить.
<	&lt^;	!можно этим воспользоватьс€,
>	&gt^;	!и что-то сделать ƒќ обычных замен
"	&quot^;	!например из << и >>
&	&amp^;	!сделать Єлочки-кавычки.
_	&nbsp^;
^#AB	&laquo^;	windows коды Єлочек
^#BB	&raquo^;	windows коды Єлочек
(c)	&copy^;
^#A9	&copy^;	windows (c)
}]
#$if(!($SQL is hash)){$SQL[$z[z]]}
$SQL[
	$drivers[^table::set{protocol	driver	client
mysql	d:/y/parser3\src\sql\mysql\release\parser3mysql.dll	d:/y/parser3\src\sql\mysql\mySQL32\lib\opt\libmySQL.dll
}]
]

#for ^file:load[name;user-name;mime-type << autodetection]
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

$LIMITS[
	$post_max_size(10*0x400*0x400)
]	
$DEFAULTS[
	$content-type0[text/html]
	$content-type[
		$value[text/html]
		$charset[windows-1251]
	]
]
#for ^match{}[i]
$CTYPE[
    $white-space[
        ^#09^#0A^#0B^#0C^#0D^#20^#A0]
    $digit[
        0123456789]
    $hex-digit[
        0123456789ABCDEFabcdef]
    $letter[
        ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz^#80^#81^#83
        ^#8A^#8C^#8D^#8E^#8F^#90^#9A^#9C^#9D^#9E^#9F^#A1^#A2^#A3^#A5^#A8
        ^#AA^#AF^#B2^#B3^#B4^#B8^#BA^#BC^#BD^#BE^#BFјЅ¬√ƒ≈∆«»… ЋћЌќѕ–—“
        ”‘’÷„ЎўЏџ№Ёёяабвгдежзийклмнопрстуфхцчшщъыьэю€]
    $word[
        0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz
        ^#80^#81^#83^#8A^#8C^#8D^#8E^#8F^#90^#9A^#9C^#9D^#9E^#9F^#A1^#A2
        ^#A3^#A5^#A8^#AA^#AF^#B2^#B3^#B4^#B8^#BA^#BC^#BD^#BE^#BFјЅ¬√ƒ≈∆
        «»… ЋћЌќѕ–—“”‘’÷„ЎўЏџ№Ёёяабвгдежзийклмнопрстуфхцчшщъыьэю€]

    $lowercase[
        AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz^#80^#90^#81^#83
        ^#8A^#9A^#8C^#9C^#8D^#9D^#8E^#9E^#8F^#9F^#A1^#A2^#A3^#BC^#A5^#B4
        ^#A8^#B8^#AA^#BA^#AF^#BF^#B2^#B3^#BD^#BEјаЅб¬в√гƒд≈е∆ж«з»и…й кЋл
        ћмЌнќоѕп–р—с“т”у‘ф’х÷ц„чЎшўщЏъџы№ьЁэёюя€]
]


$MAIL[
	$SMTP[mail.office.design.ru]
#	$prog1[/usr/sbin/sendmail -t]
#	$prog2[/usr/lib/sendmail -t]
]

@exception[origin;source;comment;type;code][i;j]
<body bgcolor=white>
<font color=black>
CONF_ERROR:
^if(def $source){$origin '$source'}
${comment}.
^if(def $type){type=$type}
^if(def $code){code=$code}

@auto_test[]
<li>ROOT

