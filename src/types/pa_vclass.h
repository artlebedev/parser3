/**	@file
	Parser: @b class parser class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

static const char * const IDENT_VCLASS_H="$Date: 2009/08/11 10:18:43 $";

// includes

#include "pa_vstateless_class.h"
#include "pa_vjunction.h"
#include "pa_vobject.h"

/**	stores 
- static fields, getters & setters: VClass::ffields
*/
class VClass: public VStateless_class {
public: // Value
	
	override const char* type() const { return name_cstr(); }

	/// VClass: true
	override bool as_bool() const { return true; }
	override Value* as(const char* atype);

	override Value* get_element(Value& aself, const String& aname);
	override const VJunction* put_element(Value& self, const String& name, Value* value, bool replace);
	override Value* create_new_value(Pool&);

public: 
	
	// VStateless_class
	override void set_method(const String& aname, Method* amethod);
	override HashStringProperty* get_properties(){ return &ffields; };
	override void set_base(VStateless_class* abase);

private:

	Property& get_property(const String& aname);

private: // self

	HashStringProperty ffields;

};

#endif
