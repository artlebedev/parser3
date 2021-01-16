/** @file
	Parser: IIS extension.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

volatile const char * IDENT_PARSER3ISAPI_C="$Id: parser3isapi.C,v 1.126 2021/01/16 15:47:06 moko Exp $";

#ifndef _MSC_VER
#	error compile ISAPI module with MSVC [no urge for now to make it autoconf-ed (PAF)]
#endif

#include "pa_config_includes.h"

#include "pa_sapi.h"
#include "pa_globals.h"
#include "pa_request.h"
#include "pa_version.h"

#include <windows.h>
#include <process.h>

#include <httpext.h>

// defines

#if defined(_MSC_VER) && !defined(_DEBUG)
#	define PA_SUPPRESS_SYSTEM_EXCEPTION
#endif


#define MAX_STATUS_LENGTH sizeof("xxxx LONGEST STATUS DESCRIPTION")

// consts

const char* IIS51vars[]={
	"APPL_MD_PATH", "APPL_PHYSICAL_PATH",
	"AUTH_PASSWORD", "AUTH_TYPE", "AUTH_USER",
	"CERT_COOKIE", "CERT_FLAGS", "CERT_ISSUER", "CERT_KEYSIZE", "CERT_SECRETKEYSIZE",
	"CERT_SERIALNUMBER", "CERT_SERVER_ISSUER", "CERT_SERVER_SUBJECT", "CERT_SUBJECT",
	"CONTENT_LENGTH", "CONTENT_TYPE",
	"GATEWAY_INTERFACE",
	"HTTPS", "HTTPS_KEYSIZE", "HTTPS_SECRETKEYSIZE", "HTTPS_SERVER_ISSUER", "HTTPS_SERVER_SUBJECT",
	"INSTANCE_ID", 	"INSTANCE_META_PATH",
	"LOCAL_ADDR", "LOGON_USER",
	"PATH_INFO", 	"PATH_TRANSLATED",
	"QUERY_STRING",
	"REMOTE_ADDR", "REMOTE_HOST", "REMOTE_PORT", "REMOTE_USER", "REQUEST_METHOD",
	"SCRIPT_NAME",
	"SERVER_NAME", "SERVER_PORT", "SERVER_PORT_SECURE", "SERVER_PROTOCOL", "SERVER_SOFTWARE",
	"URL",
};
const int IIS51var_count=sizeof(IIS51vars)/sizeof(*IIS51vars);

// globals

char argv0[MAX_STRING]="";

// SAPI

#ifndef DOXYGEN
/*
	ISAPI SAPI functions receive this context information. 
	see Pool::set_context
*/
class SAPI_Info {
public:
	LPEXTENSION_CONTROL_BLOCK lpECB;
	String *header;
	DWORD http_response_code;
};
#endif

// goes to 'cs-uri-query' log file field. webmaster: switch it ON[default OFF].
void SAPI::log(SAPI_Info& SAPI_info, const char* fmt, ...) {
	va_list args;
	va_start(args,fmt);
	char buf[MAX_LOG_STRING];
	const char* prefix="PARSER_ERROR:";
	strcpy(buf, prefix);
	char *start=buf+strlen(prefix);
	DWORD size=vsnprintf(start, MAX_LOG_STRING-strlen(prefix), fmt, args);
	size=remove_crlf(start, start+size);

	SAPI_info.lpECB->ServerSupportFunction(SAPI_info.lpECB->ConnID, HSE_APPEND_LOG_PARAMETER, buf, &size, 0);
}

/// @todo event log
void SAPI::die(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	if(FILE *log=fopen("c:\\parser3die.log", "at")) {
		vfprintf(log, fmt, args);
		fclose(log);
	}
	// abnormal exit
	abort();
//	va_end(args);
}

char* SAPI::Env::get(SAPI_Info& SAPI_info, const char* name) {
	char *variable_buf=new(PointerFreeGC) char[MAX_STRING];
	DWORD variable_len = MAX_STRING-1;

	if(SAPI_info.lpECB->GetServerVariable(SAPI_info.lpECB->ConnID, const_cast<char *>(name), 
		variable_buf, &variable_len)) {
		if(*variable_buf) { // saw returning len=1 && *buf=0 :(
			variable_buf[variable_len]=0;
			return variable_buf;
		}
	} else if (GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
		variable_buf=new(PointerFreeGC) char[variable_len+1];
		
		if(SAPI_info.lpECB->GetServerVariable(SAPI_info.lpECB->ConnID, const_cast<char *>(name), 
			variable_buf, &variable_len)) {
			if(*variable_buf) {
				variable_buf[variable_len]=0;
				return variable_buf;
			}
		}
	}
		
	return 0;
}

bool SAPI::Env::set(SAPI_Info&, const char*, const char*) {
	return false;
}

static int grep_char(const char* s, char c) {
	int result=0;
	if(s) {
		while(s=strchr(s, c)) {
			s++; // skip found c
			result++;
		}
	}
	return result;
}

const char* const *SAPI::Env::get(SAPI_Info& info) {
	// we know this buf is writable
	char* all_http_vars=SAPI::Env::get(info, "ALL_HTTP");
	const int http_var_count=grep_char(all_http_vars, '\n')+1/*\n for theoretical(never saw) this \0*/;

	const char* *result=new(PointerFreeGC) const char*[IIS51var_count+http_var_count+1/*0*/];
	const char* *cur=result;

	// IIS5.1 vars
	for(int i=0; i<IIS51var_count; i++) {
		const char* key=IIS51vars[i];
		if(const char* value=SAPI::Env::get(info, key))
			*cur++=pa_strcat(key, "=", value);
	}

	// HTTP_* vars
	if(char *s=all_http_vars) {
		while(char *key=lsplit(&s, '\n'))
			if(char *value=lsplit(key, ':'))
				*cur++=pa_strcat(key, "=", value);
	}
	
	// mark EOE
	*cur=0;

	return result;
}

size_t SAPI::read_post(SAPI_Info& SAPI_info, char *buf, size_t max_bytes) {
	DWORD read_from_buf=0;
	DWORD read_from_input=0;
	DWORD total_read=0;

	read_from_buf=min(SAPI_info.lpECB->cbAvailable, max_bytes);
	memcpy(buf, SAPI_info.lpECB->lpbData, read_from_buf);
	total_read+=read_from_buf;

	if(read_from_buf<max_bytes &&
		read_from_buf<SAPI_info.lpECB->cbTotalBytes) {
		DWORD cbRead=0, cbSize;

		read_from_input=min(max_bytes-read_from_buf, 
			SAPI_info.lpECB->cbTotalBytes-read_from_buf);
		while(cbRead < read_from_input) {
			cbSize=read_from_input - cbRead;
			if(!SAPI_info.lpECB->ReadClient(SAPI_info.lpECB->ConnID, 
				buf+read_from_buf+cbRead, &cbSize) || 
				cbSize==0) 
				break;
			cbRead+=cbSize;
		}
		total_read+=cbRead;
	}
	return total_read;
}

void SAPI::add_header_attribute(SAPI_Info& SAPI_info, const char* dont_store_key, const char* dont_store_value) {
	if(strcasecmp(dont_store_key, "location")==0)
		SAPI_info.http_response_code=302;

	if(strcasecmp(dont_store_key, HTTP_STATUS)==0)
		SAPI_info.http_response_code=atoi(dont_store_value);
	else
		(*SAPI_info.header) << capitalize(dont_store_key) << ": " << pa_strdup(dont_store_value) << "\r\n";
}

/// @todo intelligent cache-control
void SAPI::send_header(SAPI_Info& SAPI_info) {
	HSE_SEND_HEADER_EX_INFO header_info;

	char status_buf[MAX_STATUS_LENGTH];
	switch(SAPI_info.http_response_code) {
		case 200:
			header_info.pszStatus="200 OK";
			break;
		case 302:
			header_info.pszStatus="302 Moved Temporarily";
			break;
		case 401:// useless untill parser auth mech
			header_info.pszStatus="401 Authorization Required";
			break;
		default:
			snprintf(status_buf, MAX_STATUS_LENGTH, 
				"%d Undescribed", SAPI_info.http_response_code);
			header_info.pszStatus=status_buf;
			break;
	}
	header_info.cchStatus=strlen(header_info.pszStatus);
	*SAPI_info.header << "\r\n"; // ISAPI v<5 did quite well without it
	header_info.pszHeader=SAPI_info.header->cstr();
	header_info.cchHeader=SAPI_info.header->length();
	header_info.fKeepConn=true;

	SAPI_info.lpECB->dwHttpStatusCode=SAPI_info.http_response_code;

	SAPI_info.lpECB->ServerSupportFunction(SAPI_info.lpECB->ConnID, 
		HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);
}

size_t SAPI::send_body(SAPI_Info& SAPI_info, const void *buf, size_t size) {
	DWORD num_bytes=size;
	if(!SAPI_info.lpECB->WriteClient(SAPI_info.lpECB->ConnID, 
		const_cast<void *>(buf), &num_bytes, HSE_IO_SYNC))
		return 0;
	return (size_t)num_bytes;
}


static bool parser_init() {
	static bool globals_inited=false;
	if(globals_inited)
		return true;
	globals_inited=true;

	try {
		// init libraries
		pa_globals_init();
		// successful finish
		return true;
	} catch(.../*const Exception& e*/) { // global problem 
		// unsuccessful finish
		return false;
	}
}

static void parser_done() {
	// finalize libraries
	pa_globals_done();
}

/// ISAPI //
BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer) {
	pVer->dwExtensionVersion = HSE_VERSION;
	strncpy(pVer->lpszExtensionDesc, "Parser " PARSER_VERSION, HSE_MAX_EXT_DLL_NAME_LEN-1);
	pVer->lpszExtensionDesc[HSE_MAX_EXT_DLL_NAME_LEN-1]=0;
	return parser_init();
}
// dwFlags & HSE_TERM_MUST_UNLOAD means we can't return false
BOOL WINAPI TerminateExtension(
  DWORD /*dwFlags*/
)
{
	parser_done();

	return TRUE;
}

/** 
	ISAPI // main workhorse

	@todo 
		IIS: remove trailing default-document[index.html] from $request.uri.
		to do that we need to consult metabase,
		wich is tested&works but seems slow runtime 
		and not could-be-quickly-implemented if prepared.
	@test
		PARSER_VERSION from outside
*/
void real_parser_handler(SAPI_Info& SAPI_info, bool header_only) {
	// collect garbage from prev request
	pa_gc_collect();
	
	SAPI_info.header=new String;
	LPEXTENSION_CONTROL_BLOCK lpECB=SAPI_info.lpECB;
	
	// Request info
	Request_info request_info;  memset(&request_info, 0, sizeof(request_info));

	char *filespec_to_process=pa_strdup(lpECB->lpszPathTranslated);
#ifdef WIN32
	back_slashes_to_slashes(filespec_to_process);
#endif

	if(const char* path_info=SAPI::Env::get(SAPI_info, "PATH_INFO")) {
		// IIS
		size_t len=strlen(filespec_to_process)-strlen(path_info);
		char *buf=new(PointerFreeGC) char[len+1];
		strncpy(buf, filespec_to_process, len); buf[len]=0;
		request_info.document_root=buf;
	} else
		throw Exception(PARSER_RUNTIME, 0, "ISAPI: no PATH_INFO defined (in reinventing DOCUMENT_ROOT)");

	request_info.path_translated=filespec_to_process;
	request_info.method=lpECB->lpszMethod;
	request_info.query_string=lpECB->lpszQueryString;
	request_info.uri=lpECB->lpszQueryString && *lpECB->lpszQueryString ? pa_strcat(lpECB->lpszPathInfo, "?", lpECB->lpszQueryString) : lpECB->lpszPathInfo;
	request_info.content_type=lpECB->lpszContentType;
	request_info.content_length=lpECB->cbTotalBytes;
	request_info.cookie=SAPI::Env::get(SAPI_info, "HTTP_COOKIE");
	request_info.mail_received=false;
	
	// prepare to process request
	Request request(SAPI_info, request_info, String::Language(String::L_HTML|String::L_OPTIMIZE_BIT));

	// beside by binary
	static char beside_binary_path[MAX_STRING];
	strncpy(beside_binary_path, argv0, MAX_STRING-1);  beside_binary_path[MAX_STRING-1]=0; // filespec of my binary
	if(!(rsplit(beside_binary_path, '/') || rsplit(beside_binary_path, '\\'))) { // strip filename
		// no path, just filename
		beside_binary_path[0]='.'; beside_binary_path[1]=0;
	}
	char config_filespec[MAX_STRING];
	snprintf(config_filespec, MAX_STRING, "%s/%s", beside_binary_path, AUTO_FILE_NAME);

	// process the request
	request.core(entry_exists(config_filespec) ? config_filespec : NULL, header_only);
}

#ifdef PA_SUPPRESS_SYSTEM_EXCEPTION
static const Exception call_real_parser_handler__do_PEH_return_it(SAPI_Info& SAPI_info, bool header_only) {
	try {
		real_parser_handler(SAPI_info, header_only);
	} catch(const Exception& e) {
		return e;
	}

	return Exception();
}

static void call_real_parser_handler__supress_system_exception(SAPI_Info& SAPI_info, bool header_only) {
	Exception parser_exception;
	LPEXCEPTION_POINTERS system_exception=0;

	__try {
		parser_exception=call_real_parser_handler__do_PEH_return_it(SAPI_info, header_only);
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
#endif

DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB) {
	//_asm int 3;
	SAPI_Info SAPI_info={
		lpECB,
		0, // filling later: so that if there would be error pool would have SAPI_info
		200 // default http_response_code [lpECB->dwHttpStatusCode seems to be always 0, even on 404 redirect to /404.html]
	};

	bool header_only=strcasecmp(lpECB->lpszMethod, "HEAD")==0;
	try { // global try
#ifdef PA_SUPPRESS_SYSTEM_EXCEPTION
		call_real_parser_handler__supress_system_exception(
#else
		real_parser_handler(
#endif
			SAPI_info, header_only);
		// successful finish
	} catch(const Exception& e) { // exception in unhandled exception
		// log it
		SAPI::log(SAPI_info, "%s", e.comment());

		HSE_SEND_HEADER_EX_INFO header_info;
		header_info.pszStatus="200 OK";
		header_info.cchStatus=strlen(header_info.pszStatus);
		header_info.pszHeader=HTTP_CONTENT_TYPE_CAPITALIZED ": text/plain\r\n\r\n";
		header_info.cchHeader=strlen(header_info.pszHeader);
		header_info.fKeepConn=true;
		
		// send header
		lpECB->dwHttpStatusCode=200;
		lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);

		// send body
		if(!header_only)
			SAPI::send_body(SAPI_info, e.comment(), strlen(e.comment()));

		// unsuccessful finish
	}
	return HSE_STATUS_SUCCESS_AND_KEEP_CONN;
}

BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,  // handle to the DLL module
  DWORD /*fdwReason*/,     // reason for calling function
  LPVOID /*lpvReserved*/   // reserved
  ) {

	GetModuleFileName(
	  hinstDLL,    // handle to module
	  argv0,  // file name of module
	  sizeof(argv0)         // size of buffer
	);

	return TRUE;
}