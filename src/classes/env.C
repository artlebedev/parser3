/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: env.C,v 1.4 2001/03/11 08:16:31 paf Exp $
*/

#include "pa_request.h"
#include "_env.h"

void initialize_env_class(Pool& pool, VClass& vclass) {
	String& name=*new(pool) String(pool);
	String& string=*new(pool) String(pool);
	char *file="environment";
	int line=3;
	name.APPEND("test", 0, file, line);
	string.APPEND_TAINTED("<value>", 0, file, line);
	vclass.set_field(name, new(pool) VString(string));
}
