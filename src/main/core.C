/*
$Id: core.C,v 1.50 2001/03/10 11:44:42 paf Exp $
*/

#include "core.h"
#include "classes/_string.h"
#include "classes/_double.h"
#include "classes/_int.h"
#include "pa_request.h"

String *unnamed_name;
String *empty_string;

void core() {
	Pool pool;
	unnamed_name=new(pool) String(pool); unnamed_name->APPEND_CONST("unnamed");
	empty_string=new(pool) String(pool); 

	initialize_string_class(pool, *(string_class=new(pool) VClass(pool)));
	initialize_double_class(pool, *(double_class=new(pool) VClass(pool)));
	initialize_int_class(pool, *(int_class=new(pool) VClass(pool)));
	Request request(pool);
	request.core();
}

