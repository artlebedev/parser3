@CLASS
input_date

@BASE
date

@parse[string][parts]
$parts[^string.match[^^(\d{1,2})\.(\d{1,2})\.(\d{2,4})^$]]
$parts{
	^if(!^empty[]){$:result[^date:set($3;$2;$1)]}{$:result[^date:set[bad date]]}
}
