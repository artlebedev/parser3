/** @file
	Parser: scripting and CGI main.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

volatile const char * IDENT_PARSER3_C="$Id: parser3.C,v 1.342 2020/12/31 19:48:47 moko Exp $";

#include "pa_config_includes.h"

#include "pa_sapi.h"
#include "classes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_version.h"
#include "pa_threads.h"
#include "pa_vconsole.h"
#include "pa_sapi_info.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#include <windows.h>
#include <direct.h>

extern "C" HANDLE WINAPI GC_CreateThread(LPSECURITY_ATTRIBUTES, DWORD, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);

#else

extern "C" int GC_pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void * /* arg */);

#endif

// defines

// comment remove me after debugging
//#define PA_DEBUG_CGI_ENTRY_EXIT "parser3-debug.log"

#if defined(_MSC_VER) && !defined(_DEBUG)
#	define PA_SUPPRESS_SYSTEM_EXCEPTION
#endif

// consts

#define REDIRECT_PREFIX "REDIRECT_"
#define PARSER_CONFIG_ENV_NAME "CGI_PARSER_CONFIG"
#define PARSER_LOG_ENV_NAME "CGI_PARSER_LOG"

static const char* filespec_to_process = 0; // [file]
static const char* httpd_host_port = 0; // -p option
static const char* config_filespec = 0; // -f option or from env or next to the executable if exists
static bool mail_received = false; // -m option? [asked to parse incoming message to $mail:received]
static char* parser3_filespec = 0; // argv[0]
static char** argv_extra = NULL;

// for error logging
static THREAD_LOCAL Request_info *request_info_4log = NULL; // global for correct log() reporting
static const char* filespec_4log = NULL; // null only if system-wide auto.p used

// SAPI

static void pa_log(const char* fmt, va_list args) {
	bool opened=false;
	FILE *f=0;

	const char* log_by_env=getenv(PARSER_LOG_ENV_NAME);
	if(!log_by_env)
		log_by_env=getenv(REDIRECT_PREFIX PARSER_LOG_ENV_NAME);
	if(log_by_env) {
		f=fopen(log_by_env, "at");
		opened=f!=0;
	}
#ifdef PA_DEBUG_CGI_ENTRY_EXIT
	f=fopen(PA_DEBUG_CGI_ENTRY_EXIT, "at");
	opened=f!=0;
#endif

	if(!opened && filespec_4log) {
		char beside_config_path[MAX_STRING];
		strncpy(beside_config_path, filespec_4log, MAX_STRING-1);  beside_config_path[MAX_STRING-1]=0;
		if(!(rsplit(beside_config_path, '/') || rsplit(beside_config_path, '\\'))) { // strip filename
			// no path, just filename
			strcpy(beside_config_path, ".");
		}

		char file_spec[MAX_STRING];
		snprintf(file_spec, MAX_STRING, "%s/parser3.log", beside_config_path);
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
			fprintf(f, "[%.*s] [%u] ", (int)len-1, stamp, (unsigned int)pa_get_thread_id() );
	}
	// message

	char buf[MAX_LOG_STRING];
	size_t size=vsnprintf(buf, MAX_LOG_STRING, fmt, args);
	size=remove_crlf(buf, buf+size);
	fwrite(buf, size, 1, f);

	if(request_info_4log && request_info_4log->method) {
		fprintf(f, " [uri=%s, method=%s, cl=%lu]\n", request_info_4log->uri ? request_info_4log->uri : "<unknown>", request_info_4log->method, (unsigned long)request_info_4log->content_length);
	} else
		fputs(" [no request info]\n", f);

	if(opened)
		fclose(f);
	else
		fflush(f);
}

void pa_log(const char* fmt, ...) {
	va_list args;
	va_start(args,fmt);
	pa_log(fmt, args);
	va_end(args);
}

// appends to parser3.log located next to the config file if openable, to stderr otherwize
void SAPI::log(SAPI_Info&, const char* fmt, ...) {
	va_list args;
	va_start(args,fmt);
	pa_log(fmt, args);
	va_end(args);
}

void SAPI::die(const char* fmt, ...) {
	va_list args;

	// logging first, first vsnprintf
	va_start(args,fmt);
	pa_log(fmt, args);
	va_end(args);

	// inform user, second vsnprintf
	va_start(args, fmt);
	char message[MAX_STRING];
	vsnprintf(message, MAX_STRING, fmt, args);

	SAPI::send_error(*sapiInfo, message);
	exit(1);
//	va_end(args);
}

char* SAPI::Env::get(SAPI_Info& info, const char* name) {
	return info.get_env(name);
}

bool SAPI::Env::set(SAPI_Info& info, const char* name, const char* value) {
	return info.set_env(name, value);
}

const char* const *SAPI::Env::get(SAPI_Info& info) {
	return info.get_env();
}

size_t SAPI::read_post(SAPI_Info& info, char *buf, size_t max_bytes) {
	return info.read_post(buf, max_bytes);
}

void SAPI::add_header_attribute(SAPI_Info& info, const char* dont_store_key, const char* dont_store_value) {
	info.add_header_attribute(dont_store_key, dont_store_value);
}

void SAPI::send_header(SAPI_Info& info) {
	info.send_header();
}

size_t SAPI::send_body(SAPI_Info& info, const void *buf, size_t size) {
	return info.send_body(buf, size);
}

static void full_disk_path(const char* file_name, char *buf, size_t buf_size) {
	if(file_name[0]=='/' 
#ifdef WIN32
		|| file_name[0] && file_name[1]==':'
#endif
	){
		strncpy(buf, file_name, buf_size-1); buf[buf_size-1]=0;
	} else {
		char cwd[MAX_STRING];
		snprintf(buf, buf_size, "%s/%s", getcwd(cwd, MAX_STRING) ? cwd : "", file_name);
	}
#ifdef WIN32
	back_slashes_to_slashes(buf);
#endif
}

static void log_signal(const char* signal_name) {
	SAPI::log(*sapiInfo, "%s received %s processing request", signal_name, request ? "while" : "before or after");
}

#ifdef SIGPIPE
#define SIGPIPE_NAME "SIGPIPE"
static const String sigpipe_name(SIGPIPE_NAME);
static void SIGPIPE_handler(int /*sig*/){
	Value* sigpipe=0;
	if(request)
		sigpipe=request->main_class.get_element(sigpipe_name);
	if(sigpipe && sigpipe->as_bool())
		log_signal(SIGPIPE_NAME);

	if(request)
		request->set_skip(Request::SKIP_INTERRUPTED);
}
#endif

// requires pa_thread_request() in entry_exists() under Windows
static const char *locate_config(const char *config_filespec_option, const char *executable_path){
	filespec_4log=config_filespec_option;
	if(!filespec_4log)
		filespec_4log=getenv(PARSER_CONFIG_ENV_NAME);
	if(!filespec_4log)
		filespec_4log=getenv(REDIRECT_PREFIX PARSER_CONFIG_ENV_NAME);
	if(!filespec_4log){
			// next to the executable
			char *beside_executable_path = pa_strdup(executable_path);
			bool stripped_filename = rsplit(beside_executable_path, '/') || rsplit(beside_executable_path, '\\');
			filespec_4log = pa_strcat(stripped_filename ? beside_executable_path : "." /* no path, just filename */ , "/" AUTO_FILE_NAME);
			if(entry_exists(filespec_4log))
				return filespec_4log;
#ifdef SYSTEM_CONFIG_FILE
			if(entry_exists(SYSTEM_CONFIG_FILE)){
				filespec_4log=NULL;
				return SYSTEM_CONFIG_FILE;
			}
#endif
			return NULL;
	}
	return filespec_4log;
}

#ifdef WIN32
const char* maybe_reconstruct_IIS_status_in_qs(const char* original) {
	// 404;http://servername/page[?param=value...]
	// ';' should be urlencoded by HTTP standard, so we shouldn't get it from browser 
	// and can consider that as an indication that this is IIS way to report errors

	if(original && isdigit((unsigned char)original[0]) && isdigit((unsigned char)original[1]) && isdigit((unsigned char)original[2]) && original[3]==';'){
		size_t original_len=strlen(original);
		char* reconstructed=new(PointerFreeGC) char[original_len +12/*IIS-STATUS=&*/ +14/*IIS-DOCUMENT=&*/ +1];
		char* cur=reconstructed;
		memcpy(cur, "IIS-STATUS=", 11);	 cur+=11;
		memcpy(cur, original, 3); cur+=3;
		*cur++='&';

		const char* qmark_at=strchr(original, '?');
		memcpy(cur, "IIS-DOCUMENT=", 13);  cur+=13;
		{
			size_t value_len=(qmark_at ? qmark_at-original : original_len)-4;
			memcpy(cur, original+4, value_len);  cur+=value_len;
		}

		if(qmark_at) {
			*cur++='&';
			strcpy(cur, qmark_at+1/*skip ? itself*/);
		} else
			*cur=0;

		return reconstructed;
	}
	
	return original;
}

#define MAYBE_RECONSTRUCT_IIS_STATUS_IN_QS(s) maybe_reconstruct_IIS_status_in_qs(s)
#else
#define MAYBE_RECONSTRUCT_IIS_STATUS_IN_QS(s) s
#endif

class RequestController {
public:
	RequestController(Request* r){
		request=r;
	}
	~RequestController(){
		request=0;
	}
};

class RequestInfoController {
public:
	RequestInfoController(Request_info* rinfo){
		request_info_4log=rinfo;
	}
	~RequestInfoController(){
		request_info_4log=0;
	}
};

/** httpd support */
static const String httpd_class_name("httpd");

static void config_handler(SAPI_Info &info) {
	char document_root_buf[MAX_STRING];
	full_disk_path("", document_root_buf, sizeof(document_root_buf));

	Request_info request_info;
	RequestInfoController ric(&request_info);

	request_info.document_root = document_root_buf;
	request_info.uri = "";
	request_info.argv = argv_extra;

	// prepare to process request
	Request r(info, request_info, String::Language(String::L_HTML|String::L_OPTIMIZE_BIT));
	// only once
	config_filespec = locate_config(config_filespec, parser3_filespec);
	// process main auto.p only
	r.core(config_filespec, false, String::Empty);
}

static void connection_handler(SAPI_Info_HTTPD &info, HTTPD_Connection &connection) {
	Request_info request_info;
	RequestInfoController ric(&request_info);

	try {
		if(!connection.read_header())
			return; // ignore "void" connections
		info.populate_env();

		char document_root_buf[MAX_STRING];
		full_disk_path("", document_root_buf, sizeof(document_root_buf));

		request_info.document_root = document_root_buf;
		request_info.path_translated = filespec_to_process;
		request_info.method = connection.method();
		request_info.query_string = connection.query();
		request_info.uri = request_info.strip_absolute_uri(connection.uri());
		request_info.content_type = connection.content_type();
		request_info.content_length = (size_t)connection.content_length();
		request_info.cookie = info.get_env("HTTP_COOKIE");
		request_info.mail_received = false;
		request_info.argv = argv_extra;

		// prepare to process request
		Request r(info, request_info, String::Language(String::L_HTML|String::L_OPTIMIZE_BIT));
		// process the request
		r.core(config_filespec, strcasecmp(request_info.method, "HEAD")==0, main_method_name, &httpd_class_name);
	} catch(const Exception& e) { // exception in connection handling or unhandled exception
		SAPI::log(info, "%s", e.comment());
		const char *status = info.exception_http_status(e.type());
		if(*status){
			info.clear_response_headers();
			SAPI::send_error(info, e.comment(), status);
		}
	}
}

#ifdef _MSC_VER
DWORD WINAPI connection_thread(void *arg){
#else
static void *connection_thread(void *arg){
#endif
	HTTPD_Connection &connection=*(HTTPD_Connection*)arg;
	SAPI_Info_HTTPD info(connection);

	try {
		connection_handler(info, connection);
	} catch(const Exception& e) { // exception in send_error
		SAPI::log(*sapiInfo, "%s", e.comment());
	}

	delete(&connection);
	return 0;
}

static void httpd_mode() {
	config_handler(*sapiInfo);

	int sock = HTTPD_Server::bind(httpd_host_port);

#ifdef SIGCHLD
	signal(SIGCHLD, SIG_IGN);
#endif
#ifdef SIGPIPE
	signal(SIGPIPE, SIG_IGN);
#endif

	while(1){
#ifndef _MSC_VER
		pid_t pid=1;
#endif
		try {
			HTTPD_Connection connection;
			if(!connection.accept(sock, 5))
				continue;

			switch (HTTPD_Server::mode) {
				case HTTPD_Server::MULTITHREADED:
#ifdef _MSC_VER
					if (!GC_CreateThread(0, 0, connection_thread, new HTTPD_Connection(connection), 0, 0))
						throw Exception("httpd.fork", 0, "thread creation failed");
					connection.sock = -1;
					break;
#else
#ifdef HAVE_TLS
					pthread_t thread;
					pthread_attr_t attr;
					pthread_attr_init(&attr);
					pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

					if(int result=GC_pthread_create(&thread, &attr, connection_thread, new HTTPD_Connection(connection)))
						throw Exception("httpd.fork", 0, "thread creation failed (%d)", result);
					connection.sock=-1;
					break;
#endif
				case HTTPD_Server::PARALLEL:
					pid=fork();
					if(pid<0)
						throw Exception("httpd.fork", 0, "fork failed: %s (%d)", strerror(errno), errno);
					if(pid>0)
						continue; // parent should close connection.sock as well
#endif
				case HTTPD_Server::SEQUENTIAL: // and fork child

					SAPI_Info_HTTPD info(connection);
					connection_handler(info, connection);
			}
			// closing connection socket in HTTPD_Connection destructor
		} catch(const Exception& e) { // exception in accept or send_error
			SAPI::log(*sapiInfo, "%s", e.comment());
		}

#ifndef _MSC_VER
		if(pid==0) // fork child
			exit(0);
#endif
	}
}

/** main workhorse */

static void real_parser_handler(bool cgi) {
	// init libraries
	pa_globals_init();

	if(httpd_host_port){
		httpd_mode();
	}

	const char* request_method=getenv("REQUEST_METHOD");

	if(!filespec_to_process)
		SAPI::die("Parser/%s", PARSER_VERSION);
	
	char document_root_buf[MAX_STRING];

	// global request info
	Request_info request_info;
	RequestInfoController ric(&request_info);

	request_info.path_translated = filespec_to_process;
	request_info.method = request_method ? request_method : "GET";
	request_info.query_string = MAYBE_RECONSTRUCT_IIS_STATUS_IN_QS(getenv("QUERY_STRING"));

	if(cgi) {
		// obligatory
		const char* path_info=getenv("PATH_INFO");
		if(!path_info)
			SAPI::die("CGI: illegal call (missing PATH_INFO)");
		
		request_info.document_root = getenv("DOCUMENT_ROOT");
		if(!request_info.document_root) {
			// IIS or fcgiwrap minimalistic setup
			ssize_t prefix_len = strlen(filespec_to_process) - strlen(path_info);
			if(prefix_len < 0 || strcmp(filespec_to_process + prefix_len, path_info) != 0)
				SAPI::die("CGI: illegal call (invalid PATH_INFO in reinventing DOCUMENT_ROOT)");

			char* document_root = new(PointerFreeGC) char[prefix_len + 1/*0*/];
			memcpy(document_root, filespec_to_process, prefix_len); document_root[prefix_len] = 0;
			request_info.document_root = document_root;
		}

		request_info.uri = request_info.strip_absolute_uri(getenv("REQUEST_URI"));
		if(request_info.uri) { // apache & others stuck to standards
			// another obligatory
			const char* script_name = getenv("SCRIPT_NAME");
			if(!script_name)
				SAPI::die("CGI: illegal call (missing SCRIPT_NAME)");
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
			size_t script_name_len = strlen(script_name);
			size_t uri_len = strlen(request_info.uri);
			if(strncmp(request_info.uri, script_name, script_name_len)==0 && script_name_len != uri_len) // under IIS they are the same
				SAPI::die("CGI: illegal call (1)");
		} else { // fcgiwrap minimalistic setup
			request_info.uri = request_info.query_string && *request_info.query_string ? pa_strcat(path_info, "?", request_info.query_string) : path_info;
		}
	} else{
		full_disk_path("", document_root_buf, sizeof(document_root_buf));
		request_info.document_root = document_root_buf;
		request_info.uri = "";
	}
	
	request_info.content_type = getenv("CONTENT_TYPE");
	request_info.content_length = (size_t)pa_atoul(getenv("CONTENT_LENGTH"));
	request_info.cookie = getenv("HTTP_COOKIE");
	request_info.mail_received = mail_received;

	request_info.argv = argv_extra;

#ifdef PA_DEBUG_CGI_ENTRY_EXIT
	log("request_info: method=%s, uri=%s, q=%s, dr=%s, pt=%s", request_info.method, request_info.uri, request_info.query_string, request_info.document_root, request_info.path_translated);
#endif

	// prepare to process request
	Request r(*sapiInfo, request_info, cgi ? String::Language(String::L_HTML|String::L_OPTIMIZE_BIT) : String::L_AS_IS);
	{
		// initing ::request ptr for signal handlers
		RequestController rc(&r);
		// process the request
		r.core(locate_config(config_filespec, parser3_filespec), strcasecmp(request_info.method, "HEAD")==0);
		// clearing ::request in RequestController destructor to prevent signal handlers from accessing invalid memory
	}

	// finalize libraries
	pa_globals_done();
}

#ifdef PA_SUPPRESS_SYSTEM_EXCEPTION
static const Exception call_real_parser_handler__do_PEH_return_it(bool cgi) {
	try {
		real_parser_handler(cgi);
	} catch(const Exception& e) {
		return e;
	}

	return Exception();
}

static void call_real_parser_handler__supress_system_exception(bool cgi) {
	Exception parser_exception;
	LPEXCEPTION_POINTERS system_exception=0;

	__try {
		parser_exception=call_real_parser_handler__do_PEH_return_it(cgi);
	} __except ( (system_exception=GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER) {
		if(system_exception)
			if(_EXCEPTION_RECORD *er=system_exception->ExceptionRecord)
				throw Exception("system", 0, "0x%08X at 0x%08X", er->ExceptionCode,  er->ExceptionAddress);
			else
				throw Exception("system", 0, "<no exception record>");
		else
			throw Exception("system", 0, "<no exception information>");
	}

	if(parser_exception)
		throw Exception(parser_exception);
}

#define REAL_PARSER_HANDLER call_real_parser_handler__supress_system_exception
#else
#define REAL_PARSER_HANDLER real_parser_handler
#endif

static void usage(const char* program) {
	printf(
		"Parser/%s\n"
		"Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)\n"
		"Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\n"
		"\n"
		"Usage: %s [options] [file]\n"
		"Options are:\n"
#ifdef WITH_MAILRECEIVE
		"    -m              Parse mail, put received letter to $mail:received\n"
#endif
		"    -f config_file  Use this config file (/path/to/auto.p)\n"
		"    -p [host:]port  Start web server on this port\n"
		"    -h              Display usage information (this message)\n",
		PARSER_VERSION,
		program);
	exit(EINVAL);
}


int main(int argc, char *argv[]) {
#ifdef PA_DEBUG_CGI_ENTRY_EXIT
	log("main: entry");
#endif

	parser3_filespec = argv[0];
	umask(2);

	// were we started as CGI?
	bool cgi=(getenv("SERVER_SOFTWARE") || getenv("SERVER_NAME") || getenv("GATEWAY_INTERFACE") || getenv("REQUEST_METHOD")) && !getenv("PARSER_VERSION");
	sapiInfo = cgi ? new SAPI_Info_CGI() : new SAPI_Info();

#ifdef SIGPIPE
	signal(SIGPIPE, SIGPIPE_handler);
#endif

	char *raw_filespec_to_process = NULL;
	if(cgi) {
		raw_filespec_to_process=getenv("PATH_TRANSLATED");
		argv_extra=argv + 1;
	} else {
		int optind=1;
		while(optind < argc){
			char *carg = argv[optind];
			if(carg[0] != '-')
				break;

			for(size_t k = 1; k < strlen(carg); k++){
				char c = carg[k];
				switch (c) {
					case 'h':
						usage(argv[0]);
						break;
					case 'f':
						if(optind < argc - 1){
							optind++;
							config_filespec=argv[optind];
						}
						break;
					case 'p':
						if(optind < argc - 1){
							optind++;
							httpd_host_port=argv[optind];
						}
						break;
#ifdef WITH_MAILRECEIVE
					case 'm':
						mail_received=true;
						break;
#endif
					default:
						fprintf(stderr, "%s: invalid option '%c'\n", argv[0], c);
						usage(argv[0]);
						break;
				}
			}
			optind++;
		}
		
		if (optind > argc - 1) {
			if(!httpd_host_port) {
				fprintf(stderr, "%s: file not specified\n", argv[0]);
				usage(argv[0]);
			}
		} else {
			raw_filespec_to_process=argv[optind];
		}

		if (httpd_host_port && mail_received) {
				fprintf(stderr, "%s: -p and -m options should not be used together\n", argv[0]);
				usage(argv[0]);
		}

		argv_extra=argv + optind;
	}

#ifdef _MSC_VER
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

	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR );
#endif

	try { // global try
		char filespec_to_process_buf[MAX_STRING];
		if(raw_filespec_to_process && *raw_filespec_to_process){
			full_disk_path(raw_filespec_to_process, filespec_to_process_buf, sizeof(filespec_to_process_buf));
			filespec_to_process=filespec_to_process_buf;
		}

		REAL_PARSER_HANDLER(cgi);
	} catch(const Exception& e) { // exception in unhandled exception
		SAPI::die("%s", e.comment());
	}

#ifdef PA_DEBUG_CGI_ENTRY_EXIT
	log("main: successful return");
#endif
	return sapiInfo->http_response_code < 100 ? sapiInfo->http_response_code : 0;
}
