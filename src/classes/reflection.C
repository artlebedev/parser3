/** @file
	Parser: @b reflection parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_REFLECTION_C="$Date: 2010/10/21 15:06:29 $";

#include "pa_vmethod_frame.h"
#include "pa_request.h"
#include "pa_vbool.h"

static const String class_type_methoded("methoded");

static const String method_type_native("native");
static const String method_type_parser("parser");

static const String method_call_type("call_type");
static const String method_inherited("inherited");
static const String method_call_type_static("static");
static const String method_call_type_dynamic("dynamic");

static const String method_min_params("min_params");
static const String method_max_params("max_params");

// class

class MReflection: public Methoded {
public:
	MReflection();
};

// global variable

DECLARE_CLASS_VAR(reflection, new MReflection, 0);

// methods


static void _create(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	Value* class_value=r.get_class(class_name);

	if(!class_value)
		throw Exception(PARSER_RUNTIME,
			&class_name,
			"class is undefined");

	const String& constructor_name=params.as_string(1, "constructor_name must be string");
	Value* constructor_value=class_value->get_element(constructor_name);

	if(!constructor_value || !constructor_value->get_junction())
		throw Exception(PARSER_RUNTIME,
			&constructor_name,
			"constructor must be declared in class '%s'",
			class_value->get_class()->name_cstr());

	Junction* junction=constructor_value->get_junction();
	const Method* method=junction->method;

	int nparams=params.count()-2;
	int max_params_count;

	if(method->native_code){
		if(method->call_type==Method::CT_STATIC)
			throw Exception(PARSER_RUNTIME,
				&constructor_name,
				"native method of class '%s' (%s) is not allowed to be called dynamically",
				class_value->get_class()->name_cstr(),
				class_value->type());

		if(nparams<method->min_numbered_params_count)
			throw Exception(PARSER_RUNTIME,
				&constructor_name,
				"native method of class '%s' (%s) accepts minimum %d parameter(s) (%d passed)",
				class_value->get_class()->name_cstr(),
				class_value->type(),
				method->min_numbered_params_count,
				nparams);

		max_params_count=method->max_numbered_params_count;
	} else {
		max_params_count=method->params_names?method->params_names->count():0;
	}

	if(nparams>max_params_count)
		throw Exception(PARSER_RUNTIME,
			&constructor_name,
			"method of class '%s' (%s) accepts maximum %d parameter(s) (%d passed)",
			class_value->get_class()->name_cstr(),
			class_value->type(),
			max_params_count,
			nparams);

	Value &object = r.construct(*class_value, *method);
	VConstructorFrame frame(*method, r.get_method_frame(), object);

	Value* v[100];
	if(nparams>0){
		for(int i=0; i<nparams; i++)
			v[i]=&r.process_to_value(params[i+2]);
		frame.store_params((Value**)&v, nparams);
	} else {
		frame.empty_params();
	}
	r.op_call(frame);
	object.enable_default_setter();
	r.write_pass_lang(frame.result());
}


static void store_vlass_info(
		HashStringValue::key_type key, 
		HashStringValue::value_type value,
		HashStringValue* result
){
	Value* v;
	if(value->get_class())
		v=new VString(class_type_methoded);
	else
		v=VVoid::get();
	result->put(key, v);
}

static void _classes(Request& r, MethodParams&) {
	VHash& result=*new VHash;
	r.classes().for_each(store_vlass_info, result.get_hash());
	r.write_no_lang(result);
}


static Value* get_class(Value* value){
	if(VStateless_class* result=value->get_class())
		return result;
	else
		// classes with fields only, like env & console
		return value;
}

static const String* get_class_name(Value* value){
	if(VStateless_class* lclass=value->get_class())
		return &lclass->name();
	else
		// classes with fields only, like env & console
		return new String(value->type());
}


static void _class(Request& r, MethodParams& params) {
	r.write_no_lang(*get_class(&params[0]));
}


static void _class_name(Request& r, MethodParams& params) {
	r.write_no_lang(*get_class_name(&params[0]));
}


static void _base(Request& r, MethodParams& params) {
	if(VStateless_class* lclass=params[0].get_class())
		if(Value* base=lclass->base()){
			r.write_no_lang(*get_class(base));
			return;
		}

	// classes with fields only, like env & console or without base
	r.write_no_lang(*VVoid::get());
}


static void _base_name(Request& r, MethodParams& params) {
	if(VStateless_class* lclass=params[0].get_class())
		if(Value* base=lclass->base())
			r.write_no_lang(*get_class_name(base));
}

static void store_method_info(
		HashStringMethod::key_type key, 
		HashStringMethod::value_type method,
		HashStringValue* result
) {
	result->put(key, new VString(method->native_code?method_type_native:method_type_parser));
}

static void _methods(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	Value* class_value=r.get_class(class_name);
	if(!class_value)
		throw Exception(PARSER_RUNTIME,
			&class_name,
			"class is undefined");

	VHash& result=*new VHash;
	if(VStateless_class* lclass=class_value->get_class()){
		HashStringMethod methods=lclass->get_methods();
		methods.for_each(store_method_info, result.get_hash());
	} else {
		// class which does not have methods (env, console, etc)
	}
	r.write_no_lang(result);
}

static void _fields(Request& r, MethodParams& params) {
	if(HashStringValue* fields=params[0].get_fields()){
		VHash& result=*new VHash(*fields);
		r.write_no_lang(result);
	} else
		r.write_no_lang(*new VHash());
}

static void _method_info(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	Value* class_value=r.get_class(class_name);
	if(!class_value)
		throw Exception(PARSER_RUNTIME,
			&class_name,
			"class is undefined");

	VStateless_class* lclass=class_value->get_class();
	if(!lclass)
		throw Exception(PARSER_RUNTIME,
			&class_name,
			"class does not have methods");

	const String& method_name=params.as_string(1, "method_name must be string");
	Method* method=lclass->get_method(method_name);
	if(!method)
		throw Exception(PARSER_RUNTIME,
			&method_name,
			"method not found in class %s",
			class_name.cstr());

	VHash& result=*new VHash;
	HashStringValue* hash=result.get_hash();

	VStateless_class* c=lclass;
	while(c->base() && c->base()->get_method(method_name))
		c=c->base()->get_class();

	if(c!=lclass)
		hash->put(method_inherited, new VString(c->name()));

	if(method->native_code){
		// native code
		hash->put(method_min_params, new VInt(method->min_numbered_params_count));
		hash->put(method_max_params, new VInt(method->max_numbered_params_count));
		Value* call_type=0;
		switch(method->call_type){
			case Method::CT_DYNAMIC:
				call_type=new VString(method_call_type_dynamic);
				break;
			case Method::CT_STATIC:
				call_type=new VString(method_call_type_static);
				break;
		}
		if(call_type)
			hash->put(method_call_type, call_type);
	} else {
		// parser code
		const String* filespec = r.get_method_filename(method);
		if( filespec )
			hash->put("file", new VString(*filespec));
		if(method->params_names)
			for(size_t i=0; i<method->params_names->count(); i++)
				hash->put(String::Body::Format(i), new VString(*method->params_names->get(i)));
	}

	r.write_no_lang(result);
}

static void _dynamical(Request& r, MethodParams& params) {
	if(params.count()){
		r.write_no_lang(VBool::get(params[0].get_class() != &params[0]));
	} else {
		VMethodFrame* caller=r.get_method_frame()->caller();
		r.write_no_lang(VBool::get(caller && caller->get_class() != &caller->self()));
	}
}

static void _copy(Request& r, MethodParams& params) {
	HashStringValue* src=params.as_no_junction(0, "source must not be code").get_hash();

	if(src==NULL) 
		throw Exception(PARSER_RUNTIME, 0, "source must have hash representation");

	Value& dst=params.as_no_junction(1, "destination must not be code");

	for(HashStringValue::Iterator i(*src); i; i.next())
		r.put_element(dst, *new String(i.key(), String::L_TAINTED), i.value());
}

// constructor
MReflection::MReflection(): Methoded("reflection") {
	// ^reflection:create[class_name;constructor_name[;param1[;param2[;...]]]]
	add_native_method("create", Method::CT_STATIC, _create, 2, 102);

	// ^reflection:classes[]
	add_native_method("classes", Method::CT_STATIC, _classes, 0, 0);

	// ^reflection:class[object]
	add_native_method("class", Method::CT_STATIC, _class, 1, 1);

	// ^reflection:class_name[object]
	add_native_method("class_name", Method::CT_STATIC, _class_name, 1, 1);

	// ^reflection:base_class[object]
	add_native_method("base", Method::CT_STATIC, _base, 1, 1);

	// ^reflection:base_class_name[object]
	add_native_method("base_name", Method::CT_STATIC, _base_name, 1, 1);

	// ^reflection:methods[class_name]
	add_native_method("methods", Method::CT_STATIC, _methods, 1, 1);

	// ^reflection:fields[object or class]
	add_native_method("fields", Method::CT_STATIC, _fields, 1, 1);

	// ^reflection:method_info[class_name;method_name]
	add_native_method("method_info", Method::CT_STATIC, _method_info, 2, 2);

	// ^reflection:dynamical[[object or class, caller if absent]]
	add_native_method("dynamical", Method::CT_STATIC, _dynamical, 0, 1);

	// ^reflection:copy[src;dst]
	add_native_method("copy", Method::CT_STATIC, _copy, 2, 2);
}
