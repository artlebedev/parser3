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

# ��� �������� ����� ��� ������� ������ ����� �� �������/�����
$repl[^table::create{a	b
&nbsp^;	 
&hellip^;	...
&rdquo^;	"
&ldquo^;	"
&raquo^;	"
&laquo^;	"
&mdash^;	-
&ndash^;	-
&#8470^;	�
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

	^rem{ *** ��� ������������� ������� *** }
#	$banner[^rotabanner::init[�������� �������;����� �� ���������]]
	^rem{ *** ������������ [parser]���[/parser] *** }
	$result[^result.match[\[(parser)\](.+?)\[/\1\]][g]{^taint[as-is][^process{$match.2}]}]

	^rem{ *** ���� ������ ��������� �������, �� ������ &amp^; �� &, � �� � ���� ����� ������ *** }
	^if(^env:HTTP_USER_AGENT.match[yandex][i]){
		$result[^result.match[&amp^;][g]{&}]
	}
}

# �������� ���������� �� sql ��������
^getSQLStat[]

# �������� rusage, ������� ������� � log ������� ��������� ���������� � ������� �������� ������
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
# ���� ����� ������� ����������, ��������� �������� $MAIN:pSQL
@getSQLStat[]
^if(def $MAIN:pSQL && !in "/admin/"){
	^if(def $form:mode && ^form:tables.mode.locate[field;debug]){
		^rem{ *** ���� ��������� � ?mode=debug �� �������� � ��������� ���������� ��� ���� �������� �� �������� *** }
		^MAIN:pSQL.print_statistic[$.file[/../data/sql.txt]]
	}{
		^rem{ *** � �� ��������� � ��� ����� ������ ���������� � ���������� ��������� *** }
		^MAIN:pSQL.print_statistic[
			$.file[/../data/sql.log]
			$.debug_time_limit(500)
			$.debug_queries_limit(25)
			$.debug_result_limit(3000)
		]
	}
}
#end @getSQLStat[]




