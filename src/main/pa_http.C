/** @file
	Parser: http support functions.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
 */

static const char * const IDENT_HTTP_C="$Date: 2010/10/16 22:24:20 $"; 

#include "pa_http.h"
#include "pa_common.h"
#include "pa_charsets.h"
#include "pa_request_charsets.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_random.h"

// defines

#define HTTP_METHOD_NAME	"method"
#define HTTP_FORM_NAME	"form"
#define HTTP_BODY_NAME	"body"
#define HTTP_TIMEOUT_NAME	"timeout"
#define HTTP_HEADERS_NAME	"headers"
#define HTTP_COOKIES_NAME	"cookies"
#define HTTP_FORM_ENCTYPE_NAME	"enctype"
#define HTTP_ANY_STATUS_NAME	"any-status"
#define HTTP_OMIT_POST_CHARSET_NAME	"omit-post-charset"	// ^file::load[...;http://...;$.form[...]$.method[post]]
													// by default add charset to content-type

#define HTTP_TABLES_NAME "tables"

#define HTTP_USER "user"
#define HTTP_PASSWORD "password"

#define DEFAULT_USER_AGENT "parser3"

#	ifndef INADDR_NONE
#		define INADDR_NONE ((ulong) -1)
#	endif

#undef CRLF
#define CRLF "\r\n"

static bool set_addr(struct sockaddr_in *addr, const char* host, const short port){
	memset(addr, 0, sizeof(*addr)); 
	addr->sin_family=AF_INET;
	addr->sin_port=htons(port); 
	if(host) {
		ulong packed_ip=inet_addr(host);
		if(packed_ip!=INADDR_NONE)
			memcpy(&addr->sin_addr, &packed_ip, sizeof(packed_ip)); 
		else {
			struct hostent *hostIP=gethostbyname(host);
			if(hostIP) 
				memcpy(&addr->sin_addr, hostIP->h_addr, hostIP->h_length); 
			else
				return false;
		} 
	} else 
		addr->sin_addr.s_addr=INADDR_ANY;
	return true;
}

size_t guess_content_length(char* buf) {
	char* ptr;
	if((ptr=strstr(buf, "Content-Length:"))) // Apache
		goto found;
	if((ptr=strstr(buf, "content-length:"))) // Parser 3 before 3.4.0
		goto found;
	if((ptr=strstr(buf, "Content-length:"))) // maybe 1
		goto found;
	if((ptr=strstr(buf, "CONTENT-LENGTH:"))) // maybe 2
		goto found;
	return 0;
found:
	char *error_pos;
	size_t result=(size_t)strtol(ptr+15/*strlen("Content-Length:")*/, &error_pos, 0);
	
	const size_t reasonable_initial_max=0x400*0x400*10 /*10M*/;
	if(result>reasonable_initial_max) // sanity check
		return reasonable_initial_max;
	return 0;//result;
}

static int http_read_response(char*& response, size_t& response_size, int sock, bool fail_on_status_ne_200) {
	int result=0;
	// fetching some to local buffer, guessing on possible Content-Length
	response_size=0x400*20; // initial size if Content-Length could not be determined	
	const size_t preview_size=0x400*20;
	char preview_buf[preview_size+1/*terminator*/];  // 20K buffer to preview headers
	ssize_t received_size=recv(sock, preview_buf, preview_size, 0); 
	if(received_size==0)
		goto done;
	if(received_size<0) {
		if(int no=pa_socks_errno())
			throw Exception("http.timeout", 
				0, 
				"error receiving response header: %s (%d)", pa_socks_strerr(no), no); 
		goto done;
	}
	// terminator [helps futher string searches]
	preview_buf[received_size]=0; 
	// checking status
	if(char* EOLat=strstr(preview_buf, "\n")) { 
		const String status_line(pa_strdup(preview_buf, EOLat-preview_buf));
		ArrayString astatus; 
		size_t pos_after=0;
		status_line.split(astatus, pos_after, " "); 
		const String& status_code=*astatus.get(astatus.count()>1?1:0);
		result=status_code.as_int(); 

		if(fail_on_status_ne_200 && result!=200)
			throw Exception("http.status",
				&status_code,
				"invalid HTTP response status");
	}
	// detecting response_size
	{
		if(size_t content_length=guess_content_length(preview_buf))
			response_size=preview_size+content_length; // a little more than needed, will adjust response_size by actual received size later
	}

	// [gcc is happier this way, see goto above]
	{
		// allocating initial buf
		response=(char*)pa_malloc_atomic(response_size+1/*terminator*/); // just setting memory block type
		char* ptr=response;
		size_t todo_size=response_size;
		// coping part of already received body
		memcpy(ptr, preview_buf, received_size);
		ptr+=received_size;
		todo_size-=received_size;		

		// we use terminator byte for two purposes here:
		// 1. we return there zero always, not knowing: maybe they would want to create String form $file.body?
		//     invariant: all Strings should have zero-terminated buffers
		// 2. we use that out-of-size byte to detect if our Content-Length guess was wrong
		//    when recv gets more than we expected
		//    a) we know that the Content-Length guess was wrong
		//    b) we have space to put the first byte of extra data
		//    c) we use less code to detect normal situation: on last while-cycle recv expected to just return 0
		while(true) {
			received_size=recv(sock, ptr, todo_size+1/*there is always a place for terminator*/, 0); 
			if(received_size==0) {
				response_size-=todo_size; // in case we received less than expected, cut down the reported size
				break;
			}
			if(received_size<0) {
				if(int no=pa_socks_errno())
					throw Exception("http.timeout", 
						0, 
						"error receiving response body: %s (%d)", pa_socks_strerr(no), no); 
				break;
			}
			// they've touched the terminator?
			if((size_t)received_size>todo_size)
			{
				// that means that our guessed response_size was not big enough
				const size_t grow_chunk_size=0x400*0x400; // 1M
				response_size+=grow_chunk_size;
				size_t ptr_offset=ptr-response;
				response=(char*)pa_realloc(response, response_size+1/*terminator*/);
				ptr=response+ptr_offset;
				todo_size+=grow_chunk_size;
			}
			// can't do this before realloc: we need <todo_size check
			ptr+=received_size;
			todo_size-=received_size;
		}
	}
done:
	if(result)
	{
		response[response_size]=0;
		return result;
	}
	else
		throw Exception("http.response",
			0,
			"bad response from host - no status found (size=%u)", response_size); 
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
#endif

static int http_request(char*& response, size_t& response_size,
			const char* host, short port, 
			const char* request, size_t request_size,
			int timeout_secs,
			bool fail_on_status_ne_200) {
	if(!host)
		throw Exception("http.host", 
			0, 
			"zero hostname");  //never

	volatile // to prevent makeing it register variable, because it will be clobbered by longjmp [thanks gcc warning]
		int sock=-1;
#ifdef PA_USE_ALARM
	signal(SIGALRM, timeout_handler); 
#endif
#ifdef PA_USE_ALARM
	if(sigsetjmp(timeout_env, 1)) {
		// stupid gcc [2.95.4] generated bad code
		// which failed to handle sigsetjmp+throw: crashed inside of pre-throw code.
		// rewritten simplier [athough duplicating closesocket code]
		if(sock>=0) 
			closesocket(sock); 
		throw Exception("http.timeout", 
			0, 
			"timeout occured while retrieving document"); 
		return 0; // never
	} else {
		alarm(timeout_secs); 
#endif
		try {
			int result;
			struct sockaddr_in dest;
		
			if(!set_addr(&dest, host, port))
				throw Exception("http.host", 
					0, 
					"can not resolve hostname \"%s\"", host); 
			
			if((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP/*0*/))<0) {
				int no=pa_socks_errno();
				throw Exception("http.connect", 
					0, 
					"can not make socket: %s (%d)", pa_socks_strerr(no), no); 
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
				throw Exception("http.connect", 
					0, 
					"can not connect to host \"%s\": %s (%d)", host, pa_socks_strerr(no), no); 
			}

			if(send(sock, request, request_size, 0)!=(ssize_t)request_size) {
				int no=pa_socks_errno();
				throw Exception("http.timeout", 
					0, 
					"error sending request: %s (%d)", pa_socks_strerr(no), no); 
			}

			result=http_read_response(response, response_size, sock, fail_on_status_ne_200); 
			closesocket(sock); 
#ifdef PA_USE_ALARM
			alarm(0); 
#endif
			return result;
		} catch(...) {
#ifdef PA_USE_ALARM
			alarm(0); 
#endif
			if(sock>=0) 
				closesocket(sock); 
			rethrow;
		}
#ifdef PA_USE_ALARM
	}
#endif
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
static void http_pass_header(HashStringValue::key_type aname, 
				HashStringValue::value_type avalue, 
				Http_pass_header_info *info) {

	const char* name_cstr=aname.cstr();

	if(strcasecmp(name_cstr, HTTP_CONTENT_LENGTH)==0)
		return;

	String name=String(capitalize(name_cstr), String::L_URI);
	String value=attributed_meaning_to_string(*avalue, String::L_URI, true);

	*info->request << name << ": " << value << CRLF;
	
	if(strcasecmp(name_cstr, HTTP_USER_AGENT)==0)
		*info->user_agent_specified=true;
	if(strcasecmp(name_cstr, HTTP_CONTENT_TYPE)==0){
		*info->content_type_specified=true;
		*info->content_type_url_encoded=StrStartFromNC(value.cstr(), HTTP_CONTENT_TYPE_FORM_URLENCODED);
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
	*result<<pa_base64_encode(combined.cstr(), combined.length());
	return result;
}

static void form_string_value2string(
					HashStringValue::key_type key, 
					const String& value, 
					String& result) 
{
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
static void form_value2string(
					HashStringValue::key_type key, 
					HashStringValue::value_type value, 
					String* result) 
{
	if(const String* svalue=value->get_string())
		form_string_value2string(key, *svalue, *result);
	else if(Table* tvalue=value->get_table()) {
		Form_table_value2string_info info(key, *result);
		tvalue->for_each(form_table_value2string, &info);
	} else
		throw Exception(PARSER_RUNTIME,
			new String(key, String::L_TAINTED),
			"is %s, "HTTP_FORM_NAME" option value can be string or table only (file is allowed for $."HTTP_METHOD_NAME"[POST] + $."HTTP_FORM_ENCTYPE_NAME"["HTTP_CONTENT_TYPE_MULTIPART_FORMDATA"])", value->type());
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

static void form_part_boundary_header(FormPart& part, String::Body name, const char* file_name=0){
	*part.string << "--" << part.boundary
				<< CRLF CONTENT_DISPOSITION_CAPITALIZED ": form-data; name=\"" 
				<< name
				<< "\"";
	if(file_name){
		if(strcmp(file_name, NONAME_DAT)!=0)
			*part.string << "; filename=\"" << file_name << "\"";
		*part.string << CRLF HTTP_CONTENT_TYPE_CAPITALIZED ": " << part.r->mime_type_of(file_name);
	}
	*part.string << CRLF CRLF;
}

static void form_string_value2part(
				HashStringValue::key_type key,
				const String& value,
				FormPart& part)
{
	form_part_boundary_header(part, key);
	*part.string << value << CRLF;
}

static void form_file_value2part(
				HashStringValue::key_type key,
				VFile& vfile,  
				FormPart& part)
{
	form_part_boundary_header(part, key, vfile.fields().get(name_name)->as_string().cstr());
	part.blocks+=FormPart::BinaryBlock(part.string, part.r);
	part.blocks+=FormPart::BinaryBlock(vfile.value_ptr(), vfile.value_size());
	part.string=new String();
	*part.string << CRLF;
}

static void form_table_value2part(Table::element_type row, FormPart* part) {
	form_string_value2part(part->info->key, *row->get(0), *part);
}

static void form_value2part(
				HashStringValue::key_type key,
				HashStringValue::value_type value,
				FormPart& part)
{
	if(const String* svalue=value->get_string())
		form_string_value2part(key, *svalue, part);
	else if(Table* tvalue=value->get_table()) {
		Form_table_value2string_info info(key, *part.string);
		part.info = &info;
		tvalue->for_each(form_table_value2part, &part);
	} else if(VFile* vfile=static_cast<VFile *>(value->as("file"))){
		form_file_value2part(key, *vfile, part);
	} else
		throw Exception(PARSER_RUNTIME,
			new String(key, String::L_TAINTED),
			"is %s, "HTTP_FORM_NAME" option value can be string, table or file only", value->type());
}

const char* pa_form2string_multipart(HashStringValue& form, Request& r, const char* boundary, size_t& post_size){
	FormPart formpart(&r, boundary);
	form.for_each<FormPart&>(form_value2part, formpart);
	*formpart.string << "--" << boundary << "--";
	// @todo: return binary blocks here to save memory in pa_internal_file_read_http
	return formpart.post(post_size);
}

static void find_headers_end(char* p,
		char*& headers_end_at,
		char*& raw_body)
{
	raw_body=p;
	// \n\n
	// \r\n\r\n
	while((p=strchr(p, '\n'))) {
		headers_end_at=++p; // \n>.<
		if(*p=='\r')  // \r\n>\r?<\n
			p++;
		if(*p=='\n') { // \r\n\r>\n?<
			raw_body=p+1;
			return;			
		}
	}
	headers_end_at=0;
}

/// @todo build .cookies field. use ^file.tables.SET-COOKIES.menu{ for now
File_read_http_result pa_internal_file_read_http(Request& r,
						const String& file_spec,
						bool as_text,
						HashStringValue *options,
						bool transcode_text_result) {
	File_read_http_result result;
	char host[MAX_STRING];
	const char* uri; 
	short port;
	const char* method="GET";
	bool method_is_get=true;
	HashStringValue* form=0;
	int timeout_secs=2;
	bool fail_on_status_ne_200=true;
	bool omit_post_charset=false;
	Value* vheaders=0;
	Value* vcookies=0;
	Value* vbody=0;
	Charset *asked_remote_charset=0;
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
			asked_remote_charset=&charsets.get(vcharset_name->as_string().
				change_case(r.charsets.source(), String::CC_UPPER));
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
			throw Exception(PARSER_RUNTIME,
				0,
				"you can not use $."HTTP_FORM_ENCTYPE_NAME" option with method GET");

		multipart=strcasecmp(encode, HTTP_CONTENT_TYPE_MULTIPART_FORMDATA)==0;

		if(!multipart && strcasecmp(encode, HTTP_CONTENT_TYPE_FORM_URLENCODED)!=0)
			throw Exception(PARSER_RUNTIME,
				0,
				"$."HTTP_FORM_ENCTYPE_NAME" option value can be "HTTP_CONTENT_TYPE_FORM_URLENCODED" or "HTTP_CONTENT_TYPE_MULTIPART_FORMDATA" only");
	}

	if(vbody){
		if(method_is_get)
			throw Exception(PARSER_RUNTIME,
				0,
				"you can not use $."HTTP_BODY_NAME" option with method GET");

		if(form)
			throw Exception(PARSER_RUNTIME,
				0,
				"you can not use options $."HTTP_BODY_NAME" and $."HTTP_FORM_NAME" together");
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
			throw Exception(PARSER_RUNTIME, 
				&connect_string, 
				"does not start with http://"); //never
		current+=7;

		strncpy(host, current, sizeof(host)-1);  host[sizeof(host)-1]=0;
		char* host_uri=lsplit(host, '/');
		uri=host_uri?current+(host_uri-1-host):"/";
		char* port_cstr=lsplit(host, ':');
		char* error_pos=0;
		port=port_cstr?(short)strtol(port_cstr, &error_pos, 0):80;

		// making request head
		String head;
		head << method << " " << uri;
		if(method_is_get && form)
			head << (strchr(uri, '?')!=0?"&":"?") << pa_form2string(*form, r.charsets);

		head <<" HTTP/1.0" CRLF "Host: "<< host << CRLF;

		char* boundary=0;

		if(multipart){
			uuid uuid=get_uuid();
			const int boundary_bufsize=10+32+1/*for zero-teminator*/+1/*for faulty snprintfs*/;
			boundary=new(PointerFreeGC) char[boundary_bufsize];
			snprintf(boundary, boundary_bufsize,
				"----------%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
				uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
				uuid.clock_seq >> 8, uuid.clock_seq & 0xFF,
				uuid.node[0], uuid.node[1], uuid.node[2],
				uuid.node[3], uuid.node[4], uuid.node[5]);
		}

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
				throw Exception(PARSER_RUNTIME, 
					0,
					"headers param must be hash"); 
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
				request_body=Charset::transcode(
					String::C(vbody->as_string().cstr(), vbody->as_string().length()),
					r.charsets.source(),
					*asked_remote_charset
				);
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
			throw Exception(PARSER_RUNTIME,
				0,
				"$.content-type can't be specified with method POST"); 

		if(vcookies && !vcookies->is_string()){ // allow empty
			if(HashStringValue* cookies=vcookies->get_hash()) {
				head << "Cookie: ";
				Http_pass_header_info info={&(r.charsets), &head, 0, 0, 0};
				cookies->for_each<Http_pass_header_info*>(http_pass_cookie, &info); 
				head << CRLF;
			} else
				throw Exception(PARSER_RUNTIME, 
					0,
					"cookies param must be hash");
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
	
	char* response;
	size_t response_size;

	// sending request
	int status_code=http_request(response, response_size,
		host, port, request, request_size,
		timeout_secs, fail_on_status_ne_200); 
	
	// processing results	
	char* raw_body; size_t raw_body_size;
	char* headers_end_at;
	find_headers_end(response, 
		headers_end_at,
		raw_body);
	raw_body_size=response_size-(raw_body-response);
	
	result.headers=new HashStringValue;
	VHash* vtables=new VHash;
	result.headers->put(HTTP_TABLES_NAME, vtables);
	Charset* real_remote_charset=0; // undetected, yet

	if(headers_end_at) {
		*headers_end_at=0;
		const String header_block(String::C(response, headers_end_at-response), String::L_TAINTED);
		
		ArrayString aheaders;
		HashStringValue& tables=vtables->hash();

		size_t pos_after=0;
		header_block.split(aheaders, pos_after, "\n"); 
		
		// processing headers
		size_t aheaders_count=aheaders.count();
		for(size_t i=1; i<aheaders_count; i++) {
			const String& line=*aheaders.get(i);
			size_t pos=line.pos(':'); 
			if(pos==STRING_NOT_FOUND || pos<1)
				throw Exception("http.response", 
					&connect_string,
					"bad response from host - bad header \"%s\"", line.cstr());
			const String::Body HEADER_NAME=line.mid(0, pos).change_case(r.charsets.source(), String::CC_UPPER);
			const String& HEADER_VALUE=line.mid(pos+1, line.length()).trim(String::TRIM_BOTH, " \t\r");
			if(as_text && HEADER_NAME==HTTP_CONTENT_TYPE_UPPER)
				real_remote_charset=detect_charset(HEADER_VALUE.cstr());

			// tables
			{
				Value *valready=(Value *)tables.get(HEADER_NAME);
				bool existed=valready!=0;
				Table *table;
				if(existed) {
					// second+ appearence
					table=valready->get_table();
				} else {
					// first appearence
					Table::columns_type columns=new ArrayString(1);
					*columns+=new String("value");
					table=new Table(columns);
				}
				// this string becomes next row
				ArrayString& row=*new ArrayString(1);
				row+=&HEADER_VALUE;
				*table+=&row;
				// not existed before? add it
				if(!existed)
					tables.put(HEADER_NAME, new VTable(table));
			}

			result.headers->put(HEADER_NAME, new VString(HEADER_VALUE));
		}
	}

	if(as_text && raw_body_size>=3 && strncmp(raw_body, "\xEF\xBB\xBF", 3)==0){
		// skip UTF-8 signature (BOM code)
		raw_body+=3;
		raw_body_size-=3;
		if(!real_remote_charset)
			real_remote_charset=&UTF8_charset;
	}

	// output response
	String::C real_body=String::C(raw_body, raw_body_size);

	if(as_text && transcode_text_result && raw_body_size) { // raw_body_size must be checked because transcode returns CONST string in case length==0, which contradicts hacking few lines below
		// defaulting to used-asked charset [it's never empty!]
		if(!real_remote_charset)
			real_remote_charset=asked_remote_charset;

		real_body=Charset::transcode(real_body, *real_remote_charset, r.charsets.source());

	}

	result.str=const_cast<char *>(real_body.str); // hacking a little
	result.length=real_body.length;

	if(as_text && result.length)
		fix_line_breaks(result.str, result.length);

	result.headers->put(file_status_name, new VInt(status_code));

	return result;
}
