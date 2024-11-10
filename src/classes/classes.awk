#	Parser: classes.inc generator
#
#	Copyright (c) 2000-2024 Art. Lebedev Studio (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.16 2024/11/10 20:28:15 moko Exp $

/\.C$/ && !/(op|classes)/ {
	s=$0
	c=substr(s,1,length(s)-2)
	printf "extern Methoded* %s_init(); *this+=%s_init(); \n", c,c,c,c,c,c
}
