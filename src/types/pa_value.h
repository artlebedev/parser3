/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_value.h,v 1.13 2001/03/14 08:50:06 paf Exp $
*/

/*
	data core
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_globals.h"

class Value;
class VStateless_class;
class Junction;
class WContext;
class VAliased;
class Request;
class VTable;

typedef void (*Native_code_ptr)(Request& request, const String& method_name, Array *params);

class Method : public Pooled {
public:
	const String& name;
	// either numbered params // for native-code methods = operators
	int min_numbered_params_count, max_numbered_params_count;
	// or named params&locals // for parser-code methods
	Array *params_names;  Array *locals_names;
	// the Code
	const Array *parser_code;/*OR*/Native_code_ptr native_code;

	Method(
		Pool& apool,
		const String& aname,
		int amin_numbered_params_count, int amax_numbered_params_count,
		Array *aparams_names, Array *alocals_names,
		const Array *aparser_code, Native_code_ptr anative_code) : 

		Pooled(apool),
		name(aname),
		min_numbered_params_count(amin_numbered_params_count),
		max_numbered_params_count(amax_numbered_params_count),
		params_names(aparams_names), locals_names(alocals_names),
		parser_code(aparser_code), native_code(anative_code) {
	}

	void check_actual_numbered_params(
		const String& actual_name, Array *actual_numbered_params) const {
		int actual_count=actual_numbered_params?actual_numbered_params->size():0;
		if(actual_count<min_numbered_params_count) // not proper count? bark
			THROW(0, 0,
				&actual_name,
				"native method accepts minimum %d parameter(s)", 
					min_numbered_params_count);

	}
};

class Junction : public Pooled {
public:

	Junction(Pool& apool,
		Value& aself,
		VStateless_class *avclass, const Method *amethod,
		Value *aroot,
		Value *arcontext,
		WContext *awcontext,
		const Array *acode) : Pooled(apool),
		
		self(aself),
		vclass(avclass), method(amethod),
		root(aroot),
		rcontext(arcontext),
		wcontext(awcontext),
		code(acode) {
	}

	// always present
	Value& self;
	// either these // so called 'method-junction'
	VStateless_class *vclass;  const Method *method;
	// or these are present // so called 'code-junction'
	Value *root;
	Value *rcontext;
	WContext *wcontext;
	const Array *code;
};

class Value : public Pooled {
public: // Value

	// all: for error reporting after fail(), etc
	virtual const char *type() const =0;
	const String& name() const { return *fname; }

	// unknown: false
	// others: true
	virtual bool get_defined() { return true; }
	// string: fvalue as VDouble
	// bool: this
	// double: this
	// int: this
	virtual Value *get_expr_result() { bark("(%s) can not be used in expression"); return 0; }

	// string: value
	// unknown: ""
	// double: value
	// bool: must be 0: so in ^if(1>2) it would'nt become "FALSE" string which is 'true'
	// others: 0
	virtual const String *get_string() { return 0; }
	
	// string: value
	// double: value
	// integer: finteger
	// bool: value
	virtual double get_double() { bark("(%s) does not have numerical value"); return 0; }

	// unknown: false
	// bool: value
	// double: 0 or !0
	// string: empty or not
	// hash: size!=0
	// table: empty or not
	// others: true
	virtual bool get_bool() { return true; }

	// junction: itself
	virtual Junction *get_junction() { return 0; }

	// table: itself
	virtual VTable *get_vtable() { return 0; }

	// hash: (key)=value
	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	// object_base: (CLASS)=vclass;(BASE)=base;(method)=method_ref
	// object_instance: (field)=value;(CLASS)=vclass;(method)=method_ref
	// operator_class: (field)=value - static values only
	// codeframe: wcontext_transparent
	// methodframe: my or self_transparent
	// table: column
	// env: CLASS,BASE,method,field
	// form: CLASS,BASE,method,field
	virtual Value *get_element(const String& name) { bark("(%s) does not have elements"); return 0; }
	
	// hash: (key)=value
	// object_class, operator_class: (field)=value - static values only
	// object_instance: (field)=value
	// codeframe: wcontext_transparent
	// methodframe: my or self_transparent
	virtual void put_element(const String& name, Value *value) { bark("(%s) does not accept elements"); }

	// object_class, object_instance: object_class
	// wcontext: none yet | transparent
	// form: this
	// class: this
	// env: this
	virtual VStateless_class *get_class() { return 0; }

	// valiased: this
	// wcontext: transparent
	// methodframe: self_transparent
	virtual VAliased *get_aliased() { return 0; }

public: // usage

	Value(Pool& apool) : Pooled(apool), fname(unnamed_name) {
	}

	void set_name(const String& aname) { fname=&aname; }

	const String& as_string() {
		const String *result=get_string(); 
		if(!result)
			bark("(%s) not a string");

		return *result;
	}

	VTable& as_vtable() {
		VTable *result=get_vtable(); 
		if(!result)
			bark("(%s) not a table object");

		return *result;
	}

private:

	const String *fname;

protected: 

	void bark(char *action) const {
		THROW(0,0,
			&name(),
			action, type());
	}

};

#endif
