/** @file
	Parser: @b array parser type decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VARRAY_H
#define PA_VARRAY_H

#define IDENT_PA_VARRAY_H "$Id: pa_varray.h,v 1.22 2024/10/28 13:05:14 moko Exp $"

#include "classes.h"
#include "pa_value.h"
#include "pa_array.h"
#include "pa_vhash.h"
#include "pa_vint.h"
#include "pa_globals.h"
#include "pa_symbols.h"

// defines

#define VARRAY_TYPE "array"
//#define DEBUG_ARRAY_USED

extern Methoded* array_class;

/// Sparse Array
template<typename T> class SparseArray: public Array<T> {

	mutable size_t fused;

public:
	inline SparseArray(size_t initial=0) : Array<T>(initial), fused(0) {}

	inline SparseArray(size_t size, T* elements) : Array<T>(size), fused(size) {
		memcpy(this->felements, elements, size * sizeof(T));
		this->fsize=size;
	}

	// note: 'append' and 'operator+=' are used directly from Array.

	// not virtual, thus Array::append does not use it, but that's OK.
	void fit(size_t index);

	inline T get(size_t index) const {
		return index < this->fsize ? this->felements[index] : NULL;
	}

	inline void put(size_t index, T element){
		this->fit(index);
		this->felements[index]=element;
		if(index >= this->fsize){
			this->fsize=index+1;
		}
	}

	inline bool put_dont_replace(size_t index, T element){
		this->fit(index);
		if(this->felements[index])
			return true;
		this->felements[index]=element;
		if(index >= this->fsize){
			this->fsize=index+1;
		}
		return false;
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
#ifndef DEBUG_ARRAY_USED
		if(!fused)
#endif
		{
			size_t used=0;
			for(Array_iterator<T> i(*this); i;) {
				if(i.next())
					used++;
			}
#ifdef DEBUG_ARRAY_USED
			if(fused && fused!=used)
				throw Exception(PARSER_RUNTIME, 0, "cached elements count %d differs from actual %d", fused, used);
			printf(fused ? "cached used %d\n" : "counted used %d\n", used);
#endif
			fused=used;
		}
		return fused;
	}

	inline void clear(size_t index) {
		if(index < this->fsize){
			this->felements[index]=NULL;
			if(index == this->fsize-1){
				this->fsize--;
				locate_last_used();
			}
		}
	}

	inline void clear() { Array<T>::clear(); }

	inline void remove(size_t index) {
		if(index < this->fsize){
			Array<T>::remove(index);
			if(index==this->fsize)
				locate_last_used();
		}
	}

	inline T pop() {
		if(this->fsize){
			T result=this->felements[this->fsize-1];
			this->fsize--;
			locate_last_used();
			return result;
		}
		return NULL;
	}

	inline void invalidate(){
		fused=0;
	}

	inline void locate_last_used(){
		for(;this->fsize>0 && !this->felements[this->fsize-1];this->fsize--);
	}

	inline void confirm_all_used(){
		fused=this->fsize;
	}

	inline void change_used(int delta){
		if(fused){
			fused+=delta;
		}
	}

	void compact(bool compact_undef){
		T* dst=this->felements;
		T* elements_end=dst + this->fsize;

		if(compact_undef){
			for(T* src=this->felements; src < elements_end; src++)
				if(*src && (*src)->is_defined())
					*dst++=*src;
		} else {
			for(T* src=this->felements; src < elements_end; src++)
				if(*src)
					*dst++=*src;
		}

		this->fsize=dst-this->felements;
	}

};


class VArray: public VHashBase {
public: // value

	override const char* type() const { return VARRAY_TYPE; }
	override VStateless_class *get_class() { return array_class; }

	/// VArray: used elements count
	override int as_int() const { return farray.used(); }
	override double as_double() const { return farray.used(); }
	override bool is_defined() const { return farray.count()!=0; }
	override bool as_bool() const { return farray.count()!=0; }
	override Value& as_expr_result() { return *new VInt(farray.used()); }

	/// VArray: virtual hash
	override HashStringValue *get_hash() { return &hash(); }
	override HashStringValue* get_fields() { return &hash(); }
	override HashStringValue* get_fields_reference() { return &hash(); }

	/// VArray: json-string
	override const String* get_json_string(Json_options& options);

	/// VArray: (key)=value
	override Value* get_element(const String& aname) {
#if !defined(FEATURE_GET_ELEMENT4CALL) || !defined(OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL)
		// $method, CLASS, CLASS_NAME
		if(Value* result=VStateless_object::get_element(aname))
			return result;
#endif
		// $element, here as index() allows only numbers
		if(Value* result=farray.get(index(aname)))
			return result;

		return NULL;
	}

#ifdef FEATURE_GET_ELEMENT4CALL
	override Value* get_element4call(const String& aname) {
		// $method first
		if(Value* result=VStateless_object::get_element(aname))
			return result;

		// $element
		if(is_index(aname))
			if(Value* result=farray.get(index(aname)))
				return result;

		return bark("%s method not found", &aname);
	}
#endif

	/// VArray: (key)=value
	override const VJunction* put_element(const String& aname, Value* avalue) {
		farray.put(index(aname), avalue);
		farray.invalidate();
		return 0;
	}

public: // VHashBase

	override HashStringValue& hash();
	override void set_default(Value*) { }
	override Value* get_default() { return 0; }
	override void add(Value* avalue) { farray+=avalue; /* only json uses it, thus no need to invalidate()*/ }

public: // usage

	VArray(size_t initial=0): farray(initial) {}
	VArray(size_t size, Value** elements): farray(size, elements) {}

	ArrayValue &array() { return farray; }

	static size_t index(int aindex){
		if(aindex<0)
			throw Exception("number.format", 0, "index is out of range (negative)");
		return aindex;
	}

	static size_t index(const String::Body& aindex){ return pa_atoui(aindex.cstr()); }
	static size_t index(const String& aindex){ return pa_atoui(aindex.cstr(), 10, &aindex); }

	static bool is_index(const String& aindex){
		for(const char *pos=aindex.cstr(); *pos ;pos++){
			if ((*pos < '0') || (*pos > '9'))
				return false;
		}
		return true;
	}

	bool contains(size_t index){
		return farray.get(index) != NULL;
	}

private:

	ArrayValue farray;

};

#endif
