/*
  $Id: pa_value.C,v 1.3 2001/02/23 22:22:08 paf Exp $
*/

#include "pa_value.h"
#include "pa_exception.h"

void Value::failed(char *action) {
	const String *string=name();
	THROW(0,0,
		name(),
		action, type(), string?string->cstr():"unknown");
}
