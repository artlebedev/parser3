#	Parser: ident.C generator
#
#	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
#
#	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
#
#	$Id: ident.awk,v 1.1 2002/01/31 10:20:14 paf Exp $

BEGIN {
	OFS=""
	print "const char *", project, "_RCSIds[]={"
	print "\t\"$Project: ", project, " ------------------------------------------ $\","
}

/\$Id\: .*? \$/ {
    match($0, /\$Id\: .*? \$/)
	print "\t\"", substr($0, RSTART, RLENGTH), "\","
}

END {
	print "\t0};"
}
