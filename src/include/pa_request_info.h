/** @file
	Parser: request info class decl.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_REQUEST_INFO_H
#define PA_REQUEST_INFO_H

#define IDENT_PA_REQUEST_INFO_H "$Id: pa_request_info.h,v 1.10 2016/11/24 22:18:59 moko Exp $"

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

	static const char* remove_absolute_uri(const char *auri){
		if(*auri == '/')
			return auri;

		// extractring https?://site.name prefix allowed by http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html#sec5.1.2

		if(!pa_strncasecmp(auri, "http://"))
			auri+=7;
		else if(!pa_strncasecmp(auri, "https://"))
			auri+=8;
		else
			return auri;
		for(; *auri && *auri != '/'; auri++);
		return auri;
	}
};

#endif
