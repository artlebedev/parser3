/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: core.C,v 1.67 2001/03/13 19:35:06 paf Exp $
*/

#include "core.h"
#include "_string.h"
#include "_double.h"
#include "_int.h"
#include "_table.h"
#include "_form.h"
#include "_env.h"

String *exception_method_name;

String *unnamed_name;
String *empty_string;

String *auto_method_name;
String *main_method_name;

String *root_class_name;
String *main_class_name;
String *env_class_name;
String *table_class_name;
String *form_class_name;

Hash *untaint_lang_name2enum;


void core(Pool& pool) {
	#define NEW_STRING(name, value)  name=new(pool) String(pool); name->APPEND_CONST(value)
	#define LOCAL_STRING(name, value)  String name(pool); name.APPEND_CONST(value)

	// names
	NEW_STRING(exception_method_name, EXCEPTION_METHOD_NAME);

	NEW_STRING(unnamed_name, UNNAMED_NAME);
	empty_string=new(pool) String(pool); 

	NEW_STRING(auto_method_name, AUTO_METHOD_NAME);
	NEW_STRING(main_method_name, MAIN_METHOD_NAME);

	NEW_STRING(root_class_name, ROOT_CLASS_NAME);
	NEW_STRING(main_class_name, MAIN_CLASS_NAME);
	NEW_STRING(table_class_name, TABLE_CLASS_NAME);
	NEW_STRING(env_class_name, ENV_CLASS_NAME);	
	NEW_STRING(form_class_name, FORM_CLASS_NAME);	

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

	// read-only stateless classes
	initialize_string_class(pool, *(string_class=new(pool) VStateless_class(pool)));  string_class->freeze();
	initialize_double_class(pool, *(double_class=new(pool) VStateless_class(pool)));  double_class->freeze();
	initialize_int_class(pool, *(int_class=new(pool) VStateless_class(pool)));  int_class->freeze();
	initialize_table_class(pool, *(table_class=new(pool) VStateless_class(pool)));  table_class->freeze();

	// read-only stateless base classes
	initialize_env_base_class(pool, *(env_base_class=new(pool) VStateless_class(pool)));  env_base_class->freeze();
	initialize_form_base_class(pool, *(form_base_class=new(pool) VStateless_class(pool)));  form_base_class->freeze();
}
