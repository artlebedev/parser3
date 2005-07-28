@USE
common/lib.p
common/layout.p
common/implode.p
date/dtf.p
mysql.p
#rotabanner.p


###########################################################################
@auto[]
$MAIN:pSQL[^mysql::init[$SQL.connect-string;
	$.is_debug(1)
	$.cache_dir[/../data/cache]
]]

^detectBrowser[]

# эта табличка нужна для очистки всякой фигни от ентитей/тегов
$repl[^table::create{a	b
&nbsp^;	 
&hellip^;	...
&rdquo^;	"
&ldquo^;	"
&raquo^;	"
&laquo^;	"
&mdash^;	-
&ndash^;	-
&#8470^;	№
&bull^;	 }]

#end @auto[]



###########################################################################
@error404[]
#$response:status[404]
^location[/404/]
#end @error404[]



###########################################################################
@postprocess[body][repl]
$result[$body]
^if(def $result && $result is "string"){
    ^if($MAIN:browser eq "nn" && $MAIN:browser_ver < 5){
        $repl[^table::create[nameless]{
&hellip^;	&#133^;
&rdquo^;	&#148^;
&ldquo^;	&#147^;
&mdash^;	&#151^;
&ndash^;	&#150^;
&bull^;	&#149^;
}]
        $result[^result.replace[$repl]]
    }

	^rem{ *** это инициализация банеров *** }
#	$banner[^rotabanner::init[название проекта;банер по умолчанию]]
	^rem{ *** обрабатываем [parser]код[/parser] *** }
	$result[^result.match[\[(parser)\](.+?)\[/\1\]][g]{^taint[as-is][^process{$match.2}]}]

	^rem{ *** если отдаем страничку яндексу, то меняем &amp^; на &, а то у него крышу сносит *** }
	^if(^env:HTTP_USER_AGENT.match[yandex][i]){
		$result[^result.match[&amp^;][g]{&}]
	}
}

# получаем статистику по sql запросам
^getSQLStat[]

# вызываем rusage, который напишет в log столько страничка генерилась и сколько отожрала памяти
#^rusage[]

#end @postprocess[]



###########################################################################
@rusage[msg][now;s]
^if(!in "/admin/"){
	$now[^date::now[]]
	$s[[^now.sql-string[]]	$status:rusage.utime	$status:rusage.maxrss	[$status:memory.used/$status:memory.free/$status:memory.ever_allocated_since_compact/$status:memory.ever_allocated_since_start]	$env:REMOTE_ADDR	$request:uri^if(def $msg){ $msg}^#0A]
	^s.save[append;/../data/rusage.log]
}
#end @rusage[]



###########################################################################
# этот метод выведет статистику, собранную объектом $MAIN:pSQL
@getSQLStat[]
^if(def $MAIN:pSQL && !in "/admin/"){
	^if(def $form:mode && ^form:tables.mode.locate[field;debug]){
		^rem{ *** если пусканули с ?mode=debug то получаем и сохраняем информацию обо всех запросах на странице *** }
		^MAIN:pSQL.print_statistic[$.file[/../data/sql.txt]]
	}{
		^rem{ *** а по умолчанию в лог пишем только информацию о проблемных страницах *** }
		^MAIN:pSQL.print_statistic[
			$.file[/../data/sql.log]
			$.debug_time_limit(500)
			$.debug_queries_limit(25)
			$.debug_result_limit(3000)
		]
	}
}
#end @getSQLStat[]




