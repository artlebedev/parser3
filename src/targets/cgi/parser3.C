/** @file
	Parser: scripting and CGI main.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3.C,v 1.38 2001/03/23 10:32:53 paf Exp $
*/

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif


#ifdef WIN32
#	include <windows.h>
#	include <io.h>
#else
#	include <unistd.h>
#endif

//\ifwin32
#include <io.h>
//#include <fcntl.h>
//\endifwin32

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_globals.h"
#include "pa_request.h"

Pool pool; // global pool
bool cgi; ///< we were started as CGI?

#if defined(WIN32) && _MSC_VER
// intercept global system errors
static LONG WINAPI TopLevelExceptionFilter (struct _EXCEPTION_POINTERS *ExceptionInfo) {
	char buf[MAX_STRING];
	if(ExceptionInfo && ExceptionInfo->ExceptionRecord) {
		struct _EXCEPTION_RECORD *er=ExceptionInfo->ExceptionRecord;
		snprintf(buf, MAX_STRING, "Exception %#X at %p", 
			er->ExceptionCode, 
			er->ExceptionAddress);
	} else 
		strcpy(buf, "Exception <unknown>");
	
	PTHROW(0, 0,
		0,
		buf);

	return EXCEPTION_EXECUTE_HANDLER; // never reached
}
#endif

//\if
static void fix_slashes(char *s) {
	if(s)
		for(; *s; s++)
			if(*s=='\\')
				*s='/';
}
//\endif

//@{
/// SAPI funcs decl
const char *SAPI::get_env(Pool& pool, const char *name) {
 	return getenv(name);
}

uint SAPI::read_post(Pool& pool, char *buf, uint max_bytes) {
	uint read_size=0;
	do {
		int chunk_size=read(fileno(stdin), 
			buf+read_size, min(0x400*0x400, max_bytes-read_size));
		if(chunk_size<0)
			break;
		read_size+=chunk_size;
	} while(read_size<max_bytes);

	return read_size;
}

void SAPI::add_header_attribute(Pool& pool, const char *key, const char *value) {
	if(cgi)
		printf("%s: %s\n", key, value);
}

/// @todo intelligent cache-control
void SAPI::send_header(Pool& pool) {
	if(cgi) {
		puts("Expires: Fri, 23 Mar 2001 09:32:23 GMT");

		// header | body  delimiter
		puts("");
	}
}

void SAPI::send_body(Pool& pool, const char *buf, size_t size) {
	stdout_write(buf, size);
}
//@}

// main

int main(int argc, char *argv[]) {
	umask(2);

#ifdef WIN32
	setmode(fileno(stdin), _O_BINARY);
	setmode(fileno(stdout), _O_BINARY);
	setmode(fileno(stderr), _O_BINARY);
#endif

	// were we started as CGI?
	cgi=
		getenv("SERVER_SOFTWARE") || 
		getenv("SERVER_NAME") || 
		getenv("GATEWAY_INTERFACE") || 
		getenv("REQUEST_METHOD");
	
	if(!cgi) {
		if(argc<2) {
			char *binary=argv[0];
			printf("Usage: %s <file>\n", binary?binary:"parser3");
			exit(1);
		}
	}

	char *filespec_to_process=cgi?getenv("PATH_TRANSLATED"):argv[1];
#ifdef WIN32
	fix_slashes(filespec_to_process);
#endif

	const char *request_method=getenv("REQUEST_METHOD");
	bool header_only=request_method && strcasecmp(request_method, "HEAD")==0;
	PTRY { // global try
		// must be first in PTRY{}PCATCH
#if defined(WIN32) && _MSC_VER
		SetUnhandledExceptionFilter(&TopLevelExceptionFilter);
		//TODO: initSocks();
#endif

		// init global variables
		pa_globals_init(pool);

		if(!filespec_to_process)
			PTHROW(0, 0,
				0,
				"no file to process");

		// Request info
		Request::Info request_info;
		if(cgi)
			request_info.document_root=getenv("DOCUMENT_ROOT");
		else {
			static char document_root[MAX_STRING];
			strncpy(document_root, filespec_to_process, MAX_STRING);
			rsplit(document_root, '/');  rsplit(document_root, '\\');// strip filename
			request_info.document_root=document_root;
		}
		request_info.path_translated=filespec_to_process;
		request_info.method=request_method;
		request_info.query_string=getenv("QUERY_STRING");
		request_info.uri=getenv("REQUEST_URI");
		request_info.content_type=getenv("CONTENT_TYPE");
		const char *content_length=getenv("CONTENT_LENGTH");
		request_info.content_length=(content_length?atoi(content_length):0);
		request_info.cookie=getenv("HTTP_COOKIE");

		// prepare to process request
		Request request(pool,
			request_info,
			cgi ? String::UL_HTML_TYPO : String::UL_NO
			);
		
		// some root-controlled location
#ifdef WIN32
		// c:\windows
		static char root_auto_path[MAX_STRING];
		GetWindowsDirectory(root_auto_path, MAX_STRING);
#else
		// ~nobody
		char *root_auto_path=getenv("HOME");
#endif
		
		// beside by binary
		static char site_auto_path[MAX_STRING];
		strncpy(site_auto_path, argv[0], MAX_STRING);  // filespec of my binary
		rsplit(site_auto_path, '/');  rsplit(site_auto_path, '\\');// strip filename
		
		// process the request
		request.core(
			root_auto_path, false,
			site_auto_path, false,
			header_only);

		// must be last in PTRY{}PCATCH
#if defined(WIN32) && _MSC_VER
		SetUnhandledExceptionFilter(0);
#endif
		// successful finish
		return 0;
	} PCATCH(e) { // global problem 
		const char *body=e.comment();
		int content_length=strlen(body);

		// prepare header
		SAPI::add_header_attribute(pool, "content-type", "text/plain");
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%lu", content_length);
		SAPI::add_header_attribute(pool, "content-length", content_length_cstr);

		// send header
		SAPI::send_header(pool);

		// body
		if(!header_only)
			SAPI::send_body(pool, body, content_length);

		// unsuccessful finish
		return 1;
	}
	PEND_CATCH
}
