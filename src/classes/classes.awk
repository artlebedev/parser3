#	Parser: classes.inc generator
#
#	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.4 2001/10/27 10:14:28 paf Exp $

/\.C/ && !/(op|classes|ident)\.C/ {
	s=$0
	c=sprintf("M%s%s", toupper(substr(s,0,1)),substr(s,2,length(s)-3))
	printf "extern Methoded *%s_create(Pool& pool);  *this+=%s_create(pool());\n", c,c
}
