#ifndef _MSC_VER
#	error compile ISAPI module with MSVC
#endif

#include <windows.h>
#include <process.h>

#include <httpext.h>
#include <httpfilt.h>
#include <httpext.h>

#include "pa_globals.h"
#include "pa_request.h"
#include "pa_version.h"

#define MAX_STATUS_LENGTH sizeof("xxxx LONGEST STATUS DESCRIPTION")
#define ISAPI_SERVER_VAR_BUF_SIZE 0x400

//@{
/// service func decl
static const char *get_env(Pool& pool, const char *name) {
	LPEXTENSION_CONTROL_BLOCK lpECB=static_cast<LPEXTENSION_CONTROL_BLOCK>(pool.context());

	char *variable_buf=(char *)pool.malloc(MAX_STRING);
	DWORD variable_len = MAX_STRING-1;

	if(lpECB->GetServerVariable(lpECB->ConnID, const_cast<char *>(name), 
		variable_buf, &variable_len)) {
		variable_buf[variable_len]=0;
		return variable_buf;
	}

	variable_buf=(char *)pool.malloc(variable_len+1);
	
	if(lpECB->GetServerVariable(lpECB->ConnID, const_cast<char *>(name), 
		variable_buf, &variable_len)) {
		variable_buf[variable_len]=0;
		return variable_buf;
	}

	return 0;
}

static uint read_post(Pool& pool, char *buf, uint max_bytes) {
	LPEXTENSION_CONTROL_BLOCK lpECB=static_cast<LPEXTENSION_CONTROL_BLOCK>(pool.context());

	// todo: timeout
	DWORD read_from_buf=0;
	DWORD read_from_input=0;
	DWORD total_read=0;

	read_from_buf=min(lpECB->cbAvailable, max_bytes);
	memcpy(buf, lpECB->lpbData, read_from_buf);
	total_read+=read_from_buf;

	if(read_from_buf<max_bytes &&
		read_from_buf<lpECB->cbTotalBytes) {
		DWORD cbRead=0, cbSize;

		read_from_input=min(max_bytes-read_from_buf, lpECB->cbTotalBytes-read_from_buf);
		while(cbRead < read_from_input) {
			cbSize=read_from_input - cbRead;
			if(!lpECB->ReadClient(lpECB->ConnID, buf+read_from_buf+cbRead, &cbSize) || 
				cbSize==0) 
				break;
			cbRead+=cbSize;
		}
		total_read+=cbRead;
	}
	return total_read;
}

static void add_header_attribute(Pool& pool, const char *key, const char *value) {
	LPEXTENSION_CONTROL_BLOCK lpECB=static_cast<LPEXTENSION_CONTROL_BLOCK>(pool.context());
	String *header=static_cast<String *>(pool.tag());
	if(!header) 
		pool.set_tag(header=new(pool) String(pool));

	header->APPEND_CONST(key);
	header->APPEND_CONST(": ");
	header->APPEND_CONST(value);
	header->APPEND_CONST("\r\n");
}

static void send_header(Pool& pool) {
	LPEXTENSION_CONTROL_BLOCK lpECB=static_cast<LPEXTENSION_CONTROL_BLOCK>(pool.context());
	String *header=static_cast<String *>(pool.tag());
	if(!header) // never
		return;

	header->APPEND_CONST("\r\n");
	HSE_SEND_HEADER_EX_INFO header_info;

	int http_response_code=200; // todo: dig from headers

	char status_buf[MAX_STATUS_LENGTH];
	switch(http_response_code) {
		case 200:
			header_info.pszStatus="200 OK";
			break;
		case 302:
			header_info.pszStatus="302 Moved Temporarily";
			break;
		case 401:
			header_info.pszStatus="401 Authorization Required";
			break;
		default:
			snprintf(status_buf, MAX_STATUS_LENGTH, "%d Undescribed", http_response_code);
			header_info.pszStatus=status_buf;
			break;
	}
	header_info.cchStatus=strlen(header_info.pszStatus);
	header_info.pszHeader=header->cstr();
	header_info.cchHeader=header->size();

	lpECB->dwHttpStatusCode=http_response_code;

	lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, 
		&header_info, NULL, NULL);
}

static void send_body(Pool& pool, const char *buf, size_t size) {
	LPEXTENSION_CONTROL_BLOCK lpECB=static_cast<LPEXTENSION_CONTROL_BLOCK>(pool.context());

	DWORD num_bytes=size;
	lpECB->WriteClient(lpECB->ConnID, const_cast<char *>(buf), &num_bytes, HSE_IO_SYNC);
}
//@}

/// Service funcs 
Service_funcs service_funcs={
	get_env,
	read_post,
	add_header_attribute,
	send_header,
	send_body
};

// 

static void parser_init() {
	static bool globals_inited=false;
	if(globals_inited)
		return;
	globals_inited=true;

	static Pool pool; // global pool
	PTRY {
		// init global variables
		globals_init(pool);
		
		//...
	} PCATCH(e) { // global problem 
		const char *body=e.comment();
		// TODO: somehow report that error
	}
	PEND_CATCH
}

/// ISAPI //
BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer) {
	pVer->dwExtensionVersion = HSE_VERSION;
	strncpy(pVer->lpszExtensionDesc, "Parser " PARSER_VERSION, HSE_MAX_EXT_DLL_NAME_LEN);
	return TRUE;
}

static BOOL exception_handler(LPEXCEPTION_POINTERS *e,LPEXCEPTION_POINTERS ep) {
	*e=ep;
	return TRUE;
}

/// ISAPI // main workhorse
DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB) {
	Pool pool;
	pool.set_context(lpECB);

	// TODO r->no_cache=1;

	bool header_only=strcasecmp(lpECB->lpszMethod, "HEAD")==0;
	PTRY { // global try
		// must be first in PTRY{}PCATCH
		LPEXCEPTION_POINTERS e;
		__try {
			
			const char *filespec_to_process=lpECB->lpszPathTranslated;
			
			// Request info
			Request::Info request_info;
			
			const char *document_root=0; // todo: get from somewhere?
			if(!document_root) {
				static char fake_document_root[MAX_STRING];
				strncpy(fake_document_root, filespec_to_process, MAX_STRING);
				rsplit(fake_document_root, '/');  rsplit(fake_document_root, '\\');// strip filename
				document_root=fake_document_root;
			}
			request_info.document_root=document_root;
			request_info.path_translated=filespec_to_process;
			request_info.method=lpECB->lpszMethod;
			request_info.query_string=lpECB->lpszQueryString;
			request_info.uri=lpECB->lpszPathInfo;//todo: check for ?zzz included?
			request_info.content_type=lpECB->lpszContentType;
			request_info.content_length=lpECB->cbTotalBytes;
			{// cookie
				char variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
				DWORD variable_len = ISAPI_SERVER_VAR_BUF_SIZE-1;
				
				if (lpECB->GetServerVariable(lpECB->ConnID, "HTTP_COOKIE", variable_buf, &variable_len)) {
					variable_buf[variable_len]=0;
					request_info.cookie=variable_buf;
				} else if (GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
					char *tmp_variable_buf=(char *)pool.malloc(variable_len+1);
					
					if (lpECB->GetServerVariable(lpECB->ConnID, "HTTP_COOKIE", tmp_variable_buf, &variable_len)) {
						tmp_variable_buf[variable_len] = 0;
						request_info.cookie=tmp_variable_buf;
					} else {
						request_info.cookie=0;
					}
				}
			}
			
			// prepare to process request
			Request request(pool,
				request_info,
				String::UL_HTML_TYPO
				);
			

			// some root-controlled location
			char *root_auto_path;
			// c:\windows
			root_auto_path=(char *)pool.malloc(MAX_STRING);
			GetWindowsDirectory(root_auto_path, MAX_STRING);
			strcat(root_auto_path, "/");
			
			// process the request
			request.core(
				root_auto_path, true, // /path/to/admin/auto.p
				0/*parser_site_auto_path*/, false, // /path/to/site/auto.p
				header_only);
			
			// successful finish
			
			// must be last in PTRY{}PCATCH
		} __except(exception_handler(&e, GetExceptionInformation())) {
			// converting C++ exception into Parser exception

			if(_exception_code()==EXCEPTION_STACK_OVERFLOW) 
				PTHROW(0, 0,
					0,
					"Stack overflow");
			else if(_exception_code()==EXCEPTION_ACCESS_VIOLATION)
				PTHROW(0, 0,
					0,
					"Access violation at %p", 
						e->ExceptionRecord->ExceptionAddress);
			else
				PTHROW(0, 0,
					0,
					"Exception %#X at %p", 
						e->ExceptionRecord->ExceptionCode,
						e->ExceptionRecord->ExceptionAddress);
		}
		// no further code here
	} PCATCH(e) { // global problem 
		const char *body=e.comment();
		int content_length=strlen(body);

		// prepare header
		(*service_funcs.add_header_attribute)(pool, "content-type", "text/plain");
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%d", content_length);
		(*service_funcs.add_header_attribute)(pool, "content-length", 
			content_length_cstr);

		// send header
		(*service_funcs.send_header)(pool);

		// send body
		if(!header_only)
			(*service_funcs.send_body)(pool, body, content_length);

		// unsuccessful finish
		_endthread();
	}
	PEND_CATCH
	
	return HSE_STATUS_SUCCESS;
}


BOOL APIENTRY DllMain(HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  ) {
    switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			parser_init();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
