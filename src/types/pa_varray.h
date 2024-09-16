/** @file
	Parser: @b array parser type decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VARRAY_H
#define PA_VARRAY_H

#define IDENT_PA_VARRAY_H "$Id: pa_varray.h,v 1.5 2024/09/16 23:22:52 moko Exp $"

#include "classes.h"
#include "pa_value.h"
#include "pa_array.h"
#include "pa_vhash.h"
#include "pa_vint.h"
#include "pa_globals.h"
#include "pa_symbols.h"

// defines

#define VARRAY_TYPE "array"

extern Methoded* array_class;

/// Sparse Array
template<typename T> class SparseArray: public Array<T> {

	mutable size_t fused;

public:
	inline SparseArray(size_t initial=0) : Array<T>(initial), fused(0) {}

	inline T get(size_t index) const {
		return index < this->count() ? this->felements[index] : NULL;
	}

	inline void put(size_t index, T element){
		this->fit(index);
		this->felements[index]=element;
		if(index >= this->fsize){
			this->fsize=index+1;
		}
	}

	inline void insert(size_t index, T element) {
		if(index >= this->fsize){
			this->fit(index);
			this->felements[index]=element;
			this->fsize=index+1;
		} else {
			Array<T>::insert(index, element);
		}
	}

	size_t used() const{
		if(!fused){
			for(Array_iterator<T> i(*this); i;) {
				if(i.next())
					fused++;
			}
		}
		return fused;
	}

	inline void clear(size_t index) {
		if(index<this->count()){
			this->felements[index]=NULL;
		}
	}

	inline void clear() { Array<T>::clear(); }

	inline void invalidate(){
		fused=0;
	}

};


class VArray: public VHashBase {
public: // value

	override const char* type() const { return VARRAY_TYPE; }
	override VStateless_class *get_class() { return array_class; }

	/// VArray: used elements count
	override int as_int() const { return farray.used(); }
	override double as_double() const { return farray.used(); }
	override bool is_defined() const { return farray.used()!=0; }
	override bool as_bool() const { return farray.used()!=0; }
	override Value& as_expr_result() { return *new VInt(farray.used()); }

	/// VArray: virtual hash
	override HashStringValue *get_hash() { return &hash(); }
	override HashStringValue* get_fields() { return &hash(); }
	override HashStringValue* get_fields_reference() { return &hash(); }

	/// VArray: json-string
	override const String* get_json_string(Json_options& options);

	/// VArray: (key)=value
	override Value* get_element(const String& aname) {
		// $element first
		if(Value* result=farray.get(index(aname)))
			return result;

		// $fields -- pseudo field to make 'hash' and 'array' more like 'table'
		if(SYMBOLS_EQ(aname,FIELDS_SYMBOL))
			return this;

#if !defined(FEATURE_GET_ELEMENT4CALL) || !defined(OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL)
		// $method, CLASS, CLASS_NAME
		if(Value* result=VStateless_object::get_element(aname))
			return result;
#endif
		return NULL;
	}

#ifdef FEATURE_GET_ELEMENT4CALL
	override Value* get_element4call(const String& aname) {
		// $method first
		if(Value* result=VStateless_object::get_element(aname))
			return result;

		// $element
		if(Value* result=farray.get(index(aname)))
			return result;

		return bark("%s method not found", &aname);
	}
#endif

	/// VArray: (key)=value
	override const VJunction* put_element(const String& aname, Value* avalue) {
		farray.put(index(aname), avalue);
		invalidate();
		return 0;
	}

public: // VHashBase

	override HashStringValue& hash();
	override void set_default(Value*) { }
	override Value* get_default() { return 0; }
	override void add(Value* avalue) { farray+=avalue; /* only json uses it, thus no need to invalidate()*/ }

public: // usage

	VArray(size_t initial=0): farray(initial), fhash(0) {}
	~VArray() { invalidate(); }

	ArrayValue &array() { return farray; }

	static size_t index(int aindex){
		if(aindex<0)
			throw Exception("number.format", 0, "out of range (negative)");
		return aindex;
	}

	static size_t index(const String& aindex){
		int result=aindex.as_int();
		if(result<0)
			throw Exception("number.format", &aindex, "out of range (negative)");
		return result;
	}

	static size_t index(const Value& aindex){ return index(aindex.as_int()); }

	bool contains(size_t index){
		return farray.get(index) != NULL;
	}

	void clear(size_t index){
		farray.clear(index);
		invalidate();
	}

	void clear(){
		farray.clear();
		invalidate();
	}

	void invalidate() {
#ifdef USE_DESTRUCTORS
		if(fhash)
			delete(fhash);
#endif
		fhash=0;
		farray.invalidate();
	}

private:

	ArrayValue farray;
	HashStringValue *fhash;

};

#endif
