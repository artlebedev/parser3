#	Parser: classes.inc generator
#
#	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
#
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.2 2001/04/28 15:47:02 paf Exp $

/\.C/ && !/(op|classes)\.C/ {
	$s=$0
	$c=sprintf("M%s%s", toupper(substr($s,0,1)),substr($s,2,length($s)-3))
	printf "extern Methoded *%s_create(Pool& pool);  *this+=%s_create(pool());\n", $c,$c
}
