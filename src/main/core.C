/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: core.C,v 1.58 2001/03/11 12:04:44 paf Exp $
*/

#include "core.h"
#include "_string.h"
#include "_double.h"
#include "_int.h"
#include "pa_request.h"

#define GLOBAL_STRING(name, value)  name=new(pool) String(pool); name->APPEND_CONST(value)
#define LOCAL_STRING(name, value)  String name(pool); name.APPEND_CONST(value)

String *unnamed_name;
String *empty_string;

String *auto_method_name;
String *main_method_name;

String *main_class_name;
String *root_class_name;
String *env_class_name;

Hash *untaint_lang_name_to_value;


void core() {
	Pool pool;
	
	// names
	GLOBAL_STRING(unnamed_name, "unnamed");
	empty_string=new(pool) String(pool); 

	GLOBAL_STRING(auto_method_name, AUTO_METHOD_NAME);
	GLOBAL_STRING(main_method_name, MAIN_METHOD_NAME);
	
	GLOBAL_STRING(main_class_name, MAIN_CLASS_NAME);
	GLOBAL_STRING(root_class_name, ROOT_CLASS_NAME);
	GLOBAL_STRING(env_class_name, ENV_CLASS_NAME);	

	// hashes
	untaint_lang_name_to_value=new(pool) Hash(pool);
	LOCAL_STRING(as_is, "as-is");  
	untaint_lang_name_to_value->put(as_is, 
		static_cast<int>(String::Untaint_lang::AS_IS));
	LOCAL_STRING(sql, "sql");
	untaint_lang_name_to_value->put(sql, 
		static_cast<int>(String::Untaint_lang::SQL));
	LOCAL_STRING(js, "js");
	untaint_lang_name_to_value->put(js, 
		static_cast<int>(String::Untaint_lang::JS));
	LOCAL_STRING(html, "html");
	untaint_lang_name_to_value->put(html, 
		static_cast<int>(String::Untaint_lang::HTML));
	LOCAL_STRING(html_typo, "html-typo");
	untaint_lang_name_to_value->put(html_typo, 
		static_cast<int>(String::Untaint_lang::HTML_TYPO));

	// classes
	initialize_string_class(pool, *(string_class=new(pool) VClass(pool)));
	initialize_double_class(pool, *(double_class=new(pool) VClass(pool)));
	initialize_int_class(pool, *(int_class=new(pool) VClass(pool)));

	// request
	Request request(pool);
	request.core();
}

