/** @file
	Parser: http support functions.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
 */

#include "pa_http.h"
#include "pa_common.h"
#include "pa_base64.h"
#include "pa_charsets.h"
#include "pa_request_charsets.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_random.h"

volatile const char * IDENT_PA_HTTP_C="$Id: pa_http.C,v 1.118 2021/01/21 16:46:53 moko Exp $" IDENT_PA_HTTP_H; 

#ifdef _MSC_VER
#include <windows.h>
#define socklen_t int
#else
#define closesocket close
#endif

// defines

#define HTTP_METHOD_NAME	"method"
#define HTTP_FORM_NAME	"form"
#define HTTP_BODY_NAME	"body"
#define HTTP_TIMEOUT_NAME	"timeout"
#define HTTP_HEADERS_NAME	"headers"
#define HTTP_FORM_ENCTYPE_NAME	"enctype"
#define HTTP_ANY_STATUS_NAME	"any-status"
#define HTTP_OMIT_POST_CHARSET_NAME	"omit-post-charset"	// ^file::load[...;http://...;$.method[post]] by default adds charset to content-type

#define HTTP_USER "user"
#define HTTP_PASSWORD "password"

#define HTTP_USER_AGENT "user-agent"
#define DEFAULT_USER_AGENT "parser3"

#ifndef INADDR_NONE
#define INADDR_NONE ((ulong) -1)
#endif

#undef CRLF
#define CRLF "\r\n"

// helpers

bool HTTP_Headers::add_header(const char *line){
	const char *value=strchr(line, ':');

	if(value && value != line){ // we need only headers, not the response code
		Header header(str_upper(line, value-line), String::Body(value+1).trim(String::TRIM_BOTH, " \t\n\r"));

		if(header.name == String::Body(HTTP_CONTENT_TYPE_UPPER) && content_type.is_empty())
			content_type=header.value;

		if(header.name == String::Body("CONTENT-LENGTH") && content_length==0)
			ALTER_EXCEPTION_COMMENT(content_length=pa_atoul(header.value.cstr()), " for content-length");

		headers+=header;

		return true;
	}
	return false;
}

class Cookies_table_template_columns: public ArrayString {
public:
	Cookies_table_template_columns() {
		*this+=new String("name");
		*this+=new String("value");
		*this+=new String("expires");
		*this+=new String("max-age");
		*this+=new String("domain");
		*this+=new String("path");
		*this+=new String("httponly");
		*this+=new String("secure");
	}
};


static bool set_addr(struct sockaddr_in *addr, const char* host, const short port){
	memset(addr, 0, sizeof(*addr)); 
	addr->sin_family=AF_INET;
	addr->sin_port=htons(port); 
	if(host) {
		struct hostent *hostIP=gethostbyname(host);
		if(hostIP && hostIP->h_addrtype == AF_INET){
			memcpy(&addr->sin_addr, hostIP->h_addr, hostIP->h_length);
			return true;
		}
	}
	return false;
}

class HTTP_response : public PA_Allocated {
public:
	char *buf;
	size_t length;
	size_t buf_size;
	size_t body_offset;

	HTTP_Headers headers;

	HTTP_response() : buf(NULL), length(0), buf_size(0), body_offset(0){}

	void resize(size_t size){
		buf_size=size;
		buf=(char *)pa_realloc(buf, size + 1);
	}

	bool read(int sock, size_t size){
		if(length + size > buf_size)
			resize(buf_size * 2 + size);
		ssize_t received_size=recv(sock, buf + length, size, 0);
		if(received_size == 0)
			return false;
		if(received_size < 0) {
			if(int no = pa_socks_errno())
				throw Exception("http.timeout", 0, "error receiving response: %s (%d)", pa_socks_strerr(no), no);
			return false;
		}
		length+=received_size;
		buf[length]='\0';
		return true;
	}

	size_t first_line(){
		char *header=strchr(buf, '\n');
		if(!header)
			return false;

		return header-buf;
	}

	const char *status_code(char *status_line, int &result){
		char* status_start = strchr(status_line, ' ');

		if(!(status_start++))
			return status_line;

		char* status_end=strchr(status_start, ' ');

		if(!status_end)
			return status_line;

		if(status_end==status_start)
			return status_line;

		const char *result_str=pa_strdup(status_start, status_end-status_start);
		ALTER_EXCEPTION_COMMENT(result=pa_atoui(result_str), " for HTTP status");
		return result_str;
	}

	bool body_start(){
		char *p=buf;
		while((p=strchr(p, '\n'))) {
			if(p[1]=='\r' && p[2]=='\n'){  // \r\n\r\n
				*p='\0';
				body_offset=p-buf+3;
				return true;
			}
			if(p[1]=='\n') { // \n\n
				*p='\0';
				body_offset=p-buf+2;
				return true;
			}
			p++;
		}
		return false;
	}

	void parse_headers(){
		const String header_block(buf, String::L_TAINTED);
		
		ArrayString aheaders;
		header_block.split(aheaders, 0, "\n");

		Array_iterator<const String*> i(aheaders);
		i.next(); // skipping status
		for(;i.has_next();){
			const char *line=i.next()->cstr();
			if(!headers.add_header(line))
				throw Exception("http.response", 0, "bad response from host - bad header \"%s\"", line);
		}
	}

	int read_response(int sock, bool fail_on_status_ne_200);
};

enum HTTP_response_state {
	HTTP_STATUS_CODE,
	HTTP_HEADERS,
	HTTP_BODY
};

int HTTP_response::read_response(int sock, bool fail_on_status_ne_200) {
	HTTP_response_state state=HTTP_STATUS_CODE;
	int result=0;

	size_t chunk_size=0x400*16;
	resize(2*chunk_size);

	while(read(sock, chunk_size)){
		switch(state){
			case HTTP_STATUS_CODE: {
				size_t status_size=first_line();
				if(!status_size)
					break;

				const char *status=status_code(pa_strdup(buf, status_size), result);

				if(!result || fail_on_status_ne_200 && result!=200)
					throw Exception("http.status", status ? new String(status) : &String::Empty, "invalid HTTP response status");

				state=HTTP_HEADERS;
			}

			case HTTP_HEADERS: {
				if(!body_start())
					break;

				parse_headers();

				size_t content_length=check_file_size(headers.content_length, 0);
				if(content_length>0 && (content_length + body_offset) > length){
					resize(content_length + body_offset + 0x400*64);
				}

				state=HTTP_BODY;
				break;
			}

			case HTTP_BODY: {
				chunk_size=0x400*64;
				break;
			}
		}
	}

	if(state==HTTP_STATUS_CODE)
		throw Exception("http.response", 0, "bad response from host - no status found (size=%u)", length);

	if(state==HTTP_HEADERS){
		parse_headers();
		body_offset=length;
	}

	return result;
}

/* ********************** request *************************** */

#if defined(SIGALRM) && defined(HAVE_SIGSETJMP) && defined(HAVE_SIGLONGJMP)
#	define PA_USE_ALARM
#endif

#ifdef PA_USE_ALARM
static sigjmp_buf timeout_env;
static void timeout_handler(int /*sig*/){
	siglongjmp(timeout_env, 1);
}

#define PA_NO_THREADS (HTTPD_Server::mode != HTTPD_Server::MULTITHREADED)

#define ALARM(value) if(PA_NO_THREADS) alarm(value)
#else
#define ALARM(value)
#endif

static int http_request(HTTP_response& response, const char* host, short port, const char* request, size_t request_size, int timeout_secs, bool fail_on_status_ne_200) {
	if(!host)
		throw Exception("http.host", 0, "zero hostname");  //never

	volatile int sock=-1; // to prevent makeing it register variable, because it will be clobbered by longjmp [thanks gcc warning]
		
#ifdef PA_USE_ALARM
	if(PA_NO_THREADS) signal(SIGALRM, timeout_handler);
	if(PA_NO_THREADS && sigsetjmp(timeout_env, 1)) {
		// duplicating closesocket to make code more simple for old compilers
		if(sock>=0)
			closesocket(sock);
		throw Exception("http.timeout", 0, "timeout occurred while retrieving document");
		return 0; // never
	} else
#endif
	{
		ALARM(timeout_secs);
		try {
			int result;
			struct sockaddr_in dest;
		
			if(!set_addr(&dest, host, port))
				throw Exception("http.host", 0, "can not resolve hostname \"%s\"", host); 
			
			if((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP/*0*/))<0) {
				int no=pa_socks_errno();
				throw Exception("http.connect", 0, "can not make socket: %s (%d)", pa_socks_strerr(no), no); 
			}

			// To enable SO_DONTLINGER (that is, disable SO_LINGER) 
			// l_onoff should be set to zero and setsockopt should be called
			linger dont_linger={0,0};
			setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char *)&dont_linger, sizeof(dont_linger));

#ifdef WIN32
// SO_*TIMEO can be defined in .h but not implemlemented in protocol,
// failing subsequently with Option not supported by protocol (99) message
// could not suppress that, so leaving this only for win32
			int timeout_ms=timeout_secs*1000;
			setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
#endif

			if(connect(sock, (struct sockaddr *)&dest, sizeof(dest))) {
				int no=pa_socks_errno();
				throw Exception("http.connect", 0, "can not connect to host \"%s\": %s (%d)", host, pa_socks_strerr(no), no);
			}

			if(send(sock, request, request_size, 0)!=(ssize_t)request_size) {
				int no=pa_socks_errno();
				throw Exception("http.timeout", 0, "error sending request: %s (%d)", pa_socks_strerr(no), no);
			}

			result=response.read_response(sock, fail_on_status_ne_200);
			closesocket(sock);
			ALARM(0);
			return result;
		} catch(...) {
			ALARM(0);
			if(sock>=0)
				closesocket(sock);
			rethrow;
		}
	}
}

#ifndef DOXYGEN
struct Http_pass_header_info {
	Request_charsets* charsets;
	String* request;
	bool* user_agent_specified;
	bool* content_type_specified;
	bool* content_type_url_encoded;
};
#endif

char *pa_http_safe_header_name(const char *name) {
	char *result=pa_strdup(name);
	char *n=result;
	if(!pa_isalpha((unsigned char)*n))
		*n++ = '_';
	for(; *n; ++n) {
		if (!pa_isalnum((unsigned char)*n) && *n != '-' && *n != '_')
			*n = '_';
	}
	return result;
}

static void http_pass_header(HashStringValue::key_type aname, HashStringValue::value_type avalue, Http_pass_header_info *info) {

	const char* name_cstr=aname.cstr();

	if(strcasecmp(name_cstr, HTTP_CONTENT_LENGTH)==0)
		return;

	String name=String(pa_http_safe_header_name(capitalize(name_cstr)), String::L_AS_IS);
	String value=attributed_meaning_to_string(*avalue, String::L_HTTP_HEADER, true);

	*info->request << name << ": " << value << CRLF;
	
	if(strcasecmp(name_cstr, HTTP_USER_AGENT)==0)
		*info->user_agent_specified=true;
	if(strcasecmp(name_cstr, HTTP_CONTENT_TYPE)==0){
		*info->content_type_specified=true;
		*info->content_type_url_encoded=pa_strncasecmp(value.cstr(), HTTP_CONTENT_TYPE_FORM_URLENCODED)==0;
	}
}

static void http_pass_cookie(HashStringValue::key_type name, 
				HashStringValue::value_type value, 
				Http_pass_header_info *info) {
	
	*info->request << String(name, String::L_HTTP_COOKIE) << "="
		<< attributed_meaning_to_string(*value, String::L_HTTP_COOKIE, true)
		<< "; "; 

}

static const String* basic_authorization_field(const char* user, const char* pass) {
	if(!user&& !pass)
		return 0;

	String combined;  
	if(user)
		combined<<user;
	combined<<":";
	if(pass)
		combined<<pass;
	
	String* result=new String("Basic ");
	*result<<pa_base64_encode(combined.cstr(), combined.length(), Base64Options(false /*no wrap*/));
	return result;
}

static void form_string_value2string(HashStringValue::key_type key, const String& value, String& result) {
	result << String(key, String::L_URI) << "=" << String(value, String::L_URI) << "&";
}

#ifndef DOXYGEN
struct Form_table_value2string_info {
	HashStringValue::key_type key;
	String& result;

	Form_table_value2string_info(HashStringValue::key_type akey, String& aresult): 
		key(akey), result(aresult) {}
};
#endif
static void form_table_value2string(Table::element_type row, Form_table_value2string_info* info) {
	form_string_value2string(info->key, *row->get(0), info->result);
}

static void form_value2string(HashStringValue::key_type key, HashStringValue::value_type value, String* result) {
	if(const String* svalue=value->get_string())
		form_string_value2string(key, *svalue, *result);
	else if(Table* tvalue=value->get_table()) {
		Form_table_value2string_info info(key, *result);
		tvalue->for_each(form_table_value2string, &info);
	} else
		throw Exception(PARSER_RUNTIME, new String(key, String::L_TAINTED),
			"is %s, " HTTP_FORM_NAME " option value can be string or table only (file is allowed for $." HTTP_METHOD_NAME "[POST] + $." HTTP_FORM_ENCTYPE_NAME "[" HTTP_CONTENT_TYPE_MULTIPART_FORMDATA "])", value->type());
}

const char* pa_form2string(HashStringValue& form, Request_charsets& charsets) {
	String string;
	form.for_each<String*>(form_value2string, &string);
	return string.untaint_and_transcode_cstr(String::L_URI, &charsets);
}

struct FormPart {
	Request* r;
	const char* boundary;
	String* string;
	Form_table_value2string_info* info;

	struct BinaryBlock{
		const char* ptr;
		size_t length;

		BinaryBlock(String* astring, Request* r): ptr(astring->untaint_and_transcode_cstr(String::L_AS_IS, &r->charsets)), length(strlen(ptr)){}
		BinaryBlock(const char* aptr, size_t alength): ptr(aptr), length(alength){}
	};

	Array<BinaryBlock> blocks;

	FormPart(Request* ar, const char* aboundary): r(ar), boundary(aboundary), string(new String()){}

	const char *post(size_t &length){
		if(blocks.count()){
			blocks+=BinaryBlock(string, r);

			length=0;
			for(size_t i=0; i<blocks.count(); i++)
				length+=blocks[i].length;

			char *result=(char *)pa_malloc_atomic(length);
			char *ptr=result;

			for(size_t i=0; i<blocks.count(); i++){
				memcpy(ptr, blocks[i].ptr, blocks[i].length);
				ptr+=blocks[i].length;
			}

			return result;
		} else {
			BinaryBlock result(string, r);
			length=result.length;
			return result.ptr;
		}
	}
};

static void form_part_boundary_header(FormPart& part, String::Body name, const char* file_name=0) {
	*part.string << "--" << part.boundary << CRLF CONTENT_DISPOSITION_CAPITALIZED ": form-data; name=\"" << name << "\"";
	if(file_name){
		if(strcmp(file_name, NONAME_DAT)!=0)
			*part.string << "; filename=\"" << file_name << "\"";
		*part.string << CRLF HTTP_CONTENT_TYPE_CAPITALIZED ": " << part.r->mime_type_of(file_name);
	}
	*part.string << CRLF CRLF;
}

static void form_string_value2part(HashStringValue::key_type key, const String& value, FormPart& part) {
	form_part_boundary_header(part, key);
	*part.string << value << CRLF;
}

static void form_file_value2part(HashStringValue::key_type key, VFile& vfile, FormPart& part) {
	form_part_boundary_header(part, key, vfile.fields().get(name_name)->as_string().cstr());
	part.blocks+=FormPart::BinaryBlock(part.string, part.r);
	part.blocks+=FormPart::BinaryBlock(vfile.value_ptr(), vfile.value_size());
	part.string=new String();
	*part.string << CRLF;
}

static void form_table_value2part(Table::element_type row, FormPart* part) {
	form_string_value2part(part->info->key, *row->get(0), *part);
}

static void form_value2part(HashStringValue::key_type key, HashStringValue::value_type value, FormPart& part) {
	if(const String* svalue=value->get_string())
		form_string_value2part(key, *svalue, part);
	else if(Table* tvalue=value->get_table()) {
		Form_table_value2string_info info(key, *part.string);
		part.info = &info;
		tvalue->for_each(form_table_value2part, &part);
	} else if(VFile* vfile=static_cast<VFile *>(value->as("file"))){
		form_file_value2part(key, *vfile, part);
	} else
		throw Exception(PARSER_RUNTIME, new String(key, String::L_TAINTED), "is %s, " HTTP_FORM_NAME " option value can be string, table or file only", value->type());
}

const char* pa_form2string_multipart(HashStringValue& form, Request& r, const char* boundary, size_t& post_size){
	FormPart formpart(&r, boundary);
	form.for_each<FormPart&>(form_value2part, formpart);
	*formpart.string << "--" << boundary << "--";
	// @todo: return binary blocks here to save memory in pa_internal_file_read_http
	return formpart.post(post_size);
}

// Set-Cookie: name=value; Domain=docs.foo.com; Path=/accounts; Expires=Wed, 13-Jan-2021 22:23:01 GMT; Secure; HttpOnly
static ArrayString* parse_cookie(Request& r, const String& cookie) {
	char *current=pa_strdup(cookie.cstr());
	
	const String* name=0;
	const String* value=&String::Empty;
	const String* expires=&String::Empty;
	const String* max_age=&String::Empty;
	const String* path=&String::Empty;
	const String* domain=&String::Empty;
	const String* httponly=&String::Empty;
	const String* secure=&String::Empty;

	bool first_pair=true;

	do {
		if(char *meaning=search_stop(current, ';'))
			if(char *attribute=search_stop(meaning, '=')) {
				const String* sname=new String(unescape_chars(attribute, strlen(attribute), &r.charsets.source(), true/*don't convert '"' to space*/), String::L_TAINTED);
				const String* smeaning=0;
				if(meaning)
					smeaning=new String(unescape_chars(meaning, strlen(meaning), &r.charsets.source(), true/*don't convert '"' to space*/), String::L_TAINTED);

				if(first_pair) {
					// name + value
					name=sname;
					value=smeaning;
					first_pair=false;
				} else {
					const String& slower=sname->change_case(r.charsets.source(), String::CC_LOWER);

					if(slower == "expires")
						expires=smeaning;
					else if(slower == "max-age")
						max_age=smeaning;
					else if(slower == "domain")
						domain=smeaning;
					else if(slower == "path")
						path=smeaning;
					else if(slower == "httponly")
						httponly=new String("1", String::L_CLEAN);
					else if(slower == "secure")
						secure=new String("1", String::L_CLEAN);
					else {
						// todo@ ?
					}
				}
			}
	} while(current);

	if(!name)
		return 0;

	ArrayString* result=new ArrayString(8);
	*result+=name;
	*result+=value;
	*result+=expires;
	*result+=max_age;
	*result+=domain;
	*result+=path;
	*result+=httponly;
	*result+=secure;

	return result;
}

Table* parse_cookies(Request& r, Table *cookies){
	Table& result=*new Table(new Cookies_table_template_columns);

	for(Array_iterator<Table::element_type> i(*cookies); i.has_next(); )
		if(ArrayString* row=parse_cookie(r, *i.next()->get(0)))
			result+=row;

	return &result;
}

void tables_update(HashStringValue& tables, const String::Body name, const String& value){
	Table *table;
	if(Value *valready=tables.get(name)) {
		// second+ appearence
		table=valready->get_table();
	} else {
		// first appearence
		Table::columns_type columns=new ArrayString(1);
		*columns+=new String("value");
		table=new Table(columns);
		tables.put(name, new VTable(table));
	}
	// this string becomes next row
	ArrayString& row=*new ArrayString(1);
	row+=&value;
	*table+=&row;
}

/// @todo build .cookies field. use ^file.tables.SET-COOKIES.menu{ for now
File_read_http_result pa_internal_file_read_http(Request& r, const String& file_spec, bool as_text, HashStringValue *options, bool transcode_text_result) {
	File_read_http_result result;
	char host[MAX_STRING];
	const char *idna_host;
	const char* uri; 
	short port=80;
	const char* method="GET";
	bool method_is_get=true;
	HashStringValue* form=0;
	int timeout_secs=2;
	bool fail_on_status_ne_200=true;
	bool omit_post_charset=false;
	Value* vheaders=0;
	Value* vcookies=0;
	Value* vbody=0;
	Charset* asked_remote_charset=0;
	Charset* real_remote_charset=0;
	const char* user_cstr=0;
	const char* password_cstr=0;
	const char* encode=0;
	bool multipart=false;

	if(options) {
		int valid_options=pa_get_valid_file_options_count(*options);

		if(Value* vmethod=options->get(HTTP_METHOD_NAME)) {
			valid_options++;
			method=vmethod->as_string().change_case(r.charsets.source(), String::CC_UPPER).cstr();
			method_is_get=strcmp(method, "GET")==0;
		}
		if(Value* vencode=options->get(HTTP_FORM_ENCTYPE_NAME)) {
			valid_options++;
			encode=vencode->as_string().cstr();
		}
		if(Value* vform=options->get(HTTP_FORM_NAME)) {
			valid_options++;
			form=vform->get_hash(); 
		} 
		if(vbody=options->get(HTTP_BODY_NAME)) {
			valid_options++;
		} 
		if(Value* vtimeout=options->get(HTTP_TIMEOUT_NAME)) {
			valid_options++;
			timeout_secs=vtimeout->as_int(); 
		} 
		if(vheaders=options->get(HTTP_HEADERS_NAME)) {
			valid_options++;
		} 
		if(vcookies=options->get(HTTP_COOKIES_NAME)) {
			valid_options++;
		} 
		if(Value* vany_status=options->get(HTTP_ANY_STATUS_NAME)) {
			valid_options++;
			fail_on_status_ne_200=!vany_status->as_bool(); 
		}
		if(Value* vomit_post_charset=options->get(HTTP_OMIT_POST_CHARSET_NAME)){
			valid_options++;
			omit_post_charset=vomit_post_charset->as_bool();
		}
		if(Value* vcharset_name=options->get(PA_CHARSET_NAME)) {
			asked_remote_charset=&pa_charsets.get(vcharset_name->as_string());
		} 
		if(Value* vresponse_charset_name=options->get(PA_RESPONSE_CHARSET_NAME)) {
			valid_options++;
			real_remote_charset=&pa_charsets.get(vresponse_charset_name->as_string());
		} 
		if(Value* vuser=options->get(HTTP_USER)) {
			valid_options++;
			user_cstr=vuser->as_string().cstr();
		} 
		if(Value* vpassword=options->get(HTTP_PASSWORD)) {
			valid_options++;
			password_cstr=vpassword->as_string().cstr();
		}

		if(valid_options!=options->count())
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
	}
	if(!asked_remote_charset) // defaulting to $request:charset
		asked_remote_charset=&(r.charsets).source();

	if(encode){
		if(method_is_get)
			throw Exception(PARSER_RUNTIME, 0, "you can not use $." HTTP_FORM_ENCTYPE_NAME " option with method GET");

		multipart=strcasecmp(encode, HTTP_CONTENT_TYPE_MULTIPART_FORMDATA)==0;

		if(!multipart && strcasecmp(encode, HTTP_CONTENT_TYPE_FORM_URLENCODED)!=0)
			throw Exception(PARSER_RUNTIME, 0, "$." HTTP_FORM_ENCTYPE_NAME " option value can be " HTTP_CONTENT_TYPE_FORM_URLENCODED " or " HTTP_CONTENT_TYPE_MULTIPART_FORMDATA " only");
	}

	if(vbody){
		if(method_is_get)
			throw Exception(PARSER_RUNTIME, 0, "you can not use $." HTTP_BODY_NAME " option with method GET");

		if(form)
			throw Exception(PARSER_RUNTIME, 0, "you can not use options $." HTTP_BODY_NAME " and $." HTTP_FORM_NAME " together");
	}

	//preparing request
	String& connect_string=*new String(file_spec);

	const char* request;
	size_t request_size;
	{
		// influence URLencoding of tainted pieces to String::L_URI lang
		Temp_client_charset temp(r.charsets, *asked_remote_charset);

		const char* connect_string_cstr=connect_string.untaint_and_transcode_cstr(String::L_URI, &(r.charsets));

		const char* current=connect_string_cstr;
		if(strncmp(current, "http://", 7)!=0)
			throw Exception(PARSER_RUNTIME, &connect_string, "does not start with http://"); //never
		current+=7;

		strncpy(host, current, sizeof(host)-1);  host[sizeof(host)-1]=0;
		char* host_uri=lsplit(host, '/');
		uri=host_uri?current+(host_uri-1-host):"/";
		char* port_cstr=lsplit(host, ':');
		
		if (port_cstr){
			char* error_pos=0;
			port=(short)strtol(port_cstr, &error_pos, 10);
			if(port==0 || *error_pos)
				throw Exception(PARSER_RUNTIME, &connect_string, "invalid port number '%s'", port_cstr);
		}

		idna_host=pa_idna_encode(host, r.charsets.source());

		// making request head
		String head;
		head << method << " " << uri;
		if(method_is_get && form)
			head << (strchr(uri, '?')!=0?"&":"?") << pa_form2string(*form, r.charsets);

		head <<" HTTP/1.0" CRLF "Host: "<< idna_host;
		if (port != 80)
			head << ":" << port_cstr;
		head << CRLF;

		char* boundary= multipart ? get_uuid_boundary() : 0;

		String user_headers;
		bool user_agent_specified=false;
		bool content_type_specified=false;
		bool content_type_url_encoded=false;
		if(vheaders && !vheaders->is_string()) { // allow empty
			if(HashStringValue *headers=vheaders->get_hash()) {
				Http_pass_header_info info={
					&(r.charsets),
					&user_headers,
					&user_agent_specified,
					&content_type_specified,
					&content_type_url_encoded};
				headers->for_each<Http_pass_header_info*>(http_pass_header, &info); 
			} else
				throw Exception(PARSER_RUNTIME, 0, "headers param must be hash"); 
		};

		const char* request_body=0;
		size_t post_size=0;
		if(form && !method_is_get) {
			head << "Content-Type: " << (multipart ? HTTP_CONTENT_TYPE_MULTIPART_FORMDATA : HTTP_CONTENT_TYPE_FORM_URLENCODED);

			if(!omit_post_charset)
				head << "; charset=" << asked_remote_charset->NAME_CSTR();

			if(multipart) {
				head << "; boundary=" << boundary;
				request_body=pa_form2string_multipart(*form, r/*charsets & mime_type needed*/, boundary, post_size/*correct post_size returned here*/);
			} else {
				request_body=pa_form2string(*form, r.charsets);
				post_size=strlen(request_body);
			}
			head << CRLF;
		} else if(vbody) {
			// $.body was specified
			if(content_type_url_encoded){
				// transcode + url-encode
				request_body=vbody->as_string().untaint_and_transcode_cstr(String::L_URI, &(r.charsets));
			} else {
				// content-type != application/x-www-form-urlencoded -> transcode only, don't url-encode!
				const String &sbody=vbody->as_string();
				request_body=Charset::transcode(String::C(sbody.cstr(), sbody.length()), r.charsets.source(), *asked_remote_charset).str;
			}
			post_size=strlen(request_body);
		}

		// http://www.ietf.org/rfc/rfc2617.txt
		if(const String* authorization_field_value=basic_authorization_field(user_cstr, password_cstr))
			head << "Authorization: " << *authorization_field_value << CRLF;

		head << user_headers;

		if(!user_agent_specified) // defaulting
			head << "User-Agent: " DEFAULT_USER_AGENT CRLF;

		if(form && !method_is_get && content_type_specified) // POST + form + content-type was specified
			throw Exception(PARSER_RUNTIME, 0, "$.content-type can't be specified with method POST"); 

		if(vcookies && !vcookies->is_string()){ // allow empty
			if(HashStringValue* cookies=vcookies->get_hash()) {
				head << "Cookie: ";
				Http_pass_header_info info={&(r.charsets), &head, 0, 0, 0};
				cookies->for_each<Http_pass_header_info*>(http_pass_cookie, &info); 
				head << CRLF;
			} else
				throw Exception(PARSER_RUNTIME, 0, "cookies param must be hash");
		}

		if(request_body)
			head << "Content-Length: " << format(post_size, "%u") << CRLF;
		
		head << CRLF;
		
		const char *request_head=head.untaint_and_transcode_cstr(String::L_URI, &(r.charsets));

		if(request_body){
			size_t head_size = strlen(request_head);
			request_size=post_size + head_size;
			char *ptr=(char *)pa_malloc_atomic(request_size);
			memcpy(ptr, request_head, head_size);
			memcpy(ptr+head_size, request_body, post_size);
			request=ptr;
		} else {
			request_size=strlen(request_head);
			request=request_head;
		}
	}
	

	HTTP_response response;

	// sending request
	int status_code;
	ALTER_EXCEPTION_SOURCE(status_code=http_request(response, idna_host, port, request, request_size, timeout_secs, fail_on_status_ne_200), &connect_string);

	// processing results
	char* raw_body=response.buf + response.body_offset;
	size_t raw_body_size=response.length - response.body_offset;

	result.headers=new HashStringValue;
	VHash* vtables=new VHash;
	result.headers->put("tables", vtables);

	if (!real_remote_charset && !response.headers.content_type.is_empty())
		real_remote_charset=detect_charset(response.headers.content_type.cstr());

	if(as_text)
		real_remote_charset=pa_charsets.checkBOM(raw_body, raw_body_size, real_remote_charset);

	if (!real_remote_charset)
		real_remote_charset=asked_remote_charset; // never null

	for(Array_iterator<HTTP_Headers::Header> i(response.headers.headers); i.has_next(); ){
		HTTP_Headers::Header header=i.next();

		header.transcode(*real_remote_charset, r.charsets.source());

		String &header_value=*new String(header.value, String::L_TAINTED);

		tables_update(vtables->hash(), header.name, header_value);
		result.headers->put(header.name, new VString(header_value));
	}

	// filling $.cookies
	if(vcookies=vtables->hash().get("SET-COOKIE"))
		result.headers->put(HTTP_COOKIES_NAME, new VTable(parse_cookies(r, vcookies->get_table())));

	// output response
	String::C real_body=String::C(raw_body, raw_body_size);

	if(as_text && transcode_text_result && raw_body_size) { // raw_body_size must be checked because transcode returns CONST string in case length==0, which contradicts hacking few lines below
		real_body=Charset::transcode(real_body, *real_remote_charset, r.charsets.source());
	}

	result.str=const_cast<char *>(real_body.str); // hacking a little
	result.length=real_body.length;

	if(as_text && result.length)
		fix_line_breaks(result.str, result.length);

	result.headers->put(file_status_name, new VInt(status_code));

	return result;
}

/* ********************** httpd *************************** */

#ifdef HTTPD_DEBUG
void pa_log(const char* fmt, ...);
#define LOG(action) action
#else
#define LOG(action)
#endif

enum EscapeState {
        Initial,
        Default,
        EscapeFirst,
        EscapeSecond
};

static bool check_uri(const char *uri){
	EscapeState state=Initial;
	uint escapedValue;

	const char *pattern="/../";
	const char *pos=pattern;

	while(*uri){
		uchar c=(uchar)*(uri++);
		switch(state) {
			case Initial:
				if(c!='/')
					return false;
				state=Default;
				break;
			case Default:
				if(c=='%'){
					state=EscapeFirst;
					continue;
				}
				if(c=='?')
					return true;
				break;
			case EscapeFirst:
				if(isxdigit(c)){
					state=EscapeSecond;
					escapedValue=hex_value[c] << 4;
					continue;
				}
				return false;
			case EscapeSecond:
				if(isxdigit(c)){
					state=Default;
					c=(uchar)(escapedValue + hex_value[c]);

					// implementing Apache AllowEncodedSlashes Off just in case
					if(c=='/' || c=='\\')
						return false;

					break;
				}
				return false;
		}

		if(c==*pos || c=='\\' && *pos=='/'){
			if(!*(++pos))
				return false;
		} else {
			pos=pattern;
		}
	}
	return true;
}

class HTTPD_request : public HTTP_response {
public:
	const char *method;
	const char *uri;

	HTTPD_request() : HTTP_response(), method(NULL), uri(NULL){};

	ssize_t pa_recv(int sockfd, char *buf, size_t len);

	bool read(int sock, size_t size){
		if(length + size > buf_size)
			resize(buf_size * 2 + size);
		ssize_t received_size=pa_recv(sock, buf + length, size);
		if(received_size == 0)
			return false;
		if(received_size < 0) {
			if(int no = pa_socks_errno())
				throw Exception("httpd.read", 0, "error receiving request: %s (%d)", pa_socks_strerr(no), no);
			return false;
		}
		length+=received_size;
		buf[length]='\0';
		return true;
	}

	const char *extract_method(char *method_line){
		char* uri_start = strchr(method_line, ' ');

		if(!uri_start || uri_start == method_line)
			return NULL;

		char* uri_end=strchr(uri_start+1, ' ');

		if(!uri_end || uri_end == uri_start+1)
			return NULL;

		uri=pa_strdup(uri_start+1, uri_end-uri_start-1);
		if(!check_uri(uri))
			throw Exception("httpd.request", 0, "invalid uri '%s'", uri);

		return str_upper(method_line, uri_start-method_line);
	}


	bool read_header(int);
	size_t read_post(int, char *, size_t);
};

enum HTTPD_request_state {
	HTTPD_METHOD,
	HTTPD_HEADERS
};

ssize_t HTTPD_request::pa_recv(int sockfd, char *buffer, size_t len){
	LOG(pa_log("httpd [%d] recv %d appending to %d ...", sockfd, len, length));

#ifdef PA_USE_ALARM
	if(PA_NO_THREADS) signal(SIGALRM, timeout_handler);
	if(PA_NO_THREADS && sigsetjmp(timeout_env, 1)) {
		LOG(pa_log("httpd [%d] recv got %d sec timeout", sockfd, pa_httpd_timeout));
		if(length) // timeout on "void" connection is normal
			throw Exception("httpd.timeout", 0, "timeout occurred while receiving request");
		return 0;
	} else
#endif
	{
		ALARM(pa_httpd_timeout);
		ssize_t result=recv(sockfd, buffer, len, 0);
		ALARM(0);
		LOG(pa_log("httpd [%d] recv got %d bytes", sockfd, result));
		LOG(pa_log("httpd [%d] %s", sockfd, buffer));
		return result;
	}
}

static bool valid_http_method(const char * method){
	return method && (
		!strcmp(method, "GET") ||
		!strcmp(method, "HEAD") ||
		!strcmp(method, "POST") ||
		!strcmp(method, "PUT") ||
		!strcmp(method, "DELETE") ||
		!strcmp(method, "CONNECT") ||
		!strcmp(method, "OPTIONS") ||
		!strcmp(method, "TRACE") ||
		!strcmp(method, "PATCH")
	);
}

bool HTTPD_request::read_header(int sock) {
	enum HTTPD_request_state state = HTTPD_METHOD;

	size_t chunk_size = 0x400*4;
	resize(chunk_size);

	while(read(sock, chunk_size)){
		switch(state){
			case HTTPD_METHOD: {
				size_t method_size = first_line();
				if(!method_size)
					break;

				char *method_line = pa_strdup(buf, method_size);
				method = extract_method(method_line);

				if(!valid_http_method(method))
					throw Exception("httpd.method", new String(method ? method : method_line), "invalid request method");
				state = HTTPD_HEADERS;
			}

			case HTTPD_HEADERS: {
				if(!body_start())
					break;

				parse_headers();
				return true;
			}
		}
	}

	if(!length){ // browsers open connections in advance and they will be empty unless user requests more pages
		LOG(pa_log("httpd [%d] void request", sock));
		return false;
	}

	if(state == HTTPD_METHOD)
		throw Exception("httpd.request", 0, "bad request from host - no method found (size=%u)", length);

	if(state == HTTPD_HEADERS){
		parse_headers();
		body_offset=length;
	}

	return true;
}

size_t HTTPD_request::read_post(int sock, char *body, size_t max_bytes) {
	size_t total_read = min(length - body_offset, max_bytes);
	memcpy(body, buf + body_offset, total_read);

	while (total_read < max_bytes){
		ssize_t received_size = pa_recv(sock, body + total_read, max_bytes - total_read);
		if(received_size == 0)
			return total_read;
		if(received_size < 0) {
			if(int no = pa_socks_errno())
				throw Exception("httpd.read", new String(uri), "error receiving request body: %s (%d)", pa_socks_strerr(no), no);
			return total_read;
		}
		total_read += received_size;
	}
	return total_read;
}

/* ********************************************************** */

Array<HTTP_Headers::Header> &HTTPD_Connection::headers() {
	return request->headers.headers;
}

const char *HTTPD_Connection::method() {
	return request->method;
}

const char *HTTPD_Connection::uri() {
	return request->uri;
}

const char *HTTPD_Connection::content_type() {
	return request->headers.content_type.cstr();
}

uint64_t HTTPD_Connection::content_length(){
	return request->headers.content_length;
}

bool HTTPD_Connection::read_header(){
	request = new HTTPD_request();
	bool result = request->read_header(sock);
	LOG(if(result){
		pa_log("httpd [%d] got %s \"%s\"", sock, method(), uri());
	})
	return result;
}

size_t HTTPD_Connection::read_post(char *body, size_t max_bytes) {
	return request->read_post(sock, body, max_bytes);
}

size_t HTTPD_Connection::send_body(const void *buf, size_t size) {
	LOG(pa_log("httpd [%d] response %d bytes", sock, size));
	LOG(pa_log("httpd [%d] %s", sock, buf));
	if(send(sock, (const char*)buf, size, 0) != (ssize_t)size) {
		int no=pa_socks_errno();
		throw Exception("httpd.write", 0, "error sending response: %s (%d)", pa_socks_strerr(no), no);
	}
	return size;
}

HTTPD_Connection::~HTTPD_Connection(){
	if(sock != -1){
		LOG(pa_log("httpd [%d] closed", sock));
		closesocket(sock);
	}
}

static int sock_ready(int fd,int operation,int timeout_value){
	struct timeval timeout = {0, timeout_value * 1000};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	switch (operation){
		case 0: return select(fd + 1, &fds, NULL, NULL, &timeout)>0;  /* read */
		case 1: return select(fd + 1, NULL, &fds, NULL, &timeout)>0;  /* write */
		default: return select(fd + 1, &fds, &fds, NULL, &timeout)>0;  /* both */
	}
}

bool HTTPD_Connection::accept(int server_sock, int timeout_value) {
	int ready = sock_ready(server_sock, 0, timeout_value);
	if (ready < 0) {
		int no=pa_socks_errno();
		if(no == EINTR)
			return false;
		throw Exception("httpd.accept", 0, "error waiting for connection: %s (%d)", pa_socks_strerr(no), no);
	}
	if (ready == 0)
		return false; /* Timeout */

	struct sockaddr_in addr;
	socklen_t sock_addr_len = sizeof(struct sockaddr_in);
	memset(&addr, 0, sock_addr_len);

	sock = ::accept(server_sock, (struct sockaddr *)&addr, &sock_addr_len);
	if(server_sock == -1){
		int no=pa_socks_errno();
		throw Exception("httpd.accept", 0, "error accepting connection: %s (%d)", pa_socks_strerr(no), no);
	}

	remote_addr = pa_strdup(inet_ntoa(addr.sin_addr));
	LOG(pa_log("httpd [%d] accepted from %s", sock, remote_addr));
	return true;
}

HTTPD_Server::HTTPD_MODE HTTPD_Server::mode = HTTPD_Server::SEQUENTIAL;
const char *HTTPD_Server::port=NULL;

void HTTPD_Server::set_mode(const String &value){
                if(value == "sequental") mode = SEQUENTIAL;
#ifdef HAVE_TLS
                else if (value == "threaded") mode = MULTITHREADED;
#endif
#ifdef _MSC_VER
		else throw Exception("httpd.mode", &value, "$MAIN:HTTPD.mode must be 'sequental' or 'threaded'");
#else
                else if (value == "parallel") mode = PARALLEL;
		else throw Exception("httpd.mode", &value, "$MAIN:HTTPD.mode must be 'sequental', 'parallel' or 'threaded'");
#endif
}

int HTTPD_Server::bind(const char *host_port){
	struct sockaddr_in me;

	port = strchr(host_port, ':');
	const char *host = NULL;
	if(port){
		if(port > host_port)
			host = pa_strdup(host_port, port - host_port);
		port += 1;
	} else {
		port = host_port;
	}

	if(!set_addr(&me, host, (short)pa_atoui(port))){
		if (host)
			throw Exception("httpd.bind", 0, "can not resolve hostname \"%s\"", host);
		me.sin_addr.s_addr=INADDR_ANY;
	}

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP/*0*/);

	if(sock < 0){
		int no=pa_socks_errno();
		throw Exception("httpd.bind", 0, "can not make socket: %s (%d)", pa_socks_strerr(no), no);
	}

	static int sock_on = 1;

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&sock_on, sizeof(sock_on)) ||
	    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&sock_on, sizeof(sock_on)) ||
	    ::bind(sock, (struct sockaddr*)&me, sizeof(me)) ||
	    listen(sock, 16)) {
		closesocket(sock);
		int no = pa_socks_errno();
		throw Exception("httpd.bind", 0, "can not bind socket: %s (%d)", pa_socks_strerr(no), no);
	}
	return sock;
}
