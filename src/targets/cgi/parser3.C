/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3.C,v 1.22 2001/03/18 20:32:53 paf Exp $
*/

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif


#ifdef WIN32
#	include <windows.h>
#	include <io.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "pa_globals.h"
#include "pa_request.h"
#include "pa_common.h"

Pool pool; // global pool
bool cgi;

#ifdef WIN32
#	if _MSC_VER
// intercept global system errors
LONG WINAPI TopLevelExceptionFilter (
									 struct _EXCEPTION_POINTERS *ExceptionInfo
									 ) {
	char buf[MAX_STRING];
	if(ExceptionInfo && ExceptionInfo->ExceptionRecord) {
		struct _EXCEPTION_RECORD *r=ExceptionInfo->ExceptionRecord;
		
		int printed=0;
		printed+=snprintf(buf+printed, MAX_STRING-printed, "Exception 0x%X at 0x%p", 
			r->ExceptionCode, 
			r->ExceptionAddress);
		for(unsigned int i=0; i<r->NumberParameters; i++)
			printed+=snprintf(buf+printed, MAX_STRING-printed, ", 0x%X", 
				r->ExceptionInformation[i]);
	} else 
		strcpy(buf, "Exception <unknown>");
	
	PTHROW(0, 0,
		0,
		buf);

	return EXCEPTION_EXECUTE_HANDLER; // never reached
}
#	endif
#endif

// service funcs

int read_post(char *buf, int max_bytes) {
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

void output_header_attribute(const char *key, const char *value) {
	if(cgi)
		printf("%s: %s\n", key, value);
}

void output_body(const char *buf, size_t size) {
	if(cgi) // header | body  delimiter
		puts("");

	stdout_write(buf, size);
}

// main

int main(int argc, char *argv[]) {
	//TODO: umask(2);
#ifdef WIN32
	_setmode(fileno(stdin), _O_BINARY);
	_setmode(fileno(stdout), _O_BINARY);
	_setmode(fileno(stderr), _O_BINARY);
#endif

	// Service funcs 
	service_funcs.read_post=read_post;
	service_funcs.output_header_attribute=output_header_attribute;
	service_funcs.output_body=output_body;
	
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

	const char *filespec_to_process=cgi?getenv("PATH_TRANSLATED"):argv[1];

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
		// TODO: ifdef WIN32 flip \\ to /
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
		Request request(Pool(),
			request_info,
			cgi ? String::Untaint_lang::HTML_TYPO : String::Untaint_lang::NO
			);
		
		// some root-controlled location
		char *sys_auto_path1;
#ifdef WIN32
		// c:\windows
		sys_auto_path1=(char *)pool.malloc(MAX_STRING);
		GetWindowsDirectory(sys_auto_path1, MAX_STRING);
		strcat(sys_auto_path1, PATH_DELIMITER_STRING);
#else
		// ~nobody
		sys_auto_path1=getenv("HOME");
#endif
		
		// beside by binary
		char *sys_auto_path2=(char *)pool.malloc(MAX_STRING);
		strncpy(sys_auto_path2, argv[0], MAX_STRING);  // filespec of my binary
		rsplit(sys_auto_path2, '/');  rsplit(sys_auto_path2, '\\');// strip filename
		strcat(sys_auto_path2, PATH_DELIMITER_STRING);
		
		// process the request
		request.core(pool.exception(),
			sys_auto_path1, 
			sys_auto_path2);
		// no actions with request' data past this point
		// request.exception not not handled here, but all
		// request' data are associated with it's pool=exception

		// must be last in PTRY{}PCATCH
#ifdef WIN32
#	if _MSC_VER
		SetUnhandledExceptionFilter(0);
#	endif
		// successful finish
		return 0;
#endif
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
