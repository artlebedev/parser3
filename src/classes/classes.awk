#	Parser: classes.inc generator
#
#	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.3 2001/09/26 10:32:25 parser Exp $

/\.C/ && !/(op|classes|ident)\.C/ {
	$s=$0
	$c=sprintf("M%s%s", toupper(substr($s,0,1)),substr($s,2,length($s)-3))
	printf "extern Methoded *%s_create(Pool& pool);  *this+=%s_create(pool());\n", $c,$c
}
