/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vform.C,v 1.2 2001/03/14 17:09:21 paf Exp $
*/

#include "pa_vform.h"
#include "pa_vstring.h"

void VForm::fill_fields(
						Request& request,
						size_t post_max_size
						)
{
	String& ename=*NEW String(pool());
	ename.APPEND_CONST("test");

	String& evalue=*NEW String(pool());
	evalue.APPEND_TAINTED("<value>", 0, "form", 0);

	fields.put(ename, NEW VString(evalue));
}

