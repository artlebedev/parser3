@USE
hosting.p
document.p
dtf.p
secure.p
chart.p


@404[]
$response:location[/404/]


@main[]
#���������� ������� ���� ��������
$server_name[$env:SERVER_NAME]
$CUR_LANG(^if(^server_name.pos[ua]>0){1}{2})

#����������������� �����
^hosting:server{
	$DOCUMENT[^document::init[]]
	^if(def $htmlDocument){^htmlDocument[]}
	^DOCUMENT.paint[]
}
