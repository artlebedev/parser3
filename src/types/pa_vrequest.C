/** @file
	Parser: @b request class.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_vrequest.h"
#include "pa_request_info.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vform.h"
#include "pa_vvoid.h"
#include "pa_vfile.h"

volatile const char * IDENT_PA_VREQUEST_C="$Id: pa_vrequest.C,v 1.74 2025/05/26 01:56:54 moko Exp $" IDENT_PA_VREQUEST_H;

// defines

#define DOCUMENT_ROOT_NAME "document-root"
#define REQUEST_HEADERS_ELEMENT_NAME "headers"

VRequest::VRequest(Request_info& ainfo, Request_charsets& acharsets, VForm& aform, SAPI_Info& asapi_info):
	finfo(ainfo),
	fsapi_info(asapi_info),
	fcharsets(acharsets),
	fform(aform)
{
	if(ainfo.argv)
		for(size_t i=0; ainfo.argv[i]; i++) {
			fargv.put_dont_replace(
				String(i, "%d"),
				new VString(pa_strdup(ainfo.argv[i]))
			);
		}
}

Value* VRequest::get_element(const String& aname) {
#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	// $request:CLASS, $request:CLASS_NAME
	if(Value* result=VStateless_class::get_element(aname))
		return result;
#endif

	// $request:charset
	if(aname==CHARSET_NAME)
		return new VString(fcharsets.source().NAME());

	// $request:body-charset
	if(aname==REQUEST_BODY_CHARSET_NAME || aname==POST_CHARSET_NAME /*backward*/){
		if(Charset* body_charset=fform.get_body_charset())
			return new VString(body_charset->NAME());
		else
			return VVoid::get();
	}

	// $resuest:body-file
	if(aname==REQUEST_BODY_BODY_NAME || aname==POST_BODY_NAME /*backward*/){
		VFile& result=*new VFile;
		result.set_binary(true/*tainted*/, (finfo.post_data)?finfo.post_data:"" /*to distinguish from stat-ed file*/, finfo.post_size);
		return &result;
	}

	// $request:argv
	if(aname==REQUEST_ARGV_ELEMENT_NAME)
		return new VHash(fargv);
	
	//$request:headers
	if(aname==REQUEST_HEADERS_ELEMENT_NAME){
		if(!ffields.count())
			fill();
		return new VHash(ffields);
	}

	// $request:query $request:uri $request:document-root $request:body $request:method
	const char* buf;
	if(aname=="query")
		buf=finfo.query_string;
	else if(aname=="uri")
		buf=finfo.uri;
	else if(aname=="path")
		buf=path_from_uri(pa_strdup(finfo.uri));
	else if(aname==DOCUMENT_ROOT_NAME)
		buf=finfo.document_root;
	else if(aname=="body")
		buf=finfo.post_data;
	else if(aname=="method")
		buf=finfo.method;
	else
		return bark("%s field not found", &aname);

	return new VString(buf);
}

const VJunction* VRequest::put_element(const String& aname, Value* avalue) {
	// $charset
	if(aname==CHARSET_NAME) {
		fcharsets.set_source(pa_charsets.get(avalue->as_string()));
		return 0;
	} 

	// $document-root
	if(aname==DOCUMENT_ROOT_NAME) {
		finfo.document_root=avalue->as_string().taint_cstr(String::L_FILE_SPEC);
		return 0;
	} 

	return Value::put_element(aname, avalue);
}

void VRequest::fill(){
	for(SAPI::Env::Iterator i(fsapi_info); i; i.next() ){
		char* key=i.key();

		if(pa_strncasecmp(key, "HTTP_")==0) {
			for(char* c=key+5; *c; c++)
				*c=(char)toupper((unsigned char)*c);

			ffields.put(
				key+5 /*skip "HTTP_" */,
				new VString(i.value())
			);
		}
	}
}

const char* VRequest::path_from_uri(char* uri){
	const char *result=uri;
	lsplit(uri,'?');
	if(result && *result)
		result=unescape_chars(result, strlen(result), &pa_UTF8_charset, false /* uri */);
	if(result && *result)
		result=Charset::transcode(result, pa_UTF8_charset, fcharsets.source()).cstr();
	if(result && *result)
		return result;
	return ""; // to match an empty URI in console mode
}
