/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3.C,v 1.9 2001/03/14 09:12:06 paf Exp $
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
#include "vform_fields_fill.h"

Pool pool; // global pool

#ifdef WIN32
#	if MSVC
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

int main(int argc, char *argv[]) {
	// were we started as CGI?
	bool cgi=
		getenv("SERVER_SOFTWARE") || 
		getenv("SERVER_NAME") || 
		getenv("GATEWAY_INTERFACE") || 
		getenv("REQUEST_METHOD");
	
	char *result;  char error[MAX_STRING];  error[0]=0;
	PTRY { // global try
		// must be first in PTRY{}PCATCH
#ifdef WIN32
#	if MSVC
		SetUnhandledExceptionFilter(&TopLevelExceptionFilter);
		//TODO: initSocks();
#	endif
#endif

		globals_init(pool);
		
		// TODO: ifdef WIN32 flip \\ to /
		const char *document_root="Y:/parser3/src/";
		const char *page_filespec="Y:/parser3/src/test.p";
		
		// prepare to process request
		Request request(Pool(),
			cgi ? String::Untaint_lang::HTML_TYPO : String::Untaint_lang::NO,
			document_root,
			page_filespec
			);
		
		// fill user passed forms
		vform_fields_fill(pool, cgi, request.form_class.fields());
		
		// some root-controlled location
		char *sys_auto_path1;
#ifdef WIN32
		sys_auto_path1=(char *)pool.malloc(MAX_STRING);
		GetWindowsDirectory(sys_auto_path1, MAX_STRING);
		strcat(sys_auto_path1, "\\");
#else
		sys_auto_path1=getenv("HOME");
#endif
		
		// beside by binary
		char *sys_auto_path2=(char *)pool.malloc(MAX_STRING);
		strncpy(sys_auto_path2, argv[0], MAX_STRING);  // filespec of my binary
		rsplit(sys_auto_path2, PATH_DELIMITER_CHAR);  // strip filename
		
		// process the request
		result=request.core(
			sys_auto_path1, 
			sys_auto_path2);
		// set error, will be reported in case result==0
		strcpy(error, "exception occured in request exception handler");

		// must be last in PTRY{}PCATCH
#ifdef WIN32
#	if MSVC
		SetUnhandledExceptionFilter(0);
#	endif
#endif
	} PCATCH(e) { // global problem @globals fill @Request create @prepare to .core()
		result=0;
		strcpy(error, e.comment());
	}
	PEND_CATCH

	// write out the result	
	if(cgi) {
		if(result) {
			const char *content_type="text/html";
			printf(
				"Content-type: %s\n"
				"Content-length: %d\n"
				"\n", 
				content_type,
				strlen(result));
			stdout_write(result);
		} else {
			printf(
				"Content-type: text/plain\n"
				"Content-length: %d\n"
				"\n", 
				strlen(error));
			stdout_write(error);
		}
	} else
		if(result)
			printf("%s", result);
		else
			fputs(error, stderr);

	return 0;
}
