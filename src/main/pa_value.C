/*
  $Id: pa_value.C,v 1.5 2001/02/25 13:23:02 paf Exp $
*/

#include "pa_value.h"
#include "pa_exception.h"

void Value::failed(char *action) {
	THROW(0,0,
		&name(),
		action, type());
}
