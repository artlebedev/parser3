##  ���������� ���������� ����������
@auto[]
$MAIN:globalXSL[/_templates/normal/global.xsl]]				^rem{ ������ �� ��������� }
$MAIN:startpageXSL[/_templates/normal/startpage.xsl]]		^rem{ ������ ������ �������� }
$pSQL[^MYSQL::create[]]


##  �������� XML ��������, ����������� �� ���� XSL ������ � ������ �������
@postprocess[body]
^if( $env:QUERY_STRING ne 'debug' ){

	$xml[^xdoc::set{$body}]
	
	^if( $env:REQUEST_URI eq '/' || $env:REQUEST_URI eq '/index.html' ){
		$html[^xml.transform[$MAIN:startpageXSL].]
	}{
		$html[^xml.transform[$MAIN:globalXSL].string]
	}

	$response:body[
		^html.file[
			$.encoding[windows-1251]
			$.method[html]
		]
	]
}{
	$body
}


## ���������� ������
@USE
hosting.p
mysql.p
utils.p
