/** @file
	Parser: cookie class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vcookie.C,v 1.19 2001/04/19 15:38:06 paf Exp $
*/

#include "pa_config_includes.h"
#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_vcookie.h"
#include "pa_vstring.h"
#include "pa_request.h"

#define SESSION_NAME "session"
#define DEFAULT_EXPIRES_DAYS 90

// VCookie

Value *VCookie::get_element(const String& aname) {
	// $cookie
	if(deleted.get(aname)) // deleted?
		return 0;
	
	if(Value *after_meaning=static_cast<Value *>(after.get(aname))) // assigned 'after'?
		if(Hash *hash=after_meaning->get_hash())
			return static_cast<Value *>(hash->get(*value_name));
		else
			return after_meaning;
	
	// neither deleted nor assigned 
	// return any value it had 'before'
	return static_cast<Value *>(before.get(aname));
}

void VCookie::put_element(const String& aname, Value *avalue) {
	// $cookie
	bool remove;
	if(Hash *hash=avalue->get_hash())
		remove=hash->size()==0;
	else
		remove=avalue->as_string().size()==0;

	(remove?deleted:after).put(aname, avalue);
	(remove?after:deleted).put(aname, 0);
}

//#include <stdio.h>
void VCookie::fill_fields(Request& request) {
	//request.info.cookie="test-session=value%3D5; test-default1=value%3D1; test-default2=value%3D2; test-tomorrow=value%3D3";
	request.info.cookie="\"вот\"=\"прислал \\\"browser\\\"\"";
	if(!request.info.cookie)
		return;
/*
	FILE *f=fopen("c:\\temp\\a", "wt");
	fprintf(f, "cookie=%s", request.info.cookie);
	fclose(f);*/
	char *cookies=(char *)malloc(strlen(request.info.cookie)+1);
	strcpy(cookies, request.info.cookie);
    char *current=cookies;
    do {
		if(char *attribute=unquote(current, '='))
			if(char *meaning=unquote(current, ';')) {
				String& sattribute=*NEW String(pool(), attribute, 0, true);
				String& smeaning=*NEW String(pool(), meaning, 0, true);
				before.put(sattribute, NEW VString(smeaning));
			}
	} while(current);
}

static VString *expires_timestamp(Pool& pool, double days_till_expire) {
    const char month_names[12][4]={
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    const char days[7][4]={
		"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	
	time_t when=time(NULL)+(time_t)(60*60*24*days_till_expire);
	struct tm *tms=gmtime(&when);
	char *buf=(char *)pool.malloc(MAX_STRING);
	snprintf(buf, MAX_STRING, "%s, %.2d-%s-%.4d %.2d:%.2d:%.2d GMT", 
		days[tms->tm_wday],
		tms->tm_mday,month_names[tms->tm_mon],tms->tm_year+1900,
		tms->tm_hour,tms->tm_min,tms->tm_sec);
	return new(pool) VString(*new(pool) String(pool, buf));
}

static void output_set_cookie(const Hash::Key& aattribute, Hash::Val *ameaning) {
	Pool& pool=aattribute.pool();
	String string(pool);
	// attribute
	string.append(aattribute, String::UL_HTTP_HEADER, true);
	// attribute=
	string << "=";
	Value *meaning;
	// figure out 'meaning'
	if(ameaning) { // assigning value
		// Set-Cookie: (attribute)=(value); path=/
		meaning=static_cast<Value *>(ameaning);
		if(Hash *hash=meaning->get_hash()) { // ...[hash value]
			// $expires
			if(Value *expires=static_cast<Value *>(hash->get(*expires_name))) {
				const String *string;
				if((string=expires->get_string()) && (*string==SESSION_NAME))  {
					// $expires[session]
					hash->put(*expires_name, 0);
				} else {
					// $expires(days)
					hash->put(*expires_name, 
						expires_timestamp(pool, expires->as_double()));
				}
			} else // $expires not assigned, defaulting
				hash->put(*expires_name, expires_timestamp(pool, DEFAULT_EXPIRES_DAYS));
		} else { // ...[string value]
			Value *wrap_meaning=new(pool) VHash(pool);
			// wrapping meaning into hash
			wrap_meaning->get_hash()->put(*value_name, meaning);
			// string = $expires not assigned, defaulting
			wrap_meaning->get_hash()->put(*expires_name, 
				expires_timestamp(pool, DEFAULT_EXPIRES_DAYS));
			// replacing meaning with hash-wrapped one
			meaning=wrap_meaning;
		}
	} else {// removing value
		// Set-Cookie: (attribute)=; path=/
		meaning=new(pool) VHash(pool);
	}
	// defaulting path
	if(!meaning->get_hash()->get(*path_name))
		meaning->get_hash()->put(*path_name, 
			new(pool) VString(*new(pool) String(pool, "/")));

	// append meaning
	string << attributed_meaning_to_string(*meaning, String::UL_HTTP_HEADER);

	// output
	SAPI::add_header_attribute(pool, "set-cookie", string.cstr());
}
static void output_after(const Hash::Key& aattribute, Hash::Val *ameaning, void *) {
	output_set_cookie(aattribute, ameaning);
}
static void output_deleted(const Hash::Key& aattribute, Hash::Val *ameaning, void *) {
	output_set_cookie(aattribute, 0);
}
void VCookie::output_result() {
	after.for_each(output_after, this);
	deleted.for_each(output_deleted, this);
}
