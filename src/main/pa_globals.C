/** @file
	Parser: globals.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_globals.C,v 1.108 2002/01/23 13:07:53 paf Exp $
*/

#include "pa_config_includes.h"

#ifdef XML
//#include "libxml/parser.h"
//#include "libxslt/xslt.h"
//#include "libxslt/libxslt.h"
#include "libxslt/extensions.h"
#include "libxslt/xsltutils.h"
extern "C" {
#include "libexslt/exslt.h"
};
#endif

#include "pa_globals.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_sql_driver_manager.h"
#include "pa_dictionary.h"
#include "pa_stylesheet_manager.h"
#include "pa_sapi.h"
#include "pa_cache_managers.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_threads.h"

#ifdef DB2
#include "pa_db_manager.h"
#endif

String *content_type_name;
String *charset_name;
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

String *auto_method_name;
String *main_method_name;

String *main_class_name;

String *result_var_name;
String *match_var_name;
String *string_pre_match_name;
String *string_match_name;
String *string_post_match_name;

String *charsets_name;
String *mime_types_name;
String *vfile_mime_type_name;
String *origins_mode_name;

String *class_path_name;

String *switch_data_name;
String *case_default_value;

String *sql_limit_name;
String *sql_offset_name;
String *sql_default_name;

String *charset_UTF8_name;

String *hash_default_element_name;

Hash *untaint_lang_name2enum;

GdomeDOMImplementation *domimpl;

Charset *utf8_charset;

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

#ifdef XML

const int MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS=10;

struct XML_Generic_error_info {
	pa_thread_t thread_id;
	char *message;
} xml_generic_error_infos[MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS];

XML_Generic_error_info *xml_generic_error_info(pa_thread_t thread_id) {
	for(int i=0; i<MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS; i++) {
		XML_Generic_error_info *p=xml_generic_error_infos+i;
		if(p->thread_id==thread_id)
			return p;
	}
	return 0;
}

static void
xmlParserGenericErrorFunc(void *ctx, const char *msg, ...) { 
    pa_thread_t thread_id=pa_get_thread_id();

	// infinitely looking for free slot to fill it
	while(true) {
		SYNCHRONIZED;  // find+fill blocked

		// first try to get existing for this thread_id
		XML_Generic_error_info *p=xml_generic_error_info(thread_id);
		if(!p) { // occupy empty one
			p=xml_generic_error_info(0);
			if(!p) // wait for empty for it to appear
				continue;
		}

		p->thread_id=thread_id;
		size_t offset=p->message?strlen(p->message):0;
		p->message=(char *)realloc(p->message, offset+MAX_STRING);
		if(!p->message)
			SAPI::die(
				"out of memory in 'xmlParserGenericErrorFunc', failed to reallocate to %u bytes", 
				offset+MAX_STRING);
		
		va_list args;
		va_start(args, msg);
		vsnprintf(p->message+offset, MAX_STRING, msg, args);
		va_end(args);

		break;
	}
}

bool xmlHaveGenericErrors() {
    pa_thread_t thread_id=pa_get_thread_id();

	SYNCHRONIZED;  // find blocked

	return xml_generic_error_info(thread_id)!=0;
}

const char *xmlGenericErrors() {
    pa_thread_t thread_id=pa_get_thread_id();

	SYNCHRONIZED;  // find+free blocked

	XML_Generic_error_info *p=xml_generic_error_info(thread_id);
	if(!p) // no errors for our thread_id registered
		return 0;

	const char *result=p->message;

	// free slot up 
	memset(p, 0, sizeof(*p));

	// it is up to caller to free it
	return result;
}
#endif

void pa_globals_destroy(void *) {
	try {
#ifdef XML
		GdomeException exc;
		gdome_di_unref (domimpl, &exc);
#endif
		if(cache_managers)
			cache_managers->~Cache_managers();

		charsets->~Charsets();
		
	} catch(const Exception& e) {
		SAPI::die("pa_globals_destroy failed: %s", e.comment());
	}
}

/// @test hint on one should call this for each thread xmlSubstituteEntitiesDefault(1);
void pa_globals_init(Pool& pool) {
	pool.register_cleanup(pa_globals_destroy, 0);

	#undef NEW
	#define NEW new(pool)

	// hex value
	setup_hex_value();

	// names
	content_type_name=NEW String(pool, CONTENT_TYPE_NAME);
	charset_name=NEW String(pool, CHARSET_NAME);
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

	auto_method_name=NEW String(pool, AUTO_METHOD_NAME);
	main_method_name=NEW String(pool, MAIN_METHOD_NAME);

	main_class_name=NEW String(pool, MAIN_CLASS_NAME);

	result_var_name=NEW String(pool, RESULT_VAR_NAME);
	match_var_name=NEW String(pool, MATCH_VAR_NAME);
	string_pre_match_name=NEW String(pool, STRING_PRE_MATCH_NAME);
	string_match_name=NEW String(pool, STRING_MATCH_NAME);
	string_post_match_name=NEW String(pool, STRING_POST_MATCH_NAME);


	charsets_name=NEW String(pool, CHARSETS_NAME);
	mime_types_name=NEW String(pool, MIME_TYPES_NAME);
	vfile_mime_type_name=NEW String(pool, VFILE_MIME_TYPE_NAME);
	origins_mode_name=NEW String(pool, ORIGINS_MODE_NAME);

	class_path_name=NEW String(pool, CLASS_PATH_NAME);

	//^switch ^case
	switch_data_name=NEW String(pool, SWITCH_DATA_NAME);
	case_default_value=NEW String(pool, CASE_DEFAULT_VALUE);

	// sql
	sql_limit_name=NEW String(pool, SQL_LIMIT_NAME);
	sql_offset_name=NEW String(pool, SQL_OFFSET_NAME);
	sql_default_name=NEW String(pool, SQL_DEFAULT_NAME);

	// charsets
	charset_UTF8_name=NEW String(pool, CHARSET_UTF8_NAME);

	// hash
	hash_default_element_name=NEW String(pool, HASH_DEFAULT_ELEMENT_NAME);


	// hashes
	untaint_lang_name2enum=NEW Hash(pool);
	#define ULN(cstr, LANG) \
		untaint_lang_name2enum->put(*NEW String(pool, cstr), (int)String::UL_##LANG);
	ULN("as-is", AS_IS);
	ULN("file-spec", FILE_SPEC);
	ULN("http-header", HTTP_HEADER);
	ULN("mail-header", MAIL_HEADER);
	ULN("uri", URI);
	ULN("table", TABLE);
	ULN("sql", SQL);
	ULN("js", JS);
	ULN("xml", XML);
	ULN("html", HTML);
	ULN("optimized-html", HTML|String::UL_OPTIMIZE_BIT);

	// charsets
	charsets=NEW Charsets(pool);
	charsets->put(*charset_UTF8_name, 
		utf8_charset=NEW Charset(pool, *charset_UTF8_name, 0/*no file=system*/));


	// Status registration, must be initialized before all registrants
	cache_managers=NEW Cache_managers(pool);

	// SQL driver manager
	cache_managers->put(*NEW String(pool, "sql"), 
		SQL_driver_manager=NEW SQL_Driver_manager(pool));

#ifdef DB2
	// DB driver manager
	cache_managers->put(*NEW String(pool, "db"), 
		DB_manager=NEW DB_Manager(pool));
#endif

#ifdef XML
	// initializing xml libs

	/* First I get a DOMImplementation reference */
	domimpl = gdome_di_mkref ();
    /*
     * Register the EXSLT extensions and the test module
     */
    exsltRegisterAll();
    xsltRegisterTestModule();
    xmlDefaultSAXHandlerInit();
    /*
     * disable CDATA from being built in the document tree
     */
    // never added yet  xmlDefaultSAXHandler.cdataBlock = NULL;

	/*
	 * Initialization function for the XML parser.
	 * This is not reentrant. Call once before processing in case of
	 * use in multithreaded programs.
	*/
	xmlInitParser();

	// 1. this is needed for proper parsing of stylesheets
	// there were a situation where honest entity ruined innocent xpath compilation
	// doc says "you sould turn it on on stylesheet load" without deepening into details
	// 2. when dom tree with entites goes under transform text nodes 
	// got [erroreosly] cut on first entity occurance
	// that is why this is:
	xmlSubstituteEntitiesDefault(1);
	
	// Bit in the loadsubset context field to tell to do ID/REFs lookups 
	xmlLoadExtDtdDefaultValue |= XML_DETECT_IDS;
	// Bit in the loadsubset context field to tell to do complete the elements attributes lists 
	// with the ones defaulted from the DTDs 
    //never added yet xmlLoadExtDtdDefaultValue |= XML_COMPLETE_ATTRS;

//regretfully this not only replaces entities on parse, but also on generate	xmlSubstituteEntitiesDefault(1);
	// never switched this on xmlIndentTreeOutput=1;

	memset(xml_generic_error_infos, 0, sizeof(xml_generic_error_infos));
	xmlSetGenericErrorFunc(0, xmlParserGenericErrorFunc);
	xsltSetGenericErrorFunc(0, xmlParserGenericErrorFunc);
//	FILE *f=fopen("y:\\xslt.log", "wt");
//	xsltSetGenericDebugFunc(f/*stderr*/, 0);

	// XSLT stylesheet manager
	cache_managers->put(*NEW String(pool, "stylesheet"), 
		stylesheet_manager=NEW Stylesheet_manager(pool));
#endif
}

#if defined(XML) && defined(_MSC_VER)
#	define XML_LIBS "/parser3project/win32xml"
#	pragma comment(lib, XML_LIBS "/glib/lib/libglib-1.3-11.lib")
#	pragma comment(lib, XML_LIBS "/libxml2-2.4.12/win32/dsp/libxml2_so/libxml2.lib")
#	pragma comment(lib, XML_LIBS "/libxslt-1.0.9/win32/dsp/libexslt_so/libexslt.lib")
#	pragma comment(lib, XML_LIBS "/libxslt-1.0.9/win32/dsp/libxslt_so/libxslt.lib")
#	ifdef _DEBUG
#		pragma comment(lib, XML_LIBS "/gdome2-0.7.0/win32/dsp/Debug/libgdome.lib")
#	else
#		pragma comment(lib, XML_LIBS "/gdome2-0.7.0/win32/dsp/Release/libgdome.lib")
#	endif
#endif


#if defined(DB2) && defined(_MSC_VER)
#	define LIBDB2_WIN32_BUILD "/parser3project/win32db/lib"
#	ifdef _DEBUG
#		pragma comment(lib, LIBDB2_WIN32_BUILD "/debug/libdb.lib")
#	else
#		pragma comment(lib, LIBDB2_WIN32_BUILD "/release/libdb.lib")
#	endif
#endif
