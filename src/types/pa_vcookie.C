/** @file
	Parser: cookie class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vcookie.C,v 1.35 2002/02/08 08:32:34 paf Exp $
*/

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_vcookie.h"
#include "pa_vstring.h"
#include "pa_request.h"

#define SESSION_NAME "session"
#define DEFAULT_EXPIRES_DAYS 90

// VCookie

Value *VCookie::get_element(const String& name) {
	// $CLASS
	if(name==CLASS_NAME)
		return this;

	// $cookie
	if(deleted.get(name)) // deleted?
		return 0;
	
	if(Value *after_meaning=static_cast<Value *>(after.get(name))) // assigned 'after'?
		if(Hash *hash=after_meaning->get_hash(&name))
			return static_cast<Value *>(hash->get(*value_name));
		else
			return after_meaning;
	
	// neither deleted nor assigned 
	// return any value it had 'before'
	return static_cast<Value *>(before.get(name));
}

void VCookie::put_element(const String& name, Value *value) {
	// $cookie
	bool remove;
	if(Hash *hash=value->get_hash(&name))
		remove=hash->size()==0;
	else
		remove=value->as_string().size()==0;

	(remove?deleted:after).put(name, value);
	(remove?after:deleted).put(name, 0);
}

static char *search_stop(char*& current, char cstop_at) {
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
void VCookie::fill_fields(Request& request) {
	//request.info.cookie="test-session=value%3D5; test-default1=value%3D1; test-default2=value%3D2; test-tomorrow=value%3D3";
	if(!request.info.cookie)
		return;
/*
	FILE *f=fopen("c:\\temp\\a", "wt");
	fprintf(f, "cookie=%s", request.info.cookie);
	fclose(f);*/
	char *cookies=(char *)malloc(strlen(request.info.cookie)+1);
	strcpy(cookies, request.info.cookie);
    char *current=cookies;
	uint line=0;
	//_asm int 3;
    do {
		if(char *attribute=search_stop(current, '='))
			if(char *meaning=search_stop(current, ';')) {
				String& sattribute=*NEW String(pool());
				String& smeaning=*NEW String(pool());
				sattribute.APPEND_TAINTED(unescape_chars(pool(), attribute, strlen(attribute)), 0, 
					"cookie_name", line);
				smeaning.APPEND_TAINTED(unescape_chars(pool(), meaning, strlen(meaning)), 0, 
					"cookie_value", line);
				before.put(sattribute, NEW VString(smeaning));
				line++;
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

/*
	@todo http://www.netscape.com/newsref/std/cookie_spec.html
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
		if(Hash *hash=meaning->get_hash(&aattribute)) { // ...[hash value]
			// $expires
			if(Value *expires=static_cast<Value *>(hash->get(*expires_name))) {
				const String *string;
				if((string=expires->get_string()) && (*string==SESSION_NAME))  {
					// $expires[session]
					hash->remove(*expires_name);
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
			wrap_meaning->get_hash(&aattribute)->put(*value_name, meaning);
			// string = $expires not assigned, defaulting
			wrap_meaning->get_hash(&aattribute)->put(*expires_name, 
				expires_timestamp(pool, DEFAULT_EXPIRES_DAYS));
			// replacing meaning with hash-wrapped one
			meaning=wrap_meaning;
		}
	} else {// removing value
		/*
			http://www.netscape.com/newsref/std/cookie_spec.html
			to delete a cookie, it can do so by returning a cookie with the same name, 
			and an expires time which is in the past
		*/

		// Set-Cookie: (attribute)=; path=/
		meaning=new(pool) VHash(pool);
		meaning->get_hash(&aattribute)->put(*expires_name, 
			expires_timestamp(pool, -DEFAULT_EXPIRES_DAYS));
	}
	// defaulting path
	if(!meaning->get_hash(&aattribute)->get(*path_name))
		meaning->get_hash(&aattribute)->put(*path_name, 
			new(pool) VString(*new(pool) String(pool, "/")));

	// append meaning
	string << attributed_meaning_to_string(*meaning, String::UL_HTTP_HEADER);

	// output
	SAPI::add_header_attribute(pool, "set-cookie", string.cstr(String::UL_UNSPECIFIED));
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
