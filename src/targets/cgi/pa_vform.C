/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vform.C,v 1.1 2001/03/13 17:22:13 paf Exp $
*/

#include "pa_vform.h"
#include "pa_vstring.h"

void VForm::construct_fields() {
	String& ename=*NEW String(pool());
	ename.APPEND_CONST("test");

	String& evalue=*NEW String(pool());
	evalue.APPEND_TAINTED("<value>", 0, "form", 0);

	ffields.put(ename, NEW VString(evalue));
}
