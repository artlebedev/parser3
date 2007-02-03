/**	@file
	Parser: @b class parser class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

static const char * const IDENT_VCLASS_H="$Date: 2007/02/03 18:08:38 $";

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
	override bool as_bool() const { return true; }

	override Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);
	override const VJunction* put_element(Value& self, const String& name, Value* value, bool replace);
	override Value* create_new_value(Pool& apool, HashStringValue& afields);

public: // VStateless_class

	/// override to pre-cache property accessors into fields
	override void add_method(const String& name, Method& method);

private:

	Property& add_property(const String& aname);

private:

	struct Prevent_info {
		VClass* _this;
		Value* self;
		const String* name;
	};
	static const VJunction* prevent_overwrite_property(Value* value, Prevent_info* info);
	static const VJunction* prevent_append_if_exists_in_base(Value* value, Prevent_info* info);

private: // self

	HashStringValue ffields;

};

#endif
