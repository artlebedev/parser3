/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: core.C,v 1.57 2001/03/11 08:16:34 paf Exp $
*/

#include "core.h"
#include "_string.h"
#include "_double.h"
#include "_int.h"
#include "pa_request.h"

String *unnamed_name;
String *empty_string;

String *auto_method_name;
String *main_method_name;

String *main_class_name;
String *root_class_name;
String *env_class_name;

void core() {
	Pool pool;
	
	// names
	unnamed_name=new(pool) String(pool); unnamed_name->APPEND_CONST("unnamed");
	empty_string=new(pool) String(pool); 

	auto_method_name=new(pool) String(pool);  auto_method_name->APPEND_CONST(AUTO_METHOD_NAME);
	main_method_name=new(pool) String(pool);  main_method_name->APPEND_CONST(MAIN_METHOD_NAME);
	
	main_class_name=new(pool) String(pool);  main_class_name->APPEND_CONST(MAIN_CLASS_NAME);
	root_class_name=new(pool) String(pool);  root_class_name->APPEND_CONST(ROOT_CLASS_NAME);
	env_class_name=new(pool) String(pool);  env_class_name->APPEND_CONST(ENV_CLASS_NAME);	

	// classes
	initialize_string_class(pool, *(string_class=new(pool) VClass(pool)));
	initialize_double_class(pool, *(double_class=new(pool) VClass(pool)));
	initialize_int_class(pool, *(int_class=new(pool) VClass(pool)));

	// request
	Request request(pool);
	request.core();
}

