/*
  $Id: pa_value.h,v 1.44 2001/03/08 13:13:39 paf Exp $
*/

/*
	data core
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_array.h"

#define NAME_NAME "NAME"

class Value;
class VClass;
class Junction;
class WContext;
class VAliased;
class Request;

typedef void (*Native_code_ptr)(Request& request, Array& params);

class Method : public Pooled {
public:
	const String& name;
	// either numbered params // for native-code methods = operators
	int numbered_params_count;
	// or named params&locals // for parser-code methods
	Array *params_names;  Array *locals_names;
	// the Code
	const Array *parser_code;/*OR*/Native_code_ptr native_code;

	Method(
		Pool& apool,
		const String& aname,
		int anumbered_params_count,
		Array *aparams_names, Array *alocals_names,
		const Array *aparser_code, Native_code_ptr anative_code) : 

		Pooled(apool),
		name(aname),
		numbered_params_count(anumbered_params_count),
		params_names(aparams_names), locals_names(alocals_names),
		parser_code(aparser_code), native_code(anative_code) {
	}
};

class Junction : public Pooled {
public:

	Junction(Pool& apool,
		Value& aself,
		VClass *avclass, Method *amethod,
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
	VClass *vclass;  Method *method;
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
	String& name() const { return *fname; }

	// unknown: false
	// others: true
	virtual bool get_defined() { return true; }

	// string: value
	// unknown: ""
	// double: value
	// others: 0
	virtual String *get_string() { return 0; }
	
	// string: value
	// double: value
	// bool: value
	virtual double get_double() { failed("getting numerical value of '%s'"); return 0; }

	// unknown: false
	// bool: value
	// double: 0 or !0
	// string: empty or not
	// hash: size!=0
	// TODO table: count!=0
	// others: true
	virtual bool get_bool() { return true; }

	// junction: auto_calc,root,self,rcontext,wcontext, code
	virtual Junction *get_junction() { return 0; }

	// hash: (key)=value
	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	// object_instance: (field)=value;(CLASS)=vclass;(method)=method_ref
	// operator_class: (field)=value - static values only
	// codeframe: wcontext_transparent
	// methodframe: my or self_transparent
	virtual Value *get_element(const String& name) { failed("type is '%s', can not get element from it"); return 0; }
	
	// hash: (key)=value
	// object_class, operator_class: (field)=value - static values only
	// object_instance: (field)=value
	// codeframe: wcontext_transparent
	// methodframe: my or self_transparent
	virtual void put_element(const String& name, Value *value) { failed("type is '%s', can not put element to it"); }

	// object_class, object_instance: object_class
	// wcontext: none yet | transparent
	virtual VClass *get_class() { return 0; }

	// valiased: this
	// wcontext: transparent
	// methodframe: self_transparent
	virtual VAliased *get_aliased() { return 0; }

public: // usage

	Value(Pool& apool) : Pooled(apool), fname(new(apool) String(apool)) {
		fname->APPEND_CONST("unnamed");
	}

	void set_name(String& aname) { fname=&aname; }

	String& as_string() {
		String *result=get_string(); 
		if(!result)
			failed("getting string of '%s'");
		return *result;
	}

private:

	String *fname;

private: 

	void failed(char *action);
};

#endif
