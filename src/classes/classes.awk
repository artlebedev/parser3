#	Parser: classes.inc generator
#
#	Copyright (c) 2000-2024 Art. Lebedev Studio (http://www.artlebedev.com)
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: classes.awk,v 1.17 2024/11/11 05:28:00 moko Exp $

/\.C$/ && !/(op|classes)/ {
	s=$0
	c=substr(s,1,length(s)-2)
	printf "	ADD_CLASS_VAR(%s)\n", c,c,c,c,c,c
}
