/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vhash.h,v 1.28 2002/02/08 07:27:52 paf Exp $
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"

// externs

extern Methoded *hash_base_class;

// forwards

class VHash_lock;

/// value of type 'hash', implemented with Hash
class VHash : public VStateless_class {
	friend class VHash_lock;
public: // value

	const char *type() const { return "hash"; }

	/// VHash: finteger
	int as_int() const { return fhash.size(); }
	/// VHash: finteger
	double as_double() const { return as_int(); }
	/// VHash: count!=0
	bool is_defined() const { return as_int()!=0; }
	/// VHash: count!=0
	bool as_bool() const { return is_defined(); }
	/// VHash: count
	Value *as_expr_result(bool ) { return NEW VInt(pool(), as_int()); }

	/// VHash: fhash
	Hash *get_hash(const String *source) { return &hash(source); }

	/// VHash: 0
	VStateless_class *get_class() { return 0; }

	/// VHash: (key)=value
	Value *get_element(const String& name) { 
		// $CLASS,$method
		if(Value *result=VStateless_class::get_element(name))
			return result;

		// $element
		if(Value *result=static_cast<Value *>(fhash.get(name)))
			return result;

		// default value
		return get_default();
	}
	
	/// VHash: (key)=value
	void put_element(const String& name, Value *value) { 
		hash(&name).put(name, value);
	}

public: // usage

	VHash(Pool& apool) : VStateless_class(apool, hash_base_class), 
		fhash(apool), locked(false) {
	}

	VHash(Pool& apool, const Hash& source) : VStateless_class(apool, hash_base_class), 
		fhash(source), locked(false) {
	}

	Hash& hash(const String *source) { 
		check_lock(source);
		return fhash; 
	}

	void set_default(Value& adefault) { 
		hash(&adefault.name()).put(*hash_default_element_name, &adefault);
	}
	Value *get_default() { 
		return static_cast<Value *>(fhash.get(*hash_default_element_name)); 
	}

	void check_lock(const String *source) {
		if(locked)
			throw Exception(0, 0,
				source,
				"can not modify hash (locked)");
	}

private:

	bool locked;
	Hash fhash;

};

class VHash_lock {
	VHash& fhash;
	bool saved;
public:
	VHash_lock(VHash& ahash) : fhash(ahash) {
		saved=fhash.locked;
		fhash.locked=true;
	}
	~VHash_lock() {
		fhash.locked=saved;
	}

};

#endif
