/*
  $Id: pa_value.C,v 1.4 2001/02/24 11:46:04 paf Exp $
*/

#include "pa_value.h"
#include "pa_exception.h"

void Value::failed(char *action) {
	THROW(0,0,
		name(),
		action, type());
}
