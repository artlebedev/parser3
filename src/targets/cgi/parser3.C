/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3.C,v 1.4 2001/03/13 19:35:07 paf Exp $
*/

#include "pa_config.h"

#ifdef WIN32
#	include <windows.h>
#	include <io.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "core.h"
#include "pa_request.h"
#include "pa_common.h"

#ifdef WIN32
// TODO:  LONG WINAPI TopLevelExceptionFilter (
#endif

int main(int argc, char *argv[]) {
#ifdef WIN32
	_fmode=_O_BINARY;			/*sets default for file streams to binary */
	setmode(_fileno(stdin), _O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stdout), _O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stderr), _O_BINARY);		/* make the stdio mode be binary */

	//TODO:	SetUnhandledExceptionFilter(&TopLevelExceptionFilter);
	//TODO: initSocks();
#endif

	
	Pool pool;
	core(pool);

	// were we started as CGI?
	bool cgi=
		getenv("SERVER_SOFTWARE") || 
		getenv("SERVER_NAME") || 
		getenv("GATEWAY_INTERFACE") || 
		getenv("REQUEST_METHOD");

	// TODO: ifdef WIN32 flip \\ to /
	const char *document_root="Y:/parser3/src/";
	const char *page_filespec="Y:/parser3/src/test.p";

	// request
	Request request(pool,
		cgi ? String::Untaint_lang::HTML_TYPO : String::Untaint_lang::NO,
		document_root,
		page_filespec
		);
	
	// some root-controlled location
	char *sys_auto_path1;
#ifdef WIN32
	sys_auto_path1=(char *)pool.malloc(MAX_STRING);
	GetWindowsDirectory(sys_auto_path1, MAX_STRING-1/*for \*/);
	strcat(sys_auto_path1, "\\");
#else
	sys_auto_path1=getenv("HOME");
#endif
	
	// beside by binary
	char *sys_auto_path2=(char *)pool.malloc(MAX_STRING);
	strncpy(sys_auto_path2, argv[0], MAX_STRING-20);  // filespec of my binary
	rsplit(sys_auto_path2, '\\');  rsplit(sys_auto_path2, '/'); // strip filename
	
	char *result=request.core(
		sys_auto_path1, 
		sys_auto_path2);
	
	const char *error="nested exception";
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
