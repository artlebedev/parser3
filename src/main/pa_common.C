/** @file
	Parser: commonly functions.

	Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_COMMON_C="$Date: 2003/12/25 07:31:13 $"; 

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_hash.h"
#include "pa_globals.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"

#define PA_HTTP

#ifdef PA_HTTP
#include "pa_vstring.h"
#include "pa_vint.h"
#include "pa_vhash.h"
#include "pa_vtable.h"

#ifdef CYGWIN
#define _GNU_H_WINDOWS32_SOCKETS
// for PASCAL
#include <windows.h>
// SOCKET
typedef u_int	SOCKET;
int PASCAL closesocket(SOCKET);
#else
#	if defined(WIN32)
#		include <windows.h>
#	else
#		define closesocket close
#	endif
#endif

#else

#	if defined(WIN32)
#		include <windows.h>
#	endif

#endif

// some maybe-undefined constants

#ifndef _O_TEXT
#	define _O_TEXT 0
#endif
#ifndef _O_BINARY
#	define _O_BINARY 0
#endif

#ifdef HAVE_FTRUNCATE
#	define PA_O_TRUNC 0
#else
#	ifdef _O_TRUNC
#		define PA_O_TRUNC _O_TRUNC
#	else
#		error you must have either ftruncate function or _O_TRUNC bit declared
#	endif
#endif

// defines for globals

#define FILE_STATUS_NAME  "status"

// globals

const String file_status_name(FILE_STATUS_NAME);

// defines for statics

#define HTTP_METHOD_NAME  "method"
#define HTTP_TIMEOUT_NAME    "timeout"
#define HTTP_HEADERS_NAME "headers"
#define HTTP_ANY_STATUS_NAME "any-status"
#define HTTP_CHARSET_NAME "charset"
#define HTTP_TABLES_NAME "tables"

// statics

static const String http_method_name(HTTP_METHOD_NAME);
static const String http_timeout_name(HTTP_TIMEOUT_NAME);
static const String http_headers_name(HTTP_HEADERS_NAME);
static const String http_any_status_name(HTTP_ANY_STATUS_NAME);
static const String http_charset_name(HTTP_CHARSET_NAME);
static const String http_tables_name(HTTP_TABLES_NAME);

// defines

#define DEFAULT_USER_AGENT "parser3"

// functions

void fix_line_breaks(char *str, size_t& length) {
	//_asm int 3;
	const char* const eob=str+length;
	char* dest=str;
	// fix DOS: \r\n -> \n
	// fix Macintosh: \r -> \n
	char* bol=str;
	while(char* eol=(char*)memchr(bol, '\r', eob -bol)) {
		size_t len=eol-bol;
		if(dest!=bol)
			memcpy(dest, bol, len); 
		dest+=len;
		*dest++='\n'; 

		if(&eol[1]<eob && eol[1]=='\n') { // \r, \n = DOS
			bol=eol+2;
			length--; 
		} else // \r, not \n = Macintosh
			bol=eol+1;
	}
	// last piece without \r
	if(dest!=bol)
		memcpy(dest, bol, eob-bol); 
	str[length]=0; // terminating
}

char* file_read_text(Request_charsets& charsets, 
		     const String& file_spec, 
		     bool fail_on_read_problem,
		     HashStringValue* params/*, HashStringValue* * out_fields*/) {
	File_read_result file=
		file_read(charsets, file_spec, true, params, fail_on_read_problem);
	return file.success?file.str:0;
}

#ifdef PA_HTTP
//http request stuff

#undef CRLF
#define CRLF "\r\n"

static bool set_addr(struct sockaddr_in *addr, const char* host, const short port){
    memset(addr, 0, sizeof(*addr)); 
    addr->sin_family=AF_INET;
    addr->sin_port=htons(port); 
    if(host) {
		if(struct hostent *hostIP=gethostbyname(host)) 
			memcpy(&addr->sin_addr, hostIP->h_addr, hostIP->h_length); 
		else
			return false;
    } else 
		addr->sin_addr.s_addr=INADDR_ANY;
    return true;
}

static int http_read_response(char*& response, size_t& response_size, int sock, bool fail_on_status_ne_200){
	response=(char*)pa_malloc_atomic(1/*terminator*/); // setting memory block type
	response[(response_size=0)]=0;
	int result=0;
	char* EOLat=0;
	while(true) {
		char buf[MAX_STRING*10]; 
		ssize_t received_size=recv(sock, buf, sizeof(buf), 0); 
		if(received_size<=0)
			break;
		response=(char*)pa_realloc(response, response_size+received_size+1/*terminator*/);
		memcpy(response+response_size, buf, received_size);
		response_size+=received_size;
		response[response_size]=0;

		if(!result && (EOLat=strstr(response, CRLF))) { // checking status in first response
			const String status_line(pa_strdup(response, EOLat-response));
			ArrayString astatus; 
			size_t pos_after=0;
			status_line.split(astatus, pos_after, " "); 
			const String& status_code=*astatus.get(1);
			result=status_code.as_int(); 

			if(fail_on_status_ne_200 && result!=200)
				throw Exception("http.status",
					&status_code,
					"invalid HTTP response status");
		}
	}
	if(result)
		return result;
	else
		throw Exception("http.response",
			0,
			"bad response from host - no status found (size=%lu)", response_size); 
}

/* ********************** request *************************** */

#if defined(SIGALRM) && defined(HAVE_SIGSETJMP) && defined(HAVE_SIGLONGJMP)
#	define PA_USE_ALARM
#endif

#ifdef PA_USE_ALARM
static sigjmp_buf timeout_env;
static void timeout_handler(int sig){
    siglongjmp(timeout_env, 1); 
}
#endif

static int http_request(char*& response, size_t& response_size,
			const char* host, int port, 
			const char* request, 
			int 
#ifdef PA_USE_ALARM
			timeout
#endif
			,
			bool fail_on_status_ne_200) {
	if(!host)
		throw Exception("http.host", 
			0, 
			"zero hostname");  //never

#ifdef PA_USE_ALARM
	signal(SIGALRM, timeout_handler); 
#endif
	int sock=-1;
#ifdef PA_USE_ALARM
	if(sigsetjmp(timeout_env, 1)) {
		// stupid gcc [2.95.4] generated bad code
		// which failed to handle sigsetjmp+throw: crashed inside of pre-throw code.
		// rewritten simplier [though duplicating closesocket code]
		if(sock>=0) 
			closesocket(sock); 
		throw Exception("http.timeout", 
			origin_string, 
			"timeout occured while retrieving document"); 
		return 0; // never
	} else {
		alarm(timeout); 
#endif
		try {
			int result;
			struct sockaddr_in dest;
		
			if(!set_addr(&dest, host, port))
				throw Exception("http.host", 
					0, 
					"can not resolve hostname \"%s\"", host); 
			
			if((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP/*0*/))<0)
				throw Exception("http.connect", 
					0, 
					"can not make socket: %s (%d)", strerror(errno), errno); 
			if(connect(sock, (struct sockaddr *)&dest, sizeof(dest)))
				throw Exception("http.connect", 
					0, 
					"can not connect to host \"%s\": %s (%d)", host, strerror(errno), errno); 
			size_t request_size=strlen(request);
			if(send(sock, request, request_size, 0)!=(ssize_t)request_size)
				throw Exception("http.connect", 
					0, 
					"error sending request: %s (%d)", strerror(errno), errno); 

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
	bool user_agent_specified;
};
#endif
static void http_pass_header(HashStringValue::key_type key, 
			     HashStringValue::value_type value, 
			     Http_pass_header_info *info) {
    *info->request <<key<<": "
	<< attributed_meaning_to_string(*value, String::L_HTTP_HEADER, false)
	<< CRLF; 
	
    if(String(key, String::L_TAINTED).change_case(info->charsets->source(), String::CC_UPPER)=="USER-AGENT")
	info->user_agent_specified=true;
}


static Charset* detect_charset(Charset& source_charset, const String& content_type_value) {
	const String::Body CONTENT_TYPE_VALUE=
		content_type_value.change_case(source_charset, String::CC_UPPER);
	// content-type: xxx/xxx; source_charset=WE-NEED-THIS
	// content-type: xxx/xxx; source_charset="WE-NEED-THIS"
	// content-type: xxx/xxx; source_charset="WE-NEED-THIS";
	size_t before_charseteq_pos=CONTENT_TYPE_VALUE.pos("CHARSET=");
	if(before_charseteq_pos!=STRING_NOT_FOUND) {
		size_t charset_begin=before_charseteq_pos+8/*CHARSET="*/;
		size_t open_quote_pos=CONTENT_TYPE_VALUE.pos('"', charset_begin);
		bool quoted=open_quote_pos==charset_begin;
		if(quoted)
			charset_begin++; // skip opening '"'
		size_t charset_end=CONTENT_TYPE_VALUE.length();
		if(quoted) {
			size_t close_quote_pos=CONTENT_TYPE_VALUE.pos('"', charset_begin);
			if(close_quote_pos!=STRING_NOT_FOUND)
				charset_end=close_quote_pos;
		} else {
			size_t delim_pos=CONTENT_TYPE_VALUE.pos(';', charset_begin);
			if(delim_pos!=STRING_NOT_FOUND)
				charset_end=delim_pos;
		}
		const String::Body CHARSET_NAME_BODY=
			CONTENT_TYPE_VALUE.mid(charset_begin, charset_end);

		return &charsets.get(CHARSET_NAME_BODY);
	}

	return 0;
}


#ifndef DOXYGEN
struct File_read_http_result {
	char *str; size_t length;
	HashStringValue* headers;
}; 
#endif
/// @todo build .cookies field. use ^file.tables.SET-COOKIES.menu{ for now
static File_read_http_result file_read_http(Request_charsets& charsets, 
					    const String& file_spec, 
					    bool as_text,
						HashStringValue *options=0) {
	File_read_http_result result;
	char host[MAX_STRING]; 
	const char* uri; 
	int port;
	const char* method=0;
	int timeout=2;
	bool fail_on_status_ne_200=true;
	Value* vheaders=0;
	Charset *asked_remote_charset=0;

	if(options) {
		int valid_options=0;
		if(Value* vmethod=options->get(http_method_name)) {
			valid_options++;
			method=vmethod->as_string().cstr();
		}
		if(Value* vtimeout=options->get(http_timeout_name)) {
			valid_options++;
			timeout=vtimeout->as_int(); 
		}
		if((vheaders=options->get(http_headers_name))) {
			valid_options++;
		}
		if(Value* vany_status=options->get(http_any_status_name)) {
			valid_options++;
			fail_on_status_ne_200=!vany_status->as_bool(); 
		}
		if(Value* vcharset_name=options->get(http_charset_name)) {
			valid_options++;
			asked_remote_charset=&::charsets.get(vcharset_name->as_string().
				change_case(charsets.source(), String::CC_UPPER));
		}

		if(valid_options!=options->count())
			throw Exception("parser.runtime",
				0,
				"invalid option passed");
	}
	if(!asked_remote_charset) // defaulting to $request:charset
		asked_remote_charset=&charsets.source();

	//preparing request
	String& connect_string=*new String;
	// not in ^sql{... L_SQL ...} spirit, but closer to ^file::load one
	connect_string.append(file_spec, String::L_URI); // tainted pieces -> URI pieces

	const char* request_cstr;
	{
		// influence URLencoding of tainted pieces to String::L_URI lang
		Temp_client_charset temp(charsets, *asked_remote_charset);

		const char* connect_string_cstr=connect_string.cstr(String::L_UNSPECIFIED); 

		const char* current=connect_string_cstr;
		if(strncmp(current, "http://", 7)!=0)
			throw Exception(0, 
				&connect_string, 
				"does not start with http://"); //never
		current+=7;

		strncpy(host, current, sizeof(host)-1);  host[sizeof(host)-1]=0;
		char* host_uri=lsplit(host, '/'); 
		uri=host_uri?current+(host_uri-1-host):"/"; 
		char* port_cstr=lsplit(host, ':'); 
		char* error_pos=0;
		port=port_cstr?strtol(port_cstr, &error_pos, 0):80;


		//making request
		String request;
		if(method)
			request<<method;
		else
			request<<"GET";
		request<< " "<< uri <<" HTTP/1.0" CRLF
			"host: "<< host << CRLF; 
		bool user_agent_specified=false;
		if(vheaders && !vheaders->is_string()) { // allow empty
			if(HashStringValue *headers=vheaders->get_hash()) {
				Http_pass_header_info info={&charsets, &request, false};
				headers->for_each(http_pass_header, &info); 
				user_agent_specified=info.user_agent_specified;
			} else
				throw Exception("parser.runtime", 
					&connect_string,
					"headers param must be hash"); 
		};
		if(!user_agent_specified) // defaulting
			request << "user-agent: " DEFAULT_USER_AGENT CRLF;
		request << CRLF; 

		request_cstr=request.cstr(String::L_UNSPECIFIED);
	}
	// recode those pieces which are not in String::L_URI lang 
	// [those violating HTTP standard, but widly used]
	request_cstr=Charset::transcode(
		String::C(request_cstr, strlen(request_cstr)),
		charsets.source(),
		*asked_remote_charset);
	
	//sending request
	char* response;
	size_t response_size;
	int status_code=http_request(response, response_size,
		host, port, request_cstr, 
		timeout, fail_on_status_ne_200); 
	
	//processing results	
	char* raw_body; size_t raw_body_size;
	char* headers_end_at=strstr(response, CRLF CRLF /*change '4' below along!*/); 
	if(headers_end_at) {
		raw_body=headers_end_at+4;
		raw_body_size=response_size-(raw_body-response);
	} else
		throw Exception("http.response", 
			&connect_string,
			"bad response from host - no headers found"); 
	
	*headers_end_at=0;
	const String header_block(response, headers_end_at-response, true);
	
	ArrayString aheaders;
	result.headers=new HashStringValue;
	VHash* vtables=new VHash;
	result.headers->put(http_tables_name, vtables);
	HashStringValue& tables=vtables->hash();

	size_t pos_after=0;
	header_block.split(aheaders, pos_after, CRLF); 
	
	//processing headers
	size_t aheaders_count=aheaders.count();
	Charset* real_remote_charset=0; // undetected, yet
	for(size_t i=1; i<aheaders_count; i++) {
		const String& line=*aheaders.get(i);
		size_t pos=line.pos(": ", 2); 
		if(pos==STRING_NOT_FOUND || pos<1)
			throw Exception("http.response", 
				&connect_string,
				"bad response from host - bad header \"%s\"", line.cstr());
		const String::Body HEADER_NAME=
			line.mid(0, pos).change_case(charsets.source(), String::CC_UPPER);
		const String& header_value=line.mid(pos+2, line.length());
		if(HEADER_NAME=="CONTENT-TYPE")
			real_remote_charset=detect_charset(charsets.source(), header_value);

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
				Table::columns_type columns =new ArrayString(1);
				*columns+=new String("value");
				table=new Table(columns);
			}
			// this string becomes next row
			ArrayString& row=*new ArrayString(1);
			row+=&header_value;
			*table+=&row;
			// not existed before? add it
			if(!existed)
				tables.put(HEADER_NAME, new VTable(table));
		}

		result.headers->put(HEADER_NAME, new VString(header_value));
	}
	// defaulting to used-asked charset [it's never empty!]
	if(!real_remote_charset)
		real_remote_charset=asked_remote_charset;

	// output response
	String::C real_body=String::C(raw_body, raw_body_size);
	if(as_text && raw_body_size) // must be checked because transcode returns CONST string in case length==0, which contradicts hacking few lines below
		real_body=Charset::transcode(real_body, *real_remote_charset, charsets.source());

	result.str=const_cast<char *>(real_body.str); // hacking a little
	result.length=real_body.length;
	result.headers->put(file_status_name, new VInt(status_code));
	return result;
}

#endif

#ifndef DOXYGEN
struct File_read_action_info {
	char **data; size_t *data_size;
}; 
#endif
static void file_read_action(
			     struct stat& finfo, 
			     int f, 
			     const String& file_spec, const char* /*fname*/, bool as_text, 
			     void *context) {
	File_read_action_info& info=*static_cast<File_read_action_info *>(context); 
	if(size_t to_read_size=(size_t)finfo.st_size) { 
		*info.data=new(PointerFreeGC) char[to_read_size+(as_text?1:0)]; 
		*info.data_size=(size_t)read(f, *info.data, to_read_size); 

		if(ssize_t(*info.data_size)<0 || *info.data_size>to_read_size)
			throw Exception(0, 
				&file_spec, 
				"read failed: actually read %l bytes count not in [0..%lu] valid range", 
					*info.data_size, to_read_size); 
	} else { // empty file
		if(as_text) {
			*info.data=new(PointerFreeGC) char[1]; 
			*(char*)(*info.data)=0;
		} else 
			*info.data=0;
		*info.data_size=0;
		return;
	}
}
File_read_result file_read(Request_charsets& charsets, const String& file_spec, 
			   bool as_text, HashStringValue *params,
			   bool fail_on_read_problem) {
	File_read_result result={false, 0, 0, 0};
#ifdef PA_HTTP
	if(file_spec.starts_with("http://")) {
		// fail on read problem
		File_read_http_result http=file_read_http(charsets, file_spec, as_text, params);
		result.success=true;
		result.str=http.str;
		result.length=http.length;
		result.headers=http.headers; 
	} else {
#endif
		if(params && params->count())
			throw Exception("parser.runtime",
				0,
				"invalid option passed");

		File_read_action_info info={&result.str, &result.length}; 
		result.success=file_read_action_under_lock(file_spec, 
			"read", file_read_action, &info, 
			as_text, fail_on_read_problem); 
#ifdef PA_HTTP
	}
#endif

	if(result.success && as_text) {
		// UTF-8 signature: EF BB BF
		if(result.length>=3) {
			char *in=(char *)result.str;
			if(strncmp(in, "\xEF\xBB\xBF", 3)==0) {
				result.str=in+3; result.length-=3;// skip prefix
			}
		}

		fix_line_breaks((char *)(result.str), result.length); 
	}

	return result;
}

#ifdef PA_SAFE_MODE 
void check_safe_mode(struct stat finfo, const String& file_spec, const char* fname) { 
	if(finfo.st_uid/*foreign?*/!=geteuid() 
		&& finfo.st_gid/*foreign?*/!=getegid()) 
		throw Exception("parser.runtime",  
			&file_spec,  
			"parser is in safe mode: " 
			"reading files of foreign group and user disabled " 
			"[recompile parser with --disable-safe-mode configure option], " 
			"actual filename '%s', " 
			"fuid(%d)!=euid(%d) or fgid(%d)!=egid(%d)",  
				fname, 
				finfo.st_uid, geteuid(), 
				finfo.st_gid, getegid()); 
} 
#endif 

bool file_read_action_under_lock(const String& file_spec, 
				const char* action_name, File_read_action action, void *context, 
				bool as_text, 
				bool fail_on_read_problem) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	int f;

	// first open, next stat:
	// directory update of NTFS hard links performed on open.
	// ex: 
	//   a.html:^test[] and b.html hardlink to a.html
	//   user inserts ! before ^test in a.html
	//   directory entry of b.html in NTFS not updated at once, 
	//   they delay update till open, so we would receive "!^test[" string
	//   if would do stat, next open.
	// later: it seems, even this does not help sometimes
    if((f=open(fname, O_RDONLY|(as_text?_O_TEXT:_O_BINARY)))>=0) {
		try {
			if(pa_lock_shared_blocking(f)!=0)
				throw Exception("file.lock", 
						&file_spec, 
						"shared lock failed: %s (%d), actual filename '%s'", 
							strerror(errno), errno, fname);

			struct stat finfo;
			if(stat(fname, &finfo)!=0)
				throw Exception("file.missing", // hardly possible: we just opened it OK
					&file_spec, 
					"stat failed: %s (%d), actual filename '%s'", 
						strerror(errno), errno, fname);

#ifdef PA_SAFE_MODE
			check_safe_mode(finfo, file_spec, fname);
#endif

			action(finfo, f, file_spec, fname, as_text, context); 
		} catch(...) {
			pa_unlock(f);close(f); 
			if(fail_on_read_problem)
				rethrow;
			return false;			
		} 

		pa_unlock(f);close(f); 
		return true;
    } else {
		if(fail_on_read_problem)
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
				&file_spec, 
				"%s failed: %s (%d), actual filename '%s'", 
					action_name, strerror(errno), errno, fname);
		return false;
	}
}

static void create_dir_for_file(const String& file_spec) {
	size_t pos_after=1;
	size_t pos_before;
	while((pos_before=file_spec.pos('/', pos_after))!=STRING_NOT_FOUND) {
		mkdir(file_spec.mid(0, pos_before).cstr(String::L_FILE_SPEC), 0775); 
		pos_after=pos_before+1;
	}
}

bool file_write_action_under_lock(
				const String& file_spec, 
				const char* action_name, File_write_action action, void *context, 
				bool as_text, 
				bool do_append, 
				bool do_block, 
				bool fail_on_lock_problem) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	int f;
	if(access(fname, W_OK)!=0) // no
		create_dir_for_file(file_spec); 

	if((f=open(fname, 
		O_CREAT|O_RDWR
		|(as_text?_O_TEXT:_O_BINARY)
		|(do_append?O_APPEND:PA_O_TRUNC), 0664))>=0) {
		if((do_block?pa_lock_exclusive_blocking(f):pa_lock_exclusive_nonblocking(f))!=0) {
			Exception e("file.lock", 
				&file_spec, 
				"shared lock failed: %s (%d), actual filename '%s'", 
				strerror(errno), errno, fname);
			close(f); 
			if(fail_on_lock_problem)
				throw e;
			return false;
		}

		try {
			action(f, context); 
		} catch(...) {
#ifdef HAVE_FTRUNCATE
			if(!do_append)
				ftruncate(f, lseek(f, 0, SEEK_CUR)); // one can not use O_TRUNC, read lower
#endif
			pa_unlock(f);close(f); 
			rethrow;
		}
		
#ifdef HAVE_FTRUNCATE
		if(!do_append)
			ftruncate(f, lseek(f, 0, SEEK_CUR)); // O_TRUNC truncates even exclusevely write-locked file [thanks to Igor Milyakov <virtan@rotabanner.com> for discovering]
#endif
		pa_unlock(f);close(f); 
		return true;
	} else
		throw Exception(errno==EACCES?"file.access":0, 
			&file_spec, 
			"%s failed: %s (%d), actual filename '%s'", 
				action_name, strerror(errno), errno, fname);
	// here should be nothing, see rethrow above
}

#ifndef DOXYGEN
struct File_write_action_info {
	const char* str; size_t length;
}; 
#endif
static void file_write_action(int f, void *context) {
	File_write_action_info& info=*static_cast<File_write_action_info *>(context); 
	if(info.length) {
		int written=write(f, info.str, info.length); 
		if(written<0)
			throw Exception(0, 
				0, 
				"write failed: %s (%d)",  strerror(errno), errno); 
	}
}
void file_write(
				const String& file_spec, 
				const char* data, size_t size, 
				bool as_text, 
				bool do_append) {
	File_write_action_info info={data, size}; 
	file_write_action_under_lock(
		file_spec, 
		"write", file_write_action, &info, 
		as_text, 
		do_append); 
}

// throws nothing! [this is required in file_move & file_delete]
static void rmdir(const String& file_spec, size_t pos_after) {
	size_t pos_before;
	if((pos_before=file_spec.pos('/', pos_after))!=STRING_NOT_FOUND)
		rmdir(file_spec, pos_before+1); 
	
	rmdir(file_spec.mid(0, pos_after-1/* / */).cstr(String::L_FILE_SPEC)); 
}
bool file_delete(const String& file_spec, bool fail_on_problem) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	if(unlink(fname)!=0)
		if(fail_on_problem)
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
				&file_spec, 
				"unlink failed: %s (%d), actual filename '%s'", 
					strerror(errno), errno, fname);
		else
			return false;

	rmdir(file_spec, 1); 
	return true;
}
void file_move(const String& old_spec, const String& new_spec) {
	const char* old_spec_cstr=old_spec.cstr(String::L_FILE_SPEC); 
	const char* new_spec_cstr=new_spec.cstr(String::L_FILE_SPEC); 
	
	create_dir_for_file(new_spec); 

	if(rename(old_spec_cstr, new_spec_cstr)!=0)
		throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
			&old_spec, 
			"rename failed: %s (%d), actual filename '%s' to '%s'", 
				strerror(errno), errno, old_spec_cstr, new_spec_cstr);

	rmdir(old_spec, 1); 
}


bool entry_exists(const char* fname, struct stat *afinfo) {
	struct stat lfinfo;
	bool result=stat(fname, &lfinfo)==0;
	if(afinfo)
		*afinfo=lfinfo;
	return result;
}

bool entry_exists(const String& file_spec) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	return entry_exists(fname, 0); 
}

static bool entry_readable(const String& file_spec, bool need_dir) {
	char* fname=file_spec.cstrm(String::L_FILE_SPEC); 
	if(need_dir) {
		size_t size=strlen(fname); 
		while(size) {
			char c=fname[size-1]; 
			if(c=='/' || c=='\\')
				fname[--size]=0;
			else
				break;
		}
	}
	struct stat finfo;
	if(access(fname, R_OK)==0 && entry_exists(fname, &finfo)) {
		bool is_dir=(finfo.st_mode&S_IFDIR) != 0;
		return is_dir==need_dir;
	}
	return false;
}
bool file_readable(const String& file_spec) {
	return entry_readable(file_spec, false); 
}
bool dir_readable(const String& file_spec) {
	return entry_readable(file_spec, true); 
}
const String* file_readable(const String& path, const String& name) {
	String& result=*new String(path);
	result << "/"; 
	result << name;
	return file_readable(result)?&result:0;
}
bool file_executable(const String& file_spec) {
    return access(file_spec.cstr(String::L_FILE_SPEC), X_OK)==0;
}

bool file_stat(const String& file_spec, 
			   size_t& rsize, 
			   time_t& ratime, 
			   time_t& rmtime, 
			   time_t& rctime, 
			   bool fail_on_read_problem) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	struct stat finfo;
	if(stat(fname, &finfo)!=0)
		if(fail_on_read_problem)
			throw Exception("file.missing", 
				&file_spec, 
				"getting file size failed: %s (%d), real filename '%s'", 
					strerror(errno), errno, fname);
		else
			return false;
	rsize=finfo.st_size;
	ratime=finfo.st_atime;
	rmtime=finfo.st_mtime;
	rctime=finfo.st_ctime;
	return true;
}

char* getrow(char* *row_ref, char delim) {
    char* result=*row_ref;
    if(result) {
		*row_ref=strchr(result, delim); 
		if(*row_ref) 
			*((*row_ref)++)=0; 
		else if(!*result) 
			return 0;
    }
    return result;
}

char* lsplit(char* string, char delim) {
    if(string) {
		char* v=strchr(string, delim); 
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return 0;
}

char* lsplit(char* *string_ref, char delim) {
    char* result=*string_ref;
	char* next=lsplit(*string_ref, delim); 
    *string_ref=next;
    return result;
}

char* rsplit(char* string, char delim) {
    if(string) {
		char* v=strrchr(string, delim); 
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return NULL;	
}

/// @todo less stupid type detection
const char* format(double value, char* fmt) {
	char local_buf[MAX_NUMBER]; 
	size_t size;
	
	if(fmt)
		if(strpbrk(fmt, "diouxX"))
			if(strpbrk(fmt, "ouxX"))
				size=snprintf(local_buf, sizeof(local_buf), fmt, (uint)value); 
			else
				size=snprintf(local_buf, sizeof(local_buf), fmt, (int)value); 
		else
			size=snprintf(local_buf, sizeof(local_buf), fmt, value); 
	else
		size=snprintf(local_buf, sizeof(local_buf), "%d", (int)value); 
	
	return pa_strdup(local_buf, size);
}

size_t stdout_write(const void *buf, size_t size) {
#ifdef WIN32
	do{
		int chunk_written=fwrite(buf, 1, min((size_t)8*0x400, size), stdout); 
		if(chunk_written<=0)
			break;
		size-=chunk_written;
		buf=((const char*)buf)+chunk_written;
	} while(size>0); 

	return size;
#else
	return fwrite(buf, 1, size, stdout); 
#endif
}

char* unescape_chars(const char* cp, int len) {
	char* s=new(PointerFreeGC) char[len + 1]; 
	enum EscapeState {
		EscapeRest, 
		EscapeFirst, 
		EscapeSecond
	} escapeState=EscapeRest;
	int escapedValue=0;
	int srcPos=0;
	int dstPos=0;
	while(srcPos < len) {
		int ch=cp[srcPos]; 
		switch(escapeState) {
			case EscapeRest:
			if(ch=='%') {
				escapeState=EscapeFirst;
			} else if(ch=='+') {
				s[dstPos++]=' '; 
			} else {
				s[dstPos++]=ch;	
			}
			break;
			case EscapeFirst:
			escapedValue=hex_value[ch] << 4;	
			escapeState=EscapeSecond;
			break;
			case EscapeSecond:
			escapedValue +=hex_value[ch]; 
			s[dstPos++]=escapedValue;
			escapeState=EscapeRest;
			break;
		}
		srcPos++; 
	}
	s[dstPos]=0;
	return s;
}

#ifdef WIN32
void back_slashes_to_slashes(char* s) {
	if(s)
		for(; *s; s++)
			if(*s=='\\')
				*s='/'; 
}
/*
void slashes_to_back_slashes(char* s) {
	if(s)
		for(; *s; s++)
			if(*s=='/')
				*s='\\'; 
}
*/
#endif

bool StrEqNc(const char* s1, const char* s2, bool strict) {
	while(true) {
		if(!(*s1)) {
			if(!(*s2))
				return true;
			else
				return !strict;
		} else if(!(*s2))
			return !strict;
		if(isalpha(*s1)) {
			if(tolower(*s1) !=tolower(*s2))
				return false;
		} else if((*s1) !=(*s2))
			return false;
		s1++; 
		s2++; 
	}
}

static bool isLeap(int year) {
    return !(
             (year % 4) || ((year % 400) && !(year % 100))
            ); 
}

int getMonthDays(int year, int month) {
    int monthDays[]={
        31, 
        isLeap(year) ? 29 : 28, 
        31, 
        30, 
        31, 
        30, 
        31, 
        31, 
        30, 
        31, 
        30, 
        31
    }; 
    return monthDays[month]; 
}

void remove_crlf(char* start, char* end) {
	for(char* p=start; p<end; p++)
		switch(*p) {
			case '\n': *p='|';  break;
			case '\r': *p=' ';  break;
		}
}


/// must be last in this file
#undef vsnprintf
int __vsnprintf(char* b, size_t s, const char* f, va_list l) {
	if(!s)
		return 0;

	int r;
	// note: on win32& maybe somewhere else
	// vsnprintf do not writes terminating 0 in 'buffer full' case, reducing
	--s;
#if _MSC_VER
	/*
	win32: 
	mk:@MSITStore:C:\Program%20Files\Microsoft%20Visual%20Studio\MSDN\2001APR\1033\vccore.chm::/html/_crt__vsnprintf.2c_._vsnwprintf.htm

	  if the number of bytes to write exceeds buffer, then count bytes are written and Ö1 is returned
	*/
	r=_vsnprintf(b, s, f, l); 
	if(r<0) 
		r=s;
#else
	r=vsnprintf(b, s, f, l); 
	/*
	solaris: 
	man vsnprintf

	  The snprintf() function returns  the  number  of  characters
	formatted, that is, the number of characters that would have
	been written to the buffer if it were large enough.  If  the
	value  of  n  is  0  on a call to snprintf(), an unspecified
	value less than 1 is returned.
	*/

	if(r<0)
		r=0;
	else if((size_t)r>s)
		r=s;
#endif
	b[r]=0;
	return r;
}

int __snprintf(char* b, size_t s, const char* f, ...) {
	va_list l;
    va_start(l, f); 
    int r=__vsnprintf(b, s, f, l); 
    va_end(l); 
	return r;
}
