#	Parser: classes.inc generator
#
#	Copyright (c) 2000-2024 Art. Lebedev Studio (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.15 2024/11/04 03:53:25 moko Exp $

/\.C$/ && !/(op|classes)/ {
	s=$0
	c=substr(s,1,length(s)-2)
	printf "extern Methoded* %s_class; if(%s_class) *this+=%s_class; \n", c,c,c,c,c,c
}
