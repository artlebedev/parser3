/*
  $Id: pa_value.C,v 1.2 2001/02/23 21:59:08 paf Exp $
*/

#include "pa_value.h"
#include "pa_exception.h"

void Value::failed(char *action) {
	const String *string=name();
	THROW(0,0,
		name(),
		action, string?string->cstr():"unknown", type());
}
