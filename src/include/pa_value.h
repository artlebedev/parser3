/*
  $Id: pa_value.h,v 1.10 2001/02/21 11:18:57 paf Exp $
*/

/*
	data core
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

#include "pa_pool.h"
#include "pa_exception.h"
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
	String& name;
	Array& param_names;
	Array& local_names;
	Array& code;

	Method(
		Pool& apool,
		String& aname,
		Array& aparam_names,
		Array& alocal_names,
		Array& acode) : 
		Pooled(apool),
		name(aname),
		param_names(aparam_names),
		local_names(alocal_names),
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
public:

	Value(Pool& apool) : Pooled(apool) {}

public:

	// all: for error reporting after fail(), etc
	virtual const char *get_type() const =0;

	// text: value
	// object_class: [class classname]
	virtual String *get_string() { failed("getting string representation"); return 0; }
	
	// text: value
	virtual void put_string(const String *astring) { failed("storing string"); }

	// method_ref: self, method
	virtual Method_ref *get_method_ref() { failed("extracting method reference"); return 0; }

	// junction: auto_calc,root,self,rcontext,wcontext, code
	virtual Junction *get_junction() { failed("getting junction"); return 0; }

	// hash: (key)=value
	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	// object_instance: (field)=value;(STATIC)=hash;(method)=method_ref
	// operator_class: (field)=value - static values only
	virtual Value *get_element(const String& name) const =0;

	// object_class, operator_class: (field)=value - static values only
	virtual void put_element(const String& name, Value *value)=0;

	// object_instance, object_class: method
	virtual Method *get_method(const String& name) const { return 0; }

	// object_class, object_instance: object_class
	virtual VClass *get_class() { return 0; }

	// object_class: true when this class is this or derived from 'ancestor'
	virtual bool is_or_derived_from(VClass& ancestor) { failed("thoghts of ancestors"); return false; }

private: 

	void failed(char *action) {
		pool().exception().raise(0,0,
			0,
			action);
	}
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
