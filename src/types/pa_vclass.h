/**	@file
	Parser: @b class parser class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

static const char * const IDENT_VCLASS_H="$Date: 2005/08/05 13:03:04 $";

// includes

#include "pa_vstateless_class.h"
#include "pa_vjunction.h"
#include "pa_vobject.h"

/**	stores 
- static fields: VClass::ffields
*/
class VClass: public VStateless_class {
public: // Value
	
	const char* type() const { return name_cstr(); }

	override Value* as(const char* atype, bool looking_up);

	/// VClass: true
	override Value& as_expr_result(bool) { return *new VBool(as_bool()); }
	/// VClass: true
	override bool as_bool() const { return true; }

	override Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);
	override bool put_element(const String& aname, Value* avalue, bool replace);
	override Value* create_new_value(Pool& apool);

protected:
		
	bool replace_field(const String& name, Value* value) {
		return 
			(fbase && fbase->replace_field(name, value)) ||
			ffields.put_replace(name, value);
	}

private: // self

	HashStringValue ffields;

};

#endif
