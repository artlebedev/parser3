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
        ^#AA^#AF^#B2^#B3^#B4^#B8^#BA^#BC^#BD^#BE^#BFÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒ
        ÓÔÕÖ×ØÙÚÛÜİŞßàáâãäåæçèéêëìíîïğñòóôõö÷øùúûüışÿ]
    $word[
        0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz
        ^#80^#81^#83^#8A^#8C^#8D^#8E^#8F^#90^#9A^#9C^#9D^#9E^#9F^#A1^#A2
        ^#A3^#A5^#A8^#AA^#AF^#B2^#B3^#B4^#B8^#BA^#BC^#BD^#BE^#BFÀÁÂÃÄÅÆ
        ÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞßàáâãäåæçèéêëìíîïğñòóôõö÷øùúûüışÿ]

    $lowercase[
        AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz^#80^#90^#81^#83
        ^#8A^#9A^#8C^#9C^#8D^#9D^#8E^#9E^#8F^#9F^#A1^#A2^#A3^#BC^#A5^#B4
        ^#A8^#B8^#AA^#BA^#AF^#BF^#B2^#B3^#BD^#BEÀàÁáÂâÃãÄäÅåÆæÇçÈèÉéÊêËë
        ÌìÍíÎîÏïĞğÑñÒòÓóÔôÕõÖö×÷ØøÙùÚúÛûÜüİıŞşßÿ]
]


$SQL[
	$drivers[^table:set{protocol	driver	client
mysql	Y:\parser3\src\sql\mysql\Debug\parser3mysql.dll	Y:\parser3\src\sql\mysql\mySQL32\lib\opt\libmySQL.dll
}]
]
$MAIL[
	$SMTP[mail.office.design.ru]
	$SMTP[194.85.21.1:2500]
#	$prog1[/usr/sbin/sendmail -t]
#	$prog2[/usr/lib/sendmail -t]
]

@auto_test[]
<li>ROOT

