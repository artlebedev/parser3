/** @file
	Parser: scripting and CGI main.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3.C,v 1.31 2001/03/22 15:30:47 paf Exp $
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

#include "pa_common.h"
#include "pa_globals.h"
#include "pa_request.h"

Pool pool; // global pool
bool cgi; ///< we were started as CGI?

#ifdef WIN32
#	if _MSC_VER
// intercept global system errors
static LONG WINAPI TopLevelExceptionFilter (
									 struct _EXCEPTION_POINTERS *ExceptionInfo
									 ) {
	char buf[MAX_STRING];
	if(ExceptionInfo && ExceptionInfo->ExceptionRecord) {
		struct _EXCEPTION_RECORD *rr=ExceptionInfo->ExceptionRecord;
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
#	endif

#endif

//\if
static void fix_slashes(char *s) {
	if(s)
		for(; *s; s++)
			if(*s=='\\')
				*s='/';
}
//\endif

// service funcs

static const char *get_env(Pool& pool, const char *name) {
 	return getenv(name);
}

static uint read_post(char *buf, uint max_bytes) {
	int read_size=0;
	do {
		int chunk_size=read
			(fileno(stdin), buf+read_size, min(0x400*0x400, max_bytes-read_size));
		if(chunk_size<0)
			break;
		read_size+=chunk_size;
	} while(read_size<max_bytes);

	return read_size;
}

static void add_header_attribute(const char *key, const char *value) {
	if(cgi)
		printf("%s: %s\n", key, value);
}

static void send_header(const char *buf, size_t size) {
	if(cgi) // header | body  delimiter
		puts("");
}

static void send_body(const char *buf, size_t size) {
	stdout_write(buf, size);
}

/// Service funcs 
 Service_funcs service_funcs={
		get_env,
		read_post,
		add_header_attribute,
		send_header,
		send_body
 };


// main

int main(int argc, char *argv[]) {
	// TODO:umask(2);
//\#ifdef WIN32
	setmode(fileno(stdin), _O_BINARY);
	setmode(fileno(stdout), _O_BINARY);
	setmode(fileno(stderr), _O_BINARY);
//\#endif

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
//\#ifdef WIN32
	fix_slashes(filespec_to_process);
//\#endif

	PTRY { // global try
		// must be first in PTRY{}PCATCH
#ifdef WIN32
#	if _MSC_VER
		SetUnhandledExceptionFilter(&TopLevelExceptionFilter);
		//TODO: initSocks();
#	endif
#endif

		// init global variables
		globals_init(pool);

		if(!filespec_to_process)
			PTHROW(0, 0,
				0,
				"no file to process");

		// Request info
		Request::Info request_info;
		const char *document_root=getenv("DOCUMENT_ROOT");
		if(!document_root) {
			static char fake_document_root[MAX_STRING];
			strncpy(fake_document_root, filespec_to_process, MAX_STRING);
			rsplit(fake_document_root, '/');  rsplit(fake_document_root, '\\');// strip filename
			document_root=fake_document_root;
		}
		request_info.document_root=document_root;
		request_info.path_translated=filespec_to_process;
		request_info.method=getenv("REQUEST_METHOD");
		request_info.query_string=getenv("QUERY_STRING");
		request_info.uri=getenv("REQUEST_URI");
		request_info.content_type=getenv("CONTENT_TYPE");
		const char *content_length=getenv("CONTENT_LENGTH");
		request_info.content_length=(content_length?atoi(content_length):0);
		request_info.cookie=getenv("HTTP_COOKIE");

		// prepare to process request
		Pool request_pool;
		Request request(request_pool,
			request_info,
			cgi ? String::UL_HTML_TYPO : String::UL_NO
			);
		
		// some root-controlled location
		char *root_auto_path;
#ifdef WIN32
		// c:\windows
		root_auto_path=(char *)pool.malloc(MAX_STRING);
		GetWindowsDirectory(root_auto_path, MAX_STRING);
		strcat(root_auto_path, "/");
#else
		// ~nobody
		root_auto_path=getenv("HOME");
#endif
		
		// beside by binary
		char *site_auto_path=(char *)pool.malloc(MAX_STRING);
		strncpy(site_auto_path, argv[0], MAX_STRING);  // filespec of my binary
		rsplit(site_auto_path, '/');  rsplit(site_auto_path, '\\');// strip filename
		strcat(site_auto_path, "/");
		
		// process the request
		request.core(pool.exception(),
			root_auto_path, false,
			site_auto_path, false,
			strcasecmp(request_info.method, "HEAD")==0);

		// must be last in PTRY{}PCATCH
#ifdef WIN32
#	if _MSC_VER
		SetUnhandledExceptionFilter(0);
#	endif
#endif
		// successful finish
		return 0;
	} PCATCH(e) { // global problem 
		const char *body=e.comment();
		int content_length=strlen(body);

		// header
		(*service_funcs.output_header_attribute)("content-type", "text/plain");
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%d", content_length);
		(*service_funcs.output_header_attribute)("content-length", 
			content_length_cstr);

		// body
		(*service_funcs.output_body)(body, content_length);

		// unsuccessful finish
		return 1;
	}
	PEND_CATCH
}
