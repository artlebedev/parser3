/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

static const char * const IDENT_VHASH_H="$Date: 2004/12/23 14:51:32 $";

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"
#include "pa_globals.h"

// defines

#define VHASH_TYPE "hash"
#define HASH_FIELDS_NAME "fields"

#define HASH_DEFAULT_ELEMENT_NAME "_default"

// externs

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
	override Value& as_expr_result(bool ) { return *new VInt(as_int()); }

	/// VHash: fhash
	override HashStringValue *get_hash() { return &hash(); }

	/// VHash: (key)=value
	override Value* get_element(const String& aname, Value& aself, bool looking_up) { 
		// $element
		if(Value* result=fhash.get(aname))
			return result;

		// $fields -- pseudo field to make 'hash' more like 'table'
		if(aname == HASH_FIELDS_NAME)
			return this;

		// $method
		if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
			return result;

		// default value
		return get_default();
	}
	
	/// VHash: (key)=value
	override bool put_element(const String& aname, Value* avalue, bool /*replace*/) { 
		if(aname==HASH_DEFAULT_ELEMENT_NAME)
			set_default(avalue);
		else 
			if(flocked) {
				if(!fhash.put_replace(aname, avalue))
					throw Exception("parser.runtime",
						&aname,
						"can not insert new hash key (hash flocked)");
			} else
					fhash.put(aname, avalue);

		return true;
	}

	override VFile* as_vfile(String::Language lang=String::L_UNSPECIFIED,
		const Request_charsets *charsets=0);

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
			throw Exception("parser.runtime",
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
