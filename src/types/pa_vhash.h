/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhash.h,v 1.22 2001/10/11 11:52:55 parser Exp $
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"

extern Methoded *hash_base_class;

/// value of type 'hash', implemented with Hash
class VHash : public VStateless_class {
public: // value

	const char *type() const { return "hash"; }

	/// VHash: finteger
	int as_int() const { return fhash.size(); }
	/// VHash: finteger
	double as_double() const { return as_int(); }
	/// VHash: count!=0
	bool is_defined() const { return as_int()!=0; }
	/// VHash: count
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VInt(pool(), as_int()); }

	/// VHash: fhash
	Hash *get_hash() { return &fhash; }

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
		fhash.put(name, value);
	}

public: // usage

	VHash(Pool& apool) : VStateless_class(apool, hash_base_class), 
		fhash(apool) {
	}

	VHash(Pool& apool, const Hash& source) : VStateless_class(apool, hash_base_class), 
		fhash(source) {
	}

	Hash& hash() { return fhash; }

	void set_default(Value& adefault) { 
		fhash.put(*hash_default_element_name, &adefault);
	}
	Value *get_default() { 
		return static_cast<Value *>(fhash.get(*hash_default_element_name)); 
	}

private:

	Hash fhash;

};

#endif
