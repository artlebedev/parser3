/** @file
	Parser: @b mail class decls.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VMAIL_H
#define PA_VMAIL_H

#define IDENT_PA_VMAIL_H "$Id: pa_vmail.h,v 1.27 2024/11/04 03:53:26 moko Exp $"

#include "classes.h"
#include "pa_common.h"
#include "pa_vhash.h"

// defines

#define MAIL_RECEIVED_ELEMENT_NAME "received"
#define MAIL_OPTIONS_NAME "options"
#define MAIL_DEBUG_NAME "print-debug"

// forwards

class Request_info;

/**
	$mail:received letter
*/
class VMail: public VStateless_class {

	VHash vreceived;

public: // Value
	
	override const char* type() const { return "mail"; }
	
	// mail: methods,received field
	Value* get_element(const String& aname);

public: // usage

	VMail();
	
	void fill_received(Request& r);
	const String& message_hash_to_string(Request& r, HashStringValue* message_hash, const String* & from, bool extract_to, String* & to);
};

#endif
