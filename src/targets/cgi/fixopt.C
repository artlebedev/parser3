/** @file
	Parser: CGI: fixing command line options decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_FIXOPT_C="$Date: 2002/08/14 11:38:07 $";

#include "pa_config_includes.h"

#include "pa_common.h"
#include "pa_exception.h"

// defines

#define MAX_ARGV 100

void fixopt(
	int beforec, char **beforev, 
	int& afterc, char **& aafterv) {

	static char *afterv[MAX_ARGV];
	aafterv=afterv;
	afterc=0;

	if(beforec)
		afterv[afterc++]=beforev[0]; // do not fix argv[0]

	for(int i=1; i<beforec-1; i++) {
		char *many=beforev[i];
		while(char *one=lsplit(&many, ' ')) {
			if(afterc==MAX_ARGV-1 /*so not to check later*/)
				throw Exception(0,
					0,
					"too many command line arguments (more then %d)", MAX_ARGV-1);
			afterv[afterc++]=one;				
		}
	}

	afterv[afterc++]=beforev[beforec-1];
}
