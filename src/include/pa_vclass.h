/*
  $Id: pa_vclass.h,v 1.2 2001/02/21 17:36:30 paf Exp $
*/

/*
	data core
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_value.h"
///#include "pa_vhash.h"
class VHash {
public:
	VHash(Pool& apool, Hash& hash) {}
};

class VClass : public Value {
public: // Value
	
	// all: for error reporting after fail(), etc
	/*virtual*/ const char *get_type() const { return "Class"; }

	// object_class: [class classname]
	/*virtual*/ String *get_string() const { 
		String *result=new(pool()) String(pool()); 
		result->APPEND("[class ", 0, 0, 0);
		result->APPEND(name().cstr(), 0, 0, 0);
		result->APPEND("]", 0, 0, 0);
		return result;
	}

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	/*virtual*/ Value *get_element(const String& name) const {
		// $STATIC=STATIC hash
		if(name==STATIC_NAME)
			return 0;//TODO:new(pool()) VHash(pool(), STATIC);

		// $field=STATIC.field
		Value *result=static_cast<Value *>(STATIC.get(name));
		if(!result) {
			// $method=VMethod_ref
			if(Method *method=get_method(name))
				result=0;///new(pool()) VMethod_ref(this, method);
		}

		return result;
	}

	// object_class, operator_class: (field)=value - static values only
	/*virtual*/ void put_element(const String& name, Value *value) {
		STATIC.put(name, value);
	}

	// object_instance, object_class: method
	/*virtual*/ Method *get_method(const String& name) const {
		return static_cast<Method *>(methods.get(name));
	}

	// object_class, object_instance: object_class
	/*virtual*/ VClass *get_class() { return this; /*TODO: think when?*/ }

	// object_class: true when this class is derived from 'ancestor'
	/*virtual*/ bool is_or_derived_from(VClass& ancestor) {
		if(this==&ancestor)
			return true; // it's me

		return parents_hash.get(ancestor.name())!=0;
	}

public: // usage
	
	const String& name() const { return *fname; }

public: // creation

	VClass(Pool& apool, String& aname, const Array& immediate_parents) : 
		Value(apool), 
		fname(&aname),
		STATIC(apool),
		methods(apool),
		parents(apool),
		parents_hash(apool) {
		// TODO: monkey immediate_parents 
			// fill parents & parents_hash
	}
	
	void rename(String *aname) { fname=aname; }

	void add_method(const String& name, Method& method) {
		methods.put(name, &method);
	}
	void add_parent(VClass& parent) {
		parents+=&parent;
		parents_hash.put(parent.name(), &parent);
	}

private:

	String *fname;
	Hash STATIC;
	Hash methods;
	Array parents;  Hash parents_hash;
};

#endif
