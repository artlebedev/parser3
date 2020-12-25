/** @file
	Parser: cookie class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_vcookie.h"
#include "pa_vstring.h"
#include "pa_vdate.h"
#include "pa_vhash.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VCOOKIE_C="$Id: pa_vcookie.C,v 1.101 2020/12/25 12:38:12 moko Exp $" IDENT_PA_VCOOKIE_H;

// defines

#define PATH_NAME "path"
#define PATH_VALUE_DEFAULT "/"

#define SESSION_NAME "session"
#define DEFAULT_EXPIRES_DAYS 90

#define COOKIE_FIELDS_ELEMENT_NAME "fields"

// statics

static const String path_name(PATH_NAME);
static const String path_value_default(PATH_VALUE_DEFAULT);

// VCookie

VCookie::VCookie(Request_charsets& acharsets, Request_info& arequest_info):
	filled_source(0), filled_client(0),
	fcharsets(acharsets), frequest_info(arequest_info) {
}

Value* VCookie::get_element(const String& aname) {
#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	// CLASS, CLASS_NAME
	if(Value* result=VStateless_class::get_element(aname))
		return result;
#endif

	// $fields
	if(aname==COOKIE_FIELDS_ELEMENT_NAME){
		if(should_refill())
			refill();

		HashStringValue *result=new HashStringValue(before);
		for(HashStringValue::Iterator i(after); i; i.next())
			result->put(i.key(), i.value());
		for(HashStringValue::Iterator i(deleted); i; i.next())
			result->remove(i.key());
		return new VHash(*result);
	}

	// $cookie
	if(deleted.get(aname)) // deleted?
		return 0;
	
	if(Value* after_meaning=after.get(aname)) { // assigned 'after'?
		if(HashStringValue *hash=after_meaning->get_hash())
			return hash->get(value_name);
		else
			return after_meaning;
	}
	
	if(should_refill())
		refill();

	// neither deleted nor assigned 
	// return any value it had 'before'
	return before.get(aname);
}

time_t expires_sec(double days_till_expire) {
	time_t result=time(NULL)+(time_t)(60*60*24*days_till_expire);
	struct tm* tms=gmtime(&result);
	if(!tms)
		throw Exception(DATE_RANGE_EXCEPTION_TYPE,
			0,
			"bad expires time (seconds from epoch=%u)", result);
	return result;
}

const VJunction* VCookie::put_element(const String& aname, Value* avalue) {
	// $cookie
	Value* lvalue;
	if(HashStringValue *hash=avalue->get_hash()) {
		if(Value* expires=hash->get(expires_name)){
			const String* string;
			if(!(expires->is_string() && (string=expires->get_string()) && (*string==SESSION_NAME)))
				if(!expires->as(VDATE_TYPE))
					if(double days_till_expire=expires->as_double())
						expires_sec(days_till_expire);
		}
		lvalue=hash->get(value_name);
	} else
		lvalue=avalue;

	if(lvalue && lvalue->is_string()) {
		// taint string being assigned
		String& tainted=*new String;
		tainted.append(*lvalue->get_string(), String::L_TAINTED, true /*forced*/);
		lvalue=new VString(tainted);
	}

	if( !lvalue || lvalue->as_string().is_empty() ) {
		deleted.put(aname, avalue);
		after.put(aname, 0);
	} else {
		after.put(aname, avalue);
		deleted.put(aname, 0);
	}
	return 0;
}

static Value& expires_vdate(double days_till_expire) {
	return *new VDate((pa_time_t)expires_sec(days_till_expire));
}

/*
	@todo 
	http://curl.haxx.se/rfc/cookie_spec.html
	http://www.w3.org/Protocols/rfc2109/rfc2109
	When sending cookies to a server, 
	all cookies with a more specific path mapping should be sent before cookies 
	with less specific path mappings. 
	For example, a cookie "name1=foo" with a path mapping of "/" should be sent after 
	a cookie "name1=foo2" with a path mapping of "/bar" if they are both to be sent. 

	There are limitations on the number of cookies that a client can store at any one time. 
	This is a specification of the minimum number of cookies that a client should be prepared 
	to receive and store. 
		300 total cookies 
		4 kilobytes per cookie, where the name and the OPAQUE_STRING combine 
			to form the 4 kilobyte limit. 
		20 cookies per server or domain. (note that completely specified hosts 
			and domains are treated as separate entities and have a 20 cookie limitation 
			for each, not combined) 
*/

const String* output_set_cookie_value(
					HashStringValue::key_type aname,
					HashStringValue::value_type ameaning,
					bool adelete){
	String* result=new String();
	// attribute=
	*result << String(aname, String::L_HTTP_COOKIE) << "=";

	Value* lmeaning;
	// figure out 'meaning'
	// Set-Cookie: (attribute)=(value); path=/
	HashStringValue *hash;
	double default_expires_days=adelete?-DEFAULT_EXPIRES_DAYS:+DEFAULT_EXPIRES_DAYS;
	if((hash=ameaning->get_hash())) { // ...[hash value]
		// clone to safely change it
		lmeaning=new VHash(*hash);
		hash=lmeaning->get_hash();

		// $expires
		if(Value* expires=hash->get(expires_name)) {
			const String* string;
			if(expires->is_string() && (string=expires->get_string()) && (*string==SESSION_NAME))  {
				// $expires[session]
				hash->remove(expires_name);
			} else {
				if(Value* vdate=expires->as(VDATE_TYPE))
					hash->put(expires_name, vdate); // $expires[DATE]
				else if(double days_till_expire=expires->as_double())
					hash->put(expires_name, &expires_vdate(days_till_expire)); // $expires(days)
				else
					hash->remove(expires_name); // $expires(0)
			}
		} else // $expires not assigned, defaulting
			hash->put(expires_name, &expires_vdate(default_expires_days));
	} else { // ...[string value]
		Value* wrap_meaning=new VHash;
		hash=wrap_meaning->get_hash();
		// wrapping lmeaning into hash
		hash->put(value_name, ameaning);
		// string = $expires not assigned, defaulting
		hash->put(expires_name, &expires_vdate(default_expires_days));
		// replacing lmeaning with hash-wrapped one
		lmeaning=wrap_meaning;
	}

	if(adelete) {// removing value
		/*
			http://curl.haxx.se/rfc/cookie_spec.html
			http://www.w3.org/Protocols/rfc2109/rfc2109
			to delete a cookie, it can do so by returning a cookie with the same name, 
			and an expires time which is in the past
		*/

		// Set-Cookie: (attribute)=; path=/
		lmeaning->get_hash()->remove(value_name);
	}

	// defaulting path
	if(!lmeaning->get_hash()->get(path_name))
		lmeaning->get_hash()->put(path_name, new VString(path_value_default));

	// append lmeaning
	*result << attributed_meaning_to_string(*lmeaning, String::L_HTTP_COOKIE, true, true /* allow bool attr */);

	return result;
}


struct Cookie_pass_info {
	SAPI_Info* sapi_info;
	Request_charsets* charsets;
};

void output_set_cookie_header(
			HashStringValue::key_type aattribute, 
			HashStringValue::value_type ameaning,
			bool adelete,
			Cookie_pass_info& cookie_info
	){
		SAPI::add_header_attribute(*cookie_info.sapi_info, "set-cookie",
			output_set_cookie_value(aattribute, ameaning, adelete)->untaint_cstr(String::L_AS_IS, 0, cookie_info.charsets));
}

void output_after(
			HashStringValue::key_type aattribute, 
			HashStringValue::value_type ameaning,
			Cookie_pass_info& cookie_info
	){
		output_set_cookie_header(aattribute, ameaning, false, cookie_info);
}

void output_deleted(
			HashStringValue::key_type aattribute, 
			HashStringValue::value_type ameaning, 
			Cookie_pass_info& cookie_info
	){
		if(ameaning)
			output_set_cookie_header(aattribute, ameaning, true, cookie_info);
}

void VCookie::output_result(SAPI_Info& sapi_info) {
	Cookie_pass_info cookie_info={&sapi_info, &fcharsets};

	after.for_each<Cookie_pass_info&>(output_after, cookie_info);
	deleted.for_each<Cookie_pass_info&>(output_deleted, cookie_info);
}

bool VCookie::should_refill(){
	return !(
		&fcharsets.source()==filled_source
		&& &fcharsets.client()==filled_client
	);
}

//#include <stdio.h>
void VCookie::refill(){
	//request_info.cookie="test-session=value%3D5; test-default1=value%3D1; test-default2=value%3D2; test-tomorrow=value%3D3";
	//request_info.cookie="enabled=yes; auth.uid=196325308053599810; enabled=yes; msnames; msuri"; // mdm 
	if(!frequest_info.cookie)
		return;

	char *cookies=strdup(frequest_info.cookie);
	char *current=cookies;
	//_asm int 3;
	do {
		if(char *attribute=search_stop(current, '='))
			if(char *meaning=search_stop(current, ';')) {
				const String& sattribute=
					*new String(unescape_chars(attribute, strlen(attribute), &fcharsets.source(), true), String::L_TAINTED);
				const String& smeaning=
					*new String(unescape_chars(meaning, strlen(meaning), &fcharsets.source(), true), String::L_TAINTED);
				before.put(sattribute, new VString(smeaning));

				//if(sattribute == "test_js") throw Exception(0, 0, "'%s' '%s'", meaning, smeaning.cstr());
			}
	} while(current);

	filled_source=&fcharsets.source();
	filled_client=&fcharsets.client();
}

