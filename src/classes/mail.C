/** @file
	Parser: @b mail parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: mail.C,v 1.1 2001/04/07 10:34:39 paf Exp $
*/

#include "pa_config_includes.h"

#include "_mail.h"
#include "pa_common.h"
#include "pa_request.h"

// global var

VStateless_class *mail_class;

// methods

struct Mail_info {
	String *attribute_to_exclude;
	String *header;
};

static void add_header_attribute(const Hash::Key& aattribute, Hash::Val *ameaning, 
								 void *info) {

	Value& lmeaning=*static_cast<Value *>(ameaning);
	Mail_info& mi=*static_cast<Mail_info *>(info);
	if(aattribute==*mi.attribute_to_exclude)
		return;

	mi.header->append(aattribute, String::UL_PASS_APPENDED);
	mi.header->APPEND_CONST(":");
	mi.header->append(
		attributed_meaning_to_string(lmeaning, String::UL_MAIL_HEADER), 
		String::UL_PASS_APPENDED);
	mi.header->APPEND_CONST("\n");
}
static void _send(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value& vhash=*static_cast<Value *>(params->get(0));
	// forcing [this body type]
	r.fail_if_junction_(true, vhash, method_name, "message must not be code");

	Hash *hash=vhash.get_hash();
	if(!hash)
		PTHROW(0, 0,
			&method_name,
			"message must be hash");

	// prepare header: 'hash' without "body"
	String& header=*new(pool) String(pool);
	Mail_info mail_info={
		/*excluding*/ body_name,
		&header
	};
	hash->for_each(add_header_attribute, &mail_info);


	r.write_assign_lang(*new(pool) VString(*mail_info.header));
}

// initialize
void initialize_mail_class(Pool& pool, VStateless_class& vclass) {
	// ^mail:send{hash}
	vclass.add_native_method("send", Method::CT_STATIC, _send, 1, 1);
}
