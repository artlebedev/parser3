/** @file
	Parser: scripting and CGI main.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: parser3.C,v 1.182 2002/06/12 14:18:05 paf Exp $
*/

#include "pa_config_includes.h"

#if _MSC_VER
#	include <new.h>
#	include <crtdbg.h>
#endif

#include "pa_sapi.h"
#include "classes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_socks.h"
#include "pa_version.h"
#include "pool_storage.h"

#ifdef WIN32
#	include <windows.h>
#endif

//#define DEBUG_POOL_MALLOC
//#define DEBUG_STRING_APPENDS_VS_EXPANDS

#ifdef DEBUG_STRING_APPENDS_VS_EXPANDS
extern ulong 
	string_piece_appends, 
	wcontext_result_size,
	total_alloc_size,
	string_string_shortcut_economy;
#endif

// consts

#ifndef _PROFILE
extern const char *main_RCSIds[];
#ifdef USE_SMTP
extern const char *smtp_RCSIds[];
#endif
extern const char *gd_RCSIds[];
extern const char *classes_RCSIds[];
extern const char *types_RCSIds[];
extern const char *parser3_RCSIds[];
const char **RCSIds[]={
	main_RCSIds,
#ifdef USE_SMTP
	smtp_RCSIds,
#endif
	gd_RCSIds,
	classes_RCSIds,
	types_RCSIds,
	parser3_RCSIds,
	0
};
#endif

#define REDIRECT_PREFIX "REDIRECT_"
#define PARSER_CONFIG_ENV_NAME "CGI_PARSER_CONFIG"

/// IIS refuses to read bigger chunks
const size_t READ_POST_CHUNK_SIZE=0x400*0x400; // 1M 

const char *argv0;
Pool *pool; // global pool [dont describe to doxygen: it confuses it with param names]
bool cgi; ///< we were started as CGI?

// SAPI

static void log(const char *fmt, va_list args) {
	bool opened;
	FILE *f=0;

	if(argv0) {
		// beside by binary
		char file_spec[MAX_STRING];
		strncpy(file_spec, argv0, MAX_STRING-1);  file_spec[MAX_STRING-1]=0; // filespec of my binary
		rsplit(file_spec, '/');  rsplit(file_spec, '\\');// strip filename
		strcat(file_spec, "/parser3.log");
		f=fopen(file_spec, "at");
	}
	opened=f!=0;
	if(!opened)
		f=stderr;

	// prefix
	time_t t=time(0);
	if(const char *stamp=ctime(&t)) { // never saw that
		if(size_t len=strlen(stamp)) // saw once stamp being =""
			fprintf(f, "[%.*s] ", len-1, stamp);
	}
	// message

	char buf[MAX_STRING];
	size_t size=vsnprintf(buf, MAX_STRING, fmt, args);
	remove_crlf(buf, buf+size);

	fwrite(buf, size, 1, f);
	// newline
	fprintf(f, "\n");

	if(opened)
		fclose(f);
	else
		fflush(f);
}

// appends to parser3.log located beside my binary if openable, to stderr otherwize
void SAPI::log(Pool& , const char *fmt, ...) {
    va_list args;
	va_start(args,fmt);
	::log(fmt, args);
	va_end(args);
}

void SAPI::die(const char *fmt, ...) {
#ifdef DEBUG_POOL_MALLOC
	extern void log_pool_stats(Pool& pool);
	log_pool_stats(*pool);
#endif

    va_list args;
	va_start(args,fmt);
	// log

	// logging is more important than user 
	// she can cancel download, we'd get SIG_PIPE, 
	// nothing would be logged then
	::log(fmt, args);

	// inform user

	char body[MAX_STRING];
	int content_length=vsnprintf(body, MAX_STRING, fmt, args);

	va_end(args);

	// prepare header
	// let's be honest, that's bad we couldn't produce valid output
	SAPI::add_header_attribute(*pool, "status", "500");
	SAPI::add_header_attribute(*pool, "content-type", "text/plain");
	char content_length_cstr[MAX_NUMBER];
	snprintf(content_length_cstr, sizeof(content_length_cstr), "%u", content_length);
	SAPI::add_header_attribute(*pool, "content-length", content_length_cstr);

	// send header
	SAPI::send_header(*pool);

	// body
	SAPI::send_body(*pool, body, content_length);

	exit(1);
}

const char *SAPI::get_env(Pool& , const char *name) {
	return getenv(name);
}

const char *const *SAPI::environment(Pool&) {
#ifdef _MSC_VER
	extern char **_environ;
	return _environ;
#else
	extern char **environ;
	return environ;
#endif
}

size_t SAPI::read_post(Pool& , char *buf, size_t max_bytes) {
	size_t read_size=0;
	do {
		int chunk_size=read(fileno(stdin), 
			buf+read_size, min(READ_POST_CHUNK_SIZE, max_bytes-read_size));
		if(chunk_size<=0)
			break;
		read_size+=chunk_size;
	} while(read_size<max_bytes);

	return read_size;
}

void SAPI::add_header_attribute(Pool& , const char *key, const char *value) {
	if(cgi)
		printf("%s: %s\n", key, value);
}

/// @todo intelligent cache-control
void SAPI::send_header(Pool& ) {
	if(cgi) {
//		puts("expires: Fri, 23 Mar 2001 09:32:23 GMT");

		// header | body  delimiter
		puts("");
	}
}

void SAPI::send_body(Pool& , const void *buf, size_t size) {
	stdout_write(buf, size);
}

//

void full_file_spec(const char *file_name, char *buf, size_t buf_size) {
	if(file_name)
		if(file_name[0]=='/' 
#ifdef WIN32
			|| (file_name[0] && file_name[1]==':')
#endif
			) 
			strncpy(buf, file_name, buf_size);
		else {
			char cwd[MAX_STRING];  getcwd(cwd, MAX_STRING);
			snprintf(buf, buf_size, "%s/%s", cwd, file_name);
		}
	else
		buf[0]=0;
#ifdef WIN32
	back_slashes_to_slashes(buf);
#endif
}

/**
main workhorse

  @todo 
		IIS: remove trailing default-document[index.html] from $request.uri.
		to do that we need to consult metabase,
		wich is tested but seems slow.
		IIS5 todo find out proper 'illegal call' check 
*/
void real_parser_handler(
					const char *filespec_to_process,
					const char *request_method, bool header_only) {
	// init socks
	init_socks(*pool);
	
	// init global classes
	init_methoded_array(*pool);
	// init global variables
	pa_globals_init(*pool);
	
	if(!filespec_to_process)
		SAPI::die("Parser/%s", PARSER_VERSION);
	
	// Request info
	Request::Info request_info;
	char document_root_buf[MAX_STRING];
	if(cgi) {
		if(const char *env_document_root=SAPI::get_env(*pool, "DOCUMENT_ROOT"))
			request_info.document_root=env_document_root;
		else if(const char *path_info=SAPI::get_env(*pool, "PATH_INFO")) {
			// IIS
			size_t len=min(sizeof(document_root_buf)-1, strlen(filespec_to_process)-strlen(path_info));
			memcpy(document_root_buf, filespec_to_process, len); document_root_buf[len]=0;
			request_info.document_root=document_root_buf;
		} else
			throw Exception("parser.runtime",
				0,
				"CGI: no PATH_INFO defined(in reinventing DOCUMENT_ROOT)");
	} else {
		full_file_spec("", document_root_buf, sizeof(document_root_buf));
		request_info.document_root=document_root_buf;
	}
	request_info.path_translated=filespec_to_process;
	request_info.method=request_method ? request_method : "GET";
	const char *query_string=SAPI::get_env(*pool, "QUERY_STRING");
	request_info.query_string=query_string;
	if(cgi) {
		if(const char *env_request_uri=SAPI::get_env(*pool, "REQUEST_URI"))
			request_info.uri=env_request_uri;
		else if(const char *path_info=SAPI::get_env(*pool, "PATH_INFO"))
			if(query_string) {
				char *reconstructed_uri=(char *)pool->malloc(
					strlen(path_info)+1/*'?'*/+
					strlen(query_string)+1/*0*/);
				strcpy(reconstructed_uri, path_info);
				strcat(reconstructed_uri, "?");
				strcat(reconstructed_uri, query_string);
				request_info.uri=reconstructed_uri;
			} else
				request_info.uri=path_info;
		else
			throw Exception("parser.runtime",
				0,
				"CGI: no PATH_INFO defined(in reinventing REQUEST_URI)");
			
#ifndef WIN32
			// they've changed this under IIS5.
			if(const char *script_name=SAPI::get_env(*pool, "SCRIPT_NAME")) {
				size_t script_name_len=strlen(script_name);
				size_t uri_len=strlen(request_info.uri);
				if(strncmp(request_info.uri,script_name, script_name_len)==0 &&
					script_name_len != uri_len) // under IIS they are the same
					SAPI::die("CGI: illegal call");
			}
#endif
	} else
		request_info.uri="";
	
	request_info.content_type=SAPI::get_env(*pool, "CONTENT_TYPE");
	const char *content_length=SAPI::get_env(*pool, "CONTENT_LENGTH");
	request_info.content_length=(content_length?atoi(content_length):0);
	request_info.cookie=SAPI::get_env(*pool, "HTTP_COOKIE");
	
	// prepare to process request
	Request request(*pool,
		request_info,
#ifdef _DEBUG
		String::UL_HTML|String::UL_OPTIMIZE_BIT
#else
		cgi ? String::UL_HTML|String::UL_OPTIMIZE_BIT : String::UL_AS_IS
#endif
		,
		true /* status_allowed */);
	
	const char *config_filespec_cstr;
	char config_filespec_buf[MAX_STRING];
	const char *config_by_env=getenv(PARSER_CONFIG_ENV_NAME);
	if(!config_by_env)
		config_by_env=getenv(REDIRECT_PREFIX PARSER_CONFIG_ENV_NAME);
	if(config_by_env)
		config_filespec_cstr=config_by_env;
	else {
	// beside by binary
	// @todo full path, not ./!
		char beside_binary_path[MAX_STRING];
		strncpy(beside_binary_path, argv0, MAX_STRING-1);  beside_binary_path[MAX_STRING-1]=0; // filespec of my binary
		if(!(
			rsplit(beside_binary_path, '/') || 
			rsplit(beside_binary_path, '\\'))) { // strip filename
			// no path, just filename
			beside_binary_path[0]='.'; beside_binary_path[1]=0;
		}
		snprintf(config_filespec_buf, MAX_STRING, 
			"%s/%s", 
			beside_binary_path, CONFIG_FILE_NAME);
		config_filespec_cstr=config_filespec_buf;
	}
	
	// process the request
	request.core(
		config_filespec_cstr, false /*fail_on_read_problem*/,
		header_only);
	
	//
	done_socks();
	
#ifdef DEBUG_POOL_MALLOC
	extern void log_pool_stats(Pool& pool);
	log_pool_stats(*pool);
#endif

#ifdef DEBUG_STRING_APPENDS_VS_EXPANDS
	SAPI::log(*pool, 
		"string piece appends=%lu, wcontext_result_size=%lu, string_string_shortcut_economy_closer=%lu, total_alloc_size=%lu", 
		string_piece_appends,
		wcontext_result_size,
		string_string_shortcut_economy,
		total_alloc_size);
#endif

}

void call_real_parser_handler__do_SEH(
								 const char *filespec_to_process,
								 const char *request_method, bool header_only) {
#if _MSC_VER && !defined(_DEBUG)
	LPEXCEPTION_POINTERS system_exception=0;
	__try {
#endif
		real_parser_handler(
			filespec_to_process,
			request_method, header_only);
		
#if _MSC_VER && !defined(_DEBUG)
	} __except (
		(system_exception=GetExceptionInformation()), 
		EXCEPTION_EXECUTE_HANDLER) {
		
		if(system_exception)
			if(_EXCEPTION_RECORD *er=system_exception->ExceptionRecord)
				throw Exception(0,
					0,
					"Exception 0x%08X at 0x%08X", er->ExceptionCode,  er->ExceptionAddress);
			else
				throw Exception(0, 0, "Exception <no exception record>");
			else
				throw Exception(0, 0, "Exception <no exception information>");
	}
#endif
}

#if _MSC_VER
int failed_new(size_t size) {
	SAPI::die("out of memory in 'new', failed to allocated %u bytes", size);
	return 0; // not reached
}
#endif

#ifdef HAVE_SET_NEW_HANDLER
void failed_new() {
    SAPI::die("out of memory in 'new'");
}
#endif

int main(int argc, char *argv[]) {
	Pool_storage global_pool_storage;
	Pool global_pool(&global_pool_storage);
	pool=&global_pool;

#ifdef _DEBUG
//	_crtBreakAlloc=33112;
#endif
//	_asm int 3;
	argv0=argv[0];

	umask(2);

	// were we started as CGI?
	cgi=
		getenv("SERVER_SOFTWARE") || 
		getenv("SERVER_NAME") || 
		getenv("GATEWAY_INTERFACE") || 
		getenv("REQUEST_METHOD");
	
	if(!cgi) {
		if(argc<2) {
			printf(
				"Parser/%s Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)\n"
				"Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\n"
				"\n"
				"Usage: %s <file>\n",
				PARSER_VERSION, 
				argv0?argv0:"parser3");
			return 1;
		}
	}

#ifdef WIN32
	setmode(fileno(stdin), _O_BINARY);
	setmode(fileno(stdout), _O_BINARY);
	setmode(fileno(stderr), _O_BINARY);
#endif

#if _MSC_VER
	_set_new_handler(failed_new);

#ifdef _DEBUG
	// Get current flag
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

	// Turn on leak-checking bit
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	// Set flag to the new value
	_CrtSetDbgFlag( tmpFlag );
//	_CrtSetBreakAlloc(471);

#endif

#endif

#ifdef HAVE_SET_NEW_HANDLER
	std::set_new_handler(failed_new);
#endif

	char *raw_filespec_to_process=cgi?getenv("PATH_TRANSLATED"):argv[1];
	char filespec_to_process[MAX_STRING];
	full_file_spec(raw_filespec_to_process, filespec_to_process, sizeof(filespec_to_process));

	const char *request_method=getenv("REQUEST_METHOD");
	bool header_only=request_method && strcasecmp(request_method, "HEAD")==0;

	try { // global try
		call_real_parser_handler__do_SEH(
			filespec_to_process,
			request_method, header_only);
	} catch(const Exception& e) { // global problem 
		// don't allocate anything on pool here:
		//   possible pool' exception not catch-ed now
		//   and there could be out-of-memory exception

		SAPI::die("exception in request exception handler: %s", e.comment());
#ifndef _DEBUG
	} catch(...) { 
		SAPI::die("<unknown exception>");
#endif
	}
	

#ifndef WIN32
	// 
	if(!cgi)
		SAPI::send_body(*pool, "\n", 1);
#endif
//_asm int 3;
	return 0;
}
