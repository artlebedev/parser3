/** @file
	Parser: IIS extension.

	Copyright (c) 2000,2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: parser3isapi.C,v 1.62 2002/02/08 07:27:51 paf Exp $
*/

#ifndef _MSC_VER
#	error compile ISAPI module with MSVC [no urge for now to make it autoconf-ed (PAF)]
#endif

#include "pa_config_includes.h"

#include <windows.h>
#include <process.h>
#include <new.h>

#include <httpext.h>

#include "pa_sapi.h"
#include "pa_globals.h"
#include "pa_request.h"
#include "pa_version.h"
#include "pool_storage.h"
#include "pa_socks.h"

#ifdef XML
#include <XalanTransformer/XalanCAPI.h>
#endif

#define MAX_STATUS_LENGTH sizeof("xxxx LONGEST STATUS DESCRIPTION")

// consts

extern const char *main_RCSIds[];
#ifdef USE_SMTP
extern const char *smtp_RCSIds[];
#endif
extern const char *gd_RCSIds[];
extern const char *classes_RCSIds[];
extern const char *types_RCSIds[];
extern const char *parser3isapi_RCSIds[];
#ifdef XML
extern const char *xalan_patched_RCSIds[];
#endif
const char **RCSIds[]={
	main_RCSIds,
#ifdef USE_SMTP
	smtp_RCSIds,
#endif
	gd_RCSIds,
	classes_RCSIds,
	types_RCSIds,
	parser3isapi_RCSIds,
#ifdef XML
	xalan_patched_RCSIds,
#endif
	0
};

// SAPI

#ifndef DOXYGEN
/*
	ISAPI SAPI functions receive this context information. 
	see Pool::set_context
*/
struct SAPI_func_context {
	LPEXTENSION_CONTROL_BLOCK lpECB;
	String *header;
	DWORD http_response_code;
};
#endif

#ifdef XML
/**
 * Terminate Xalan and Xerces.
 *
 * Should be called only once per process after deleting all
 * instances of XalanTransformer.  Once a process has called
 * this function, it cannot use the API for the remaining
 * lifetime of the process.

	
	this requirement is fullfilled by using Pool::register_cleanup
 */
void callXalanTerminate(void *) {
	//_asm int 3;
	XalanTerminate();
}
#endif

// goes to 'cs-uri-query' log file field. webmaster: switch it ON[default OFF].
void SAPI::log(Pool& pool, const char *fmt, ...) {
	SAPI_func_context& ctx=*static_cast<SAPI_func_context *>(pool.get_context());
	
	va_list args;
	va_start(args,fmt);
	char buf[MAX_STRING];
	const char *prefix="PARSER_ERROR:";
	strcpy(buf, prefix);
	char *start=buf+strlen(prefix);
	DWORD size=vsnprintf(start, MAX_STRING-strlen(prefix), fmt, args);
	remove_crlf(start, start+size);

	ctx.lpECB->ServerSupportFunction(ctx.lpECB->ConnID, 
		HSE_APPEND_LOG_PARAMETER, buf, &size, 0);
}

/// @todo event log
void SAPI::die(const char *fmt, ...) {
	exit(1);
}

const char *SAPI::get_env(Pool& pool, const char *name) {
	SAPI_func_context& ctx=*static_cast<SAPI_func_context *>(pool.get_context());

	char *variable_buf=(char *)pool.malloc(MAX_STRING);
	DWORD variable_len = MAX_STRING-1;

	if(ctx.lpECB->GetServerVariable(ctx.lpECB->ConnID, const_cast<char *>(name), 
		variable_buf, &variable_len)) {
		variable_buf[variable_len]=0;
		return variable_buf;
	} else if (GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
		variable_buf=(char *)pool.malloc(variable_len+1);
		
		if(ctx.lpECB->GetServerVariable(ctx.lpECB->ConnID, const_cast<char *>(name), 
			variable_buf, &variable_len)) {
			variable_buf[variable_len]=0;
			return variable_buf;
		}
	}
		
	return 0;
}

size_t SAPI::read_post(Pool& pool, char *buf, size_t max_bytes) {
	SAPI_func_context& ctx=*static_cast<SAPI_func_context *>(pool.get_context());

	DWORD read_from_buf=0;
	DWORD read_from_input=0;
	DWORD total_read=0;

	read_from_buf=min(ctx.lpECB->cbAvailable, max_bytes);
	memcpy(buf, ctx.lpECB->lpbData, read_from_buf);
	total_read+=read_from_buf;

	if(read_from_buf<max_bytes &&
		read_from_buf<ctx.lpECB->cbTotalBytes) {
		DWORD cbRead=0, cbSize;

		read_from_input=min(max_bytes-read_from_buf, 
			ctx.lpECB->cbTotalBytes-read_from_buf);
		while(cbRead < read_from_input) {
			cbSize=read_from_input - cbRead;
			if(!ctx.lpECB->ReadClient(ctx.lpECB->ConnID, 
				buf+read_from_buf+cbRead, &cbSize) || 
				cbSize==0) 
				break;
			cbRead+=cbSize;
		}
		total_read+=cbRead;
	}
	return total_read;
}

void SAPI::add_header_attribute(Pool& pool, const char *key, const char *value) {
	SAPI_func_context& ctx=*static_cast<SAPI_func_context *>(pool.get_context());

	if(strcasecmp(key, "location")==0) 
		ctx.http_response_code=302;

	if(strcasecmp(key, "status")==0) 
		ctx.http_response_code=atoi(value);
	else {
		ctx.header->APPEND_CONST(key);
		ctx.header->APPEND_CONST(": ");
		ctx.header->APPEND_CONST(value);
		ctx.header->APPEND_CONST("\r\n");
	}
}

/// @todo intelligent cache-control
void SAPI::send_header(Pool& pool) {
	SAPI_func_context& ctx=*static_cast<SAPI_func_context *>(pool.get_context());

/*	ctx.header->APPEND_CONST(
		"expires: Fri, 23 Mar 2001 09:32:23 GMT\r\n"
		"\r\n");
*/	HSE_SEND_HEADER_EX_INFO header_info;

	char status_buf[MAX_STATUS_LENGTH];
	switch(ctx.http_response_code) {
		case 200:
			header_info.pszStatus="200 OK";
			break;
		case 302:
			header_info.pszStatus="302 Moved Temporarily";
			break;
		case 401:// useless untill parser auth mech
			header_info.pszStatus="401 Authorization Required";
			break;
		default:
			snprintf(status_buf, MAX_STATUS_LENGTH, 
				"%d Undescribed", ctx.http_response_code);
			header_info.pszStatus=status_buf;
			break;
	}
	header_info.cchStatus=strlen(header_info.pszStatus);
	header_info.pszHeader=ctx.header->cstr();
	header_info.cchHeader=ctx.header->size();
	header_info.fKeepConn=true;

	ctx.lpECB->dwHttpStatusCode=ctx.http_response_code;

	ctx.lpECB->ServerSupportFunction(ctx.lpECB->ConnID, 
		HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);
}

void SAPI::send_body(Pool& pool, const void *buf, size_t size) {
	SAPI_func_context& ctx=*static_cast<SAPI_func_context *>(pool.get_context());

	DWORD num_bytes=size;
	ctx.lpECB->WriteClient(ctx.lpECB->ConnID, 
		const_cast<void *>(buf), &num_bytes, HSE_IO_SYNC);
}

// 

int failed_new(size_t size) {
	SAPI::die("out of memory in 'new', failed to allocated %u bytes", size);
	return 0; // not reached
}

static bool parser_init() {
	static bool globals_inited=false;
	if(globals_inited)
		return true;
	globals_inited=true;

	_set_new_handler(failed_new);

	static Pool pool(0); // global pool
	try {
		// init socks
		init_socks(pool);

#ifdef XML
		/**
		* Initialize Xerces and Xalan.
		*
		* Should be called only once per process before making
		* any other API calls.
		*/
		//_asm int 3;
		XalanInitialize();
		pool.register_cleanup(callXalanTerminate, 0);
#endif

		// init global classes
		init_methoded_array(pool);
		// init global variables
		pa_globals_init(pool);
		
		// successful finish
		return true;
	} catch(const Exception& e) { // global problem 
		const char *body=e.comment();
		
		// unsuccessful finish
		return false;
	}
}

/// ISAPI //
BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer) {
	pVer->dwExtensionVersion = HSE_VERSION;
	strncpy(pVer->lpszExtensionDesc, "Parser "PARSER_VERSION, HSE_MAX_EXT_DLL_NAME_LEN-1);
	pVer->lpszExtensionDesc[HSE_MAX_EXT_DLL_NAME_LEN-1]=0;
	return parser_init();
}

/** 
	ISAPI // main workhorse

	@todo 
		IIS: remove trailing default-document[index.html] from $request.uri.
		to do that we need to consult metabase,
		wich is tested&works but seems slow runtime 
		and not could-be-quickly-implemented if prepared.
	@test
		PARSER_VERSION from outside
*/

void real_parser_handler(Pool& pool, LPEXTENSION_CONTROL_BLOCK lpECB, bool header_only) {
	static_cast<SAPI_func_context *>(pool.get_context())->header=new(pool) String(pool);
	
	// Request info
	Request::Info request_info;

	size_t path_translated_buf_size=strlen(lpECB->lpszPathTranslated)+1;
	char *filespec_to_process=(char *)pool.malloc(path_translated_buf_size);
	memcpy(filespec_to_process, lpECB->lpszPathTranslated, path_translated_buf_size);
#ifdef WIN32
	back_slashes_to_slashes(filespec_to_process);
#endif

	if(const char *path_info=SAPI::get_env(pool, "PATH_INFO")) {
		// IIS
		size_t len=strlen(filespec_to_process)-strlen(path_info);
		char *buf=(char *)pool.malloc(len+1);
		strncpy(buf, filespec_to_process, len); buf[len]=0;
		request_info.document_root=buf;
	} else
		throw Exception(0, 0,
			0,
			"ISAPI: no PATH_INFO defined (in reinventing DOCUMENT_ROOT)");

	request_info.path_translated=filespec_to_process;
	request_info.method=lpECB->lpszMethod;
	request_info.query_string=lpECB->lpszQueryString;
	if(lpECB->lpszQueryString && *lpECB->lpszQueryString) {
		char *reconstructed_uri=(char *)pool.malloc(
			strlen(lpECB->lpszPathInfo)+1/*'?'*/+
			strlen(lpECB->lpszQueryString)+1/*0*/);
		strcpy(reconstructed_uri, lpECB->lpszPathInfo);
		strcat(reconstructed_uri, "?");
		strcat(reconstructed_uri, lpECB->lpszQueryString);
		request_info.uri=reconstructed_uri;
	} else
		request_info.uri=lpECB->lpszPathInfo;
	
	request_info.content_type=lpECB->lpszContentType;
	request_info.content_length=lpECB->cbTotalBytes;
	request_info.cookie=SAPI::get_env(pool, "HTTP_COOKIE");
	request_info.user_agent=SAPI::get_env(pool, "HTTP_USER_AGENT");

	
	// prepare to process request
	Request request(pool,
		request_info,
		String::UL_HTML|String::UL_OPTIMIZE_BIT,
		false /* status_allowed */);

	// some root-controlled location
	//   c:\windows
	char root_config_path[MAX_STRING];
	GetWindowsDirectory(root_config_path, MAX_STRING);
	// must be dynamic: rethrowing from request.core 
	//   may return 'source' which can be inside of 'root auto.p@exeception'
	char *root_config_filespec=(char *)pool.malloc(MAX_STRING);
	snprintf(root_config_filespec, MAX_STRING, 
		"%s/%s", 
		root_config_path, CONFIG_FILE_NAME);

	// process the request
	request.core(
		root_config_filespec, false/*may be abcent*/, // /path/to/admin/auto.p
		0/*parser_site_auto_path*/, false, // /path/to/site/auto.p
		header_only);
}

void call_real_parser_handler__do_SEH(Pool& pool, 
									  LPEXTENSION_CONTROL_BLOCK lpECB,
									  bool header_only) {
#if _MSC_VER & !defined(_DEBUG)
	LPEXCEPTION_POINTERS system_exception=0;
	__try {
#endif
		real_parser_handler(pool, lpECB, header_only);
		
#if _MSC_VER & !defined(_DEBUG)
	} __except (
		(system_exception=GetExceptionInformation()), 
		EXCEPTION_EXECUTE_HANDLER) {
		
		if(system_exception)
			if(_EXCEPTION_RECORD *er=system_exception->ExceptionRecord)
				throw Exception(0, 0,
				0,
				"Exception 0x%08X at 0x%08X", er->ExceptionCode,  er->ExceptionAddress);
			else
				throw Exception(0, 0, 0, "Exception <no exception record>");
			else
				throw Exception(0, 0, 0, "Exception <no exception information>");
	}
#endif
}


DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB) {
	Pool_storage pool_storage;
	Pool pool(&pool_storage); // no allocations until assigned context [for reporting]
	SAPI_func_context ctx={
		lpECB,
		0, // filling later: so that if there would be error pool would have ctx
		200 // default http_response_code
	};
	pool.set_context(&ctx);// no allocations before this line!
	
#ifdef XML
	/**
	 * Initialize Xerces and Xalan.
	 *
	 * Should be called only once per process before making
	 * any other API calls.
	 */
	//_asm int 3;
	XalanInitialize();
	pool.register_cleanup(callXalanTerminate, 0);
#endif


	bool header_only=strcasecmp(lpECB->lpszMethod, "HEAD")==0;
	try { // global try
		call_real_parser_handler__do_SEH(pool, lpECB, header_only);
		// successful finish
	} catch(const Exception& e) { // global problem
		// don't allocate anything on pool here:
		//   possible pool' exception not catch-ed now
			//   and there could be out-of-memory exception
		const char *body=e.comment();
		// log it
		SAPI::log(pool, "exception in request exception handler: %s", body);

		//
		int content_length=strlen(body);

		// prepare header // not using SAPI func wich allocates on pool
		char header_buf[MAX_STRING];
		int header_len=snprintf(header_buf, MAX_STRING,
			"content-type: text/plain\r\n"
			"content-length: %lu\r\n"
			"expires: Fri, 23 Mar 2001 09:32:23 GMT\r\n"
			"\r\n",
			content_length);

		HSE_SEND_HEADER_EX_INFO header_info;
		header_info.pszStatus="200 OK";
		header_info.cchStatus=strlen(header_info.pszStatus);
		header_info.pszHeader=header_buf;
		header_info.cchHeader=header_len;
		header_info.fKeepConn=true;
		
		// send header
		lpECB->dwHttpStatusCode=200;
		lpECB->ServerSupportFunction(lpECB->ConnID, 
			HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);

		// send body
		if(!header_only)
			SAPI::send_body(pool, body, content_length);

		// unsuccessful finish
	}
	
	return HSE_STATUS_SUCCESS_AND_KEEP_CONN;
}
