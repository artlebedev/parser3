/** @file
	Parser: @b reflection parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_REFLECTION_C="$Date: 2009/07/24 09:29:29 $";

#include "pa_vmethod_frame.h"
#include "pa_request.h"
#include "pa_vbool.h"

// class

class MReflection: public Methoded {
public:
	MReflection();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(reflection, new MReflection, 0);

// methods

static void _construct(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	Value* class_value=r.classes().get(class_name);

	if(!class_value)
		throw Exception(PARSER_RUNTIME,
			&class_name,
			"class is undefined");

	const String& constructor_name=params.as_string(1, "constructor_name must be string");
	Value* constructor_value=class_value->get_element(constructor_name, *class_value, true);

	if(!constructor_value || !constructor_value->get_junction())
		throw Exception(PARSER_RUNTIME,
			&constructor_name,
			"constructor must be declared in class '%s'",
			class_name.cstr());

	Junction* junction=constructor_value->get_junction();

	VMethodFrame frame(*junction, r.get_method_frame());

	size_t nparams=params.count()-2;
    Value* v[100];

	if(nparams>0){
		for(size_t i=0; i<nparams; i++)
			v[i]=&r.process_to_value(params[i+2]);
		frame.store_params((Value**)&v, nparams);
	} else {
		frame.empty_params();
	}
	r.op_call(frame, true/*constructing*/);
	r.write_pass_lang(frame.result());
}

static void _class(Request& r, MethodParams& params) {
	if(Value* lclass=params[0].get_class())
		r.write_no_lang(*lclass);
	else
		throw Exception(PARSER_RUNTIME,
			0,
			"class was not defined");
}

static void _class_name(Request& r, MethodParams& params) {
	r.write_no_lang(String(params[0].type()));
}

static void store_method_info(
		HashString<Method*>::key_type key, 
		HashString<Method*>::value_type value,
		HashStringValue* result
) {
	VHash* method_info=new VHash;
	HashStringValue* method_hash=method_info->get_hash();
	bool native=false;

	if(value->native_code){
		// native code
		native=true;
		if(value->min_numbered_params_count)
			method_hash->put(String("min_params"), new VInt(value->min_numbered_params_count));

		if(value->max_numbered_params_count)
			method_hash->put(String("max_params"), new VInt(value->max_numbered_params_count));
	} else {
		// parser code
		if(value->params_names){
			VHash* params=new VHash;
			method_hash->put(String("params"), params);
			HashStringValue* params_hash=params->get_hash();

			for(size_t i=0; i<value->params_names->count(); i++)
				params_hash->put(String::Body::Format(i), new VString(*value->params_names->get(i)));
		}

		if(value->locals_names){
			VHash* locals=new VHash;
			method_hash->put(String("locals"), locals);
			HashStringValue* locals_hash=locals->get_hash();
	
			for(size_t i=0; i<value->locals_names->count(); i++)
				locals_hash->put(String::Body::Format(i), new VString(*value->locals_names->get(i)));
		}
	}

	method_hash->put(String("native"), &VBool::get(native));
	result->put(key, method_info);
}

static void _methods(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	Value* class_value=r.classes().get(class_name);

	if(!class_value)
		throw Exception(PARSER_RUNTIME,
			&class_name,
			"class is undefined");

	VHash& result=*new VHash;
	if(VStateless_class* lclass=class_value->get_class()){
		HashString<Method*> methods=lclass->get_methods();
		methods.for_each(store_method_info, result.get_hash());
	} else {
		// exception?
	}
	r.write_no_lang(result);
}

// constructor
MReflection::MReflection(): Methoded("reflection") {
	// ^reflection:construct[class;constructor;params[;]]
	add_native_method("construct", Method::CT_STATIC, _construct, 2, 102);

	// ^reflection:class[object]
	add_native_method("class", Method::CT_STATIC, _class, 1, 1);

	// ^reflection:class_name[object]
	add_native_method("class_name", Method::CT_STATIC, _class_name, 1, 1);

	// ^reflection:get_methods[object]
	add_native_method("methods", Method::CT_STATIC, _methods, 1, 1);
}
