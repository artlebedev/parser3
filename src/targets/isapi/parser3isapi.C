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

/// ISAPI // main workhorse
DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB) {
	Pool pool;
	pool.set_context(lpECB);
	
	// TODO r->no_cache=1;
	
	bool header_only=strcasecmp(lpECB->lpszMethod, "HEAD")==0;
	PTRY { // global try
		// must be first in PTRY{}PCATCH
		
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
		char reconstructed_uri[MAX_STRING];
		if(lpECB->lpszQueryString && *lpECB->lpszQueryString) {
			strncpy(reconstructed_uri, lpECB->lpszPathInfo, 
				MAX_STRING-1/*'?'*/-strlen(lpECB->lpszQueryString));
			strcat(reconstructed_uri, "?");
			strcat(reconstructed_uri, lpECB->lpszPathInfo);
			request_info.uri=reconstructed_uri;
		} else
			request_info.uri=lpECB->lpszPathInfo;
		
		request_info.content_type=lpECB->lpszContentType;
		request_info.content_length=lpECB->cbTotalBytes;
		// cookie
		char cookie_buf[MAX_STRING];
		{
			DWORD cookie_len = MAX_STRING-1;
			
			if (lpECB->GetServerVariable(lpECB->ConnID, "HTTP_COOKIE", cookie_buf, &cookie_len)) {
				cookie_buf[cookie_len]=0;
				request_info.cookie=cookie_buf;
			} else if (GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
				char *tmp_cookie_buf=(char *)pool.malloc(cookie_len+1);
				
				if (lpECB->GetServerVariable(lpECB->ConnID, "HTTP_COOKIE", tmp_cookie_buf, &cookie_len)) {
					tmp_cookie_buf[cookie_len]=0;
					request_info.cookie=tmp_cookie_buf;
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
/*		
	char *fuck_it="fff";
	DWORD num_bytes=strlen(fuck_it);;
	lpECB->WriteClient(lpECB->ConnID, fuck_it, &num_bytes, HSE_IO_SYNC);
	return HSE_STATUS_SUCCESS;
*/
		// process the request
		request.core(
			root_auto_path, false/*may be abcent*/, // /path/to/admin/auto.p
			0/*parser_site_auto_path*/, false, // /path/to/site/auto.p
			header_only);
		
		// successful finish
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
