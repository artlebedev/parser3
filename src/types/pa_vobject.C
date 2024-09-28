/**	@file
	Parser: @b object class impl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_vobject.h"
#include "pa_vhash.h"
#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vmethod_frame.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VOBJECT_C="$Id: pa_vobject.C,v 1.55 2024/09/28 14:37:54 moko Exp $" IDENT_PA_VOBJECT_H;

Value* VObject::get_scalar_value(const char* as_something) const {
	VObject* unconst_this=const_cast<VObject*>(this);
	if(Value* scalar=fclass.get_scalar(*unconst_this))
		if(Junction* junction=scalar->get_junction())
			if(const Method *method=junction->method){
				if(method->params_count>1)
					throw Exception(PARSER_RUNTIME, 0, "scalar getter method can't have more than 1 parameter (has %d parameters)", method->params_count);
				METHOD_FRAME_ACTION(*method, 0 /*no caller*/, *unconst_this, {
					Value *param;
					if(method->params_count==1){
						param=new VString(*new String(as_something));
						frame.store_params(&param, 1);
					} /* no need for else frame.empty_params() */
					pa_thread_request().call(frame);
					return &frame.result();
				});
			}
	return 0;
}

bool VObject::is(const char* atype) {
	return fclass.is(atype);
}

bool VObject::is_defined() const {
	if(Value* value=get_scalar_value("def"))
		return value->as_bool();
	return Value::is_defined();
}

Value& VObject::as_expr_result() {
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

VFile* VObject::as_vfile() {
	if(Value* value=get_scalar_value("file"))
		return value->as_vfile();
	return Value::as_vfile();
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
	// object field
	if(Value* result=ffields.get(aname))
		return result;

	// class method or property, or _object_ default getter
	return fclass.get_element(*this, aname);
}

#ifdef FEATURE_GET_ELEMENT4CALL
// get_element copy to remove extra virtual call
Value* VObject::get_element4call(const String& aname) {
	// object field
	if(Value* result=ffields.get(aname))
		return result;

	// class method or property, or _object_ default getter
	if(Value* result=fclass.get_element(*this, aname))
		return result;

	return bark("%s method not found", &aname);
}
#endif

const VJunction* VObject::put_element(const String& aname, Value* avalue){
	// class setter
	if(const VJunction* result=fclass.put_element_replace_only(*this, aname, avalue))
		return result == PUT_ELEMENT_REPLACED_FIELD ? 0 : result;
	
	// object field or default setter, avoiding virtual is_enabled_default_setter call
	if (state & IS_SETTER_ACTIVE){
		return ffields.put_replaced(aname, avalue) ? 0 : fclass.get_default_setter(*this, aname); 
	} else {
		ffields.put(aname, avalue);
	}
	return 0;
}

const String* VObject::get_json_string(Json_options& options){
	if(options.default_method){
		return default_method_2_json_string(*options.default_method, options);
	}
	return options.hash_json_string(get_hash());
}
