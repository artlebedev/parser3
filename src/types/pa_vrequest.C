/** @file
	Parser: @b request class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vrequest.C,v 1.5 2001/04/26 15:01:52 paf Exp $
*/

#include "pa_vrequest.h"
#include "pa_request.h"

// request: CLASS,BASE,method,field
Value *VRequest::get_element(const String& aname) {
	// $browser
	if(aname=="browser") {
		VHash *vhash=NEW VHash(pool());

		// analize
		bool ie=true;
		const char *version=0;
		const char *agent=frequest.info.user_agent;
		if(agent) {
			if(strstr(agent, "compatible")) {
				if(const char *msie=strstr(agent, "MSIE ")) {
					ie=true;
					version=msie+5;
				}
			} else {
				if(!strncasecmp(agent, "MOZILLA/", 8)) {
					ie=false;
					version=agent+8;
				}
			}
		}
		
		// set $browser.type
		vhash->hash().put(*NEW String(pool(), "type"), 
			NEW VString(*NEW String(pool(), ie?"ie":"nn")));
		if(version) { // we know the version
			// set $browser.version
			vhash->hash().put(*NEW String(pool(), "version"), 
				NEW VDouble(pool(), atof(version)));
		}
		//
		return vhash;
	}

	// $query $uri 
	const char *cstr=0;
	if(aname=="query")
		cstr=frequest.info.query_string;
	if(aname=="uri")
		cstr=frequest.info.uri;
	if(!cstr)
		return 0;
	
	String& string=*NEW String(pool());
	string.APPEND_TAINTED(cstr, 0, "request", 0);
	return NEW VString(string);
}

