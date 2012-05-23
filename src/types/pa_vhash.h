/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#define IDENT_PA_VHASH_H "$Id: pa_vhash.h,v 1.65 2012/05/23 16:26:40 moko Exp $"

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"
#include "pa_globals.h"

// defines

#define VHASH_TYPE "hash"

#define HASH_FIELDS_NAME "fields"
#define HASH_DEFAULT_ELEMENT_NAME "_default"
extern const String hash_fields_name, hash_default_element_name;

extern Methoded* hash_class;

// forwards

class VHash_lock;

/// value of type 'hash', implemented with Hash
class VHash: public VStateless_object {
	friend class VHash_lock;
public: // value

	override const char* type() const { return VHASH_TYPE; }
	override VStateless_class *get_class() { return hash_class; }

	/// VHash: finteger
	override int as_int() const { return fhash.count(); }
	/// VHash: finteger
	override double as_double() const { return as_int(); }
	/// VHash: count!=0
	override bool is_defined() const { return as_bool(); }
	/// VHash: count!=0
	override bool as_bool() const { return as_int()!=0; }
	/// VHash: count
	override Value& as_expr_result() { return *new VInt(as_int()); }

	/// VHash: fhash
	override HashStringValue *get_hash() { return &hash(); }

	/// VHash: (key)=value
	override Value* get_element(const String& aname) { 
		// $element
		if(Value* result=fhash.get(aname))
			return result;

		// $fields -- pseudo field to make 'hash' more like 'table'
		if(aname == hash_fields_name)
			return this;

		// $method
		if(Value* result=VStateless_object::get_element(aname))
			return result;

		// default value
		return get_default();
	}
	
	/// VHash: (key)=value
	override const VJunction* put_element(const String& aname, Value* avalue, bool /*replace*/) { 
		if(aname==hash_default_element_name)
			set_default(avalue);
		else 
			if(flocked) {
				if(!fhash.put_replaced(aname, avalue))
					throw Exception(PARSER_RUNTIME,
						&aname,
						"can not insert new hash key (hash flocked)");
			} else
					fhash.put(aname, avalue);

		return PUT_ELEMENT_REPLACED_ELEMENT;
	}

	override VFile* as_vfile(String::Language lang, const Request_charsets *charsets=0);

public: // usage

	VHash(): flocked(false), _default(0) {}

	VHash(const HashStringValue& source): fhash(source), flocked(false), _default(0) {}

	HashStringValue& hash() { 
		check_lock();
		return fhash; 
	}

	void set_default(Value* adefault) { 
		_default=adefault;
	}
	Value* get_default() { 
		return _default;
	}

	void extract_default();

	void check_lock() {
		if(flocked)
			throw Exception(PARSER_RUNTIME,
				0,
				"can not modify hash (flocked)");
	}

private:

	HashStringValue fhash;
	bool flocked;
	Value* _default;

};

class VHash_lock {
	VHash& fhash;
	bool saved;
public:
	VHash_lock(VHash& ahash): fhash(ahash) {
		saved=fhash.flocked;
		fhash.flocked=true;
	}
	~VHash_lock() {
		fhash.flocked=saved;
	}

};

#endif
