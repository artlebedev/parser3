/*
  $Id: pa_value.h,v 1.24 2001/02/24 11:20:32 paf Exp $
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

class Value;
class VClass;
//class VOperator;
class Junction;
class WContext;

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
		Method *amethod,
		Value *aroot,
		Value *arcontext,
		WContext *awcontext,
		const Array *acode) : Pooled(apool),
		
		self(aself),
		method(amethod),
		root(aroot),
		rcontext(arcontext),
		wcontext(awcontext),
		code(acode) {
	}

	Value& self;
	Method *method;
	Value *root;
	Value *rcontext;
	WContext *wcontext;
	const Array *code;
};

class Value : public Pooled {
public: // Value

	// all: for error reporting after fail(), etc
	virtual const char *type() const =0;
	const String *name() const { return fname; }

	// string: value
	// unknown: ""
	// others: 0
	virtual String *get_string() { return 0; }
	
	// string: value
	virtual void put_string(String *astring) { failed("storing string to %s:%s"); }

	// junction: auto_calc,root,self,rcontext,wcontext, code
	virtual Junction *get_junction() { return 0; }

	// hash: (key)=value
	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	// object_instance: (field)=value;(STATIC)=hash;(method)=method_ref
	// operator_class: (field)=value - static values only
	// wcontext: transparent
	// frame: my or self_transparent
	// codeframe: wcontext_transparent
	virtual Value *get_element(const String& name) { failed("getting element from %s:%s"); return 0; }
	
	// hash: (key)=value
	// object_class, operator_class: (field)=value - static values only
	// wcontext: transparent
	// frame: my or self_transparent
	// codeframe: wcontext_transparent
	virtual void put_element(const String& name, Value *value) { failed("putting element to %s:%s"); }

	// object_class, object_instance: object_class
	// frame: transparent
	// wcontext: transparent
	// codeframe: wcontext_transparent
	virtual VClass *get_class() { return 0; }

	// object_class: true when this class is this or derived from 'ancestor'
	// frame: transparent
	// wcontext: transparent
	// codeframe: wcontext_transparent
	virtual bool is_or_derived_from(VClass& ancestor) { failed("thoghts of ancestors of %s:%s"); return false; }

public: // usage

	Value(Pool& apool) : Pooled(apool), fname(0) {}

	void set_name(const String& aname) { fname=&aname; }

	String& as_string() {
		String *result=get_string(); 
		if(!result)
			failed("getting string of %s:%s");
		return *result;
	}

private:

	const String *fname;

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
