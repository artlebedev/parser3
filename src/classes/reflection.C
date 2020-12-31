/** @file
	Parser: @b reflection parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vmethod_frame.h"
#include "pa_request.h"
#include "pa_vbool.h"
#include "pa_vobject.h"

volatile const char * IDENT_REFLECTION_C="$Id: reflection.C,v 1.88 2020/12/31 12:08:34 moko Exp $";

static const String class_type_methoded("methoded");

static const String method_type_native("native");
static const String method_type_parser("parser");

static const String method_name("name");
static const String method_class_name("class");
static const String method_call_type("call_type");
static const String method_inherited("inherited");
static const String method_overridden("overridden");

static const String method_min_params("min_params");
static const String method_max_params("max_params");
static const String method_extra_param("extra_param");

static const String def_class("class");

// class

class MReflection: public Methoded {
public:
	MReflection();
};

// global variable

DECLARE_CLASS_VAR(reflection, new MReflection);

// methods

const int MAX_CREATE_PARAMS = 100;

static void _create(Request& r, MethodParams& params) {
	int params_offset;
	HashStringValue* params_hash=0;

	const String* class_name=0;
	const String* constructor_name=0;

	Value& voptions=params.as_no_junction(0, "param must not be code");
	if(HashStringValue* options=voptions.get_hash()) {
		int valid_options=0;
		if(Value* vclass_name=options->get("class")) {
			valid_options++;
			class_name=&vclass_name->as_string();
		}
		if(Value* vconstructor_name=options->get("constructor")) {
			valid_options++;
			constructor_name=&vconstructor_name->as_string();
		}
		if(Value* vparams_hash=options->get("arguments")) {
			valid_options++;
			params_hash=vparams_hash->as_hash("arguments");
			if(params.count()>1)
				throw Exception(PARSER_RUNTIME, 0, "arguments should not be specified as hash and as create params");
		}
		if(valid_options!=options->count())
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);

		if(!class_name)
			throw Exception(PARSER_RUNTIME, 0, "class name must be specified");
		if(!constructor_name)
			throw Exception(PARSER_RUNTIME, 0, "constructor name must be specified");

		params_offset=1;
	} else {
		class_name=&params.as_string(0, "param must not be code");

		if(params.count()==1)
			throw Exception(PARSER_RUNTIME, 0, "constructor name must be specified");

		constructor_name=&params.as_string(1, "constructor name must be string");

		params_offset=2;
	}

	VStateless_class& vclass=r.get_class_ref(*class_name);

	const Method* method=vclass.get_method(*constructor_name);
	if(!method)
		throw Exception(PARSER_RUNTIME, constructor_name, "constructor not found in class '%s'", vclass.type());

	Value &object = r.construct(vclass, *method);

	int nparams=params_hash ? params_hash->count() : (params.count()-params_offset);
	if(nparams>MAX_CREATE_PARAMS)
		throw Exception(PARSER_RUNTIME, 0, "arguments count should not exceed %d", MAX_CREATE_PARAMS);

	Value* args[MAX_CREATE_PARAMS];
	CONSTRUCTOR_FRAME_ACTION(*method, r.get_method_frame(), object, {
		if(nparams>0){
			if(params_hash){
				int i=0;
				for(HashStringValue::Iterator h(*params_hash); h; h.next())
					args[i++]=h.value();
			} else {
				for(int i=0; i<nparams; i++)
					args[i]=&r.process(params[i+params_offset]);
			}
			frame.store_params((Value**)&args, nparams);
		} else {
			frame.empty_params();
		}
		r.call(frame);
		object.enable_default_setter();
		r.write(frame.result());
	});
}


static void _classes(Request& r, MethodParams&) {
	VHash& result=*new VHash;
	for(HashString<VStateless_class*>::Iterator i(r.classes()); i; i.next()){
		result.hash().put(i.key(), i.value()->get_methods().count()>0 ? new VString(class_type_methoded) : VVoid::get() );
	}
	r.write(result);
}


static Value& get_class(Value& value){
	if(VStateless_class* result=value.get_class())
		return *result;
	else {
		// we can't return code junction to outside as it's stack value
		if(Junction *j=value.get_junction())
			if(j->code)
				throw Exception(PARSER_RUNTIME, 0, "param must not be code junction");
		// method junction
		return value;
	}
}

static const String& get_class_name(Value& value){
	return *new String(get_class(value).type());
}


static void _class(Request& r, MethodParams& params) {
	r.write(get_class(params[0]));
}


static void _class_name(Request& r, MethodParams& params) {
	r.write(get_class_name(params[0]));
}

static void _class_by_name(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	r.write(r.get_class_ref(class_name));
}

static void _base(Request& r, MethodParams& params) {
	if(VStateless_class* vclass=params[0].get_class())
		if(Value* base=vclass->base()){
			r.write(get_class(*base));
			return;
		}

	// classes with fields only, like env & console or without base
	r.write_value(*VVoid::get());
}


static void _base_name(Request& r, MethodParams& params) {
	if(VStateless_class* vclass=params[0].get_class())
		if(Value* base=vclass->base())
			r.write(get_class_name(*base));
}

static void _def(Request& r, MethodParams& params) {
	const String& type=params.as_string(0, "type must be string");
	if(type == def_class) {
		const String& name=params.as_string(1, "name must be string");
		// can't use get_class because it will call @autouse[] if the class wasn't loaded
		r.write(VBool::get(r.classes().get(name)!=0));
	} else {
		throw Exception(PARSER_RUNTIME, &type, "is invalid type, must be '%s'", def_class.cstr());
	}
}

static void _methods(Request& r, MethodParams& params) {
	const String& class_name=params.as_string(0, "class_name must be string");
	VStateless_class& vclass=r.get_class_ref(class_name);

	bool reverse=true;

	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1, "methods options")) {
			int valid_options=0;
			for(HashStringValue::Iterator i(*options); i; i.next() ){
				String::Body key=i.key();
				Value* value=i.value();
				if(key == "reverse") {
					reverse=r.process(*value).as_bool();
					valid_options++;
				}
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	VHash& result=*new VHash;

#ifdef HASH_ORDER
	if(reverse){
		for(HashStringMethod::ReverseIterator i(vclass.get_methods()); i; i.prev()){
			result.hash().put(i.key(), new VString(i.value()->native_code ? method_type_native : method_type_parser));
		}
	} else {
#else
	{
#endif
		for(HashStringMethod::Iterator i(vclass.get_methods()); i; i.next()){
			result.hash().put(i.key(), new VString(i.value()->native_code ? method_type_native : method_type_parser));
		}
	}

	r.write(result);
}

static VJunction &method_junction(Value &self, Method &method){
	if(method.native_code)
		throw Exception(PARSER_RUNTIME, method.name, "method must not be native");

	if(!(dynamic_cast<VObject*>(&self) || dynamic_cast<VClass*>(&self)))
		throw Exception(PARSER_RUNTIME, 0, "self must be parser object or class");

	return *method.get_vjunction(self);
}

static void _method(Request& r, MethodParams& params) {
	Value &source=params[0];

	if(Junction *j=source.get_junction()){
		if(Method* method=const_cast<Method*>(j->method)){
			Value& self=params.count()>1 ? params.as_no_junction(1, "self must be object, not junction") : r.get_method_frame()->caller()->self();
			r.write(method_junction(self, *method));
			return;
		}
		throw Exception(PARSER_RUNTIME, 0, "param must be method junction");
	}

	if(params.count()==1)
		throw Exception(PARSER_RUNTIME, 0, "method name must be specified");

	const String& name=params.as_string(1, "method name must be string");

	if(VStateless_class* vclass=source.get_class()) {
		if(Method* method=vclass->get_method(name)){
			r.write( params.count()>2 ? method_junction(params.as_no_junction(2, "self must be object, not junction"), *method) : *method->get_vjunction(source) );
			return;
		}
	}
	r.write_value(*VVoid::get());
}

static void _fields(Request& r, MethodParams& params) {
	Value& o=params.as_no_junction(0, "param must be object or class, not junction");

	if(HashStringValue* fields=o.get_fields())
		r.write(*new VHash(*fields));
	else
		r.write(*new VHash());
}

static void _fields_reference(Request& r, MethodParams& params) {
	Value& o=params.as_no_junction(0, "param must be object or hash, not junction");

	if(HashStringValue* fields=o.get_fields_reference())
		r.write(*new VHashReference(*fields));
	else
		throw Exception(PARSER_RUNTIME, 0, "param must be object or hash");
}

static void _field(Request& r, MethodParams& params) {
	Value& o=params.as_no_junction(0, "first param must be object or class, not junction");
	const String& name=params.as_string(1, "field name must be string");

	if(HashStringValue* fields=o.get_fields())
		if(Value* value=fields->get(name))
			r.write(*value);
}

static void _method_info(Request& r, MethodParams& params) {
	const Method* method;

	VHash& result=*new VHash;
	HashStringValue* hash=result.get_hash();

	if(Junction *j=params[0].get_junction()){
		if(!(method=j->method))
			throw Exception(PARSER_RUNTIME, 0, "param must be class name or method junction");

		hash->put(method_name, new VString(*method->name));
		hash->put(method_class_name, new VString(*new String(j->self.type())));

	} else {
		const String& class_name=params.as_string(0, "param must be class name or method junction");
		VStateless_class& vclass=r.get_class_ref(class_name);

		if(params.count()==1)
			throw Exception(PARSER_RUNTIME, 0, "method name must be specified");

		const String& method_name=params.as_string(1, "method name must be string");
		if(!(method=vclass.get_method(method_name)))
			throw Exception(PARSER_RUNTIME, &method_name, "method not found in class '%s'", vclass.type());

		Method* base_method;
		if(vclass.base() && (base_method=vclass.base()->get_method(*method->name))){
			VStateless_class* c=vclass.base()->get_class();
			while(c->base() && base_method==c->base()->get_method(*method->name))
				c=c->base()->get_class();
			hash->put((base_method==method) ? method_inherited : method_overridden, new VString(*new String(c->type())));
		}
	}

	Value* call_type=0;
	switch(method->call_type){
		case Method::CT_DYNAMIC:
			call_type=new VString(Symbols::DYNAMIC_SYMBOL);
			break;
		case Method::CT_STATIC:
			call_type=new VString(Symbols::STATIC_SYMBOL);
			break;
		case Method::CT_ANY:
			break;
	}
	if(call_type)
		hash->put(method_call_type, call_type);

	if(method->native_code){
		// native code
		hash->put(method_min_params, new VInt(method->min_numbered_params_count));
		hash->put(method_max_params, new VInt(method->max_numbered_params_count));
	} else {
		// parser code
		const String* filespec = r.get_method_filespec(method);
		if( filespec )
			hash->put("file", new VString(*filespec));

		hash->put(method_max_params, new VInt(method->params_names ? method->params_names->count() : 0));

		if(method->params_names)
			for(size_t i=0; i<method->params_names->count(); i++)
				hash->put(String::Body::Format(i), new VString(*method->params_names->get(i)));

		if(method->extra_params)
			hash->put(method_extra_param, new VString(*method->extra_params));
	}

	r.write(result);
}

static void _filename(Request& r, MethodParams& params) {
	if(Junction *j=params[0].get_junction()){
		if(const Method* method=j->method){
			if(!method->native_code)
				if(const String* filespec = r.get_method_filespec(method))
					r.write(*new VString(*filespec));
			return;
		}
		throw Exception(PARSER_RUNTIME, 0, "param must be object, class or method junction");
	}

	if(VClass* vclass = dynamic_cast<VClass*>(params[0].get_class())){
		r.write(*new VString(vclass->get_filespec()));
	}
}

static void _dynamical(Request& r, MethodParams& params) {
	if(params.count()){
		r.write(VBool::get(params[0].get_class() != &params[0]));
	} else {
		VMethodFrame* caller=r.get_method_frame()->caller();
		r.write(VBool::get(caller && caller->get_class() != &caller->self()));
	}
}

static void _is(Request& r, MethodParams& params) {
	const String& name=params.as_string(0, "element name must be string");
	const String& type=params.as_string(1, "class name must be string");
	Value *context=params.count()==3 ? &(params.as_no_junction(2, "context must not be code")) : r.get_method_frame()->caller();
	Value *value=context ? context->get_element(name) : 0;

	if(value) {
		if(type == "code" || type == "method") {
			Junction *junction=value->get_junction();
			r.write(VBool::get(junction && ((junction->code==0) ^ (type == "code"))) );
		} else {
			r.write(VBool::get( value->is(type.cstr()) ));
		}
	} else
		r.write(VBool::get(type == "void"));
}

static void _copy(Request& r, MethodParams& params) {
	HashStringValue* src=params.as_no_junction(0, "source must not be code").get_hash();

	if(src==NULL)
		throw Exception(PARSER_RUNTIME, 0, "source must have hash representation");

	Value& dst=params.as_no_junction(1, "destination must not be code");

	for(HashStringValue::Iterator i(*src); i; i.next())
		r.put_element(dst, *new String(i.key(), String::L_TAINTED), i.value());
}

static void _uid(Request& r, MethodParams& params) {
	Value& obj=params.as_no_junction(0, "object must not be code");

	char local_buf[MAX_NUMBER];
	int size=snprintf(local_buf, sizeof(local_buf), "%p", &obj);

	r.write(*new String(String::C(pa_strdup(local_buf, (size_t)size), size)));
}

static void _delete(Request&, MethodParams& params) {
	Value* v=&params.as_no_junction(0, "param must be object or class, not junction");
	const String& key=params.as_string(1, "field name must be string");

	if(VObject* o=dynamic_cast<VObject*>(v)){
		o->get_fields()->remove(key);
	} else if(VClass* c=dynamic_cast<VClass*>(v)){
		HashStringProperty &p=*c->get_properties();
		if(Property* property=p.get(key))
			if(property->value)
				p.remove(key);
	}
}

static void _mixin(Request& r, MethodParams& params) {
	Value& vsource=params.as_no_junction(0, "source must not be code");

	Value* vtarget=0;
	const String *name=0;
	bool copy_methods=true;
	bool copy_fields=true;
	bool overwrite=false;

	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1, "mixin options")) {
			int valid_options=0;
			for(HashStringValue::Iterator i(*options); i; i.next() ){
				String::Body key=i.key();
				Value* value=i.value();
				if(key == "to") {
					vtarget=value;
					valid_options++;
				} else if(key == "name") {
					name=&value->as_string();
					valid_options++;
				} else if(key == "methods") {
					copy_methods=r.process(*value).as_bool();
					valid_options++;
				} else if(key == "fields") {
					copy_fields=r.process(*value).as_bool();
					valid_options++;
				} else if(key == "overwrite") {
					overwrite=r.process(*value).as_bool();
					valid_options++;
				}
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	if(!vtarget)
		vtarget=&r.get_method_frame()->caller()->self();

	VClass* source=dynamic_cast<VClass*>(vsource.get_class());
	VClass* target=dynamic_cast<VClass*>(vtarget->get_class());

	if(!source)
		throw Exception(PARSER_RUNTIME, 0, "source must be parser object or class");
	if(!target)
		throw Exception(PARSER_RUNTIME, 0, "destination must be parser object or class");

	if(name){
		if(copy_methods)
			if(Method* method=source->get_method(*name))
				if(overwrite || !target->get_method(*name)){
					target->set_method(*name, new Method(*method));
				}

		if(copy_fields)
			if(Property* property=source->get_properties()->get(*name))
				if(property->value && (overwrite || !target->get_properties()->get(*name))){
					target->put_element(*target, *name, property->value);
				}

	} else {
		if(copy_methods)
			for(HashStringMethod::Iterator i(source->get_methods()); i; i.next()){
				if(overwrite || !target->get_method(i.key()))
					target->set_method(*i.value()->name, new Method(*i.value()));
			}
		if(copy_fields)
			for(HashStringProperty::Iterator i(*source->get_properties()); i; i.next()){
				if(i.value()->value && ( overwrite || !target->get_properties()->get(i.key()) ))
					target->put_element(*target, *new String(i.key(), String::L_TAINTED), i.value()->value);
			}
	}
}

String::Language get_untaint_lang(const String& lang_name); // op.C

static void _tainting(Request& r, MethodParams& params) {
	const String& str=params.as_string(params.count()-1, "param must be string");
	String::Language lang = String::L_UNSPECIFIED;
	bool optimized=false;

	if(params.count()==2){
		const String& slang=params.as_string(0, "language name must be string");
		if(slang == "optimized")
			optimized=true;
		else if(slang == "tainted")
			lang=String::L_TAINTED;
		else lang=get_untaint_lang(slang);
	}

	if(!str.is_empty()){
		char *visual=str.visualize_langs();

		if(optimized){
			for(char *c=visual; *c; c++)
				*c = *c<0 ? '+':'-';
		} else if(lang != String::L_UNSPECIFIED){
			for(char *c=visual; *c; c++)
				*c = *c==lang ? '+':'-';
		} else {
			for(char *c=visual; *c; c++)
				*c = *c & 0x7F;
		}

		r.write(*new String(visual));
	}
}

static void _stack(Request& r, MethodParams& params) {
	bool show_args=false;
	bool show_locals=false;

	int limit=1000000;
	int offset=0;

	if(params.count()>0)
		if(HashStringValue* options=params.as_hash(0, "stack options")) {
			int valid_options=0;
			for(HashStringValue::Iterator i(*options); i; i.next() ){
				String::Body key=i.key();
				Value* value=i.value();

				if(key == "args") {
					show_args=r.process(*value).as_bool();
					valid_options++;
				} else if(key == "locals") {
					show_locals=r.process(*value).as_bool();
					valid_options++;
				} else if(key == "limit") {
					limit=r.process(*value).as_int();
					valid_options++;
				} else if(key == "offset") {
					offset=r.process(*value).as_int();
					valid_options++;
				}
			}

			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	limit+=offset;

	VHash& vresult=*new VHash;
	HashStringValue* result=vresult.get_hash();
	int index=1;
	VMethodFrame* caller=r.get_method_frame()->caller();
	while(caller && index <= limit){
		if(index>offset){
			VHash& vcurrent=*new VHash;
			HashStringValue* current=vcurrent.get_hash();

			current->put(Symbols::SELF_SYMBOL, &caller->self());

			const Method& method=caller->method;

			current->put(method_name, new VString(*method.name));

			if(!method.native_code){
				Operation::Origin origin=r.get_method_origin(&method);
				if(origin.file_no){
					current->put("file", new VString(*r.get_used_filespec(origin.file_no)));
					current->put("line", new VInt(origin.line)); // no +1 as declaration before first command
				}

				if(show_args || show_locals){

					VHash& vargs=*new VHash;
					HashStringValue* args=vargs.get_hash();

					if(method.params_names){
						for(size_t i=0; i<method.params_names->count(); i++){
							const String& pname=*(*method.params_names)[i];
							Value* value=caller->get_element(pname);
							args->put(pname, value->get_junction() ? VVoid::get() : value);
						}
					}
					if(method.extra_params)
						args->put(*method.extra_params, caller->get_element(*method.extra_params));

					if(show_args)
						current->put("args", &vargs);

					if(show_locals){
						VHash& vlocals=*new VHash;
						HashStringValue* locals=vlocals.get_hash();

						if(VParserMethodFrame* frame=dynamic_cast<VParserMethodFrame*>(caller))
							for(HashString<Value*>::Iterator h(frame->my); h; h.next()){
								String::Body key=h.key();
								Value* value=h.value();
								if(!args->contains(key) && (key != "result")){
									locals->put(key, value->get_junction() ? VVoid::get() : value);
								}
							}

						current->put("locals", &vlocals);
					}
				}
			}

			result->put(format(index, 0), &vcurrent);
		}
		caller=caller->caller();
		index++;
	}

	r.write(vresult);
}

// constructor
MReflection::MReflection(): Methoded("reflection") {
	// ^reflection:create[class_name;constructor_name[;param1[;param2[;...]]]]
	// ^reflection:create[ $.class[name] $.constructor[name] $.arguments[ $.1[param1] $.2[param2] ...] ]
	add_native_method("create", Method::CT_STATIC, _create, 1, MAX_CREATE_PARAMS + 2);

	// ^reflection:classes[]
	add_native_method("classes", Method::CT_STATIC, _classes, 0, 0);

	// ^reflection:class[object]
	add_native_method("class", Method::CT_STATIC, _class, 1, 1);

	// ^reflection:class_name[object]
	add_native_method("class_name", Method::CT_STATIC, _class_name, 1, 1);

	// ^reflection:class_by_name[class_name]
	add_native_method("class_by_name", Method::CT_STATIC, _class_by_name, 1, 1);

	// ^reflection:base_class[object]
	add_native_method("base", Method::CT_STATIC, _base, 1, 1);

	// ^reflection:base_class_name[object]
	add_native_method("base_name", Method::CT_STATIC, _base_name, 1, 1);

	// ^reflection:def[class|...;name]
	add_native_method("def", Method::CT_STATIC, _def, 2, 2);

	// ^reflection:methods[class_name]
	add_native_method("methods", Method::CT_STATIC, _methods, 1, 2);

	// ^reflection:method[object or class;method_name[;self]]
	// ^reflection:method[junction[;self]]
	add_native_method("method", Method::CT_STATIC, _method, 1, 3);

	// ^reflection:method_info[class_name;method_name]
	// ^reflection:method_info[junction]
	add_native_method("method_info", Method::CT_STATIC, _method_info, 1, 2);

	// ^reflection:filename[object or class or method]
	add_native_method("filename", Method::CT_STATIC, _filename, 1, 1);

	// ^reflection:fields[object or class]
	add_native_method("fields", Method::CT_STATIC, _fields, 1, 1);

	// ^reflection:fields_reference[object]
	add_native_method("fields_reference", Method::CT_STATIC, _fields_reference, 1, 1);

	// ^reflection:field[object or class;field_name]
	add_native_method("field", Method::CT_STATIC, _field, 2, 2);

	// ^reflection:dynamical[[object or class, caller if absent]]
	add_native_method("dynamical", Method::CT_STATIC, _dynamical, 0, 1);

	// ^reflection:is[element_name;class_name|code|method[;context]]
	add_native_method("is", Method::CT_STATIC, _is, 2, 3);

	// ^reflection:copy[src;dst]
	add_native_method("copy", Method::CT_STATIC, _copy, 2, 2);

	// ^reflection:uid[object or class]
	add_native_method("uid", Method::CT_STATIC, _uid, 1, 1);

	// ^reflection:delete[object or class;field_name]
	add_native_method("delete", Method::CT_STATIC, _delete, 2, 2);

	// ^reflection:mixin[object or class or junction;options]
	add_native_method("mixin", Method::CT_STATIC, _mixin, 1, 2);

	// ^reflection:tainting[[language or 'tainted' or 'optimized';]string]
	add_native_method("tainting", Method::CT_STATIC, _tainting, 1, 2);

	// ^reflection:stack[options]
	add_native_method("stack", Method::CT_STATIC, _stack, 0, 1);

}
