@USE
common/lib.p
common/layout.p
common/implode.p
date/dtf.p
mysql.p



#################################################################################################
@auto[]
$now[^date::now[]]

# id объекта с 404 ошибкой:
$MAIN:error404ObjectId(23)
# id объекта, где лежат глобальные данные о сайте:
$MAIN:siteDataObjectId(21)

# id корневого объекта раздела вопросы и ответы
$MAIN:faq_category_root_id(3)

$MAIN:pSQL[^mysql::init[$SQL.connect-string;^if(^env:QUERY_STRING.pos[mode=debug] >= 0){debug}{500}]]

^detectBrowser[]

# константа дл€ новостей
$MAIN:objIdNews(1)

# эта табличка нужна дл€ очистки вс€кой фигни от ентитей/тегов
$repl[^table::create{a	b
&nbsp^;	 
&hellip^;	...
&rdquo^;	"
&ldquo^;	"
&raquo^;	"
&laquo^;	"
&mdash^;	-
&ndash^;	-
&#8470^;	є
&bull^;	 }]

$MAIN:user_images_dir[/i/users]
#end @auto[]



#################################################################################################
@error404[][err_lang]
^location[/404/]
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
	^body{^content[]}
	</html>
}
# @main[]



#################################################################################################
@postprocess[body][repl;tmp]
^if(def $body){
	^if($MAIN:browser eq "nn" && $MAIN:browser_ver < 5){
		$repl[^table::create{a	b
&hellip^;	&#133^;
&rdquo^;	&#148^;
&ldquo^;	&#147^;
&mdash^;	&#151^;
&ndash^;	&#150^;
&bull^;	&#149^;}]
		$result[^body.replace[$repl]]
	}{
		$result[$body]
	}
	$not_optimized_space[^taint[as-is][ ]]
	$not_optimized_tab[^taint[as-is][^#09]]
	$not_optimized_enter[^taint[as-is][^#0A]]
	$result[^result.match[прќбЁл][g]{$not_optimized_space}]
	$result[^result.match[тјб][g]{$not_optimized_tab}]
	$result[^result.match[энт≈р][g]{$not_optimized_enter}]
}{
	$result[]
}
^getStat[]
^rusage[]
# а это мы можем вывести список макросов, которые outdated, но которые вызывались...
# ^printOutdatedMacro[]
#end @postprocess[body]



#################################################################################################
@rusage[msg][rusage]
^if(!in "/admin/"){
$rusage[$status:rusage]
$now[^date::now[]]
$s[[^now.sql-string[]]	$rusage.utime	$rusage.maxrss	[$status:memory.used/$status:memory.free/$status:memory.since_compact/$status:memory.process]	$env:REMOTE_ADDR	$request:uri^if(def $msg){ $msg}^#0A]
^s.save[append;/../data/rusage.log]
}
#end @rusage[]



#################################################################################################
# этот метод выведет статистику, собранную экземпл€ром класса $MAIN:pSQL
@getStat[]
^if(def $MAIN:pSQL && $MAIN:pSQL.debug){
	$result[^MAIN:pSQL.getStatistics[/../data/sql.txt]]
}{
	$result[]
}
#end @stat[]



#################################################################################################
# метод достает названи€ методов с комментари€ми у указаного файла и показывает их
@getComments[f][file;fd;t;comments]
$file[^if(def $f){$f}{/../data/classes/engine.p}]
^if(-f $file){
	$fd[^file::load[text;$file]]
	
	$t[$fd.text]
	$t[^t.match[^^##[^^#]*^$][mg]{}]
	$t[^t.match[(\n){3,}][g]{${match.1}$match.1}]
	$comments[^t.match[(#{5,}\n#\s.*?\n@.+?\^])][g]]
	
	$result[^taint[as-is][^comments.menu{$comments.1^#0A^#0A^#0A}]]
}
#end @getComments[]



#################################################################################################
@log404[][str]
$str[^now.sql-string[]	$request:uri	$env:HTTP_REFERER	$env:REMOTE_ADDR^#0A]
^str.save[append;/../data/missing_url.txt]
#end @log404[]

