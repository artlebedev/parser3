@USE
common/lib.p
common/layout.p
common/implode.p
date/dtf.p
mysql.p
rotabanner_aval.p



#################################################################################################
@auto[]
# id объекта с 404 ошибкой
$MAIN:error404ObjectId(15)
# id объекта, где лежат глобальные данные о сайте
$MAIN:siteDataObjectId(94)
# id объекта, где хранится все о карте украины (карта, image-map)
$MAIN:branchesObjectId(62)
# id объекта, где хранится все о банкоматах украины (карта, image-map)
$MAIN:cashObjectId(63)
$MAIN:pSQL[^mysql::init[$SQL.connect-string;^if(^env:QUERY_STRING.pos[mode=debug] >= 0){debug}]]

^detectBrowser[]
^getLang[]
# это идентификаторы типов объектов для подписки/рассылки
$MAIN:objIdNews(1)
$MAIN:objIdPubs(2)
$MAIN:objIdDigest(3)
$MAIN:objIdVacancy(4)
# $MAIN:objIdInfo(5)
$MAIN:SYS_PATH_PRODUCTSIMAGE[/i/products]
$MAIN:SYS_PATH_PRODUCTSDOC[/off-line/products]

$MAIN:objIdPartners(6)

$MAIN:SYS_PATH_PARTNERSFOOTERSIMAGE[/i/pfooters]
$MAIN:SYS_PATH_NEWSIMAGE[/i/pnews]

#end @auto[]



#################################################################################################
@getLang[][qs;found]
$qs[$env:QUERY_STRING]
$found[^qs.match[(^^|&)lang=([^^&]+)]]
^if($found){
	$MAIN:lang[^found.2.lower[]]
}{
	$MAIN:lang[]
}

$MAIN:langs[^table::create{lang_id	name	prefix	sort_order	charset
1	Русский	rus	1	windows-1251
2	Украинский		2	windows-1251
3	Английский	eng	3	windows-1251}]
$MAIN:lang_id(^if(^MAIN:langs.locate[prefix;$MAIN:lang]){$MAIN:langs.lang_id}{2})

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
# возвращает строку с размером файла
@file_size[file;names;divider][f;size]
^if(!$names){$names[$.b[bytes]^if($MAIN:lang eq "eng"){$.kb[Kb]$.mb[Mb]}{$.kb[КБ]$.mb[МБ]}]}
^if(-f $file){
	$f[^file::stat[$file]]
	^if($f.size < 1000){
		$tmp[$f.size $names.b]
	}{
		^if($f.size < 1000000){
			$size($f.size/1024)
			$tmp[^size.format{%.1f} $names.kb]
		}{
			$size($f.size/1048576)
			$tmp[^size.format{%.2f} $names.mb]
		}
	}
	$result[^if(def $divider){^tmp.match[\.][]{$divider}}{$tmp}]
}{
	$result[]
}
#end @file_size[]


#################################################################################################
@error404[][err_lang]
$err_lang[^MAIN:byLang[/rus/;/eng/;/^if(def $cookie:lang && $cookie:lang ne "ukr"){${cookie:lang}/}]]
#^location[http://${env:SERVER_NAME}${err_lang}404/]
^location[${err_lang}404/]
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
<body bgcolor="#F4F4F4" text="#000000" link="#0000C4" alink="#0000C4" vlink="#073683" marginheight="0"
marginwidth="0"
topmargin="0" leftmargin="0" rightmargin="0" onLoad="init()">
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
#^if($USE_GC){^memory:compact[] ^memory:compact[]}
^if($MAIN:browser eq "nn" && $MAIN:browser_ver < 5){
	$repl[^table::create{a	b
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
$banner[^rotabanner_aval::init[aval;^if(def $form:lang){$form:lang}{ukr}_uho;$.disableCache(1)]]
$result[^result.match[\[(parser)\](.+?)\[/\1\]][gi]{^taint[as-is][^process{$match.2}]}]
# а это мы добавляем статистику к страничке. если она не нужна в страничке - то просто вызываем:
^getStat[]
^rusage[@end]
# а это мы можем вывести список макросов, которые outdated, но которые вызывались...
# ^printOutdatedMacro[]
#end @postprocess[body]



#################################################################################################
@rusage[when]
#^musage[before]
#^memory:compact[]
#^musage[after]
^if(!in "/admin/"){
$rusage[$status:rusage]
^if($rusage){
$m[
	^if($USE_GC){$status:memory;$.nothing[]}
]
$now[^date::now[]]
$s[[^now.sql-string[]] $rusage.utime $rusage.stime $rusage.maxrss $rusage.ixrss $rusage.idrss $rusage.isrss $request:uri $env:REMOTE_ADDR [$m.used/$m.free/$m.since_compact/$m.process] [$when]
]
^s.save[append;/../data/rusage.log]
}}
#end @rusage[]

@musage[comment][v;now;prefix;message;line;usec]
$v[$status:memory]
$now[^date::now[]]
$prefix[[^now.sql-string[]] $env:REMOTE_ADDR: $comment]
$message[$v.used $v.free $v.ever_allocated_since_compact $v.ever_allocated_since_start $request:uri]
$line[$prefix $message ^#0A]
^line.save[append;/../data/musage.log]
$result[]

#################################################################################################
# этот метод выведет статистику, собранную экземпляром класса $MAIN:pSQL
@getStat[]
^if(def $MAIN:pSQL && $MAIN:pSQL.debug){
	$result[^MAIN:pSQL.getStatistics[/../data/sql.txt]]
}{
	$result[]
}
#end @getStat[]


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
		is_published = 1
	ORDER BY
		sort_order
}]
#end @getNewsCategory[]


#################################################################################################
@sendSubscribeMail[type;from;pass;lang][charset]
$charset[^MAIN:byLang[koi8-r;windows-1251;windows-1251]]
^mail:send[
	$.to[$form:email]
	$.from[$from]
	$.charset[$charset]
	$.content-type[
		$.value[text/plain]
		$.charset[$charset]
	]
	$.subject[^if($type eq "insert"){aval subscribe}{aval update subscription}]
	$.body[^if($type eq "insert"){^MAIN:byLang[Адрес "$form:email" был внесен в список рассылки новостей
сайта банка
Аваль

Чтобы сделать подписку активной и/или изменить ее параметры, достаточно зайти на страницу:
http://${env:SERVER_NAME}/rus/press/subscribe/edit/?email=$form:email&code=$pass

Если подписка не будет подтверждена в течение 30  дней с момента
отправки  данного  сообщения, ваш адрес будет автоматически удален из
базы данных.

С уважением, 
пресс-служба банка "Аваль"
http://${env:SERVER_NAME}/rus/
;Your e-mail "$form:email" was listed in the news mailing list of Aval bank website.

For activization of subscription and/or change of subscription parameters you should visit our webpage:
http://${env:SERVER_NAME}/eng/press/subscribe/edit/?email=$form:email&code=$pass

If you will not confirm subscription within 30 days after receiving this
message your address will be automatically removed from mailing database.

Best regards,
Press service of Aval bank
http://${env:SERVER_NAME}/eng/
;Адреса "$form:email" була внесена в список розсилання новин сайта банку Аваль

Щоб зробити пiдписку  активної i/або  зм_нити її параметри, досить зайти на сторiнку:
http://${env:SERVER_NAME}/press/subscribe/edit/?email=$form:email&code=$pass

Якщо пiдписка  не буде пiдтверджена протягом  30  днiв з моменту 
вiдправлення  даного  повiдомлення, ваша  адреса  буде автоматично вилучений з 
бази даних.

З повагою, 
прес-служба банку "Аваль"
http://${env:SERVER_NAME}/
;$lang]}{^MAIN:byLang[Чтобы изменить параметры подписки, достаточно зайти на страницу:
http://${env:SERVER_NAME}/rus/press/subscribe/edit/?email=$form:email&code=$pass

С уважением, 
пресс-служба банка "Аваль"
http://${env:SERVER_NAME}/rus/


;For change of subscription parameters you should visit our webpage:
http://${env:SERVER_NAME}/eng/press/subscribe/edit/?email=$form:email&code=$pass

Best regards,
Press service of Aval bank
http://${env:SERVER_NAME}/eng/

;Щоб змiнити параметри пiдписки, досить зайти на сторiнку:
http://${env:SERVER_NAME}/press/subscribe/edit/?email=$form:email&code=$pass

З повагою, 
прес-служба банку "Аваль"
http://${env:SERVER_NAME}/

;$lang]}
]]
#end @sendSubscribeMail[]
