/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_HTTP_H
#define PA_HTTP_H

#define IDENT_PA_HTTP_H "$Id: pa_http.h,v 1.15 2016/07/26 13:20:23 moko Exp $"

#include "pa_vstring.h"
#include "pa_vint.h"
#include "pa_vhash.h"
#include "pa_vtable.h"
#include "pa_socks.h"
#include "pa_charset.h"
#include "pa_request.h"

#define HTTP_COOKIES_NAME "cookies"

#ifndef DOXYGEN
struct File_read_http_result {
	char *str; size_t length;
	HashStringValue* headers;
}; 
#endif

class ResponseHeaders {
public:
	class Header {
	public:
		String::Body name;
		String::Body value;

		Header(String::Body aname, String::Body avalue) : name(aname), value(avalue) {}

		void transcode(Charset &charset, Charset &source){
			name=Charset::transcode(name, charset, source);
			value=Charset::transcode(value, charset, source);
		}

	};

	Array<Header> headers;

	String::Body content_type;

	bool add_header(const char *line){
		const char *value=strchr(line, ':');

		if(value && value != line){ // we need only headers, not the response code
			Header header(str_upper(line, value-line), String::Body(value+1).trim(String::TRIM_BOTH, " \t\n\r"));

			if(header.name == String::Body(HTTP_CONTENT_TYPE_UPPER) && content_type.is_empty())
				content_type=header.value;

			headers+=header;

			return true;
		}
		return false;
	}

};

Table* parse_cookies(Request& r, Table *cookies);
void *tables_update(HashStringValue& tables, const String::Body name, const String& value);

char *pa_http_safe_header_name(const char *name);

File_read_http_result pa_internal_file_read_http(Request& r, const String& file_spec, bool as_text, HashStringValue *options=0, bool transcode_text_result=true);

#endif
