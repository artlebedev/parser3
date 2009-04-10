/** @file
	Parser: @b request class.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VREQUEST_C="$Date: 2009/04/10 11:39:47 $";

#include "pa_vrequest.h"
#include "pa_request_info.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vform.h"
#include "pa_vvoid.h"

// defines

#define DOCUMENT_ROOT_NAME "document-root"

VRequest::VRequest(Request_info& ainfo, Request_charsets& acharsets, VForm& aform): 
		finfo(ainfo), 
		fcharsets(acharsets),
		fform(aform) {

	if(ainfo.argv) {
		for (size_t i = ainfo.args_skip; ainfo.argv[i]; i++) {
			char *name = new(PointerFreeGC) char[3 /* max 999 argvs */ + 1/* terminating 0 */];

			char *value = new(PointerFreeGC) char[strlen(ainfo.argv[i])+1];
			strcpy(value, ainfo.argv[i]);
	
			fargv.put_dont_replace(
				*new String(name, sprintf(name, "%d", i - ainfo.args_skip)),
				new VString(*new String(value, 0, true /*tainted*/))
			);
		}
	}
}

Value* VRequest::get_element(const String& aname, Value&  /*aself*/, bool /*looking_up*/) {
	// $request:charset
	if(aname==CHARSET_NAME)
		return new VString(*new String(fcharsets.source().NAME(), String::L_TAINTED));

	// $request:post-charset
	if(aname==POST_CHARSET_NAME){
		if(Charset* post_charset=fform.get_post_charset())
			return new VString(*new String(post_charset->NAME(), String::L_TAINTED));
		else
			return new VVoid();
	}
	
	// $CLASS
	if(aname==CLASS_NAME)
		return this;

	// $CLASS_NAME
	if(aname==CLASS_NAMETEXT)
		return new VString(request_class_name);

	// $request:argv
	if(aname==REQUEST_ARGV_ELEMENT_NAME)
		return new VHash(fargv);
	
	// $request:query $request:uri $request:document-root $request:body
	const char* buf;
	size_t size=0;
	if(aname=="query")
		buf=finfo.query_string;
	else if(aname=="uri")
		buf=finfo.uri;
	else if(aname==DOCUMENT_ROOT_NAME)
		buf=finfo.document_root;
	else if(aname=="body") {
		buf=finfo.post_data;
		size=finfo.post_size;
	} else
		return bark("%s field not found", &aname);

	return new VString(*new String(buf?buf:"", size, true));
}

const VJunction* VRequest::put_element(Value& aself, const String& aname, Value* avalue, bool areplace) {
	// $charset
	if(aname==CHARSET_NAME) {
		fcharsets.set_source(charsets.get(avalue->as_string().
			change_case(UTF8_charset, String::CC_UPPER)));
		return PUT_ELEMENT_REPLACED_ELEMENT;
	} 

	// $document-root
	if(aname==DOCUMENT_ROOT_NAME) {
		finfo.document_root=avalue->as_string().cstr(String::L_FILE_SPEC);
		return PUT_ELEMENT_REPLACED_ELEMENT;
	} 

	return Value::put_element(aself, aname, avalue, areplace);
}
