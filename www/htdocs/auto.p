@USE
common/lib.p
common/layout.p
common/implode.p
common/graph.p
date/dtf.p
engine_mdm.p
mysql.p


@auto[]
^if(in "/admin/"){
	^use[admin.p]
}
$MAIN:pSQL[^mysql::init[$SQL.connect-string]]
^detectBrowser[]
$MAIN:newsfileDir[/i/news/]
#end 


@exception[origin;source;comment;type;code;stack][i;j]
<body>
<font color="black">
^if(def $source){$origin '$source'}
${comment}.
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
^location[http://$env:SERVER_NAME/404/]
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
#paf manually ^rusage[]
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
#end @postprocess[body]



#################################################################################################
@getNewsType[]
$result[^table::sql{
	SELECT
		news_type_id AS id,
		name,
		path
	FROM
		news_type
	WHERE
		is_published = '1'
	ORDER BY
		sort_order
}]



#################################################################################################
@sendSubscribeMail[type;from;pass]
^mail:send[
	$.to[$form:email]
	$.from[$from]
	$.charset[KOI8-R]
	$.content-type[
		$.value[text/plain]
		$.charset[KOI8-R]
	]
	$.subject[^if($type eq "insert"){mdm subscribe}{mdm update subscription}]
	$.body[^if($type eq "insert"){Адрес "$form:email" был внесен в список рассылки новостей сайта МДМ

Чтобы сделать подписку активной и/или изменить ее параметры, достаточно зайти на страницу:
http://${env:SERVER_NAME}/press/subscribe/edit/?email=$form:email&code=$pass

Если подписка не будет подтверждена в течение 30  дней с момента
отправки  данного  сообщения, ваш адрес будет автоматически удален из
базы данных 

С уважением, 
Служба новостей МДМ
http://${env:SERVER_NAME}/
}{Чтобы изменить параметры подписки, достаточно зайти на страницу:
http://${env:SERVER_NAME}/press/subscribe/edit/?email=$form:email&code=$pass

С уважением, 
Служба новостей МДМ
http://${env:SERVER_NAME}/}]
]
#end @sendMail[]



#################################################################################################
# ПАФ сказал добавить :)))
@rusage[]
$rusage[$status:rusage]
$now[^date::now[]]
$s[[^now.sql-string[]] $rusage.utime $rusage.stime $rusage.maxrss $rusage.ixrss $rusage.idrss $rusage.isrss $request:uri
]
^s.save[append;/mdm/rusage.log]
#end

