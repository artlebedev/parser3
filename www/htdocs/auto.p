@USE
common/lib.p
common/layout.p
common/implode.p
date/dtf.p
engine_aval.p
mysql.p



#################################################################################################
@auto[]
^if(in "/admin/"){
	^use[admin.p]
}
# id объекта, где лежат глобальные данные о сайте
$MAIN:siteDataObjectId(94)
# id объекта, где хранится все о карте украины (карта, image-map)
$MAIN:branchesObjectId(62)
# id объекта, где хранится все о банкоматах украины (карта, image-map)
$MAIN:cashObjectId(63)
$MAIN:pSQL[^mysql::init[$SQL.connect-string]]
^detectBrowser[]
^getLang[]
$MAIN:objIdNews(1)
$MAIN:objIdPubs(2)
$MAIN:objIdDigest(3)
$MAIN:objIdVacancy(4)
#$MAIN:objIdInfo(5)
$MAIN:SYS_PATH_PRODUCTSIMAGE[/i/products]
#end @auto[]



#################################################################################################
@getLang[][qs;found]
$qs[$env:QUERY_STRING]
$found[^qs.match[(^^|&)lang=([^^&]+)]]
$MAIN:lang_id(2)
^if($found){
	$MAIN:lang[^found.2.lower[]]
}{
	$MAIN:lang[]
}
$result[]
#end @getLang[]



#################################################################################################
@byLang[rus;eng;ukr;lang_id]
^switch[^if(def $lang_id){$lang_id}{$MAIN:lang_id}]{
	^case[1]{$result[$rus]}
	^case[2]{$result[$ukr]}
	^case[3]{$result[$eng]}
	^case[DEFAULT]{$result[]}
}


#################################################################################################
@exception[origin;source;comment;type;code;stack][i;j]
<body>
<font color="black">
^if(def $source){$origin '$source'}
<pre>^untaint[html]{$comment}</pre>
^if(def $type){type=$type}
^if(def $code){code=$code}
^if($stack){
    <hr>
    stack backtrace:<br />
    ^stack.menu{
        $stack.origin: $stack.name<br />
    }
}
#end



#################################################################################################
@error404[]
^location[http://$env:SERVER_NAME/^MAIN:byLang[rus/;eng/;]404/]
#end @error404[]



#################################################################################################
@pageHeader[header]
$header
#end @pageHeader[]



#################################################################################################
@pageFooter[]
footer
#end @pageFooter[]



#################################################################################################
@body[content]
<body bgcolor="#F4F4F4" text="#000000" link="#0000C4" alink="#0000C4" vlink="#073683" marginheight="0" marginwidth="0" topmargin="0" leftmargin="0" rightmargin="0" onLoad="init()">
	$content
</body>
#end @body[]



#################################################################################################
@html[header;content]
<!-- Copyright (c) Art. Lebedev Studio | http://www.design.ru/ -->
<html>
<head>
	<base target="_top">
	<link rel="stylesheet" type="text/css" href="/main.css"></link>
	<title>AB.LV^if(def $header){ | ^taint[as-is][$header] }</title>
</head>
^body{
	<table cellpadding="0" cellspacing="0" border="0" width="100%" bgcolor="#ffffff">
	^pageHeader[]
	$content
	^pageFooter[]
	</table>
}
<html>
#end @html[]


#################################################################################################
@main[]
^pSQL.server{
	<html>
	<head>
		^if(def $prepare){^prepare[]}
	</head>
	^body{
		^content[]
	}
	</html>
}
# @main[]



#################################################################################################
@postprocess[body][repl]
#^rusage[]
^if($MAIN:browser eq "nn" && $MAIN:browser_ver < 5){
	$repl[^table::set{a b
&hellip^;	&#133^;
&rdquo^;	&#148^;
&ldquo^;	&#147^;
&mdash^;	&#151^;
&ndash^;	&#150^;
&bull^;	&#149^;}]
	$result[^if(def $body){^body.replace[$repl]}]
}{
	$result[$body]
}
# а это мы можем вывести список макросов, которые outdated, но которые вызывались...
# ^printOutdatedMacro[]
#end @postprocess[body]



#################################################################################################
@rusage[]
#^if(!in "/admin/"){
$rusage[$status:rusage]
$now[^date::now[]]
$s[[^now.sql-string[]] $rusage.utime $rusage.stime $rusage.maxrss $rusage.ixrss $rusage.idrss $rusage.isrss $request:uri
]
^s.save[append;/../data/rusage.log]
#}
#end



#################################################################################################
@getNewsCategory[]
$result[^table::sql{
	SELECT
		news_category_id AS id,
		name,
		name_ukr,
		name_eng,
		path
	FROM
		news_category
	WHERE
		is_published = '1'
	ORDER BY
		sort_order
}]



#################################################################################################
@sendSubscribeMail[type;from;pass;lang]
^mail:send[
	$.to[$form:email]
	$.from[$from]
	$.charset[windows-1251]
	$.content-type[
		$.value[text/plain]
		$.charset[windows-1251]
	]
	$.subject[^if($type eq "insert"){aval subscribe}{aval update subscription}]
	$.body[^if($type eq "insert"){^MAIN:byLang[Адрес "$form:email" был внесен в список рассылки новостей сайта банка Аваль

Чтобы сделать подписку активной и/или изменить ее параметры, достаточно зайти на страницу:
http://${env:SERVER_NAME}/rus/press/subscribe/edit/?email=$form:email&code=$pass

Если подписка не будет подтверждена в течение 30  дней с момента
отправки  данного  сообщения, ваш адрес будет автоматически удален из
базы данных.

С уважением, 
пресс-служба банка "Аваль"
http://${env:SERVER_NAME}/rus/
;ENGLISH TEXT: Email "$form:email" был внесен в список рассылки новостей сайта банка Аваль

Чтобы сделать подписку активной и/или изменить ее параметры, достаточно зайти на страницу:
http://${env:SERVER_NAME}/eng/press/subscribe/edit/?email=$form:email&code=$pass

Если подписка не будет подтверждена в течение 30  дней с момента
отправки  данного  сообщения, ваш адрес будет автоматически удален из
базы данных.

С уважением, 
пресс-служба банка "Аваль"
http://${env:SERVER_NAME}/eng/
;УКРАИНСКИЙ ТЕКСТ:Адрес "$form:email" был внесен в список рассылки новостей сайта банка Аваль

Чтобы сделать подписку активной и/или изменить ее параметры, достаточно зайти на страницу:
http://${env:SERVER_NAME}/press/subscribe/edit/?email=$form:email&code=$pass

Если подписка не будет подтверждена в течение 30  дней с момента
отправки  данного  сообщения, ваш адрес будет автоматически удален из
базы данных.

С уважением, 
пресс-служба банка "Аваль"
http://${env:SERVER_NAME}/
;$lang]}{^MAIN:byLang[Чтобы изменить параметры подписки, достаточно зайти на страницу:
http://${env:SERVER_NAME}/rus/press/subscribe/edit/?email=$form:email&code=$pass

С уважением, 
пресс-служба банка "Аваль"
http://${env:SERVER_NAME}/rus/


;ENGLISH TEXT: Чтобы изменить параметры подписки, достаточно зайти на страницу:
http://${env:SERVER_NAME}/eng/press/subscribe/edit/?email=$form:email&code=$pass

С уважением, 
пресс-служба банка "Аваль"
http://${env:SERVER_NAME}/eng/

;УКРАИНСКИЙ ТЕКСТ: Чтобы изменить параметры подписки, достаточно зайти на страницу:
http://${env:SERVER_NAME}/press/subscribe/edit/?email=$form:email&code=$pass

С уважением, 
пресс-служба банка "Аваль"
http://${env:SERVER_NAME}/

;$lang]}
]]
#end @sendMail[]



