/** @file
	Parser: scripting and CGI main.

	Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_PARSER3_C="$Date: 2003/11/03 11:24:53 $";

#include "pa_config_includes.h"

#if _MSC_VER
#	include <crtdbg.h>
#endif

#include "pa_sapi.h"
#include "classes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_socks.h"
#include "pa_version.h"



#ifdef WIN32
#	include <windows.h>
#	include "getopt.h"
#else
#	include <getopt.h>
#endif

//#define DEBUG_MAILRECEIVE "mailreceive.eml"

// consts

#define REDIRECT_PREFIX "REDIRECT_"
#define PARSER_CONFIG_ENV_NAME "CGI_PARSER_CONFIG"

/// IIS refuses to read bigger chunks
const size_t READ_POST_CHUNK_SIZE=0x400*0x400; // 1M 

static const char* argv0;
static const char* config_filespec_cstr=0;
static bool fail_on_config_read_problem=true;

static bool cgi; ///< we were started as CGI?
static bool mail_received=false; ///< we were started with -m option? [asked to parse incoming message to $mail:received]

// for signal handlers
Request *request=0;
Request_info *request_info=0;
bool execution_canceled=false;

// SAPI

class SAPI_Info{} SAPI_info;

static void log(const char* fmt, va_list args) {
	bool opened=false;
	FILE *f=0;

	if(config_filespec_cstr) {
		char beside_config_path[MAX_STRING];
		strncpy(beside_config_path, config_filespec_cstr, MAX_STRING-1);  beside_config_path[MAX_STRING-1]=0;
		if(!(
			rsplit(beside_config_path, '/') || 
			rsplit(beside_config_path, '\\'))) { // strip filename
			// no path, just filename
			beside_config_path[0]='.'; beside_config_path[1]=0;
		}

		char file_spec[MAX_STRING];
		snprintf(file_spec, MAX_STRING, 
			"%s/parser3.log", beside_config_path);
		f=fopen(file_spec, "at");
		opened=f!=0;
	}
	// fallback to stderr
	if(!opened)
		f=stderr;

	// use no memory [so that we could log out-of-memory error]
	setbuf(f, 0); // stderr stream is unbuffered by default, but still...

	// prefix
	time_t t=time(0);
	if(const char* stamp=ctime(&t)) { // never saw that
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
void SAPI::log(SAPI_Info&, const char* fmt, ...) {
    va_list args;
	va_start(args,fmt);
	::log(fmt, args);
	va_end(args);
}

static void die_or_abort(const char* fmt, va_list args, bool write_core) {
	// log

	// logging is more important than user 
	// she can cancel download, we'd get SIGPIPE, 
	// nothing would be logged then
	::log(fmt, args);

	// inform user

	char body[MAX_STRING];
	int content_length=vsnprintf(body, MAX_STRING, fmt, args);

	// prepare header
	// let's be honest, that's bad we couldn't produce valid output
	SAPI::add_header_attribute(SAPI_info, "status", "500");
	SAPI::add_header_attribute(SAPI_info, "content-type", "text/plain");
	char content_length_cstr[MAX_NUMBER];
	snprintf(content_length_cstr, sizeof(content_length_cstr), "%u", content_length);
	SAPI::add_header_attribute(SAPI_info, "content-length", content_length_cstr);

	// send header
	SAPI::send_header(SAPI_info);

	// body
	SAPI::send_body(SAPI_info, body, content_length);

	// exit & try to produce core dump[unix] or invoke debugger[Win32 Debug version]
	if(write_core) {
#if defined(WIN32) && !defined(_DEBUG)
		// IIS with abort failes to show STDOUT, it just barks "abnormal program termination"
		exit(1);
#else
#if _MSC_VER
		_asm int 3;
#endif
		abort();
#endif
	} 
	else
		exit(1);
}

void SAPI::die(const char* fmt, ...) {
    va_list args;
	va_start(args, fmt);
	die_or_abort(fmt, args, false/*write core?*/);
	va_end(args);
}

void SAPI::abort(const char* fmt, ...) {
    va_list args;
	va_start(args, fmt);
	die_or_abort(fmt, args, true/*write core?*/);
	va_end(args);
}

char* SAPI::get_env(SAPI_Info& , const char* name) {
	if(char *local=getenv(name))
		return pa_strdup(local);
	else
		return 0;
}

const char* const *SAPI::environment(SAPI_Info&) {
#ifdef _MSC_VER
	extern char **_environ;
	return _environ;
#else
	extern char **environ;
	return environ;
#endif
}

size_t SAPI::read_post(SAPI_Info& , char *buf, size_t max_bytes) {
	size_t read_size=0;
	do {
		ssize_t chunk_size=read(fileno(stdin), 
			buf+read_size, min(READ_POST_CHUNK_SIZE, max_bytes-read_size));
		if(chunk_size<=0)
			break;
		read_size+=chunk_size;
	} while(read_size<max_bytes);

	return read_size;
}

void SAPI::add_header_attribute(SAPI_Info& , const char* dont_store_key, const char* dont_store_value) {
	if(cgi)
		printf("%s: %s\n", dont_store_key, dont_store_value);
}

/// @todo intelligent cache-control
void SAPI::send_header(SAPI_Info& ) {
	if(cgi) {
//		puts("expires: Fri, 23 Mar 2001 09:32:23 GMT");

		// header | body  delimiter
		puts("");
	}
}

void SAPI::send_body(SAPI_Info& , const void *buf, size_t size) {
	stdout_write(buf, size);
}

//

static void full_file_spec(const char* file_name, char *buf, size_t buf_size) {
	if(file_name)
		if(file_name[0]=='/' 
#ifdef WIN32
			|| file_name[0] && file_name[1]==':'
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

static void log_signal(const char* signal_name) {
	if(request_info)
		SAPI::log(SAPI_info, "%s received while %s. uri=%s, method=%s, cl=%u",
			signal_name,
			request?"executing code":"reading data",
			request_info->uri,
			request_info->method,
			request_info->content_length);
	else
		SAPI::log(SAPI_info, "%s received before or after processing request",
			signal_name);
}

#ifdef SIGUSR1
static void SIGUSR1_handler(int /*sig*/){
	log_signal("SIGUSR1");
}
#endif

#ifdef SIGPIPE
static void SIGPIPE_handler(int /*sig*/){
	log_signal("SIGPIPE");
	execution_canceled=true;
	if(request)
		request->set_interrupted(true);
}
#endif

/**
main workhorse

  @todo 
		IIS: remove trailing default-document[index.html] from $request.uri.
		to do that we need to consult metabase,
		wich is tested but seems slow.
*/
static void real_parser_handler(const char* filespec_to_process,
				const char* request_method, bool header_only) {
	// init socks
	pa_init_socks();
	
	// init global variables
	pa_globals_init();
	
	if(!filespec_to_process || !*filespec_to_process)
		SAPI::die("Parser/%s", PARSER_VERSION);
	
	// Request info
	Request_info request_info;  memset(&request_info, 0, sizeof(request_info));
	char document_root_buf[MAX_STRING];
	if(cgi) {
		if(const char* env_document_root=getenv("DOCUMENT_ROOT"))
			request_info.document_root=env_document_root;
		else if(const char* path_info=getenv("PATH_INFO")) {
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
	const char* query_string=getenv("QUERY_STRING");
	request_info.query_string=query_string;
	if(cgi) {
		// few absolute obligatory
		const char* path_info=getenv("PATH_INFO");
		if(!path_info)
			SAPI::die("CGI: illegal call (missing PATH_INFO)");
		const char* script_name=getenv("SCRIPT_NAME");
		if(!script_name)
				SAPI::die("CGI: illegal call (missing SCRIPT_NAME)");

		const char* env_request_uri=getenv("REQUEST_URI");
		if(env_request_uri)
			request_info.uri=env_request_uri;
		else 
			if(query_string) {
				char* reconstructed_uri=new(PointerFreeGC) char[
					strlen(path_info)+1/*'?'*/+
					strlen(query_string)+1/*0*/];
				strcpy(reconstructed_uri, path_info);
				strcat(reconstructed_uri, "?");
				strcat(reconstructed_uri, query_string);
				request_info.uri=reconstructed_uri;
			} else
				request_info.uri=path_info;

		if(env_request_uri) { // apache & others stuck to standards
			/*
				http://parser3/env.html?123  =OK
				$request:uri=/env.html?123
				REQUEST_URI='/env.html?123'
				SCRIPT_NAME='/cgi-bin/parser3'
				PATH_INFO='/env.html'

				http://parser3/cgi-bin/parser3/env.html?123 =ERROR
				$request:uri=/cgi-bin/parser3/env.html?123
				REQUEST_URI='/cgi-bin/parser3/env.html?123'
				SCRIPT_NAME='/cgi-bin/parser3'
				PATH_INFO='/env.html'
			*/
			size_t script_name_len=strlen(script_name);
			size_t uri_len=strlen(env_request_uri);
			if(strncmp(env_request_uri, script_name, script_name_len)==0 &&
				script_name_len != uri_len) // under IIS they are the same
				SAPI::die("CGI: illegal call (1)");
		} else { // seen on IIS5
			/*
				http://nestle/env.html?123 =OK
				$request:uri=/env.html?123
				REQUEST_URI=''
				SCRIPT_NAME='/env.html'
				PATH_INFO='/env.html'

				http://nestle/cgi-bin/parser3.exe/env.html =ERROR
				$request:uri=/env.html
				REQUEST_URI=''
				SCRIPT_NAME='/cgi-bin/parser3.exe'
				PATH_INFO='/env.html'
			*/
			if(strcmp(script_name, path_info)!=0)
				SAPI::die("CGI: illegal call (2)");
		}
	} else
		request_info.uri="";
	
	request_info.content_type=getenv("CONTENT_TYPE");
	const char* content_length=getenv("CONTENT_LENGTH");
	request_info.content_length=(content_length?atoi(content_length):0);
	request_info.cookie=getenv("HTTP_COOKIE");
	request_info.mail_received=mail_received;

	// get request_info ptr for signal handlers
	::request_info=&request_info;
	if(execution_canceled)
		SAPI::die("Execution canceled");

	// prepare to process request
	Request request(SAPI_info, 
		request_info,
		cgi ? String::Language(String::L_HTML|String::L_OPTIMIZE_BIT) : String::L_AS_IS,
		true /* status_allowed */);

	// get request ptr for signal handlers
	::request=&request;

	char config_filespec_buf[MAX_STRING];
	if(!config_filespec_cstr) {
		const char* config_by_env=getenv(PARSER_CONFIG_ENV_NAME);
		if(!config_by_env)
			config_by_env=getenv(REDIRECT_PREFIX PARSER_CONFIG_ENV_NAME);
		if(config_by_env)
			config_filespec_cstr=config_by_env;
		else {
			// beside by binary
			char beside_binary_path[MAX_STRING];
			strncpy(beside_binary_path, argv0, MAX_STRING-1);  beside_binary_path[MAX_STRING-1]=0; // filespec of my binary
			if(!(
				rsplit(beside_binary_path, '/') || 
				rsplit(beside_binary_path, '\\'))) { // strip filename
				// no path, just filename
				// @todo full path, not ./!
				beside_binary_path[0]='.'; beside_binary_path[1]=0;
			}
			snprintf(config_filespec_buf, MAX_STRING, 
				"%s/%s", 
				beside_binary_path, AUTO_FILE_NAME);
			config_filespec_cstr=config_filespec_buf;
			fail_on_config_read_problem=entry_exists(config_filespec_cstr);
		}
	}
	
	// process the request
	request.core(
		config_filespec_cstr, fail_on_config_read_problem,
		header_only);

	// no request [prevent signal handlers from accessing invalid memory]
	::request=0;
	
	//
	pa_done_socks();
}

#if _MSC_VER && !defined(_DEBUG)
#	define PA_USE___TRY
struct Parser_executed {
	bool with_system_exception;
	LPEXCEPTION_POINTERS system_exception;
};
#endif

static 
#ifdef PA_USE___TRY
Parser_executed 
#else
void
#endif
call_real_parser_handler__do_SEH_return_it(
					   const char* filespec_to_process,
					   const char* request_method, bool header_only) {
#ifdef PA_USE___TRY
	Parser_executed result={false};
	__try {
#endif
		real_parser_handler(
			filespec_to_process,
			request_method, header_only);
		
#ifdef PA_USE___TRY
	} __except (
		(result.system_exception=GetExceptionInformation()), 
		EXCEPTION_EXECUTE_HANDLER) {
		result.with_system_exception=true;
	}
	return result;
#endif
}

static void call_real_parser_handler__do_SEH(
					     const char* filespec_to_process,
					     const char* request_method, bool header_only) {
#ifdef PA_USE___TRY
	Parser_executed executed=
#endif
		call_real_parser_handler__do_SEH_return_it(filespec_to_process, 
			request_method, header_only);

#ifdef PA_USE___TRY
	if(executed.with_system_exception)
		if(executed.system_exception)
			if(_EXCEPTION_RECORD *er=executed.system_exception->ExceptionRecord)
				throw Exception(0,
					0,
					"Exception 0x%08X at 0x%08X", er->ExceptionCode,  er->ExceptionAddress);
			else
				throw Exception(0, 
					0, 
					"Exception <no exception record>");
		else
			throw Exception(0, 
				0, 
				"Exception <no exception information>");
#endif
}

static void usage(const char* program) {
	printf(
		"Parser/%s Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)\n"
		"Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\n"
		"\n"
		"Usage: %s [options] file\n"
		"Options are:\n"
#ifdef WITH_MAILRECEIVE
		"    -m              Parse mail, put received letter to $mail:received\n"
#endif
		"    -f config_file  Use this config file (/path/to/auto.p)\n"
		"    -h              Display usage information (this message)\n"
		, PARSER_VERSION, 
		program);
	exit(EINVAL);
}

int main(int argc, char *argv[]) {
	GC_java_finalization=0;

#ifndef PA_DEBUG_DISABLE_GC
	// Dont collect unless explicitly requested 
	// this is quicker (~30% ), but less memory-efficient(~8%)
	// so deciding for speed
	GC_dont_gc=1; 
#endif
/*
	
	Array<int> test;
	test+=3;
	test+=4;
//	int a=test.count();
	int i=0;
	scanf("%d", &i);
	int b=test.get(i);
//	int b=test.get(10);
	printf("%d", b);//test.count());*/

#ifdef SIGUSR1
    if(signal(SIGUSR1, SIGUSR1_handler)==SIG_ERR)
		SAPI::die("Can not set handler for SIGUSR1");
#endif
#ifdef SIGPIPE
    if(signal(SIGPIPE, SIGPIPE_handler)==SIG_ERR)
		SAPI::die("Can not set handler for SIGPIPE");
#endif


#ifdef DEBUG_MAILRECEIVE
	if(FILE *fake_in=fopen(DEBUG_MAILRECEIVE, "rt")) {
		dup2(fake_in->_file, 0/*STDIN_FILENO*/);
	}
#endif

#ifdef _DEBUG
	//_crtBreakAlloc=4042;
#endif
	argv0=argv[0];

	umask(2);

	// were we started as CGI?
	cgi=
		getenv("SERVER_SOFTWARE") || 
		getenv("SERVER_NAME") || 
		getenv("GATEWAY_INTERFACE") || 
		getenv("REQUEST_METHOD");
	
	char *raw_filespec_to_process;
	if(cgi) {
		raw_filespec_to_process=getenv("PATH_TRANSLATED");
		if(raw_filespec_to_process && !*raw_filespec_to_process)
			raw_filespec_to_process=0;
	} else {
		optind = 1;
		opterr = 0;
		int c;
		while((c = getopt(argc, argv, "hf:"
#ifdef WITH_MAILRECEIVE
			"m"
#endif
			)) > 0) {
			switch (c) {
			case 'h':
				usage(argv[0]);
				break;
			case 'f':
				config_filespec_cstr=optarg;
				break;
#ifdef WITH_MAILRECEIVE
			case 'm':
				mail_received=true;
				break;
#endif
			default:
				fprintf(stderr, "%s: invalid option '%c'\n", argv[0], optopt);
				usage(argv[0]);
				break;
			}
		}
		if (optind != argc - 1) {
			fprintf(stderr, "%s: file not specified\n", argv[0]);
			usage(argv[0]);
		}
		
		raw_filespec_to_process=argv[optind++];
	}

#ifdef WIN32
	setmode(fileno(stdin), _O_BINARY);
	setmode(fileno(stdout), _O_BINARY);
	setmode(fileno(stderr), _O_BINARY);
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
	// Get current flag
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

	// Turn on leak-checking bit
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	// Set flag to the new value
	_CrtSetDbgFlag( tmpFlag );
//	_CrtSetBreakAlloc(61);

	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR );
#endif

	char filespec_to_process[MAX_STRING];
	full_file_spec(raw_filespec_to_process, filespec_to_process, sizeof(filespec_to_process));

	const char* request_method=getenv("REQUEST_METHOD");
	bool header_only=request_method && strcasecmp(request_method, "HEAD")==0;

	try { // global try
		call_real_parser_handler__do_SEH(
			filespec_to_process,
			request_method, header_only);
	} catch(const Exception& e) { // global problem 
		// don't allocate anything on pool here:
		//   possible pool' exception not catch-ed now
		//   and there could be out-of-memory exception
		const char* body=e.comment();
		char buf[MAX_STRING];
		snprintf(buf, MAX_STRING, "exception in request exception handler: %s",
			body);
		// log it
		SAPI::log(SAPI_info, "%s", buf);

		//
		int content_length=strlen(buf);

		// prepare header
		SAPI::add_header_attribute(SAPI_info, "content-type", "text/plain");
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%u", content_length);
		SAPI::add_header_attribute(SAPI_info, "content-length", content_length_cstr);

		// send header
		SAPI::send_header(SAPI_info);

		// send body
		if(!header_only)
			SAPI::send_body(SAPI_info, buf, content_length);

		// unsuccessful finish
	}

	return 0;
}
