/*
  $Id: pa_value.C,v 1.1 2001/02/23 17:12:58 paf Exp $
*/

#include "pa_value.h"
#include "pa_exception.h"

void Value::failed(char *action) {
	THROW(0,0,
		name(),
		action, type());
}
