/** @file
	Parser: global decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_globals.h,v 1.33 2001/04/03 07:20:52 paf Exp $
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

#include "pa_pool.h"

class String;
class Hash;
class Table;

//@{
/// constant string. class name, predefined method & co
#define AUTO_FILE_NAME "auto.p"

#define HTML_TYPO_NAME "html-typo"

#define CONTENT_TYPE_NAME "content-type"
#define BODY_NAME "body"
#define VALUE_NAME "value"
#define EXPIRES_NAME "expires"
#define PATH_NAME "path"
#define NAME_NAME "name"
#define SIZE_NAME "size"
#define TEXT_NAME "text"

#define EXCEPTION_METHOD_NAME "exception"
#define POST_PROCESS_METHOD_NAME "post-process"

#define CONTENT_DISPOSITION_NAME "content-disposition"
#define CONTENT_DISPOSITION_FILENAME_NAME "filename"

#define UNNAMED_NAME "unnamed"

#define MAIN_METHOD_NAME "main"
#define AUTO_METHOD_NAME "auto"

#define ROOT_CLASS_NAME "ROOT"
#define MAIN_CLASS_NAME "MAIN"
#define TABLE_CLASS_NAME "table"
#define FILE_CLASS_NAME "file"
#define ENV_CLASS_NAME "env"
#define FORM_CLASS_NAME "form"
#define REQUEST_CLASS_NAME "request"
#define RESPONSE_CLASS_NAME "response"
#define COOKIE_CLASS_NAME "cookie"
#define RANDOM_CLASS_NAME "random"

#define LIMITS_NAME "LIMITS"
#define POST_MAX_SIZE_NAME "post_max_size"
#define DEFAULTS_NAME "DEFAULTS"
#define MIME_TYPES_NAME "MIME_TYPES"
#define VFILE_MIME_TYPE_NAME "mime-type"

#define RESULT_VAR_NAME "result"
//@}

/// core func
void pa_globals_init(Pool& pool);

/// hex_value[c] = hex value of c
extern short hex_value[0x100];

//@{
/// global string
extern String *html_typo_name;
extern String *content_type_name;
extern String *body_name;
extern String *value_name;
extern String *expires_name;
extern String *path_name;
extern String *name_name;
extern String *size_name;
extern String *text_name;

extern String *exception_method_name;
extern String *post_process_method_name;

extern String *content_disposition_name;
extern String *content_disposition_filename_name;

extern String *unnamed_name;
extern String *empty_string;

extern String *auto_method_name;
extern String *main_method_name;

extern String *root_class_name;
extern String *main_class_name;
extern String *env_class_name;
extern String *table_class_name;
extern String *file_class_name;
extern String *form_class_name;
extern String *request_class_name;
extern String *response_class_name;
extern String *cookie_class_name;
extern String *random_class_name;

extern String *limits_name;
extern String *post_max_size_name;

extern String *defaults_name;
extern String *mime_types_name;
extern String *vfile_mime_type_name;

extern String *result_var_name;
//@}

/// global hash
extern Hash *untaint_lang_name2enum;

/// global table
extern Table *default_typo_table;

#endif
