/** @file
	Parser: @b mail class decls.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMAIL_H
#define PA_VMAIL_H

static const char* IDENT_VMAIL_H="$Date: 2003/08/19 08:22:13 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vhash.h"

// defines

#define MAIL_RECEIVED_ELEMENT_NAME "received"

// forwards

class Request_info;

/**
	$mail:received letter
*/
class VMail: public VStateless_class {

	VHash vreceived;

public: // Value
	
	override const char* type() const { return "mail"; }
	
	// mail: CLASS,methods,received field
	Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: // usage

	VMail();
	
	void fill_received(Request& r);
	const String& message_hash_to_string(Request& r,
		HashStringValue* message_hash, int level, 
		const String* & from, bool extract_to, String* & to);

};

#endif
