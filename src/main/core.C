/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: core.C,v 1.63 2001/03/12 21:18:00 paf Exp $
*/

#include "core.h"
#include "_string.h"
#include "_double.h"
#include "_int.h"
#include "_table.h"
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
String *table_class_name;

Hash *untaint_lang_name2enum;


void core() {
	Pool pool;
	
	// names
	GLOBAL_STRING(unnamed_name, UNNAMED_NAME);
	empty_string=new(pool) String(pool); 

	GLOBAL_STRING(auto_method_name, AUTO_METHOD_NAME);
	GLOBAL_STRING(main_method_name, MAIN_METHOD_NAME);
	
	GLOBAL_STRING(main_class_name, MAIN_CLASS_NAME);
	GLOBAL_STRING(root_class_name, ROOT_CLASS_NAME);
	GLOBAL_STRING(env_class_name, ENV_CLASS_NAME);	
	GLOBAL_STRING(table_class_name, TABLE_CLASS_NAME);

	// hashes
	untaint_lang_name2enum=new(pool) Hash(pool);
	LOCAL_STRING(as_is, "as-is");  
	untaint_lang_name2enum->put(as_is, (int)String::Untaint_lang::AS_IS);
	LOCAL_STRING(table, "table");
	untaint_lang_name2enum->put(table, (int)String::Untaint_lang::TABLE);
	LOCAL_STRING(sql, "sql");
	untaint_lang_name2enum->put(sql, (int)String::Untaint_lang::SQL);
	LOCAL_STRING(js, "js");
	untaint_lang_name2enum->put(js, (int)String::Untaint_lang::JS);
	LOCAL_STRING(html, "html");
	untaint_lang_name2enum->put(html, (int)String::Untaint_lang::HTML);
	LOCAL_STRING(html_typo, "html-typo");
	untaint_lang_name2enum->put(html_typo, (int)String::Untaint_lang::HTML_TYPO);

	// read-only classes
	initialize_string_class(pool, *(string_class=new(pool) VClass(pool)));  string_class->freeze();
	initialize_double_class(pool, *(double_class=new(pool) VClass(pool)));  double_class->freeze();
	initialize_int_class(pool, *(int_class=new(pool) VClass(pool)));  int_class->freeze();
	initialize_table_class(pool, *(table_class=new(pool) VClass(pool)));  table_class->freeze();

	// request
	Request request(pool);
	request.core();
}

