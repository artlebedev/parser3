/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: vform_fields_fill.C,v 1.1 2001/03/14 09:12:06 paf Exp $
*/

#include "vform_fields_fill.h"
#include "pa_vstring.h"

void vform_fields_fill(Pool& pool, bool cgi, Hash& fields) {
	String& ename=*new(pool) String(pool);
	ename.APPEND_CONST("test");

	String& evalue=*new(pool) String(pool);
	evalue.APPEND_TAINTED("<value>", 0, "form", 0);

	fields.put(ename, new(pool) VString(evalue));
}
