/** @file
	Parser: globals.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_globals.C,v 1.68 2001/08/02 07:00:08 parser Exp $"; 

#include "pa_globals.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_sql_driver_manager.h"
#include "pa_dictionary.h"

String *user_html_name;
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

String *main_class_name;

String *result_var_name;
String *match_var_name;
String *string_pre_match_name;
String *string_match_name;
String *string_post_match_name;

String *defaults_name;
String *ctype_name;
String *ctype_white_space_name;
String *ctype_digit_name;
String *ctype_hex_digit_name;
String *ctype_letter_name;
String *ctype_word_name;
String *ctype_lowercase_name;
String *mime_types_name;
String *vfile_mime_type_name;
String *origins_mode_name;

String *switch_data_name;
String *case_default_value;


Hash *untaint_lang_name2enum;

Dictionary *default_typo_dict;

short hex_value[0x100];

static void setup_hex_value() {
	memset(hex_value, 0, sizeof(hex_value));
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

void pa_globals_init(Pool& pool) {
	#undef NEW
	#define NEW new(pool)

	// hex value
	setup_hex_value();

	// names
	user_html_name=NEW String(pool, USER_HTML_NAME);
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

	main_class_name=NEW String(pool, MAIN_CLASS_NAME);

	result_var_name=NEW String(pool, RESULT_VAR_NAME);
	match_var_name=NEW String(pool, MATCH_VAR_NAME);
	string_pre_match_name=NEW String(pool, STRING_PRE_MATCH_NAME);
	string_match_name=NEW String(pool, STRING_MATCH_NAME);
	string_post_match_name=NEW String(pool, STRING_POST_MATCH_NAME);


	defaults_name=NEW String(pool, DEFAULTS_NAME);
	ctype_name=NEW String(pool, CTYPE_NAME);
	ctype_white_space_name=NEW String(pool, CTYPE_WHITE_SPACE_NAME);
	ctype_digit_name=NEW String(pool, CTYPE_DIGIT_NAME);
	ctype_hex_digit_name=NEW String(pool, CTYPE_HEX_DIGIT_NAME);
	ctype_letter_name=NEW String(pool, CTYPE_LETTER_NAME);
	ctype_word_name=NEW String(pool, CTYPE_WORD_NAME);
	ctype_lowercase_name=NEW String(pool, CTYPE_LOWERCASE_NAME);
	mime_types_name=NEW String(pool, MIME_TYPES_NAME);
	vfile_mime_type_name=NEW String(pool, VFILE_MIME_TYPE_NAME);
	origins_mode_name=NEW String(pool, ORIGINS_MODE_NAME);

	//^switch ^case
	switch_data_name=NEW String(pool, SWITCH_DATA_NAME);
	case_default_value=NEW String(pool, CASE_DEFAULT_VALUE);

	// hashes
	untaint_lang_name2enum=NEW Hash(pool);
	#define ULN(var, cstr, LANG) \
		String var(pool, cstr); \
		untaint_lang_name2enum->put(var, (int)String::UL_##LANG);
	ULN(as_is, "as-is", AS_IS);
	ULN(file_name, "file-name", FILE_NAME);
	ULN(http_header, "http-header", HTTP_HEADER);
	ULN(mail_header, "mail-header", MAIL_HEADER);
	ULN(uri, "uri", URI);
	ULN(table, "table", TABLE);
	ULN(sql, "sql", SQL);
	ULN(js, "js", JS);
	ULN(html, "html", HTML);
	ULN(user_html, "user-html", USER_HTML);

	// tables
	Table *default_typo_table=NEW Table(pool, 0, 0);
	#define DT_ROW(from, to) { \
			Array *row=NEW Array(pool); \
			*row+=NEW String(pool, from); \
			*row+=NEW String(pool, to); \
			*default_typo_table+=row; \
		}
	DT_ROW("<", "&lt;");
	DT_ROW(">", "&gt;");
	DT_ROW("\"", "&quot;");
	DT_ROW("&", "&amp;");
	DT_ROW("\\n\\n", "<p>");
	DT_ROW("\\n", "<br>");
	default_typo_dict=NEW Dictionary(*default_typo_table);

	// SQL_Driver_manager
 	SQL_driver_manager=NEW SQL_Driver_manager(pool);
}
