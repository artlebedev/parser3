/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_globals.h,v 1.4 2001/03/14 17:09:16 paf Exp $
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

#include "pa_string.h"
#include "pa_hash.h"

#define AUTO_FILE_NAME "auto.p"

#define EXCEPTION_METHOD_NAME "exception"

#define UNNAMED_NAME "unnamed"

#define MAIN_METHOD_NAME "main"
#define AUTO_METHOD_NAME "auto"

#define ROOT_CLASS_NAME "ROOT"
#define MAIN_CLASS_NAME "MAIN"
#define TABLE_CLASS_NAME "table"
#define ENV_CLASS_NAME "env"
#define FORM_CLASS_NAME "form"

#define LIMITS_NAME "limits"
#define POST_MAX_SIZE_NAME "post_max_size"

struct Service_funcs {
	size_t (*read_post)(char *&buf, size_t max_bytes);
};


// core func
void globals_init(Pool& pool);

//
extern Service_funcs service_funcs;

// global strings
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

extern String *limits_name;
extern String *post_max_size_name;

// global hashes
extern Hash *untaint_lang_name2enum;

#endif
