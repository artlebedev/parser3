/** @file
	Parser: global decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_globals.h,v 1.70 2001/12/14 13:02:32 paf Exp $
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

#include "pa_config_includes.h"
#include "pa_pool.h"

class String;
class Hash;
class Dictionary;

//@{
/// constant string. class name, predefined method & co
#define CONFIG_FILE_NAME "parser3.conf"
#define AUTO_FILE_NAME "auto.p"

#define CONTENT_TYPE_NAME "content-type"
#define CHARSET_NAME "charset"
#define BODY_NAME "body"
#define VALUE_NAME "value"
#define EXPIRES_NAME "expires"
#define PATH_NAME "path"
#define NAME_NAME "name"
#define NONAME_DAT "noname.dat"
#define SIZE_NAME "size"
#define TEXT_NAME "text"

#define EXCEPTION_METHOD_NAME "exception"
#define POST_PROCESS_METHOD_NAME "postprocess"

#define UNNAMED_NAME "unnamed"

#define MAIN_METHOD_NAME "main"
#define AUTO_METHOD_NAME "auto"

#define MAIN_CLASS_NAME "MAIN"

#define DEFAULTS_NAME "DEFAULTS"
#define CHARSETS_NAME "CHARSETS"
#define MIME_TYPES_NAME "MIME-TYPES"
#define VFILE_MIME_TYPE_NAME "mime-type"
#define ORIGINS_MODE_NAME "ORIGINS"

#define RESULT_VAR_NAME "result"
#define MATCH_VAR_NAME "match"

#define STRING_PRE_MATCH_NAME "prematch"
#define STRING_MATCH_NAME "match"
#define STRING_POST_MATCH_NAME "postmatch"

#define CONTENT_DISPOSITION_NAME "content-disposition"
#define CONTENT_DISPOSITION_FILENAME_NAME "filename"

#define SWITCH_DATA_NAME "SWITCH-DATA"
#define CASE_DEFAULT_VALUE "DEFAULT"

#define CLASS_PATH_NAME "CLASS_PATH"

#define SQL_LIMIT_NAME "limit"
#define SQL_OFFSET_NAME "offset"
#define SQL_DEFAULT_NAME "default"

#define HASH_DEFAULT_ELEMENT_NAME "_default"

//@}

/// core func
void pa_globals_init(Pool& pool);

/// hex_value[c] = hex value of c
extern short hex_value[0x100];

//@{
/// global string
extern String *optimized_html_name;
extern String *content_type_name;
extern String *charset_name;
extern String *body_name;
extern String *value_name;
extern String *expires_name;
extern String *path_name;
extern String *name_name;
extern String *size_name;
extern String *text_name;

extern String *exception_method_name;
extern String *post_process_method_name;

extern String *unnamed_name;

extern String *auto_method_name;
extern String *main_method_name;

extern String *main_class_name;


extern String *charsets_name;

extern String *mime_types_name;
extern String *vfile_mime_type_name;
extern String *origins_mode_name;

extern String *result_var_name;
extern String *match_var_name;
extern String *string_pre_match_name;
extern String *string_match_name;
extern String *string_post_match_name;

extern String *content_disposition_name;
extern String *content_disposition_filename_name;

extern String *limits_name;

extern String *class_path_name;

extern String *switch_data_name;
extern String *case_default_value;

extern String *sql_limit_name;
extern String *sql_offset_name;
extern String *sql_default_name;

extern String *hash_default_element_name;

//@}

/// global hash
extern Hash *untaint_lang_name2enum;

/// global table
extern Dictionary *default_typo_dict;

#endif
