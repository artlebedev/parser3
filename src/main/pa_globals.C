/** @file
	Parser: globals.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_globals.C,v 1.19 2001/03/19 21:59:17 paf Exp $
*/

#include "pa_globals.h"
#include "_string.h"
#include "_double.h"
#include "_int.h"
#include "_table.h"
#include "_form.h"
#include "_env.h"
#include "_request.h"
#include "_response.h"
#include "_file.h"

Service_funcs service_funcs;

String *content_type_name;
String *body_name;
String *value_name;
String *expires_name;
String *path_name;
String *name_name;
String *size_name;
String *text_name;

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
String *request_class_name;
String *response_class_name;
String *cookie_class_name;

String *result_var_name;

String *limits_name;
String *post_max_size_name;

String *defaults_name;

Hash *untaint_lang_name2enum;

short hex_value[0x100];

static void setup_hex_value() {
	int i;
	for (i=0; (i < 0x100); i++) {
		hex_value[i] = 0;
	}
	hex_value['0'] = 0;	
	hex_value['1'] = 1;	
	hex_value['2'] = 2;	
	hex_value['3'] = 3;	
	hex_value['4'] = 4;	
	hex_value['5'] = 5;	
	hex_value['6'] = 6;	
	hex_value['7'] = 7;	
	hex_value['8'] = 8;	
	hex_value['9'] = 9;
	hex_value['A'] = 10;
	hex_value['B'] = 11;
	hex_value['C'] = 12;
	hex_value['D'] = 13;
	hex_value['E'] = 14;
	hex_value['F'] = 15;
	hex_value['a'] = 10;
	hex_value['b'] = 11;
	hex_value['c'] = 12;
	hex_value['d'] = 13;
	hex_value['e'] = 14;
	hex_value['f'] = 15;
}

void globals_init(Pool& pool) {
	// hex value
	setup_hex_value();

	// names
	content_type_name=new(pool) String(pool, CONTENT_TYPE_NAME);
	body_name=new(pool) String(pool, BODY_NAME);
	value_name=new(pool) String(pool, VALUE_NAME);
	expires_name=new(pool) String(pool, EXPIRES_NAME);
	path_name=new(pool) String(pool, PATH_NAME);
	name_name=new(pool) String(pool, NAME_NAME);
	size_name=new(pool) String(pool, SIZE_NAME);
	text_name=new(pool) String(pool, TEXT_NAME);

	exception_method_name=new(pool) String(pool, EXCEPTION_METHOD_NAME);

	unnamed_name=new(pool) String(pool, UNNAMED_NAME);
	empty_string=new(pool) String(pool); 

	auto_method_name=new(pool) String(pool, AUTO_METHOD_NAME);
	main_method_name=new(pool) String(pool, MAIN_METHOD_NAME);

	root_class_name=new(pool) String(pool, ROOT_CLASS_NAME);
	main_class_name=new(pool) String(pool, MAIN_CLASS_NAME);
	table_class_name=new(pool) String(pool, TABLE_CLASS_NAME);
	env_class_name=new(pool) String(pool, ENV_CLASS_NAME);	
	form_class_name=new(pool) String(pool, FORM_CLASS_NAME);	
	request_class_name=new(pool) String(pool, REQUEST_CLASS_NAME);	
	response_class_name=new(pool) String(pool, RESPONSE_CLASS_NAME);
	cookie_class_name=new(pool) String(pool, COOKIE_CLASS_NAME);

	result_var_name=new(pool) String(pool, RESULT_VAR_NAME);


	limits_name=new(pool) String(pool, LIMITS_NAME);
	post_max_size_name=new(pool) String(pool, POST_MAX_SIZE_NAME);

	defaults_name=new(pool) String(pool, DEFAULTS_NAME);

	// hashes
	untaint_lang_name2enum=new(pool) Hash(pool);
	String as_is(pool, "as-is");  
	untaint_lang_name2enum->put(as_is, (int)String::Untaint_lang::AS_IS);
	String header(pool, "header");  
	untaint_lang_name2enum->put(header, (int)String::Untaint_lang::HEADER);
	String uri(pool, "uri");  
	untaint_lang_name2enum->put(uri, (int)String::Untaint_lang::URI);
	String table(pool, "table");
	untaint_lang_name2enum->put(table, (int)String::Untaint_lang::TABLE);
	String sql(pool, "sql");
	untaint_lang_name2enum->put(sql, (int)String::Untaint_lang::SQL);
	String js(pool, "js");
	untaint_lang_name2enum->put(js, (int)String::Untaint_lang::JS);
	String html(pool, "html");
	untaint_lang_name2enum->put(html, (int)String::Untaint_lang::HTML);
	String html_typo(pool, "html-typo");
	untaint_lang_name2enum->put(html_typo, (int)String::Untaint_lang::HTML_TYPO);

	// read-only stateless classes
	initialize_string_class(pool, *(string_class=new(pool) VStateless_class(pool)));  string_class->freeze();
	initialize_double_class(pool, *(double_class=new(pool) VStateless_class(pool)));  double_class->freeze();
	initialize_int_class(pool, *(int_class=new(pool) VStateless_class(pool)));  int_class->freeze();
	initialize_table_class(pool, *(table_class=new(pool) VStateless_class(pool)));  table_class->freeze();
	initialize_file_class(pool, *(file_class=new(pool) VStateless_class(pool)));  file_class->freeze();

	// read-only stateless base classes
	initialize_env_base_class(pool, *(env_base_class=new(pool) VStateless_class(pool)));  env_base_class->set_name(*env_class_name);  env_base_class->freeze();
	initialize_form_base_class(pool, *(form_base_class=new(pool) VStateless_class(pool)));  form_base_class->set_name(*form_class_name);  form_base_class->freeze();
	initialize_request_base_class(pool, *(request_base_class=new(pool) VStateless_class(pool)));  request_base_class->set_name(*request_class_name);  request_base_class->freeze();
	initialize_response_base_class(pool, *(response_base_class=new(pool) VStateless_class(pool)));  response_base_class->set_name(*response_class_name);  response_base_class->freeze();
}
