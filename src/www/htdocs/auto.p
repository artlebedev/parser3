@main[]
^header[]
^print_path[]
^print_menu[]
^body[]
^hr[]
^footer[]


@header[]
<!-- Copyright 2001 (c) Vladimir Eltchinov | elik@elik.ru | http://www.elik.ru/ -->

<html><head>
#^macro[load_config]
<title>^print_header[]</title><base target="_top"></head>
<body bgcolor=#$config.bg-color.value link=#$config.link-color.value vlink=#$config.vlink-color.value alink=#$config.alink-color.value text=#$config.text-color.value topmargin=5 leftmargin=5 rightmargin=5 marginwidth=5 marginheight=5 background=$config.bg-image.value>
<table width=100% height=100% align=center valign=top><tr><td width=50%></td><td width=600 valign=top>

@body[]

@footer[]
</td><td width=50%></td></tr><tr><td colspan=3 valign=bottom align=right><font size=-1><b>The matrix has you ...</b></font></td></tr></table>
</body>
</html>


@i[]
$result[$config.images.value]

@hr[]
<br><img src=^i[]hr.gif width=600 height=1><br><br>

@auto[]
$DEFAULTS[
	$content-type[
		$value[text/html]
		$charset[koi8-r]
	]
]

#$config_table[^table:load[/layout.cfg]]
#$config[^config_table.hash[name][value]]

@print_path[][p;path;links;prefix;location] Печатает строку навигации "Корень / раздел / раздел"
$p[$request:uri]
$path[^p.lsplit[/]]
^path.flip[]
^path.menu{
	^if("$prefix" eq ""){
        	^if($p eq "/"){<b>Elik.Ru</b>}{<a href=/><b>Elik.Ru</b></a>}
        }{
		^if(-f "${prefix}links.cfg"){

	       		$links[^table:load[${prefix}links.cfg]]
			$links[^links.hash[location;description]]
        	
			$location[$path.0]
			^if($p eq "${prefix}$location/"){$links.$location.description}{<a href=${prefix}$location>$links.$location.description</a>}
        	}
        }
	$prefix[${prefix}$location/]
}[&nbsp^;/&nbsp^;]<br>^hr[]

@print_header[][p;path;links;prefix;location] Печатает строку заголовка "Корень | раздел | раздел"
$p[$request:uri]
$path[^p.lsplit[/]]
^path.flip[]
^path.menu{
	^if("$prefix" eq ""){
        	Elik.Ru
        }{
		^if(-f "${prefix}links.cfg"){

	       		$links[^table:load[${prefix}links.cfg]]
			$links[^links.hash[location;description]]
        	
			$location[$path.0]
			$links.$location.description
        	}
        }
	$prefix[${prefix}$location/]
}[&nbsp^;|&nbsp^;]

@print_menu[][links]
^if(-f links.cfg){
	$links[^table:load[links.cfg]]
	$links{^menu{
		<a href=$location>$description</a>^if($:menu_separator ne ""){$:menu_separator}{<br>}
	}}
	^hr[]
}

