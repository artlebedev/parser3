/** @file
	Parser: global decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_globals.h,v 1.17 2001/03/21 14:06:43 paf Exp $
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

#include "pa_string.h"
#include "pa_hash.h"

//@{
/// constant string. class name, predefined method & co
#define AUTO_FILE_NAME "auto.p"

#define CONTENT_TYPE_NAME "content-type"
#define BODY_NAME "body"
#define VALUE_NAME "value"
#define EXPIRES_NAME "expires"
#define PATH_NAME "path"
#define NAME_NAME "name"
#define SIZE_NAME "size"
#define TEXT_NAME "text"

#define EXCEPTION_METHOD_NAME "exception"

#define UNNAMED_NAME "unnamed"

#define MAIN_METHOD_NAME "main"
#define AUTO_METHOD_NAME "auto"

#define ROOT_CLASS_NAME "ROOT"
#define MAIN_CLASS_NAME "MAIN"
#define TABLE_CLASS_NAME "table"
#define ENV_CLASS_NAME "env"
#define FORM_CLASS_NAME "form"
#define REQUEST_CLASS_NAME "request"
#define RESPONSE_CLASS_NAME "response"
#define COOKIE_CLASS_NAME "cookie"

#define LIMITS_NAME "limits"
#define POST_MAX_SIZE_NAME "post_max_size"
#define DEFAULTS_NAME "defaults"

#define RESULT_VAR_NAME "result"
//@}

/// target-specific service functions
struct Service_funcs {
	/// read POST request bytes
	int (*read_post)(Pool& pool, char *buf, int max_bytes);
	/// add response header attribute
	void (*add_header_attribute)(Pool& pool, const char *key, const char *value);
	/// add response header attribute
	void (*send_header)(Pool& pool);
	/// output body bytes
	void (*send_body)(Pool& pool, const char *buf, size_t size);
};


/// core func
void globals_init(Pool& pool);

/// hex_value[c] = hex value of c
extern short hex_value[0x100];

/// target-specific service functions
extern Service_funcs service_funcs;

//@{
/// global string
extern String *content_type_name;
extern String *body_name;
extern String *value_name;
extern String *expires_name;
extern String *path_name;
extern String *name_name;
extern String *size_name;
extern String *text_name;

extern String *exception_method_name;

extern String *unnamed_name;
extern String *empty_string;

extern String *auto_method_name;
extern String *main_method_name;

extern String *root_class_name;
extern String *main_class_name;
extern String *env_class_name;
extern String *table_class_name;
extern String *form_class_name;
extern String *request_class_name;
extern String *response_class_name;
extern String *cookie_class_name;

extern String *limits_name;
extern String *post_max_size_name;

extern String *defaults_name;

extern String *result_var_name;
//@}

/// global hash
extern Hash *untaint_lang_name2enum;

#endif
