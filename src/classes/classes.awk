#	Parser: classes.inc generator
#
#	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.5 2001/11/15 18:04:18 paf Exp $

/\.C/ && !/(op|classes|ident)\.C/ {
	s=$0
	c=sprintf("M%s%s", toupper(substr(s,1,1)),substr(s,2,length(s)-3))
	printf "extern Methoded *%s_create(Pool& pool);  *this+=%s_create(pool());\n", c,c
}
