/*
$Id: core.C,v 1.51 2001/03/10 15:44:31 paf Exp $
*/

#include "core.h"
#include "classes/_string.h"
#include "classes/_double.h"
#include "classes/_int.h"
#include "pa_request.h"

String *unnamed_name;
String *empty_string;
String *auto_method_name;
String *main_method_name;

void core() {
	Pool pool;
	unnamed_name=new(pool) String(pool); unnamed_name->APPEND_CONST("unnamed");
	empty_string=new(pool) String(pool); 
	auto_method_name=new(pool) String(pool);  auto_method_name->APPEND_CONST(AUTO_METHOD_NAME);
	main_method_name=new(pool) String(pool);  main_method_name->APPEND_CONST(MAIN_METHOD_NAME);

	initialize_string_class(pool, *(string_class=new(pool) VClass(pool)));
	initialize_double_class(pool, *(double_class=new(pool) VClass(pool)));
	initialize_int_class(pool, *(int_class=new(pool) VClass(pool)));
	Request request(pool);
	request.core();
}

