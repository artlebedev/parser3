#	Parser: classes.inc generator
#
#	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.7 2003/07/24 11:31:19 paf Exp $

/\.C$/ && !/(op|classes|ident)/ {
	s=$0
	c=substr(s,1,length(s)-2)
	printf "extern Methoded* %s_class;  extern Methoded* %s_base_class;  if(%s_class) *this+=%s_class; if(%s_base_class) *this+=%s_base_class;\n", c,c,c,c,c,c
}
