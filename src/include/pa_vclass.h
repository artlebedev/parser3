/*
  $Id: pa_vclass.h,v 1.11 2001/02/24 13:18:19 paf Exp $
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_value.h"
#include "pa_vhash.h"
#include "pa_vjunction.h"

class VClass : public Value {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "Class"; }

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Value *get_element(const String& name) {
		// $STATIC=STATIC hash
		if(name==STATIC_NAME)
			return &STATIC;

		// $method=junction(this+method)
		if(Method *method=static_cast<Method *>(methods.get(name))) {
			Junction& j=*NEW Junction(pool(), 
				*this,
				method,0,0,0,0);

			return NEW VJunction(j);
		}

		// $field=STATIC.field
		return STATIC.get_element(name);
	}

	// object_class, operator_class: (field)=value - static values only
	void put_element(const String& name, Value *value) {
		STATIC.put_element(name, value);
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

	VClass(Pool& apool, const Array& immediate_parents) : 
		Value(apool), 
		STATIC(apool),
		methods(apool),
		parents(apool),
		parents_hash(apool) {
		// TODO: monkey immediate_parents 
			// fill parents & parents_hash
	}

	void add_method(const String& name, Method& method) {
		methods.put(name, &method);
	}
	void add_parent(VClass& parent) {
		parents+=&parent;
		parents_hash.put(parent.name(), &parent);
	}

public: //usage

	VHash STATIC;

private:

	Hash methods;
	Array parents;  Hash parents_hash;
};

#endif
