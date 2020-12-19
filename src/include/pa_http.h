/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_HTTP_H
#define PA_HTTP_H

#define IDENT_PA_HTTP_H "$Id: pa_http.h,v 1.32 2020/12/19 22:34:21 moko Exp $"

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

class HTTP_Headers {
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
	uint64_t content_length;

	HTTP_Headers() : content_type(""), content_length(0){}

	bool add_header(const char *line);

	void clear(){
		headers.clear();
		content_type="";
		content_length=0;
	}

};

/*** http part ***/

Table* parse_cookies(Request& r, Table *cookies);
void tables_update(HashStringValue& tables, const String::Body name, const String& value);

char *pa_http_safe_header_name(const char *name);

File_read_http_result pa_internal_file_read_http(Request& r, const String& file_spec, bool as_text, HashStringValue *options=0, bool transcode_text_result=true);

/*** httpd part ***/

//#define HTTPD_DEBUG

class HTTPD_request;

class HTTPD_Connection : public PA_Allocated {
public:
	int sock;
	const char *remote_addr;
	HTTPD_request *request;

	HTTPD_Connection() : sock(-1), remote_addr(NULL), request(NULL){}
	~HTTPD_Connection();

	Array<HTTP_Headers::Header> &headers();

	const char *method();
	const char *uri();
	const char *content_type();
	uint64_t content_length();

	const char *query(){
		if(uri()){
			const char *result=strchr(uri(), '?');
			if(result++ && *result)
				return result;
		}
		return NULL;
	}

	bool accept(int, int);
	bool read_header();
	size_t read_post(char *, size_t);
	size_t send_body(const void *, size_t);
};

class HTTPD_Server : public PA_Allocated {
public:
	enum HTTPD_MODE {
		SEQUENTIAL,
		PARALLEL,
		MULTITHREADED
	} static mode;
	static const char *port;

	static void set_mode(const String&);
	static int bind(const char *);
};

#endif
