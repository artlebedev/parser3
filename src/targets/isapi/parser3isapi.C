#ifndef _MSC_VER
#	error compile ISAPI module with MSVC
#endif

#include <windows.h>
#include <process.h>

#include <httpext.h>

#include "pa_sapi.h"
#include "pa_globals.h"
#include "pa_request.h"
#include "pa_version.h"

#define MAX_STATUS_LENGTH sizeof("xxxx LONGEST STATUS DESCRIPTION")

//@{
/// SAPI funcs decl
struct sapi_func_context {
	LPEXTENSION_CONTROL_BLOCK lpECB;
	String *header;
	DWORD http_response_code;
};

const char *SAPI::get_env(Pool& pool, const char *name) {
	sapi_func_context& ctx=*static_cast<sapi_func_context *>(pool.context());

	char *variable_buf=(char *)pool.malloc(MAX_STRING);
	DWORD variable_len = MAX_STRING-1;

	if(ctx.lpECB->GetServerVariable(ctx.lpECB->ConnID, const_cast<char *>(name), 
		variable_buf, &variable_len)) {
		variable_buf[variable_len]=0;
		return variable_buf;
	} else if (GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
		variable_buf=(char *)pool.malloc(variable_len+1);
		
		if(ctx.lpECB->GetServerVariable(ctx.lpECB->ConnID, const_cast<char *>(name), 
			variable_buf, &variable_len)) {
			variable_buf[variable_len]=0;
			return variable_buf;
		}
	}
		
	return 0;
}

uint SAPI::read_post(Pool& pool, char *buf, uint max_bytes) {
	sapi_func_context& ctx=*static_cast<sapi_func_context *>(pool.context());

	DWORD read_from_buf=0;
	DWORD read_from_input=0;
	DWORD total_read=0;

	read_from_buf=min(ctx.lpECB->cbAvailable, max_bytes);
	memcpy(buf, ctx.lpECB->lpbData, read_from_buf);
	total_read+=read_from_buf;

	if(read_from_buf<max_bytes &&
		read_from_buf<ctx.lpECB->cbTotalBytes) {
		DWORD cbRead=0, cbSize;

		read_from_input=min(max_bytes-read_from_buf, 
			ctx.lpECB->cbTotalBytes-read_from_buf);
		while(cbRead < read_from_input) {
			cbSize=read_from_input - cbRead;
			if(!ctx.lpECB->ReadClient(ctx.lpECB->ConnID, 
				buf+read_from_buf+cbRead, &cbSize) || 
				cbSize==0) 
				break;
			cbRead+=cbSize;
		}
		total_read+=cbRead;
	}
	return total_read;
}

void SAPI::add_header_attribute(Pool& pool, const char *key, const char *value) {
	sapi_func_context& ctx=*static_cast<sapi_func_context *>(pool.context());

	if(strcasecmp(key, "location")==0) 
		ctx.http_response_code=302;

	if(strcasecmp(key, "status")==0) 
		ctx.http_response_code=atoi(value);
	else {
		ctx.header->APPEND_CONST(key);
		ctx.header->APPEND_CONST(": ");
		ctx.header->APPEND_CONST(value);
		ctx.header->APPEND_CONST("\n");
	}
}

/// @todo intelligent cache-control
void SAPI::send_header(Pool& pool) {
	sapi_func_context& ctx=*static_cast<sapi_func_context *>(pool.context());

	ctx.header->APPEND_CONST(
		"Expires: Fri, 23 Mar 2001 09:32:23 GMT\n"
		"\n");
	HSE_SEND_HEADER_EX_INFO header_info;

	char status_buf[MAX_STATUS_LENGTH];
	switch(ctx.http_response_code) {
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
				"%d Undescribed", ctx.http_response_code);
			header_info.pszStatus=status_buf;
			break;
	}
	header_info.cchStatus=strlen(header_info.pszStatus);
	header_info.pszHeader=ctx.header->cstr();
	header_info.cchHeader=ctx.header->size();
	header_info.fKeepConn=true;

	ctx.lpECB->dwHttpStatusCode=ctx.http_response_code;

	ctx.lpECB->ServerSupportFunction(ctx.lpECB->ConnID, 
		HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);
}

void SAPI::send_body(Pool& pool, const char *buf, size_t size) {
	sapi_func_context& ctx=*static_cast<sapi_func_context *>(pool.context());

	DWORD num_bytes=size;
	ctx.lpECB->WriteClient(ctx.lpECB->ConnID, 
		const_cast<char *>(buf), &num_bytes, HSE_IO_SYNC);
}
//@}

// 

static void parser_init() {
	static bool globals_inited=false;
	if(globals_inited)
		return;
	globals_inited=true;

	static Pool pool; // global pool
	PTRY {
		// init global variables
		pa_globals_init(pool);
		
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

/** 
	ISAPI // main workhorse

	@todo 
		think of a better way than @c APPL_PHYSICAL_PATH
		of obtaining the @c DOCUMENT_ROOT
		because this only gets "the place where last IIS Application was set"
		and if someone would redefine Application settings below the /
		all ^table:load[/test] would open not /test but /below/test
*/
DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB) {
	Pool pool;
	
	bool header_only=strcasecmp(lpECB->lpszMethod, "HEAD")==0;
	PTRY { // global try
		sapi_func_context ctx={
			lpECB,
			new(pool) String(pool),
			200
		};
		pool.set_context(&ctx);
		
		// Request info
		Request::Info request_info;
		
		if(!(request_info.document_root=SAPI::get_env(pool, "APPL_PHYSICAL_PATH")))
			PTHROW(0, 0,
				0,
				"can not get server variable APPL_PHYSICAL_PATH (error #%lu)",
					GetLastError()); // never

		request_info.path_translated=lpECB->lpszPathTranslated;
		request_info.method=lpECB->lpszMethod;
		request_info.query_string=lpECB->lpszQueryString;
		if(lpECB->lpszQueryString && *lpECB->lpszQueryString) {
			char *reconstructed_uri=(char *)malloc(
				strlen(lpECB->lpszPathInfo)+1/*'?'*/+
				strlen(lpECB->lpszQueryString)+1/*0*/);
			strcpy(reconstructed_uri, lpECB->lpszPathInfo);
			strcat(reconstructed_uri, "?");
			strcat(reconstructed_uri, lpECB->lpszQueryString);
			request_info.uri=reconstructed_uri;
		} else
			request_info.uri=lpECB->lpszPathInfo;
		
		request_info.content_type=lpECB->lpszContentType;
		request_info.content_length=lpECB->cbTotalBytes;
		request_info.cookie=SAPI::get_env(pool, "HTTP_COOKIE");
		
		// prepare to process request
		Request request(pool,
			request_info,
			String::UL_HTML_TYPO
			);
		
		// some root-controlled location
		//   c:\windows
		// must be dynamic: rethrowing from request.core 
		//   may return 'source' which can be inside of 'root auto.p@exeception'
		char *root_auto_path=(char *)pool.malloc(MAX_STRING);
		GetWindowsDirectory(root_auto_path, MAX_STRING);

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
		SAPI::add_header_attribute(pool, "content-type", "text/plain");
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%lu", content_length);
		SAPI::add_header_attribute(pool, "content-length", content_length_cstr);

		// send header
		SAPI::send_header(pool);

		// send body
		if(!header_only)
			SAPI::send_body(pool, body, content_length);

		// unsuccessful finish
		_endthread();
	}
	PEND_CATCH
	
	return HSE_STATUS_SUCCESS_AND_KEEP_CONN;
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
