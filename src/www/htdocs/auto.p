@USE
common/lib.p
common/layout.p
common/implode.p
date/dtf.p
product.p
mysql.p


@auto[]
^if(in "/admin/"){
	^use[admin.p]
}
$MAIN:pSQL[^mysql::init[$SQL.connect-string]]
^detectBrowser[]
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
@spacerLine[height;color]
<tr>^spacer_layout[;$height;$color;7]</tr>
#end @spacerLine[]



#################################################################################################
@pageHeader[]
<tr valign="bottom">
	<td width="33%" colspan="2">^href[^if($request:uri ne '/'){/}{}][<img src="/i/logo.gif" width="155" height="51" alt="AB.LV Aizkraukles Bank Latvia" border="0" />]</td>
	<td width="20"><img src="/i/shadow-head.gif" width="20" height="70" alt="" border="0" /></td>
	<td width="66%" align="right" colspan="3"><font size="-1">^href[^if($lang ne _en){./?lang=en};In English;class="langNav"] &nbsp^;|&nbsp^; ^href[^if($lang ne _lv){./?lang=lv};Latviski;class="langNav"] &nbsp^;|&nbsp^; ^href[^if(def $lang){./};По-русски;class="langNav"]</font></td>
	^spacer_layout[20]
</tr>
<tr>
	^fullspacer_layout[20;17]
	<td width="33%"><table cellpadding="0" cellspacing="0" border="0" width="166"><tr><td><spacer type="block" width="166" height="1" /></td></tr></table></td>
	^spacer_layout[20]
	<td width="33%"><table cellpadding="0" cellspacing="0" border="0" width="166"><tr><td><spacer type="block" width="166" height="1" /></td></tr></table></td>
	^fullspacer_layout[20]
	<td width="33%"><table cellpadding="0" cellspacing="0" border="0" width="166"><tr><td><spacer type="block" width="166" height="1" /></td></tr></table></td>
	^fullspacer_layout[20]
</tr>
#^spacerLine[1;#D9D9D9]
#end @pageHeader[]



#################################################################################################
@pageFooter[]
^spacerLine[20]
^spacerLine[1;#666666]
^spacerLine[5]
<tr valign="top">
	^spacer_layout[20]
	^if(!def $request){
		<td>
			<br /><a href="http://www.design.ru/"><img src="/i/artlebedev.gif" width="88" height="38" alt="" border="0" /></a><br />
			^byLang[
				EN: <div style="margin-top:5px"><font size="-1">Сделано в&nbsp^;<a href="http://www.design.ru/">Студии Артемия&nbsp^;Лебедева</a><br />
				<a href="http://www.design.ru/">Информация о&nbsp^;сайте</a></font></div>
			;
				LV: <div style="margin-top:5px"><font size="-1">Сделано в&nbsp^;<a href="http://www.design.ru/">Студии Артемия&nbsp^;Лебедева</a><br />
				<a href="http://www.design.ru/">Информация о&nbsp^;сайте</a></font></div>
			;
				<div style="margin-top:5px"><font size="-1">Сделано в&nbsp^;<a href="http://www.design.ru/">Студии Артемия&nbsp^;Лебедева</a><br />
				<a href="http://www.design.ru/">Информация о&nbsp^;сайте</a></font></div>
			]<br /><br />
		</td>
	}{
		<td width="20"><br /></td>
	}
	<td background="/i/shadow-bg.gif"><img src="/i/shadow-top.gif" width="20" height="100" alt="" border="0" /></td>
	<td colspan="4"><font size="-1"><br />
		^byLang[
			EN: <font color="#1C4472"><b>Центральный офис:</b></font><br />
			Ул. Элизабетес, 23, Рига, <nobr>LV-1010</nobr><br />
			телефон: <nobr>(371) 777-52-22,</nobr> факс: <nobr>(371) 777-52-00</nobr><br /><br />
			<font color="#666666"><b>Клиентская линия:</b></font> телефон <nobr>(371) 777-55-55</nobr></font>
		;
			LV: <font color="#1C4472"><b>Центральный офис:</b></font><br />
			Ул. Элизабетес, 23, Рига, <nobr>LV-1010</nobr><br />
			телефон: <nobr>(371) 777-52-22,</nobr> факс: <nobr>(371) 777-52-00</nobr><br /><br />
			<font color="#666666"><b>Клиентская линия:</b></font> телефон <nobr>(371) 777-55-55</nobr></font>
		;
			<font color="#1C4472"><b>Центральный офис:</b></font><br />
			Ул. Элизабетес, 23, Рига, <nobr>LV-1010</nobr><br />
			телефон: <nobr>(371) 777-52-22,</nobr> факс: <nobr>(371) 777-52-00</nobr><br /><br />
			<font color="#666666"><b>Клиентская линия:</b></font> телефон <nobr>(371) 777-55-55</nobr></font>
		]
	<br /><br /><br /></td>
</tr>
^spacerLine[1;#D9D9D9]
#end @pageFooter[]



#################################################################################################
@topMenu[items;current;root][f;prevActive]
$prevActive(0)
<tr bgcolor="#d9d9d9">
	<td align="right" width="20" nowrap="nowrap"><img height="20" alt="" src="/i/m-slash.gif" width="^if($items.path eq $root){1}{2}" border="0" /></td>
	<td colspan="6" width="100%">
	^if($items){
		<table cellpadding="0" cellspacing="0" border="0">
			<tr align="center">
				^items.menu{
					^if(^items.line[] != 1){
						<td><img src="/i/m-slash.gif" height="20" width="^if($items.path eq $root || $prevActive){1}{2}" border="0" /></td>
					}
					^if($items.path eq $root){
						$f[${MAIN:objectFilesDir}${items.path}/menu-on${lang}.gif]
						<td nowrap="nowrap">^href[^if($current ne $root){/^trim[${items.path};/]/};^if(-f $f){^image[$f;border="0"]}{ $items.path }]</td>
						$prevActive(1)
					}{
						$f[${MAIN:objectFilesDir}${items.path}/menu-off${lang}.gif]
						<td><a href="/^trim[${items.path};/]/">^if(-f $f){^image[$f;border="0"]}{$items.path}</a></td>
						$prevActive(0)
					}
				}
				<td><img src="/i/m-slash.gif" height="20" width="^if($prevActive){1}{2}" border="0" /></td>
			</tr>
		</table>
	}{<br />}
	</td>
</tr>
^MAIN:spacerLine[20]
#end @topMenu[]



#################################################################################################
@homeMenu[items;current][f;w]
^MAIN:spacerLine[19]
<tr bgcolor="#d9d9d9">
	<td align="right"><img height="20" alt="" src="/i/m-slash.gif" width="2" border="0" /></td>
	<td colspan="5" width="99%">
	^if($items){
		<table cellpadding="0" cellspacing="0" border="0" width="100%">
			<tr align="center">
				^items.menu{
					^if(^items.line[] != 1){
						<td><img src="/i/m-slash.gif" height="20" width="2" /></td>
					}
					$f[${MAIN:objectFilesDir}${items.path}/menu-off${lang}.gif]
					<td width="14%"><a href="/^trim[${items.path};/]/">^if(-f $f){^image[$f;border="0"]}{$items.path}</a></td>
				}
			</tr>
		</table>
	}{<br />}
	</td>
	<td nowrap="nowrap" width="20"><img height="20" alt="" src="/i/m-slash.gif" width="2" border="0" /></td>
</tr>
^MAIN:spacerLine[20]
#end @topMenu[]



#################################################################################################
@body[content]
<body bgcolor="#ffffff" text="#000000" link="#0000C4" alink="#0000C4" vlink="#073683" marginheight="0" marginwidth="0" topmargin="0" leftmargin="0" rightmargin="0">
	$content
</body>
#end @body[]



#################################################################################################
@leftMenu[items;current][subMenuL1;subMenuL2]
^if($items){
	^items.menu{
		<div class="nav">
			^if($items.id != $object.currentObject.id){
				^href[/$object.objectUp.1.path/$items.path/;$items.name]
			}{
				<b>$items.name</b><img height="7" src="/i/bullet.gif" width="8" border="0" alt="" />
			}
			^if($items.id == $object.objectUp.2.id){
				$subMenuL1[^object.getObjectsByParent[$items.id;1]]
				^if($subMenuL1){
					<ul class="menu">
					^subMenuL1.menu{
						<li>^if($subMenuL1.id != $object.currentObject.id){
								^href[/$object.objectUp.1.path/$items.path/$subMenuL1.path/;$subMenuL1.name]
							}{
								$subMenuL1.name<img height="7" src="/i/bullet.gif" width="8" border="0" alt="" />
							}
						^if($subMenuL1.id == $object.objectUp.3.id){
							$subMenuL2[^object.getObjectsByParent[$subMenuL1.id]]
							^if($subMenuL2){
								<ul class="menu">
								^subMenuL2.menu{
									<li>^if($subMenuL2.id != $object.currentObject.id){
											^href[/$object.objectUp.1.path/$items.path/$subMenuL1.path/$subMenuL2.path;$subMenuL2.name]
										}{
											$subMenuL2.name<img height="7" src="/i/bullet.gif" width="8" border="0" alt="" />
										}
								}
								</ul>
							}
						}
						</li>
					}
					</ul>
				}
			}
		</div>
	}
}
#end @leftMenu[]



#################################################################################################
@byLang[en;lv;ru]
^switch[$lang]{
	^case[_lv]{$result[$lv]}
	^case[_en]{$result[$en]}
	^case[DEFAULT]{$result[$ru]}
}
#end @byLang[]


#################################################################################################
@siteMapBlock[]
^MAIN:spacerLine[40]
<tr valign="top">
	<td><br /></td>
	<td width="33%">
		<ul class="menu">
			<li><a href="#">^byLang[пластиковые карточки (en);пластиковые карточки (lv);пластиковые карточки]</a></li>
			<li><a href="#">^byLang[курсы валют (en);курсы валют (lv);курсы валют]</a></li>
		</ul>
	</td>
	<td><br /></td>
	<td width="33%"><br /></td>
	<td><br /></td>
	<td width="33%" colspan="2">
		<table cellspacing="0" cellpadding="0" width="100%" bgcolor="#d9d9d9" border="0">
			<form action="/search/" method="get">
			<tr><td colspan="4"><img height="4" src="/i/graypl-ctopleft.gif" width="4" /></td></tr>
			<tr>
				^spacer_layout[1;30]
				<td><img height="18" ^byLang[title="Site map / search" alt="Site map / search";alt="Поиск (lv)" title="Поиск по сайту (lv)";title="Карта сайта / поиск" alt="Карта сайта / поиск"] src="/i/search${lang}.gif" width="53" vspace="4"></td>
				<td width="100%"><input style="width:100%" size="13" /></td>
				^fullspacer_layout[20;1]
			</tr>
			<tr><td colspan="4"><img height="4" src="/i/graypl-cbtmleft.gif" width="4" /></td></tr>
			<tr bgcolor="#ffffff">
				<td rowspan="2"><img height="20" src="/i/sitemap-btmleft.gif" width="4" /></td>
				<td colspan="3"><a href="/search/"><img height="19" ^byLang[title="Site map / search" alt="Site map / search";title="Карта сайта (lv)" alt="Карта сайта (lv)";title="Карта сайта / поиск" alt="Карта сайта / поиск"] src="/i/sitemap${lang}.gif" width="77" border="0" /></a></td>
			</tr>
			<tr>^spacer_layout[;1;;3]</tr>
			</form>
		</table>
	</td>
</tr>
#end @siteMapBlock[]



#################################################################################################
@drawMapTree[parent_id][i;exist;hashData]
^level.inc(1)
^if($tree.$parent_id){
	$hashData[$tree.$parent_id]
	^hashData.sort($hashData.sort_order)[asc]
	^if($parent_id){<ul class="l^eval($level-1)">}
	^hashData.menu{
		^if($allObjects.[$hashData.id].parent_id == $parent_id && ^is_flag[$allObjects.[$hashData.id].is_show_on_sitemap]){
			^if(def $allObjects.[$hashData.id].[name${lang}]){
				^elementNum.inc(1)
				^if($parent_id){<li>}{<hr class="^if($elementNum == 1){top}{body}" /><li class="l0"><b>}
				<a href="/^object.getPath[$hashData.id]/">$allObjects.[$hashData.id].[name${lang}]</a>
				^if(!$parent_id){</b>}
				^if($allObjects.[$hashData.id].object_type_id == 1){
					^drawMapTree[$hashData.id]
				}
				^if($parent_id){</li>}
			}
		}
	}
	^if($parent_id){</ul>}
}
^level.dec(1)
#end @drawMapTree[]



#################################################################################################
@siteMap[]
$objects[^table::sql{
	SELECT 
		object_id AS id,
		parent_id,
		name,
		name_en,
		name_lv,
		is_show_on_sitemap,
		path,
		object_type_id,
		is_published,
		sort_order
	FROM
		object
	WHERE
		is_published = '1'
	ORDER BY
		object.sort_order
}]

$allObjects[^objects.hash[id]]

$tree[^hash::create[]]
^objects.menu{
	^if(!$tree.[$objects.parent_id]){$tree.[$objects.parent_id][^table::set{id	sort_order}]}
	^tree.[$objects.parent_id].append{$objects.id	$objects.sort_order}
}
$level(0)
$elementNum(0)
^drawMapTree[0]
#end @siteMap[]



#################################################################################################
@html[header;content]
<!-- Copyright (c) Art. Lebedev Studio | http://www.design.ru/ -->
<html>
<head>
	<base target="_top">
	<link rel="stylesheet" type="text/css" href="/main.css"></link>
	<title>AB.LV^if(def $header){ | ^taint[as-is][$header] }</title>
	<script language="JavaScript"><!--
		document.write('<style type="text/css">')
		if (!document.all) ulMenuMarginLeft = '-1.5em'^; else ulMenuMarginLeft = '18px'
		document.write('ul.menu {margin:0px^; margin-left:'+ulMenuMarginLeft+'^;}')
		document.write('</style>')
		//-->
	</script>
</head>
^body{
	<table cellpadding="0" cellspacing="0" border="0" width="100%">
	^pageHeader[]
	$content
	^if(!$noSiteMap){^siteMapBlock[]}
	^pageFooter[]
	</table>
}
<html>
#end @html[]



#################################################################################################
@error404[]
$response:location[/_error__404.html]
#end



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

