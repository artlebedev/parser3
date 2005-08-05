/** @file
	Parser: cookie class.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VCOOKIE_C="$Date: 2005/08/05 13:03:04 $";

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_vcookie.h"
#include "pa_vstring.h"
#include "pa_vdate.h"
#include "pa_vhash.h"

// defines

#define PATH_NAME "path"
#define PATH_VALUE_DEFAULT "/"

#define SESSION_NAME "session"
#define DEFAULT_EXPIRES_DAYS 90

// statics

static const String path_name(PATH_NAME);
static const String path_value_default(PATH_VALUE_DEFAULT);

// VCookie

Value* VCookie::get_element(const String& aname, Value& /*aself*/, bool /*looking_up*/) {
	// $CLASS
	if(aname==CLASS_NAME)
		return this;

	// $cookie
	if(deleted.get(aname)) // deleted?
		return 0;
	
	if(Value* after_meaning=after.get(aname)) // assigned 'after'?
		if(HashStringValue *hash=after_meaning->get_hash())
			return hash->get(value_name);
		else
			return after_meaning;
	
	// neither deleted nor assigned 
	// return any value it had 'before'
	return before.get(aname);
}

bool VCookie::put_element(const String& aname, Value* avalue, bool /*replace*/) {
	// $cookie
	bool remove;
	Value* lvalue;
	if(HashStringValue *hash=avalue->get_hash())
		lvalue=hash->get(value_name);
	else
		lvalue=avalue;

	if(lvalue && lvalue->is_string()) {
		// taint string being assigned
		String& tainted=*new String;
		tainted.append(*lvalue->get_string(), String::L_TAINTED, true /*forced*/);
		lvalue=new VString(tainted);
	}

	remove=!lvalue || lvalue->as_string().is_empty();

	(remove?deleted:after).put(aname, avalue);
	(remove?after:deleted).put(aname, 0);
	
	return true;
}

static char *search_stop(char*& current, char cstop_at) {
	// sanity check
	if(!current)
		return 0;

	// skip leading WS
	while(*current==' ' || *current=='\t')
		current++;
	if(!*current)
		return current=0;

	char *result=current;
	if(char *pstop_at=strchr(current, cstop_at)) {
		*pstop_at=0;
		current=pstop_at+1;
	} else
		current=0;
	return result;
}


//#include <stdio.h>
void VCookie::fill_fields(Request_info& request_info) {
	//request_info.cookie="test-session=value%3D5; test-default1=value%3D1; test-default2=value%3D2; test-tomorrow=value%3D3";
	//request_info.cookie="enabled=yes; auth.uid=196325308053599810; enabled=yes; msnames; msuri"; // mdm 
	if(!request_info.cookie)
		return;
/*
	FILE *f=fopen("c:\\temp\\a", "wt");
	fprintf(f, "cookie=%s", request_info.cookie);
	fclose(f);*/
	char *cookies=strdup(request_info.cookie);
	char *current=cookies;
	//_asm int 3;
	do {
		if(char *attribute=search_stop(current, '='))
			if(char *meaning=search_stop(current, ';')) {
				const String& sattribute=
					*new String(unescape_chars(attribute, strlen(attribute)), 0, true);
				const String& smeaning=
					*new String(unescape_chars(meaning, strlen(meaning)), 0, true);
				before.put(sattribute, new VString(smeaning));
			}
	} while(current);
}

static Value& expires_vdate(double days_till_expire) {
	time_t when=time(NULL)+(time_t)(60*60*24*days_till_expire);
	struct tm *tms=gmtime(&when);
	if(!tms)
		throw Exception(0,
			0,
			"bad expires time (seconds from epoch=%u)", when);

	return *new VDate(when);
}

/*
	@todo 
	http://www.netscape.com/newsref/std/cookie_spec.html
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
static void output_set_cookie_header(
			      HashStringValue::key_type aattribute, 
			      HashStringValue::value_type ameaning,
				  bool adelete,
			      SAPI_Info& sapi_info) {
	String string;
	// attribute
	string.append(String(aattribute, String::L_TAINTED), String::L_HTTP_HEADER, true);
	// attribute=
	string << "=";
	Value* lmeaning;
	// figure out 'meaning'
	// Set-Cookie: (attribute)=(value); path=/
	HashStringValue *hash;
	double default_expires_days=adelete?-DEFAULT_EXPIRES_DAYS:+DEFAULT_EXPIRES_DAYS;
	if((hash=ameaning->get_hash())) { // ...[hash value]
		// clone to safely change it
		lmeaning=new VHash(*hash);  hash=lmeaning->get_hash();
		// $expires
		if(Value* expires=hash->get(expires_name)) {
			const String* string;
			if(expires->is_string() && (string=expires->get_string()) && (*string==SESSION_NAME))  {
				// $expires[session]
				hash->remove(expires_name);
			} else {
				if(Value* vdate=expires->as(VDATE_TYPE, false))
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
			http://www.netscape.com/newsref/std/cookie_spec.html
			to delete a cookie, it can do so by returning a cookie with the same name, 
			and an expires time which is in the past
		*/

		// Set-Cookie: (attribute)=; path=/
		lmeaning->get_hash()->remove(value_name);
	}
	// defaulting path
	if(!lmeaning->get_hash()->get(path_name))
		lmeaning->get_hash()->put(path_name, 
			new VString(path_value_default));

	// append lmeaning
	string << attributed_meaning_to_string(*lmeaning, String::L_HTTP_HEADER, true);

	// output
	SAPI::add_header_attribute(sapi_info, "set-cookie", string.cstr(String::L_UNSPECIFIED));
}
static void output_after(
						 HashStringValue::key_type aattribute, 
						 HashStringValue::value_type ameaning,
						 SAPI_Info* sapi_info) {
	output_set_cookie_header(aattribute, ameaning, false, *sapi_info);
}
static void output_deleted(
						   HashStringValue::key_type aattribute, 
						   HashStringValue::value_type ameaning, 
						   SAPI_Info* sapi_info) {
	if(ameaning)
		output_set_cookie_header(aattribute, ameaning, true, *sapi_info);
}
void VCookie::output_result(SAPI_Info& sapi_info) {
	after.for_each(output_after, &sapi_info);
	deleted.for_each(output_deleted, &sapi_info);
}
