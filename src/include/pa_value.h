/*
  $Id: pa_value.h,v 1.19 2001/02/23 14:18:26 paf Exp $
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
	Array& code;

	Method(
		Pool& apool,
		const String& aname,
		Array& aparams_names,
		Array& alocals_names,
		Array& acode) : 

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

class Method_ref {
public:
	Value *self;
	Method& method;
};

class Junction {
	bool auto_calc;
	Value& root;
	Value *self;
	Value& rcontext;
	WContext& wcontext;
	Array& code;
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
	virtual void put_string(String *astring) { failed("storing string to %s"); }

	// method_ref: self, method
	virtual Method_ref *get_method_ref() { failed("extracting method reference from %s"); return 0; }

	// junction: auto_calc,root,self,rcontext,wcontext, code
	virtual Junction *get_junction() { failed("getting junction from %s"); return 0; }

	// hash: (key)=value
	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	// object_instance: (field)=value;(STATIC)=hash;(method)=method_ref
	// operator_class: (field)=value - static values only
	// wcontext: transparent
	// frame: my or self_transparent
	virtual Value *get_element(const String& name) { failed("getting element from %s"); return 0; }
	
	// hash: (key)=value
	// object_class, operator_class: (field)=value - static values only
	// wcontext: transparent
	// frame: my or self_transparent
	virtual void put_element(const String& name, Value *value) { failed("putting element to %s"); }

	// object_instance, object_class: method
	// frame: transparent
	// wcontext: transparent
	virtual Method *get_method(const String& name) const { return 0; }

	// object_class, object_instance: object_class
	// frame: transparent
	// wcontext: transparent
	virtual VClass *get_class() { return 0; }

	// object_class: true when this class is this or derived from 'ancestor'
	// frame: transparent
	// wcontext: transparent
	virtual bool is_or_derived_from(VClass& ancestor) { failed("thoghts of ancestors of %s"); return false; }

public: // usage

	Value(Pool& apool) : Pooled(apool), fname(0) {}

	void set_name(const String& aname) { fname=&aname; }

	String& as_string() {
		String *result=get_string(); 
		if(!result)
			failed("getting string of %s");
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
