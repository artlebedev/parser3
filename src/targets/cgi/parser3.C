/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3.C,v 1.2 2001/03/13 17:54:14 paf Exp $
*/

#include "core.h"
#include "pa_request.h"
#include "pa_common.h"

int main(int argc, char *argv[]) {
	Pool pool;
	core(pool);

	bool cgi=
		getenv("SERVER_SOFTWARE") || 
		getenv("SERVER_NAME") || 
		getenv("GATEWAY_INTERFACE") || 
		getenv("REQUEST_METHOD");

	// TODO: ifdef WIN32 flip \\ to /
	char *document_root="Y:/parser3/src/";
	char *page_filespec="Y:/parser3/src/test.p";

	// request
	Request request(pool,
		cgi?String::Untaint_lang::HTML_TYPO:String::Untaint_lang::NO, // :)
		document_root,
		page_filespec
		);

	bool error;
	char *result=request.core(error);

	if(error)
		printf("ERROR: %s", result);
	else
		printf("%s", result);

	return 0;
}
