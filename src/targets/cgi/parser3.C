/** @file
	Parser: scripting and CGI main.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: parser3.C,v 1.136 2001/11/16 12:38:44 paf Exp $
*/

#include "pa_config_includes.h"

#ifdef WIN32
#	include <windows.h>
#	include <new.h>
#endif

#include "pa_sapi.h"
#include "classes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_socks.h"
#include "pa_version.h"
#include "pool_storage.h"

#ifdef XML
#include <XalanTransformer/XalanCAPI.h>
#endif

//#define DEBUG_POOL_MALLOC

// consts

extern const char *main_RCSIds[];
#ifdef USE_SMTP
extern const char *smtp_RCSIds[];
#endif
extern const char *gd_RCSIds[];
extern const char *classes_RCSIds[];
extern const char *types_RCSIds[];
extern const char *parser3_RCSIds[];
#ifdef XML
extern const char *xalan_patched_RCSIds[];
#endif
const char **RCSIds[]={
	main_RCSIds,
#ifdef USE_SMTP
	smtp_RCSIds,
#endif
	gd_RCSIds,
	classes_RCSIds,
	types_RCSIds,
	parser3_RCSIds,
#ifdef XML
	xalan_patched_RCSIds,
#endif
	0
};

/// IIS refuses to read bigger chunks
const size_t READ_POST_CHUNK_SIZE=0x400*0x400; // 1M 

const char *argv0;
Pool_storage pool_storage;
Pool pool(&pool_storage); // global pool [dont describe to doxygen: it confuses it with param names]
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
	const char *stamp=ctime(&t);
	fprintf(f, "[%.*s] ", strlen(stamp)-1, stamp);
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
    va_list args;
	va_start(args,fmt);
	::log(fmt, args);
	va_end(args);

	char body[MAX_STRING];
	size_t size=vsnprintf(body, MAX_STRING, fmt, args);

	//
	int content_length=strlen(body);

	// prepare header
	// let's be honest, that's bad we couldn't produce valid output
	SAPI::add_header_attribute(pool, "status", "500");
	SAPI::add_header_attribute(pool, "content-type", "text/plain");
	char content_length_cstr[MAX_NUMBER];
	snprintf(content_length_cstr, MAX_NUMBER, "%u", content_length);
	SAPI::add_header_attribute(pool, "content-length", content_length_cstr);

	// send header
	SAPI::send_header(pool);

	// body
	SAPI::send_body(pool, body, content_length);

	exit(1);
}

const char *SAPI::get_env(Pool& , const char *name) {
	return getenv(name);
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
		puts("expires: Fri, 23 Mar 2001 09:32:23 GMT");

		// header | body  delimiter
		puts("");
	}
}

void SAPI::send_body(Pool& , const void *buf, size_t size) {
	stdout_write(buf, size);
}

//

char *full_file_spec(char *file_name) {
	if(file_name && !strchr(file_name, '/')) {
		static char cwd[MAX_STRING];  getcwd(cwd, MAX_STRING);
		static char buf[MAX_STRING];
		snprintf(buf, MAX_STRING, "%s/%s", cwd, file_name);
		return buf;
	}
	return file_name;
}

/**
main workhorse

  @todo 
		IIS: remove trailing default-document[index.html] from $request.uri.
		to do that we need to consult metabase,
		wich is tested but seems slow.
*/
void real_parser_handler(
					const char *filespec_to_process,
					const char *request_method, bool header_only) {
	// init socks
	init_socks(pool);
	
#ifdef XML
	/**
	* Initialize Xerces and Xalan.
	*
	* Should be called only once per process before making
	* any other API calls.
	*/
	//_asm int 3;
	XalanInitialize();
#endif
	
	// init global classes
	init_methoded_array(pool);
	// init global variables
	pa_globals_init(pool);
	
	if(!filespec_to_process)
		throw Exception(0, 0,
		0,
		"Parser/%s", PARSER_VERSION);
	
	// Request info
	Request::Info request_info;
	if(cgi) {
		if(const char *env_document_root=SAPI::get_env(pool, "DOCUMENT_ROOT"))
			request_info.document_root=env_document_root;
		else if(const char *path_info=SAPI::get_env(pool, "PATH_INFO")) {
			// IIS
			size_t len=strlen(filespec_to_process)-strlen(path_info);
			char *buf=(char *)pool.malloc(len+1);
			memcpy(buf, filespec_to_process, len); buf[len]=0;
			request_info.document_root=buf;
		} else
			throw Exception(0, 0,
			0,
			"CGI: no PATH_INFO defined(in reinventing DOCUMENT_ROOT)");
	} else {
		static char buf[MAX_STRING];
		strncpy(buf, filespec_to_process, MAX_STRING-1); buf[MAX_STRING-1]=0;
		if(rsplit(buf, '/') || rsplit(buf, '\\')) // strip filename
			request_info.document_root=buf;
		else
			request_info.document_root="";
	}
	request_info.path_translated=filespec_to_process;
	request_info.method=request_method ? request_method : "GET";
	const char *query_string=SAPI::get_env(pool, "QUERY_STRING");
	request_info.query_string=query_string;
	if(cgi) {
		if(const char *env_request_uri=SAPI::get_env(pool, "REQUEST_URI"))
			request_info.uri=env_request_uri;
		else if(const char *path_info=SAPI::get_env(pool, "PATH_INFO"))
			if(query_string) {
				char *reconstructed_uri=(char *)pool.malloc(
					strlen(path_info)+1/*'?'*/+
					strlen(query_string)+1/*0*/);
				strcpy(reconstructed_uri, path_info);
				strcat(reconstructed_uri, "?");
				strcat(reconstructed_uri, query_string);
				request_info.uri=reconstructed_uri;
			} else
				request_info.uri=path_info;
			else
				throw Exception(0, 0,
				0,
				"CGI: no PATH_INFO defined(in reinventing REQUEST_URI)");
			
			if(const char *script_name=SAPI::get_env(pool, "SCRIPT_NAME")) {
				size_t script_name_len=strlen(script_name);
				size_t uri_len=strlen(request_info.uri);
				if(strncmp(request_info.uri,script_name, script_name_len)==0 &&
					script_name_len != uri_len) // under IIS they are the same
					throw Exception(0, 0,
					0,
					"CGI: illegal call");
			}
	} else
		request_info.uri=0;
	
	request_info.content_type=SAPI::get_env(pool, "CONTENT_TYPE");
	const char *content_length=SAPI::get_env(pool, "CONTENT_LENGTH");
	request_info.content_length=(content_length?atoi(content_length):0);
	request_info.cookie=SAPI::get_env(pool, "HTTP_COOKIE");
	request_info.user_agent=SAPI::get_env(pool, "HTTP_USER_AGENT");
	
	// prepare to process request
	Request request(pool,
		request_info,
		cgi ? String::UL_OPTIMIZED_HTML : String::UL_AS_IS,
		true /* status_allowed */);
	
	// some root-controlled location
#ifdef SYSCONFDIR
	const char *root_config_filespec=SYSCONFDIR "/" CONFIG_FILE_NAME;
#else
#	ifdef WIN32
	// c:\windows
	char root_config_path[MAX_STRING];
	GetWindowsDirectory(root_config_path, MAX_STRING);
	
	char root_config_filespec[MAX_STRING];
	snprintf(root_config_filespec, MAX_STRING, 
		"%s/%s", 
		root_config_path, CONFIG_FILE_NAME);
#	else
#error must be compiled either configure/make or MSVC++
#	endif
#endif
	
	// beside by binary
	// @todo full path, not ./!
	static char site_config_path[MAX_STRING];
	strncpy(site_config_path, argv0, MAX_STRING-1);  site_config_path[MAX_STRING-1]=0; // filespec of my binary
	if(!(
		rsplit(site_config_path, '/') || 
		rsplit(site_config_path, '\\'))) { // strip filename
		// no path, just filename
		site_config_path[0]='.'; site_config_path[1]=0;
	}
	
	char site_config_filespec[MAX_STRING];
	snprintf(site_config_filespec, MAX_STRING, 
		"%s/%s", 
		site_config_path, CONFIG_FILE_NAME);
	
	// process the request
	request.core(
		root_config_filespec, false,
		site_config_filespec, false,
		header_only);
	
	//
	done_socks();
	
#ifdef DEBUG_POOL_MALLOC
	extern void log_pool_stats(Pool& pool);
	log_pool_stats(pool);
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
				throw Exception(0, 0,
				0,
				"Exception 0x%08X at 0x%08X", er->ExceptionCode,  er->ExceptionAddress);
			else
				throw Exception(0, 0, 0, "Exception <no exception record>");
			else
				throw Exception(0, 0, 0, "Exception <no exception information>");
	}
#endif
}

#ifdef WIN32
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
				"Parser/%s Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)\n"
				"Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)\n"
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

	char *filespec_to_process=cgi?getenv("PATH_TRANSLATED"):argv[1];
#ifdef WIN32
	back_slashes_to_slashes(filespec_to_process);
#endif
	filespec_to_process=full_file_spec(filespec_to_process);

	const char *request_method=getenv("REQUEST_METHOD");
	bool header_only=request_method && strcasecmp(request_method, "HEAD")==0;

#ifdef WIN32
	_set_new_handler(failed_new);
#endif

#ifdef HAVE_SET_NEW_HANDLER
	std::set_new_handler(failed_new);
#endif

	try { // global try
		call_real_parser_handler__do_SEH(
			filespec_to_process,
			request_method, header_only);
	} catch(const Exception& e) { // global problem 
		// don't allocate anything on pool here:
		//   possible pool' exception not catch-ed now
		//   and there could be out-of-memory exception

		SAPI::die("exception in request exception handler: ", e.comment());
#ifndef _DEBUG
	} catch(...) { 
		SAPI::die("<unknown exception>");
#endif
	}
	

#ifndef WIN32
	// 
	if(!cgi)
		SAPI::send_body(pool, "\n", 1);
#endif
	return 0;
}
