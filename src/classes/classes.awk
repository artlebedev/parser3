#	Parser: classes.inc generator
#
#	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.7.2.1 2003/07/29 09:10:18 paf Exp $

/\.C$/ && !/(op|classes)/ {
	s=$0
	c=substr(s,1,length(s)-2)
	printf "extern Methoded* %s_class;  extern Methoded* %s_base_class;  if(%s_class) *this+=%s_class; if(%s_base_class) *this+=%s_base_class;\n", c,c,c,c,c,c
}
