/*
  $Id: pa_value.h,v 1.1 2001/02/11 19:35:38 paf Exp $
*/

/*
	data core
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

#include "pa_string.h"

class Method {
public:
	Array param_names;
	String code;
};

class Value {
public:
	// text: value
	virtual String *get_string() const =0;
	virtual String *put_string(const String *astring)=0;

	// hash: (key)=value
	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with this=object_class
	// object_instance: (field)=value;(STATIC)=hash;(method)=method_ref
	virtual Value *get_element(const String& name) const =0;
	virtual Value *put_element(const String& name, const Value *avalue)=0;

	// object_instance, object_class: method
	virtual Method* get_method(const String& name) const =0;

	// method_ref: "self" part, method
	virtual Value *get_self&method() const =0;
};

/*
descendants:
	text:+ value:String
    hash:+ keys&values:Hash
    table:+ columns_order:Array, columns:Hash, rows:Array
    object_class:+ STATIC:Hash, methods:Hash
    object_instance:+ object_class, fields:Hash
    method_ref:+ this:Value/object_class, method:String
    method_this_n_params:+ this:Value/object_class[1st try], params&values:Hash[2nd try]


*/

#endif
