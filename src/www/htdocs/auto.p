@auto[]
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
$LOCALE[
#for ^match{}[i]
	$ctype[Russian_Russia.1251]
]
$SQL[
	$connect-string[mysql://test:test@localhost/test/cp1251_koi8]
	$drivers[^table:set{protocol	driver	client
mysql	Y:\parser3\src\sql\mysql\Debug\parser3mysql.dll	Y:\parser3\src\sql\mysql\mySQL32\lib\opt\libmySQL.dll
}]
]
$MAIL[
	$SMTP[mail.office.design.ru]
#	$prog1[/usr/sbin/sendmail -t]
#	$prog2[/usr/lib/sendmail -t]
]
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

