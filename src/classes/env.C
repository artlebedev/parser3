/*
$Id: env.C,v 1.1 2001/03/10 11:18:13 paf Exp $
*/

#include "pa_request.h"
#include "_env.h"

void initialize_env_class(Pool& pool, VClass& vclass) {
	String& name=*new(pool) String(pool);
	String& string=*new(pool) String(pool);
	name.APPEND_CONST("test");
	string.APPEND_TAINTED("<value>", 0, "environment", 0);
	vclass.set_field(name, new(pool) VString(string));
}
