/*
  $Id: pa_value.h,v 1.40 2001/03/07 11:14:11 paf Exp $
*/

/*
	data core
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_array.h"
//#include "pa_voperator.h"

#define NAME_NAME "NAME"

class Value;
class VClass;
//class VOperator;
class Junction;
class WContext;
class VAliased;

class Method : public Pooled {
public:
	const String& name;
	Array& params_names;
	Array& locals_names;
	const Array& code;

	Method(
		Pool& apool,
		const String& aname,
		Array& aparams_names,
		Array& alocals_names,
		const Array& acode) : 

		Pooled(apool),
		name(aname),
		params_names(aparams_names),
		locals_names(alocals_names),
		code(acode) {
	}
};

/*
class Operator : public Method {
	// operator module static vars stored in there
	VOperator_class *self;
};
*/

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

	Value& self;
	VClass *vclass;  Method *method;
	Value *root;
	Value *rcontext;
	WContext *wcontext;
	const Array *code;
};

class Value : public Pooled {
public: // Value

	// all: for error reporting after fail(), etc
	virtual const char *type() const =0;
	/*const*/ String& name() const { return *fname; }

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

/*
descendants:
	text:+ value:String
    hash:+ keys&values:Hash
    table:+ columns_order:Array, columns:Hash, rows:Array
    object_class:+ STATIC:Hash, methods:Hash
    object_instance:+ object_class, fields:Hash
    method_ref:+ self:Value/object_class, method:String
    method_self_n_params_n_locals:+ self:Value/object_class[1st try], params_locals&values:Hash[2nd try]
	junction:+ self:Value, code:String
*/

#endif
