/** @file
	Parser: globals.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_globals.C,v 1.45 2001/04/06 10:32:20 paf Exp $
*/

#include "pa_globals.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_table.h"
#include "_string.h"
#include "_double.h"
#include "_int.h"
#include "_table.h"
#include "_form.h"
#include "_response.h"
#include "_file.h"
#include "_random.h"
#include "pa_sql_driver_manager.h"

String *html_typo_name;
String *content_type_name;
String *body_name;
String *value_name;
String *expires_name;
String *path_name;
String *name_name;
String *size_name;
String *text_name;

String *exception_method_name;
String *post_process_method_name;

String *content_disposition_name;
String *content_disposition_filename_name;

String *unnamed_name;
String *empty_string;

String *auto_method_name;
String *main_method_name;

String *op_class_name;
String *main_class_name;
String *env_class_name;
String *table_class_name;
String *file_class_name;
String *form_class_name;
String *request_class_name;
String *response_class_name;
String *cookie_class_name;
String *random_class_name;

String *result_var_name;
String *string_pre_match_name;
String *string_match_name;
String *string_post_match_name;

String *limits_name;
String *post_max_size_name;

String *defaults_name;
String *locale_name;
String *locale_ctype_name;
String *mime_types_name;
String *vfile_mime_type_name;

String *main_sql_name;
String *main_sql_drivers_name;

Hash *untaint_lang_name2enum;

Table *default_typo_table;

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

/// @test problems with $t[^table:set{}] ^t.menu{^if(){}}  if not a field!
void pa_globals_init(Pool& pool) {
	#undef NEW
	#define NEW new(pool)

	// hex value
	setup_hex_value();

	// names
	html_typo_name=NEW String(pool, HTML_TYPO_NAME);
	content_type_name=NEW String(pool, CONTENT_TYPE_NAME);
	body_name=NEW String(pool, BODY_NAME);
	value_name=NEW String(pool, VALUE_NAME);
	expires_name=NEW String(pool, EXPIRES_NAME);
	path_name=NEW String(pool, PATH_NAME);
	name_name=NEW String(pool, NAME_NAME);
	size_name=NEW String(pool, SIZE_NAME);
	text_name=NEW String(pool, TEXT_NAME);

	exception_method_name=NEW String(pool, EXCEPTION_METHOD_NAME);
	post_process_method_name=NEW String(pool, POST_PROCESS_METHOD_NAME);

	content_disposition_name=NEW String(pool, CONTENT_DISPOSITION_NAME);
	content_disposition_filename_name=NEW String(pool, CONTENT_DISPOSITION_FILENAME_NAME);

	unnamed_name=NEW String(pool, UNNAMED_NAME);
	empty_string=NEW String(pool); 

	auto_method_name=NEW String(pool, AUTO_METHOD_NAME);
	main_method_name=NEW String(pool, MAIN_METHOD_NAME);

	op_class_name=NEW String(pool, OP_CLASS_NAME);
	main_class_name=NEW String(pool, MAIN_CLASS_NAME);
	table_class_name=NEW String(pool, TABLE_CLASS_NAME);
	file_class_name=NEW String(pool, FILE_CLASS_NAME);
	env_class_name=NEW String(pool, ENV_CLASS_NAME);	
	form_class_name=NEW String(pool, FORM_CLASS_NAME);	
	request_class_name=NEW String(pool, REQUEST_CLASS_NAME);	
	response_class_name=NEW String(pool, RESPONSE_CLASS_NAME);
	cookie_class_name=NEW String(pool, COOKIE_CLASS_NAME);
	random_class_name=NEW String(pool, RANDOM_CLASS_NAME);

	result_var_name=NEW String(pool, RESULT_VAR_NAME);
	string_pre_match_name=NEW String(pool, STRING_PRE_MATCH_NAME);
	string_match_name=NEW String(pool, STRING_MATCH_NAME);
	string_post_match_name=NEW String(pool, STRING_POST_MATCH_NAME);


	limits_name=NEW String(pool, LIMITS_NAME);
	post_max_size_name=NEW String(pool, POST_MAX_SIZE_NAME);

	defaults_name=NEW String(pool, DEFAULTS_NAME);
	locale_name=NEW String(pool, LOCALE_NAME);
	locale_ctype_name=NEW String(pool, LOCALE_CTYPE_NAME);
	mime_types_name=NEW String(pool, MIME_TYPES_NAME);
	vfile_mime_type_name=NEW String(pool, VFILE_MIME_TYPE_NAME);

	main_sql_name=NEW String(pool, MAIN_SQL_NAME);
	main_sql_drivers_name=NEW String(pool, MAIN_SQL_DRIVERS_NAME);

	// hashes
	untaint_lang_name2enum=NEW Hash(pool);
	String as_is(pool, "as-is");  
	untaint_lang_name2enum->put(as_is, (int)String::UL_AS_IS);
	String file_name(pool, "file-name");  
	untaint_lang_name2enum->put(file_name, (int)String::UL_FILE_NAME);
	String header(pool, "header");  
	untaint_lang_name2enum->put(header, (int)String::UL_HEADER);
	String uri(pool, "uri");  
	untaint_lang_name2enum->put(uri, (int)String::UL_URI);
	String table(pool, "table");
	untaint_lang_name2enum->put(table, (int)String::UL_TABLE);
	String sql(pool, "sql");
	untaint_lang_name2enum->put(sql, (int)String::UL_SQL);
	String js(pool, "js");
	untaint_lang_name2enum->put(js, (int)String::UL_JS);
	String html(pool, "html");
	untaint_lang_name2enum->put(html, (int)String::UL_HTML);
	String html_typo(pool, "html-typo");
	untaint_lang_name2enum->put(html_typo, (int)String::UL_HTML_TYPO);

	// tables
	default_typo_table=NEW Table(pool, 0, 0);
	{
		// < -> &lt;
		{
			Array *row=NEW Array(pool);
			*row+=NEW String(pool, "<");	
			*row+=NEW String(pool, "&lt;");
			*default_typo_table+=row;
		}
		// > -> &gt;
		{
			Array *row=NEW Array(pool);
			*row+=NEW String(pool, ">");	
			*row+=NEW String(pool, "&gt;");
			*default_typo_table+=row;
		}
		// " -> &quot;
		{
			Array *row=NEW Array(pool);
			*row+=NEW String(pool, "\"");	
			*row+=NEW String(pool, "&quot;");
			*default_typo_table+=row;
		}
		// & -> &amp;
		{
			Array *row=NEW Array(pool);
			*row+=NEW String(pool, "&");	
			*row+=NEW String(pool, "&amp;");
			*default_typo_table+=row;
		}
		// \n\n -> <p>
		{
			Array *row=NEW Array(pool);
			*row+=NEW String(pool, "\\n\\n");	
			*row+=NEW String(pool, "<p>");
			*default_typo_table+=row;
		}
		// \n -> <br>
		{
			Array *row=NEW Array(pool);
			*row+=NEW String(pool, "\\n");	
			*row+=NEW String(pool, "<br>");
			*default_typo_table+=row;
		}
	}

	// stateless classes
	initialize_string_class(pool, *(string_class=NEW VStateless_class(pool)));
	initialize_double_class(pool, *(double_class=NEW VStateless_class(pool)));
	initialize_int_class(pool, *(int_class=NEW VStateless_class(pool)));
	initialize_table_class(pool, *(table_class=NEW VStateless_class(pool)));
	initialize_file_class(pool, *(file_class=NEW VStateless_class(pool)));
	initialize_response_class(pool, *(response_class=NEW VStateless_class(pool)));
	initialize_random_class(pool, *(random_class=NEW VStateless_class(pool)));

	// stateless base classes
	initialize_form_base_class(pool, *(form_base_class=NEW VStateless_class(pool)));  form_base_class->set_name(*form_class_name);

	// SQL_Driver_manager
 	SQL_driver_manager=NEW SQL_Driver_manager(pool);
}
