@padding[content]
<table cellspacing=0 cellpadding=8 border=0 width=100%><td>$content</td></table>

@href[url;content;flag]
^if($flag){<a href="$url">$content</a>}{$content}

@main[]
$branches[^table:load[/_site.cfg]]
<html><head>
<title>parser III</title></head>
<body bgcolor=8896A7>
<table cellspacing=2 cellpadding=2 border=0 width="90%" align=center>
	<tr>
		<td colspan=^branches.count[]><font size=2 color=white><b>parser III</b></font></td>
	</tr>
	<tr bgcolor=black>
		$branches{
			^menu{
				<td width=^eval(100/^count[])%>^:href{/$folder/;<font size=2 color=white><b>$name</b></font>}(!(in "/$folder/"))</td>
			}
		}
	</tr>
	<tr bgcolor=white>
		<td colspan=2 bgcolor=CCCCCC>^padding{^leftCol[]}</td>
		<td colspan=^eval(^branches.count[]-2)>^padding{^body[]}</td>
	</tr>
	<tr>
		<td colspan=^branches.count[]><font size=2 color=white>Copyright &copy^; 2001 <a href=http://www.design.ru/ style="color:white">Студия Артемия Лебедева</a></font></td>
	</tr>
</table>
</body></html>
