/** @file
	Parser: scripting and CGI main.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: parser3.C,v 1.44 2001/03/24 14:31:01 paf Exp $
*/

#include "pa_config_includes.h"

#ifdef WIN32
#	include <windows.h>
#endif

#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_globals.h"
#include "pa_request.h"

/// IIS refuses to read bigger chunks
const size_t READ_POST_CHUNK_SIZE=0x400*0x400; // 1M 

Pool pool(0); // global pool [dont describe to doxygen: it confuses it with param names]
bool cgi; ///< we were started as CGI?

#ifdef WIN32
/// global system errors into parser exceptions converter
static LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo) {
	char buf[MAX_STRING];
	if(ExceptionInfo && ExceptionInfo->ExceptionRecord) {
		struct _EXCEPTION_RECORD *er=ExceptionInfo->ExceptionRecord;
		snprintf(buf, MAX_STRING, "Exception %#X at %p", 
			er->ExceptionCode, 
			er->ExceptionAddress);
	} else 
		strcpy(buf, "Exception <unknown>");
	
	PTHROW(0, 0,
		0,
		buf);

	return EXCEPTION_EXECUTE_HANDLER; // never reached
}
#endif

//@{
/// SAPI funcs decl
const char *SAPI::get_env(Pool& pool, const char *name) {
 	return getenv(name);
}

uint SAPI::read_post(Pool& pool, char *buf, uint max_bytes) {
	uint read_size=0;
	do {
		int chunk_size=read(fileno(stdin), 
			buf+read_size, min(READ_POST_CHUNK_SIZE, max_bytes-read_size));
		if(chunk_size<0)
			break;
		read_size+=chunk_size;
	} while(read_size<max_bytes);

	return read_size;
}

void SAPI::add_header_attribute(Pool& pool, const char *key, const char *value) {
	if(cgi)
		printf("%s: %s\n", key, value);
}

/// @todo intelligent cache-control
void SAPI::send_header(Pool& pool) {
	if(cgi) {
		puts("expires: Fri, 23 Mar 2001 09:32:23 GMT");

		// header | body  delimiter
		puts("");
	}
}

void SAPI::send_body(Pool& pool, const char *buf, size_t size) {
	stdout_write(buf, size);
}

void SAPI::log(Pool& pool, const char *fmt, ...) {
#if 1 //winnt
	if(HANDLE elh=OpenEventLog(0, "Application")) {
		va_list args;
		va_start(args,fmt);
		char buf[MAX_STRING];
		vsnprintf(buf, MAX_STRING, fmt, args);
		
		const char *strings[]={buf};
		ReportEvent(elh, EVENTLOG_ERROR_TYPE, 0, 
			1234, 0, 1, 0, strings, 0);

		va_end(args);
		CloseEventLog(elh);
	} else {
		DWORD id=GetLastError();
		if(FILE *f=fopen("c:\\temp\\a", "wt")) {
		fprintf(f, "%d",id);
		fclose(f);
		}
	}
#else
	// todo: file
#endif
}
//@}

/**
	main workhorse

	@todo 
		IIS: remove trailing default-document[index.html] from $request.uri.
		to do that we need to consult metabase,
		wich is tested but seems slow.
*/
int main(int argc, char *argv[]) {
	umask(2);

#ifdef WIN32
	setmode(fileno(stdin), _O_BINARY);
	setmode(fileno(stdout), _O_BINARY);
	setmode(fileno(stderr), _O_BINARY);
#endif

	// were we started as CGI?
	cgi=
		getenv("SERVER_SOFTWARE") || 
		getenv("SERVER_NAME") || 
		getenv("GATEWAY_INTERFACE") || 
		getenv("REQUEST_METHOD");
	
	if(!cgi) {
		if(argc<2) {
			char *binary=argv[0];
			printf("Usage: %s <file>\n", binary?binary:"parser3");
			exit(1);
		}
	}

	char *filespec_to_process=cgi?getenv("PATH_TRANSLATED"):argv[1];
#ifdef WIN32
	back_slashes_to_slashes(filespec_to_process);
#endif

	const char *request_method=getenv("REQUEST_METHOD");
	bool header_only=request_method && strcasecmp(request_method, "HEAD")==0;
	PTRY { // global try
		// must be first in PTRY{}PCATCH
#ifdef WIN32
		SetUnhandledExceptionFilter(&TopLevelExceptionFilter);
		//TODO: initSocks();
#endif

		// init global variables
		pa_globals_init(pool);

		if(!filespec_to_process)
			PTHROW(0, 0,
				0,
				"no file to process");

		// Request info
		Request::Info request_info;
		if(cgi) {
			if(const char *env_document_root=getenv("DOCUMENT_ROOT"))
				request_info.document_root=env_document_root;
			else if(const char *path_info=getenv("PATH_INFO")) {
				// IIS
				size_t len=strlen(filespec_to_process)-strlen(path_info);
				char *buf=(char *)pool.malloc(len+1);
				strncpy(buf, filespec_to_process, len);
				buf[len]=0;
				request_info.document_root=buf;
			} else
				PTHROW(0, 0,
					0,
					"CGI: no PATH_INFO defined(in reinventing DOCUMENT_ROOT)");
		} else {
			static char buf[MAX_STRING];
			strncpy(buf, filespec_to_process, MAX_STRING);
			rsplit(buf, '/');  rsplit(buf, '\\');// strip filename
			request_info.document_root=buf;
		}
		request_info.path_translated=filespec_to_process;
		request_info.method=request_method;
		const char *query_string=getenv("QUERY_STRING");
		request_info.query_string=query_string;
		if(cgi) 
			if(const char *env_request_uri=getenv("REQUEST_URI"))
				request_info.uri=env_request_uri;
			else if(const char *path_info=getenv("PATH_INFO"))
				if(query_string) {
					char *reconstructed_uri=(char *)malloc(
						strlen(path_info)+1/*'?'*/+
						strlen(query_string)+1/*0*/);
					strcpy(reconstructed_uri, path_info);
					strcat(reconstructed_uri, "?");
					strcat(reconstructed_uri, query_string);
					request_info.uri=reconstructed_uri;
				} else
					request_info.uri=path_info;
			else
				PTHROW(0, 0,
					0,
					"CGI: no PATH_INFO defined(in reinventing REQUEST_URI)");
		else
			request_info.uri=0;

		request_info.content_type=getenv("CONTENT_TYPE");
		const char *content_length=getenv("CONTENT_LENGTH");
		request_info.content_length=(content_length?atoi(content_length):0);
		request_info.cookie=getenv("HTTP_COOKIE");

		// prepare to process request
		Request request(pool,
			request_info,
			cgi ? String::UL_HTML_TYPO : String::UL_NO
			);
		
		// some root-controlled location
#ifdef WIN32
		// c:\windows
		static char root_auto_path[MAX_STRING];
		GetWindowsDirectory(root_auto_path, MAX_STRING);
#else
		// ~nobody  todo: figure out a better place
		char *root_auto_path=getenv("HOME");
#endif
		
		// beside by binary
		static char site_auto_path[MAX_STRING];
		strncpy(site_auto_path, argv[0], MAX_STRING);  // filespec of my binary
		rsplit(site_auto_path, '/');  rsplit(site_auto_path, '\\');// strip filename
		
		// process the request
		request.core(
			root_auto_path, false,
			site_auto_path, false,
			header_only);

		// must be last in PTRY{}PCATCH
#ifdef WIN32
		SetUnhandledExceptionFilter(0);
#endif
		// successful finish
		return 0;
	} PCATCH(e) { // global problem 
		// must be first in PCATCH{}
#ifdef WIN32
		SetUnhandledExceptionFilter(0);
#endif
		// don't allocate anything on pool here:
		//   possible pool' exception not catch-ed now
		//   and there could be out-of-memory exception

		const char *body=e.comment();
		// log it
		SAPI::log(pool, "exception in request exception handler: %s", body);

		//
		int content_length=strlen(body);

		// prepare header
		SAPI::add_header_attribute(pool, "content-type", "text/plain");
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%lu", content_length);
		SAPI::add_header_attribute(pool, "content-length", content_length_cstr);

		// send header
		SAPI::send_header(pool);

		// body
		if(!header_only)
			SAPI::send_body(pool, body, content_length);

		// unsuccessful finish
		return 1;
	}
	PEND_CATCH
}
