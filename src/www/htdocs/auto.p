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
$user-html[^table:set{user	html	comment
<<	&laquo^;	������ user �����
>>	&raquo^;
\n\n	<p>	!�� ���� � ⠡��� �� 㬮�砭��
\n	<br>	!�� �.�. �����塞, ���� �������.
<	&lt^;	!����� �⨬ ��ᯮ�짮������,
>	&gt^;	!� ��-� ᤥ���� �� ������ �����
"	&quot^;	!���ਬ�� �� << � >>
&	&amp^;	!ᤥ���� �窨-����窨.
_	&nbsp^;
^#AB	&laquo^;	windows ���� �祪
^#BB	&raquo^;	windows ���� �祪
(c)	&copy^;
^#A9	&copy^;	windows (c)
}]
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
        ^#AA^#AF^#B2^#B3^#B4^#B8^#BA^#BC^#BD^#BE^#BF�������������������
        ���������������������������������������������]
    $word[
        0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz
        ^#80^#81^#83^#8A^#8C^#8D^#8E^#8F^#90^#9A^#9C^#9D^#9E^#9F^#A1^#A2
        ^#A3^#A5^#A8^#AA^#AF^#B2^#B3^#B4^#B8^#BA^#BC^#BD^#BE^#BF�������
        ���������������������������������������������������������]

    $lowercase[
        AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz^#80^#90^#81^#83
        ^#8A^#9A^#8C^#9C^#8D^#9D^#8E^#9E^#8F^#9F^#A1^#A2^#A3^#BC^#A5^#B4
        ^#A8^#B8^#AA^#BA^#AF^#BF^#B2^#B3^#BD^#BE������������������������
        ����������������������������������������]
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

