#	Parser: classes.inc generator
#
#	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.13 2020/12/15 17:10:27 moko Exp $

/\.C$/ && !/(op|classes)/ {
	s=$0
	c=substr(s,1,length(s)-2)
	printf "extern Methoded* %s_class; if(%s_class) *this+=%s_class; \n", c,c,c,c,c,c
}
