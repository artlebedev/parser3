/*
  $Id: pa_vclass.h,v 1.15 2001/02/25 08:12:21 paf Exp $
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_value.h"
#include "pa_vhash.h"
#include "pa_vstring.h"
#include "pa_vjunction.h"

class VClass : public Value {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "Class"; }

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Value *get_element(const String& aname) {
		// $NAME=name()
		if(aname==NAME_NAME)
			return NEW VString(name());
		// $PARENTS=parents table
		if(aname==PARENTS_NAME)
			return 0;// TODO: table of parents
		// $STATIC=STATIC hash
		if(aname==STATICS_NAME)
			return &STATICS;

		// $method=junction(this+method)
		if(Method *method=static_cast<Method *>(methods().get(aname))) {
			Junction& j=*NEW Junction(pool(), 
				*this,
				method,0,0,0,0);

			return NEW VJunction(j);
		}

		// $field=STATIC.field
		return STATICS.get_element(aname);
	}

	// object_class, operator_class: (field)=value - static values only
	void put_element(const String& name, Value *value) {
		STATICS.put_element(name, value);
	}

	// object_class, object_instance: object_class
	VClass *get_class() { return this; /*TODO: think when?*/ }

public: // usage

	VClass(Pool& apool) : 
		Value(apool), 
		STATICS(apool),
		fmethods(apool),
		parents(apool),
		parents_hash(apool) {
	}

	void add_method(const String& name, Method& method) {
		fmethods.put(name, &method);
	}
	Hash& methods() { return fmethods; }
	
	void add_parent(VClass& parent) {
		parents+=&parent;
		parents_hash.put(*parent.name(), &parent);
		// TODO: monkey immediate_parent
			// fill parents & parents_hash
	}

	// true when me_or_ancestor is me or my ancestor
	bool is_or_derived_from(VClass& me_or_ancestor) {
		if(this==&me_or_ancestor)
			return true; // it's me

		return parents_hash.get(*me_or_ancestor.name())!=0;
	}

public: //usage

	VHash STATICS;

private:

	Hash fmethods;
	Array parents;  Hash parents_hash;
};

#endif
