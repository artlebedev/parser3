/** @file
	Parser: request info class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_REQUEST_INFO_H
#define PA_REQUEST_INFO_H

static const char * const IDENT_REQUEST_INFO_H="$Date: 2005/08/05 13:02:59 $";

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
	bool mail_received;
	//@}
	//@{ these are filed by Request class itself: user's post data
	const char* post_data;  size_t post_size;
	//@}
};

#endif
