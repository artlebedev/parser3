/** @file
	Parser: global decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_globals.h,v 1.89 2002/06/20 13:39:57 paf Exp $
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

#include "pa_pool.h"

class String;
class Hash;
class Dictionary;
class Charset;
class Table;

//@{
/// constant string. class name, predefined method & co
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

#define CONF_METHOD_NAME "conf"
#define AUTO_METHOD_NAME "auto"

#define MAIN_CLASS_NAME "MAIN"

#define DEFAULTS_NAME "DEFAULTS"
#define CHARSETS_NAME "CHARSETS"
#define MIME_TYPES_NAME "MIME-TYPES"
#define VFILE_MIME_TYPE_NAME "mime-type"
#define ORIGINS_MODE_NAME "ORIGINS"

#define RESULT_VAR_NAME "result"
#define MATCH_VAR_NAME "match"

#define EXCEPTION_VAR_NAME "exception"
#define EXCEPTION_TYPE_PART_NAME "type"
#define EXCEPTION_SOURCE_PART_NAME "source"
#define EXCEPTION_COMMENT_PART_NAME "comment"
#define EXCEPTION_HANDLED_PART_NAME "handled"


#define STRING_PRE_MATCH_NAME "prematch"
#define STRING_MATCH_NAME "match"
#define STRING_POST_MATCH_NAME "postmatch"

#define CONTENT_DISPOSITION_NAME "content-disposition"
#define CONTENT_DISPOSITION_FILENAME_NAME "filename"

#define SWITCH_DATA_NAME "SWITCH-DATA"

#define CACHE_DATA_NAME "CACHE-DATA"

#define CLASS_PATH_NAME "CLASS_PATH"

#define SQL_LIMIT_NAME "limit"
#define SQL_OFFSET_NAME "offset"
#define SQL_DEFAULT_NAME "default"

#define CHARSET_UTF8_NAME "UTF-8"

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

extern String *conf_method_name;
extern String *auto_method_name;

extern String *main_class_name;


extern String *charsets_name;

extern String *mime_types_name;
extern String *vfile_mime_type_name;
extern String *origins_mode_name;

extern String *result_var_name;
extern String *match_var_name;

extern String *exception_var_name;
extern String *exception_type_part_name;
extern String *exception_source_part_name;
extern String *exception_comment_part_name;
extern String *exception_handled_part_name;

extern String *content_disposition_name;
extern String *content_disposition_filename_name;

extern String *limits_name;

extern String *class_path_name;

extern String *switch_data_name;

extern String *cache_data_name;

extern String *sql_limit_name;
extern String *sql_offset_name;
extern String *sql_default_name;

extern String *charset_UTF8_name;

extern String *hash_default_element_name;

//@}

/// String::match uses this as replace & global search table columns
const int MAX_STRING_MATCH_TABLE_COLUMNS=100;
extern Table *string_match_table_template;

/// name to enum language table
extern Hash *untaint_lang_name2enum;


/// UTF8 charset
extern Charset *utf8_charset;

#ifdef XML
/// DOMImplementation
extern GdomeDOMImplementation *domimpl;

/// flag whether there were some xml generic errors on current thread
bool xmlHaveGenericErrors();
/// get xml generic error accumulated for current thread. WARNING: it is up to caller to free up
const char *xmlGenericErrors();
#endif

#endif
