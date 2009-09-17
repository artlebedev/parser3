/**	@file
	Parser: @b object class impl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vobject.h"
#include "pa_vhash.h"
#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vmethod_frame.h"
#include "pa_request.h"

static const char * const IDENT_VOBJECT_C="$Date: 2009/09/17 23:31:42 $";

Value* VObject::get_scalar_value(char* as_something) const {
	VObject* unconst_this=const_cast<VObject*>(this);
	if(Value* scalar=fclass.get_scalar(*unconst_this))
		if(Junction* junction=scalar->get_junction())
			if(const Method *method=junction->method){
				VMethodFrame frame(*junction, 0/*no caller*/);

				Value *param;

				if(size_t param_count=frame.method_params_count()){
					if(param_count==1){
						param=new VString(*new String(as_something));
						frame.store_params(&param, 1);
					} else
						throw Exception(PARSER_RUNTIME,
							0,
							"scalar getter method can't have more then 1 parameter (has %d parameters)", param_count);
				} // no need for else frame.empty_params()

				frame.set_self(*unconst_this);
				return &pa_thread_request().execute_method(frame, *method).as_value();
			}
	return 0;
}

Value* VObject::as(const char* atype) {
	return fclass.as(atype) ? this:0;
}

bool VObject::is_defined() const {
	if(Value* value=get_scalar_value("def"))
		return value->is_defined();
	return Value::is_defined();
}

Value& VObject::as_expr_result(bool) {
	if(Value* value=get_scalar_value("expression"))
		return value->as_expr_result();
	return Value::as_expr_result();
}

int VObject::as_int() const {
	if(Value* value=get_scalar_value("int"))
		return value->as_int();
	return Value::as_int();
}

double VObject::as_double() const {
	if(Value* value=get_scalar_value("double"))
		return value->as_double();
	return Value::as_double();
}

bool VObject::as_bool() const { 
	if(Value* value=get_scalar_value("bool"))
		return value->as_bool();
	return Value::as_bool();
}

VFile* VObject::as_vfile(String::Language lang, const Request_charsets *charsets) {
	if(Value* value=get_scalar_value("file"))
		return value->as_vfile(lang, charsets);
	return Value::as_vfile(lang, charsets);
}

HashStringValue* VObject::get_hash() {
	if(Value* value=get_scalar_value("hash"))
		return value->get_hash();
	return &ffields;
}

Table *VObject::get_table() {
	if(Value* value=get_scalar_value("table"))
		return value->get_table();
	return Value::get_table();
}

Value* VObject::get_element(const String& aname) {
	// simple things first: $field=ffields.field
	if(Value* result=ffields.get(aname))
		return result;

	// class $virtual_method $virtual_property
	if(Value* result=fclass.get_element(*this, aname))
		return result;

	if(Value* result=fclass.get_default_getter(*this, aname))
		return result;

	return 0;
}

/// VObject: (field/property)=value
const VJunction* VObject::put_element(const String& aname, Value* avalue, bool /*areplace*/){
	if(const VJunction* result=fclass.put_element(*this, aname, avalue, true /*try to replace! NEVER overwrite*/))
		return result; // replaced in statics fields/properties
	ffields.put(aname,avalue);
	return 0;
}
