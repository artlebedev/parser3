##  Конфигурим глобальные переменные
@auto[]
$MAIN:globalXSL[/_templates/normal/global.xsl]]				^rem{ шаблон по умолчанию }
$MAIN:startpageXSL[/_templates/normal/startpage.xsl]]		^rem{ шаблон первой страницы }
$pSQL[^MYSQL::create[]]


##  Забираем XML документ, накладываем на него XSL шаблон и отдаем клиенту
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


## Подключаем классы
@USE
hosting.p
mysql.p
utils.p
