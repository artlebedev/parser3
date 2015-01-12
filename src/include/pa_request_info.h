/** @file
	Parser: request info class decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_REQUEST_INFO_H
#define PA_REQUEST_INFO_H

#define IDENT_PA_REQUEST_INFO_H "$Id: pa_request_info.h,v 1.8 2015/01/12 12:22:02 misha Exp $"

/// some information from web server
class Request_info {
public:
	//@{ these filled by Request class user
	const char* document_root;
	const char* path_translated;
	const char* method;
	const char* query_string;
	const char* uri;
	const char* content_type;
	size_t content_length;
	const char* cookie;
	
	char** argv;
	int args_skip;
	bool mail_received;
	//@}
	//@{ these are filed by Request class itself: user's post data
	const char* post_data;  size_t post_size;
	//@}

	// misha@ Probably it's a good idea to move this method into targets, but de facto the only POST and PUT can have the body and all targets should support it
	bool can_have_body(){
		return
			method
			&& (
				strcasecmp(method, "POST") == 0
				|| strcasecmp(method, "PUT") == 0
			);
	}
};

#endif
