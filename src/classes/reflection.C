/** @file
	Parser: @b reflection parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_REFLECTION_C="$Date: 2009/07/26 05:41:54 $";

#include "pa_vmethod_frame.h"
#include "pa_request.h"
#include "pa_vbool.h"

const char* const METHOD_TYPE_NATIVE="native";
const char* const METHOD_TYPE_PARSER="parser";

const char* const METHOD_CALL_TYPE_STATIC="static";
const char* const METHOD_CALL_TYPE_DYNAMIC="dynamic";
const char* const METHOD_CALL_TYPE_ANY="any";

const char* const METHOD_MIN_PARAMS="min_params";
const char* const METHOD_MAX_PARAMS="max_params";

static const String method_type_native(METHOD_TYPE_NATIVE);
static const String method_type_parser(METHOD_TYPE_PARSER);

static const String method_call_type_static(METHOD_CALL_TYPE_STATIC);
static const String method_call_type_dynamic(METHOD_CALL_TYPE_DYNAMIC);
static const String method_call_type_any(METHOD_CALL_TYPE_ANY);

static const String method_min_params(METHOD_MIN_PARAMS);
static const String method_max_params(METHOD_MAX_PARAMS);

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


static void _create(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	Value* class_value=r.classes().get(class_name);

	if(!class_value)
		throw Exception(PARSER_RUNTIME,
			&class_name,
			"class is undefined");

	const String& constructor_name=params.as_string(1, "constructor_name must be string");
	Value* constructor_value=class_value->get_element(constructor_name, *class_value, true);

	if(!constructor_value || !constructor_value->get_junction() || constructor_value->get_junction()->self.get_class()!=class_value)
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
		max_params_count=method->params_names->count();
	}

	if(nparams>max_params_count)
		throw Exception(PARSER_RUNTIME,
			&constructor_name,
			"method of class '%s' (%s) accepts maximum %d parameter(s) (%d passed)",
			class_value->get_class()->name_cstr(),
			class_value->type(),
			max_params_count,
			nparams);

	VMethodFrame frame(*junction, r.get_method_frame());

    Value* v[100];
	if(nparams>0){
		for(int i=0; i<nparams; i++)
			v[i]=&r.process_to_value(params[i+2]);
		frame.store_params((Value**)&v, nparams);
	} else {
		frame.empty_params();
	}
	r.op_call(frame, true/*constructing*/);
	r.write_pass_lang(frame.result());
}


// @todo: something wring with native classes (it returns stateless_class)
static void _class(Request& r, MethodParams& params) {
	if(Value* lclass=params[0].get_last_derived_class())
		r.write_no_lang(*lclass);
	else
		throw Exception(PARSER_RUNTIME,
			0,
			"class is undefined");
}


static void _class_name(Request& r, MethodParams& params) {
	r.write_no_lang(String(params[0].type()));
}


static void _base(Request& r, MethodParams& params) {
	if(Value* lclass=params[0].get_class())
		if(Value* base=lclass->base())
			r.write_no_lang(*lclass->base());
		else
			r.write_no_lang(*VVoid::get());
	else
		throw Exception(PARSER_RUNTIME,
			0,
			"class is undefined");
}


static void _base_name(Request& r, MethodParams& params) {
	if(Value* lclass=params[0].get_class())
		if(Value* base=lclass->base())
			r.write_no_lang(String(base->type()));
}


static void store_method_info(
		HashString<Method*>::key_type key, 
		HashString<Method*>::value_type method,
		HashStringValue* result
) {
	result->put(key, new VString(method->native_code?method_type_native:method_type_parser));
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
/*
	} else {
		// exception?
		throw Exception(PARSER_RUNTIME,
			&class_name,
			"class does not have methods");
*/
	}
	r.write_no_lang(result);
}


static void _method_params(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	Value* class_value=r.classes().get(class_name);
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
	if(method->native_code){
		// native code
		hash->put(method_min_params, new VInt(method->min_numbered_params_count));
		hash->put(method_max_params, new VInt(method->max_numbered_params_count));
		const String* call_type;
		switch(method->call_type){
			case Method::CT_DYNAMIC:
				call_type=&method_call_type_dynamic;
				break;
			case Method::CT_STATIC:
				call_type=&method_call_type_static;
				break;
			default:
				call_type=&method_call_type_any;
		}
		hash->put(String("call_type"), new VString(*call_type));
		
	} else {
		// parser code
		if(method->params_names)
			for(size_t i=0; i<method->params_names->count(); i++)
				hash->put(String::Body::Format(i), new VString(*method->params_names->get(i)));
	}

	r.write_no_lang(result);
}

// constructor
MReflection::MReflection(): Methoded("reflection") {
	// ^reflection:create[class_name;constructor_name[;param1[;param2[;...]]]]
	add_native_method("create", Method::CT_STATIC, _create, 2, 102);

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

	// ^reflection:method_params[class_name;method_name]
	add_native_method("method_params", Method::CT_STATIC, _method_params, 2, 2);

	// @todo: check how 'create' and 'methods' work with ancestors-classes
}
