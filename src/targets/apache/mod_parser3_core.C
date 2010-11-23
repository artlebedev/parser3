/** @file
Parser: apache 1.3 module, part, compiled by parser3project.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_MOD_PARSER3_MAIN_C="$Date: 2010/11/23 00:00:33 $";

#include "pa_config_includes.h"

#include "pa_globals.h"

#include "pa_httpd.h"

#include "pa_common.h"
#include "pa_sapi.h"
#include "classes.h"
#include "pa_request.h"
#include "pa_socks.h"

#if _MSC_VER && !defined(_DEBUG)
#	include <windows.h>
#	define PA_SUPPRESS_SYSTEM_EXCEPTION
#endif

// generals

void pa_setup_module_cells() {
	static bool globals_inited=false;
	if(globals_inited)
		return;
	globals_inited=true;
	
	/// no trying to __try here [yet]
	try {
		// init socks
		pa_socks_init();
		
		// init global variables
		pa_globals_init();
	} catch(const Exception& e) { // global problem 
		SAPI::abort("setup_module_cells failed: %s", e.comment());
	}
}

void pa_destroy_module_cells() {
	pa_globals_done();

	pa_socks_done();
}


//@{
/// SAPI func decl

class SAPI_Info {
public:
	pa_request_rec* r;
};

void SAPI::log(SAPI_Info& SAPI_info, const char* fmt, ...) {
	va_list args;
	va_start(args,fmt);
	char buf[MAX_LOG_STRING];
	size_t size=vsnprintf(buf, MAX_LOG_STRING, fmt, args);
	size=remove_crlf(buf, buf+size);
	pa_ap_log_rerror(0, 0, PA_APLOG_ERR | PA_APLOG_NOERRNO, SAPI_info.r, "%s", buf);
	va_end(args);
}

static void die_or_abort(const char* fmt, va_list args, bool write_core) {
	char buf[MAX_LOG_STRING];
	size_t size=vsnprintf(buf, MAX_LOG_STRING, fmt, args);
	size=remove_crlf(buf, buf+size);
	pa_ap_log_error(PA_APLOG_MARK, PA_APLOG_EMERG, 0, "%s", buf);
	
	// exit & try to produce core dump
	if(write_core)
		abort();
	else
		exit(1);
}

void SAPI::die(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	die_or_abort(fmt, args, false/*write core?*/);
	va_end(args);
}

void SAPI::abort(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	die_or_abort(fmt, args, true/*write core?*/);
	va_end(args);
}

char* SAPI::get_env(SAPI_Info& SAPI_info, const char* name) {
	const char* dont_return_me=pa_ap_table_get(SAPI_info.r->subprocess_env, name);
	return dont_return_me?pa_strdup(dont_return_me):0;
}

#ifndef DOXYGEN
struct SAPI_environment_append_info {
	const char** cur;
};
#endif
static const char* mk_env_pair(const char* key, const char* value) {
	char *result=new(PointerFreeGC) char[strlen(key)+1/*=*/+strlen(value)+1/*0*/];
	strcpy(result, key); strcat(result, "="); strcat(result, value);
	return result;
}
static int SAPI_environment_append(void *d, const char* k, const char* val) {
	if( k && val ) {
		SAPI_environment_append_info& info=
			*static_cast<SAPI_environment_append_info *>(d);
		*info.cur++=mk_env_pair(k, val);
	}
	return 1/*true*/;
}
const char* const* SAPI::environment(SAPI_Info& SAPI_info) {
	const pa_table *t=SAPI_info.r->subprocess_env;
	const char** result=new(UseGC) const char*[pa_ap_table_size(t)+1/*0*/];
	SAPI_environment_append_info info={result};
	pa_ap_table_do(SAPI_environment_append, &info, t, 0); *info.cur=0; // mark EOE
	return result;
}

size_t SAPI::read_post(SAPI_Info& SAPI_info, char *buf, size_t max_bytes) {
/*    pa_ap_log_error(PA_APLOG_MARK, PA_APLOG_DEBUG, SAPI_info.r->server, 
"mod_parser3: SAPI::read_post(max=%u)", max_bytes);
	*/
	int retval;
	if((retval = pa_ap_setup_client_block(SAPI_info.r, PA_REQUEST_CHUNKED_ERROR)))
		return 0;
	if(!pa_ap_should_client_block(SAPI_info.r))
		return 0;
	
	uint total_read_bytes=0;
	void (*handler)(int)=pa_signal(PA_SIGPIPE, PA_SIG_IGN);
	while (total_read_bytes<max_bytes) {
		pa_ap_hard_timeout("Read POST information", SAPI_info.r); /* start timeout timer */
		uint read_bytes=
			pa_ap_get_client_block(SAPI_info.r, buf+total_read_bytes, max_bytes-total_read_bytes);
		pa_ap_reset_timeout(SAPI_info.r);
		if (read_bytes<=0)
			break;
		total_read_bytes+=read_bytes;
	}
	pa_signal(PA_SIGPIPE, handler);
	return total_read_bytes;
}

/// @test location provide with protocol. think about internal redirects
void SAPI::add_header_attribute(SAPI_Info& SAPI_info,
				const char* dont_store_key, const char* dont_store_value) {
	if(strcasecmp(dont_store_key, "location")==0) 
		*SAPI_info.r->status=302;
	
	if(strcasecmp(dont_store_key, HTTP_CONTENT_TYPE)==0) {
	/* r->content_type, *not* r->headers_out("Content-type").  If you don't
	* set it, it will be filled in with the server's default type (typically
	* "text/plain").  You *must* also ensure that r->content_type is lower
	* case.
		*/
		*SAPI_info.r->content_type = pa_ap_pstrdup(SAPI_info.r->pool, dont_store_value);
	} else if(strcasecmp(dont_store_key, HTTP_STATUS)==0) 
		*SAPI_info.r->status=atoi(dont_store_value);
	else
		pa_ap_table_addn(SAPI_info.r->headers_out, 
		pa_ap_pstrdup(SAPI_info.r->pool, capitalize(dont_store_key)), 
		pa_ap_pstrdup(SAPI_info.r->pool, dont_store_value));
}

void SAPI::send_header(SAPI_Info& SAPI_info) {
	pa_ap_hard_timeout("Send header", SAPI_info.r);
	pa_ap_send_http_header(SAPI_info.r);
	pa_ap_kill_timeout(SAPI_info.r);
}

size_t SAPI::send_body(SAPI_Info& SAPI_info, const void *buf, size_t size) {
	pa_ap_hard_timeout("Send body", SAPI_info.r);
	size = (size_t)pa_ap_rwrite(buf, size, SAPI_info.r);
	pa_ap_kill_timeout(SAPI_info.r);
	return size;
}

//@}

#if !defined(PA_DEBUG_DISABLE_GC) && !defined(WIN32)
extern long GC_large_alloc_warn_suppressed; 
#endif

/**
main workhorse

	@todo intelligent cache-control
*/
static void real_parser_handler(SAPI_Info& SAPI_info, Parser_module_config *dcfg) {
	// collect garbage from prev request
#if !defined(PA_DEBUG_DISABLE_GC) && !defined(WIN32)
	{
		int saved=GC_dont_gc;
		GC_dont_gc=0;
		GC_gcollect();
		GC_dont_gc=saved;
		GC_large_alloc_warn_suppressed=0;
	}
#endif

	// populate env
	pa_ap_add_common_vars(SAPI_info.r);
	pa_ap_add_cgi_vars(SAPI_info.r);
	
	// Request info
	Request_info request_info;  memset(&request_info, 0, sizeof(request_info));
	
	request_info.document_root=SAPI::get_env(SAPI_info, "DOCUMENT_ROOT");
	request_info.path_translated=SAPI_info.r->filename;
	request_info.method=SAPI_info.r->method;
	request_info.query_string=SAPI_info.r->args;
	request_info.uri=SAPI::get_env(SAPI_info, "REQUEST_URI");
	request_info.content_type=SAPI::get_env(SAPI_info, "CONTENT_TYPE");
	const char* content_length=SAPI::get_env(SAPI_info, "CONTENT_LENGTH");
	request_info.content_length=content_length?atoi(content_length):0;
	request_info.cookie=SAPI::get_env(SAPI_info, "HTTP_COOKIE");
	request_info.mail_received=false;
	
	// prepare to process request
	Request request(
		SAPI_info,
		request_info,
		String::Language(String::L_HTML|String::L_OPTIMIZE_BIT)
		);
	
	// process the request
	request.core(
		dcfg->parser_config_filespec, true, // /path/to/config
		SAPI_info.r->header_only!=0);
}

#ifdef PA_SUPPRESS_SYSTEM_EXCEPTION
static const Exception 
call_real_parser_handler__do_PEH_return_it(
	SAPI_Info& SAPI_info, Parser_module_config *dcfg) 
{
	try {
		real_parser_handler(SAPI_info, dcfg);
	} catch(const Exception& e) {
		return e;
	}

	return Exception();
}
static void call_real_parser_handler__supress_system_exception(
	SAPI_Info& SAPI_info, Parser_module_config *dcfg) 
{
	Exception parser_exception;
	LPEXCEPTION_POINTERS system_exception=0;

	__try {
		parser_exception=call_real_parser_handler__do_PEH_return_it(
			SAPI_info, dcfg);
	} __except (
		(system_exception=GetExceptionInformation()), 
		EXCEPTION_EXECUTE_HANDLER) 
	{

		if(system_exception)
			if(_EXCEPTION_RECORD *er=system_exception->ExceptionRecord)
				throw Exception("system",
					0,
					"0x%08X at 0x%08X", er->ExceptionCode,  er->ExceptionAddress);
			else
				throw Exception("system", 
					0, 
					"<no exception record>");
		else
			throw Exception("system", 
				0, 
				"<no exception information>");
	}

	if(parser_exception)
		throw Exception(parser_exception);
}
#endif

/// @test r->finfo.st_mode check seems to work only on win32
int pa_parser_handler(pa_request_rec *r, Parser_module_config *dcfg) {
	// SAPI info
	SAPI_Info SAPI_info; SAPI_info.r=r;
	
	if(r->file_not_found ) 
		return PA_HTTP_NOT_FOUND;
	
	try { // global try
#ifdef PA_SUPPRESS_SYSTEM_EXCEPTION
		call_real_parser_handler__supress_system_exception(
#else
		real_parser_handler(
#endif
			SAPI_info, dcfg);

		// successful finish
	} catch(const Exception& e) { // global problem 
		// don't allocate anything on pool here:
		//   possible pool' exception not catch-ed now
		//   and there could be out-of-memory exception
		char buf[MAX_STRING];
		snprintf(buf, MAX_STRING, "Unhandled exception %s",
			e.comment());
		// log it
		SAPI::log(SAPI_info, "%s", buf);
		
		//
		int content_length=strlen(buf);
		
		// prepare header
		// capitalized headers are used for preventing malloc during capitalization
		SAPI::add_header_attribute(SAPI_info, HTTP_CONTENT_TYPE_CAPITALIZED, "text/plain");
		// don't use 'format' function because it calls malloc
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%u", content_length);
		SAPI::add_header_attribute(SAPI_info, HTTP_CONTENT_LENGTH_CAPITALIZED, content_length_cstr);
		
		// send header
		SAPI::send_header(SAPI_info);
		
		// send body
		if(!r->header_only)
			SAPI::send_body(SAPI_info, buf, content_length);
		
		// unsuccessful finish
	}
	
	/*
	* We did what we wanted to do, so tell the rest of the server we
	* succeeded.
	*/
	return PA_OK;
}
