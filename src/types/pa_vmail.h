/** @file
	Parser: @b mail class decls.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMAIL_H
#define PA_VMAIL_H

static const char* IDENT_VMAIL_H="$Date: 2003/01/21 15:51:19 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vhash.h"

// defines

#define MAIL_RECEIVED_ELEMENT_NAME "received"

// forwards

class Request;

/**
	$mail:received letter
*/
class VMail : public VStateless_class {
public: // Value
	
	const char *type() const { return "mail"; }
	
	// mail: CLASS,methods,received field
	Value *get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: // usage

	VMail(Pool& apool);
	
	void fill_received(Request& request);
	const String& message_hash_to_string(Request& r, const String *source, 
		Hash *message_hash, int level, 
		const String **from=0, String **to=0);

private:

	VHash vreceived;

};

#endif
