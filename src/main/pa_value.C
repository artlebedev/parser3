/*
  $Id: pa_value.C,v 1.6 2001/03/08 16:54:27 paf Exp $
*/

#include "pa_value.h"
#include "pa_exception.h"

void Value::failed(char *action) const {
	THROW(0,0,
		&name(),
		action, type());
}
