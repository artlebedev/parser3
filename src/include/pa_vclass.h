/*
  $Id: pa_vclass.h,v 1.5 2001/02/22 11:08:06 paf Exp $
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
	const char *type() const { return "Class"; }

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Value *get_element(const String& name) {
		// $STATIC=STATIC hash
		if(name==STATIC_NAME)
			return 0;//TODO:NEW VHash(pool(), STATIC);

		// $field=STATIC.field
		Value *result=static_cast<Value *>(STATIC.get(name));
		if(!result) {
			// $method=VMethod_ref
			if(Method *method=get_method(name))
				result=0;///NEW VMethod_ref(this, method);
		}

		return result;
	}

	// object_class, operator_class: (field)=value - static values only
	void put_element(const String& name, Value *value) {
		STATIC.put(name, value);
	}

	// object_instance, object_class: method
	Method *get_method(const String& name) const {
		return static_cast<Method *>(methods.get(name));
	}

	// object_class, object_instance: object_class
	VClass *get_class() { return this; /*TODO: think when?*/ }

	// object_class: true when this class is derived from 'ancestor'
	bool is_or_derived_from(VClass& ancestor) {
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
