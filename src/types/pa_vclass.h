/**	@file
	Parser: @b class parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

static const char* IDENT_VCLASS_H="$Date: 2002/10/31 15:01:54 $";

#include "pa_vstateless_class.h"
#include "pa_vjunction.h"
#include "pa_vobject.h"

/**	stores 
- static fields: VClass::ffields
*/
class VClass : public VStateless_class {
public: // Value
	
	const char *type() const { return name_cstr(); }

	/*override*/ Value *as(const char *atype, bool looking_up);

	/// VClass: true
	Value *as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }
	/// VClass: true
	bool as_bool() const { return true; }

	/*override*/ Value *get_element(const String& aname, Value& aself, bool /*looking_up*/);
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace);
	/*override*/ Value *create_new_value(Pool& );

public: // usage

	VClass(Pool& apool) : VStateless_class(apool), 
		ffields(apool) {
	}

protected:
		
	bool replace_field(const String& name, Value *value) {
		return 
			(fbase && fbase->replace_field(name, value)) ||
			ffields.put_replace(name, value);
	}

private: // self

	Hash ffields;

};

#endif
