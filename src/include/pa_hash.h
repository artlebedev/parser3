/** @file
	Parser: hash class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

/*
	The prime numbers used from zend_hash.c,
	the part of Zend scripting engine library,
	Copyrighted (C) 1999-2000    Zend Technologies Ltd.
	http://www.zend.com/license/0_92.txt
	For more information about Zend please visit http://www.zend.com/
*/

#ifndef PA_HASH_H
#define PA_HASH_H

static const char * const IDENT_HASH_H="$Date: 2009/07/29 05:01:46 $";

#include "pa_memory.h"
#include "pa_types.h"
#include "pa_string.h"

const int HASH_ALLOCATES_COUNT=29;

/** Zend comment: Generated on an Octa-ALPHA 300MHz CPU & 2.5GB RAM monster 

	paf: HPUX ld could not handle static member: unsatisfied symbols
*/
static uint Hash_allocates[HASH_ALLOCATES_COUNT]={
	5, 11, 19, 53, 107, 223, 463, 983, 1979, 3907, 7963, 
	16229, 32531, 65407, 130987, 262237, 524521, 1048793, 
	2097397, 4194103, 8388857, 16777447, 33554201, 67108961, 
	134217487, 268435697, 536870683, 1073741621, 2147483399};

/// useful generic hash function
inline void generic_hash_code(uint& result, char c) {
	result=(result<<4)+c;
	if(uint g=(result&0xF0000000)) {
		result=result^(g>>24);
		result=result^g;
	}
}
/// useful generic hash function
inline void generic_hash_code(uint& result, const char* s) {
	while(char c=*s++) {
		result=(result<<4)+c;
		if(uint g=(result&0xF0000000)) {
			result=result^(g>>24);
			result=result^g;
		}
	}
}

/// useful generic hash function
inline void generic_hash_code(uint& result, const char* buf, size_t size) {
	const char* end=buf+size;
	while(buf<end) {
		result=(result<<4)+*buf++;
		if(uint g=(result&0xF0000000)) {
			result=result^(g>>24);
			result=result^g;
		}
	}
}

/// simple hash code of int. used by EXIF mapping
inline uint hash_code(int self) {
	uint result=0;
	generic_hash_code(result, (const char*)&self, sizeof(self));
	return result;
}

#endif // PA_HASH_H

#ifndef PA_HASH_CLASS
#define PA_HASH_CLASS
/** 
	Simple hash.

	Automatically rehashed when almost is_full.
	Contains no 0 values. 
		get returning 0 means there were no such.
		"put value 0" means "remove"
*/
#ifdef HASH_ORDER

#undef HASH
#undef HASH_STRING
#undef NEW_PAIR

#define HASH OrderedHash
#define HASH_STRING OrderedHashString
#define NEW_PAIR(code, key, value) *ref=new Pair(code, key, value, *ref, this->last); this->last=&((*ref)->next)

#else

#define HASH Hash
#define HASH_STRING HashString
#define NEW_PAIR(code, key, value) *ref=new Pair(code, key, value, *ref)

#endif

template<typename K, typename V> class HASH: public PA_Object {
public:

	typedef K key_type;
	typedef V value_type;

	HASH() { 
		allocated=Hash_allocates[allocates_index=0];
		threshold=allocated*THRESHOLD_PERCENT/100;
		fpairs_count=fused_refs=0;
		refs=new(UseGC) Pair*[allocated];
#ifdef HASH_ORDER
		first=0;
		last=&first;
#endif
	}

	HASH(const HASH& source) {
		allocates_index=source.allocates_index;
		allocated=source.allocated;
		threshold=source.threshold;
		fused_refs=source.fused_refs;
		fpairs_count=source.fpairs_count;
		refs=new(UseGC) Pair*[allocated];
#ifdef HASH_ORDER
		first=0;
		last=&first;
#endif
		// clone & rehash
		Pair **old_ref=source.refs;
		for(int index=0; index<allocated; index++)
			for(Pair *pair=*old_ref++; pair; ) {
				Pair *next=pair->link;

				Pair **ref=&refs[index];
				NEW_PAIR(pair->code, pair->key, pair->value);

				pair=next;
			}
	}

#ifdef USE_DESTRUCTORS
	~HASH() {
		Pair **ref=refs;
		for(int index=0; index<allocated; index++)
			for(Pair *pair=*ref++; pair;){
				Pair *next=pair->link;
				delete pair;
				pair=next;
			}
		delete[] refs;
	}
#endif

	/// put a [value] under the [key] @returns existed or not
	bool put(K key, V value) {
		if(!value) {
			remove(key);
			return false;
		}
		if(is_full()) 
			expand();

		uint code=hash_code(key);
		uint index=code%allocated;
		Pair **ref=&refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && pair->key==key) {
				// found a pair with the same key
				pair->value=value;
				return true;
			}
		
		// proper pair not found -- create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			fused_refs++; // not, we'll use it and record the fact
		NEW_PAIR(code, key, value);
		fpairs_count++;
		return false;
	}

	/// put a [value] under the [key] @returns existed or not
	template<typename R, typename F, typename I> R replace_maybe_append(K key, V value, F prevent, I info) {
		if(!value) {
			// they can come here from somewhere (true with maybe_replace_maybe_append, keeping parallel)
			remove(key);
			// this has nothing to do with properties, doing no special property handling here
			return 0; 
		}

		if(is_full()) 
			expand();

		uint code=hash_code(key);
		uint index=code%allocated;
		Pair **ref=&refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && pair->key==key) {
				// found a pair with the same key
				pair->value=value;
				return reinterpret_cast<R>(1);
			}
		
		// proper pair not found 
		// prevent-function intercepted append?
		if(R result=prevent(value, info))
			return result;
		
		//create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			fused_refs++; // not, we'll use it and record the fact
		NEW_PAIR(code, key, value);
		fpairs_count++;
		return 0;
	}

	/// put a [value] under the [key] @returns existed or not
	template<typename R, typename F1, typename F2, typename I> 
		R maybe_replace_maybe_append(K key, V value, F1 prevent_replace, F2 prevent_append, I info) 
	{
		if(!value) {
			// they can come here from Temp_value_element::dctor to restore some empty value
			remove(key);
			// this has nothing to do with properties, doing no special property handling here
			return 0; 
		}

		if(is_full()) 
			expand();

		uint code=hash_code(key);
		uint index=code%allocated;
		Pair **ref=&refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && pair->key==key) {
				// found a pair with the same key

				// prevent-function intercepted replace?
				if(R result=prevent_replace(pair->value, info))
					return result;
				
				pair->value=value;
				return reinterpret_cast<R>(1);
			}
		
		// proper pair not found 
		// prevent-function intercepted append?
		if(R result=prevent_append(value, info))
			return result;
		
		//create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			fused_refs++; // not, we'll use it and record the fact
		NEW_PAIR(code, key, value);
		fpairs_count++;
		return 0;
	}

	/// put a [value] under the [key] @returns existed or not
	template<typename R, typename F1, typename I> 
		R maybe_replace_never_append(K key, V value, F1 prevent_replace, I info) 
	{
		if(!value) {
			// they can come here from somewhere (true with maybe_replace_maybe_append, keeping parallel)
			remove(key);
			// this has nothing to do with properties, doing no special property handling here
			return 0; 
		}

		if(is_full()) 
			expand();

		uint code=hash_code(key);
		uint index=code%allocated;
		Pair **ref=&refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && pair->key==key) {
				// found a pair with the same key

				// prevent-function intercepted replace?
				if(R result=prevent_replace(pair->value, info))
					return result;
				
				pair->value=value;
				return reinterpret_cast<R>(1);
			}
		
		return 0;
	}

	/// remove the [key] @returns existed or not
	bool remove(K key) {
		uint code=hash_code(key);
		uint index=code%allocated;
		for(Pair **ref=&refs[index]; *ref; ref=&(*ref)->link){
			Pair *pair=*ref;
			if(pair->code==code && pair->key==key) {
				// found a pair with the same key
				Pair *next=pair->link;
#ifdef HASH_ORDER
				*(pair->prev)=pair->next;
				if(pair->next)
					pair->next->prev=pair->prev;
				else
					last=pair->prev;
#endif
				delete pair;
				*ref=next;
				--fpairs_count;
				return true;
			}
		}

		return false;
	}

	/// return true if key exists
	bool contains(K key){
		uint code=hash_code(key);
		uint index=code%allocated;
		for(Pair *pair=refs[index]; pair; pair=pair->link){
			if(pair->code==code && pair->key==key)
				return true;
		}

		return false;
	}

	/// get associated [value] by the [key]
	V get(K key) const {
		uint code=hash_code(key);
		uint index=code%allocated;
		for(Pair *pair=refs[index]; pair; pair=pair->link)
			if(pair->code==code && pair->key==key)
				return pair->value;
		
		return V(0);
	}

	/// put a [value] under the [key] if that [key] existed @returns existed or not
	bool put_replaced(K key, V value) {
		if(!value) {
			remove(key);
			return false;
		}
		uint code=hash_code(key);
		uint index=code%allocated;
		for(Pair *pair=refs[index]; pair; pair=pair->link)
			if(pair->code==code && pair->key==key) {
				// found a pair with the same key, replacing
				pair->value=value;
				return true;
			}

		// proper pair not found 
		return false;
	}

	/// put a [value] under the [key] if that [key] existed @returns existed or not
	template<typename R, typename F> R maybe_put_replaced(K key, V value, F prevent) {
		if(!value) {
			// they can come here from Temp_value_element::dctor to restore some empty value
			remove(key);
			// this has nothing to do with properties, doing no special property handling here
			return 0; 
		}

		uint code=hash_code(key);
		uint index=code%allocated;
		for(Pair *pair=refs[index]; pair; pair=pair->link)
			if(pair->code==code && pair->key==key) {
				// found a pair with the same key, replacing
				// prevent-function intercepted put?
				if(R result=prevent(pair->value))
					return result;
				
				pair->value=value;
				return reinterpret_cast<R>(1);
			}

		// proper pair not found 
		return 0;
	}

	/// put a [value] under the [key] if that [key] NOT existed @returns existed or not
	bool put_dont_replace(K key, V value) {
		if(!value) {
			remove(key);
			return false;
		}
		if(is_full()) 
			expand();

		uint code=hash_code(key);
		uint index=code%allocated;
		Pair **ref=&refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && pair->key==key) {
				// found a pair with the same key, NOT replacing
				return true;
			}

		// proper pair not found -- create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			fused_refs++; // not, we'll use it and record the fact
		NEW_PAIR(code, key, value);
		fpairs_count++;
		return false;
	}

	/** put all 'src' values if NO with same key existed
		@todo optimize this.allocated==src.allocated case
	*/
	void merge_dont_replace(const HASH& src) {
		for(int i=0; i<src.allocated; i++)
			for(Pair *pair=src.refs[i]; pair; pair=pair->link)
				put_dont_replace(pair->key, pair->value);
	}

	/// number of elements in hash
	int count() const { return fpairs_count; }

	/// iterate over all pairs
	template<typename I> void for_each(void callback(K, V, I), I info) const {
#ifdef HASH_ORDER
		for(Pair *pair=first; pair; pair=pair->next)
			callback(pair->key, pair->value, info);
#else
		Pair **ref=refs;
		for(int index=0; index<allocated; index++)
			for(Pair *pair=*ref++; pair; pair=pair->link)
				callback(pair->key, pair->value, info);
#endif
	}

	/// iterate over all pairs
	template<typename I> void for_each_ref(void callback(K, V&, I), I info) const {
#ifdef HASH_ORDER
		for(Pair *pair=first; pair; pair=pair->next)
			callback(pair->key, pair->value, info);
#else
		Pair **ref=refs;
		for(int index=0; index<allocated; index++)
			for(Pair *pair=*ref++; pair; pair=pair->link)
				callback(pair->key, pair->value, info);
#endif
	}

	/// iterate over all pairs until condition becomes true, return that element
	template<typename I> V first_that(bool callback(K, V, I), I info) const {
#ifdef HASH_ORDER
		for(Pair *pair=first; pair; pair=pair->next)
			if(callback(pair->key, pair->value, info))
				return pair->value;
#else
		Pair **ref=refs;
		for(int index=0; index<allocated; index++)
			for(Pair *pair=*ref++; pair; pair=pair->link)
				if(callback(pair->key, pair->value, info))
					return pair->value;
#endif
		return V(0);
	}

	/// remove all elements
	void clear() {
		memset(refs, 0, sizeof(*refs)*allocated);
		fpairs_count=fused_refs=0;	
#ifdef HASH_ORDER
		first=0;
		last=&first;
#endif
	}

protected:

	/// expand when these %% of allocated exausted
	enum {
		THRESHOLD_PERCENT=75
	};

	/// the index of [allocated] in [Hash_allocates]
	int allocates_index;

	/// number of allocated pairs
	int allocated;

	/// helper: expanding when fused_refs == threshold
	int threshold;

	/// used pairs
	int fused_refs;

	/// stored pairs total (including those by links)
	int fpairs_count;

	/// pair storage
	class Pair: public PA_Allocated {
	public:
		uint code;
		K key;
		V value;
		Pair *link;
#ifdef HASH_ORDER
		Pair **prev;
		Pair *next;

		Pair(uint acode, K akey, V avalue, Pair *alink, Pair **aprev) : code(acode), key(akey), value(avalue), link(alink), 
			prev(aprev), next(0) { *aprev=this; }
#else
		Pair(uint acode, K akey, V avalue, Pair *alink) : code(acode), key(akey), value(avalue), link(alink) {}
#endif
	} **refs;

#ifdef HASH_ORDER
	Pair *first;
	Pair **last;
#endif

	/// filled to threshold: needs expanding
	bool is_full() { return fused_refs==threshold; }

	/// allocate larger buffer & rehash
	void expand() {
		int old_allocated=allocated;
		Pair **old_refs=refs;

		allocates_index=allocates_index+1<HASH_ALLOCATES_COUNT?allocates_index+1:HASH_ALLOCATES_COUNT-1;
		// allocated bigger refs array
		allocated=Hash_allocates[allocates_index];
		threshold=allocated*THRESHOLD_PERCENT/100;
		refs=new(UseGC) Pair*[allocated];

		// rehash
		Pair **old_ref=old_refs;
		for(int old_index=0; old_index<old_allocated; old_index++)
			for(Pair *pair=*old_ref++; pair; ) {
				Pair *next=pair->link;

				uint new_index=pair->code%allocated;
				Pair **new_ref=&refs[new_index];
				pair->link=*new_ref;
				*new_ref=pair;

				pair=next;
			}

		delete[] old_refs;
	}

private: //disabled

	HASH& operator = (const HASH&) { return *this; }
};

/** 
	Simple String::body hash.
	Allows hash code caching
*/

#ifdef HASH_CODE_CACHING

template<typename V> class HASH_STRING: public HASH<const CORD,V> {
public:

	typedef typename HASH<const CORD,V>::Pair Pair; 
	typedef const String::Body &K;

	typedef K key_type;
	
	/// put a [value] under the [key] @returns existed or not
	bool put(K str, V value) {
		if(!value) {
			remove(str);
			return false;
		}
		if(this->is_full()) 
			this->expand();

		CORD key=str.get_cord();

		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		Pair **ref=&this->refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key
				pair->value=value;
				return true;
			}

		// proper pair not found -- create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			this->fused_refs++; // not, we'll use it and record the fact
		NEW_PAIR(code, key, value);
		this->fpairs_count++;
		return false;
	}

	/// put a [value] under the [key] @returns existed or not
	template<typename R, typename F, typename I> R replace_maybe_append(K str, V value, F prevent, I info) {
		if(!value) {
			// they can come here from somewhere (true with maybe_replace_maybe_append, keeping parallel)
			remove(str);
			// this has nothing to do with properties, doing no special property handling here
			return 0; 
		}

		if(this->is_full()) 
			this->expand();

		CORD key=str.get_cord();

		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		Pair **ref=&this->refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key
				pair->value=value;
				return reinterpret_cast<R>(1);
			}

		// proper pair not found 
		// prevent-function intercepted append?
		if(R result=prevent(value, info))
			return result;

		//create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			this->fused_refs++; // not, we'll use it and record the fact
		NEW_PAIR(code, key, value);
		this->fpairs_count++;
		return 0;
	}

	/// put a [value] under the [key] @returns existed or not
	template<typename R, typename F1, typename F2, typename I> 
		R maybe_replace_maybe_append(K str, V value, F1 prevent_replace, F2 prevent_append, I info) {
		if(!value) {
			// they can come here from Temp_value_element::dctor to restore some empty value
			remove(str);
			// this has nothing to do with properties, doing no special property handling here
			return 0; 
		}

		if(this->is_full()) 
			this->expand();

		CORD key=str.get_cord();

		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		Pair **ref=&this->refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key

				// prevent-function intercepted replace?
				if(R result=prevent_replace(pair->value, info))
					return result;

				pair->value=value;
				return reinterpret_cast<R>(1);
			}

		// proper pair not found 
		// prevent-function intercepted append?
		if(R result=prevent_append(value, info))
			return result;

		//create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			this->fused_refs++; // not, we'll use it and record the fact
		NEW_PAIR(code, key, value);
		this->fpairs_count++;
		return 0;
	}

	/// put a [value] under the [key] @returns existed or not
	template<typename R, typename F1, typename I> 
		R maybe_replace_never_append(K str, V value, F1 prevent_replace, I info) {
		if(!value) {
			// they can come here from somewhere (true with maybe_replace_maybe_append, keeping parallel)
			remove(str);
			// this has nothing to do with properties, doing no special property handling here
			return 0; 
		}

		if(this->is_full()) 
			this->expand();

		CORD key=str.get_cord();

		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		Pair **ref=&this->refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key

				// prevent-function intercepted replace?
				if(R result=prevent_replace(pair->value, info))
					return result;

				pair->value=value;
				return reinterpret_cast<R>(1);
			}

		return 0;

	}

	/// remove the [key] @returns existed or not
	bool remove(K str) {
		CORD key=str.get_cord();
		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		for(Pair **ref=&this->refs[index]; *ref; ref=&(*ref)->link){
			Pair *pair=*ref;
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key
				Pair *next=pair->link;
#ifdef HASH_ORDER
				*(pair->prev)=pair->next;
				if(pair->next)
					pair->next->prev=pair->prev;
				else
					this->last=pair->prev;
#endif
				delete pair;
				*ref=next;
				--this->fpairs_count;
				return true;
			}
		}

		return false;
	}

	/// return true if key exists
	bool contains(K str){
		CORD key=str.get_cord();
		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		for(Pair *pair=this->refs[index]; pair; pair=pair->link){
			if(pair->code==code && CORD_cmp(pair->key,key)==0)
				return true;
		}

		return false;
	}

	/// get associated [value] by the [key]
	V get(K str) const {
		CORD key=str.get_cord();
		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		for(Pair *pair=this->refs[index]; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0)
				return pair->value;

		return V(0);
	}

	/// put a [value] under the [key] if that [key] existed @returns existed or not
	bool put_replaced(K str, V value) {
		if(!value) {
			remove(str);
			return false;
		}

		CORD key=str.get_cord();
		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		for(Pair *pair=this->refs[index]; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key, replacing
				pair->value=value;
				return true;
			}

		// proper pair not found 
		return false;
	}

	/// put a [value] under the [key] if that [key] existed @returns existed or not
	template<typename R, typename F> R maybe_put_replaced(K str, V value, F prevent) {
		if(!value) {
			// they can come here from Temp_value_element::dctor to restore some empty value
			remove(str);
			// this has nothing to do with properties, doing no special property handling here
			return 0; 
		}

		CORD key=str.get_cord();
		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		for(Pair *pair=this->refs[index]; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key, replacing
				// prevent-function intercepted put?
				if(R result=prevent(pair->value))
					return result;

				pair->value=value;
				return reinterpret_cast<R>(1);
			}

		// proper pair not found 
		return 0;
	}

	/// put a [value] under the [key] if that [key] NOT existed @returns existed or not
	bool put_dont_replace(K str, V value) {
		if(!value) {
			remove(str);
			return false;
		}
		if(this->is_full()) 
			this->expand();

		CORD key=str.get_cord();
		uint code=str.get_hash_code();
		uint index=code%this->allocated;
		Pair **ref=&this->refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key, NOT replacing
				return true;
			}

		// proper pair not found -- create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			this->fused_refs++; // not, we'll use it and record the fact
		NEW_PAIR(code, key, value);
		this->fpairs_count++;
		return false;
	}

	/// iterate over all pairs
	template<typename I> void for_each(void callback(K, V, I), I info) const {
#ifdef HASH_ORDER
		for(Pair *pair=this->first; pair; pair=pair->next)
			callback(pair->key, pair->value, info);
#else
		Pair **ref=this->refs;
		for(int index=0; index<this->allocated; index++)
			for(Pair *pair=*ref++; pair; pair=pair->link)
				callback(String::Body(pair->key, pair->code), pair->value, info);
#endif
	}

	/// iterate over all pairs
	template<typename I> void for_each_ref(void callback(K, V&, I), I info) const {
#ifdef HASH_ORDER
		for(Pair *pair=this->first; pair; pair=pair->next)
			callback(pair->key, pair->value, info);
#else
		Pair **ref=this->refs;
		for(int index=0; index<this->allocated; index++)
			for(Pair *pair=*ref++; pair; pair=pair->link)
				callback(String::Body(pair->key, pair->code), pair->value, info);
#endif
	}

	/// iterate over all pairs until condition becomes true, return that element
	template<typename I> V first_that(bool callback(K, V, I), I info) const {
#ifdef HASH_ORDER
		for(Pair *pair=this->first; pair; pair=pair->next)
			if(callback(String::Body(pair->key, pair->code), pair->value, info))
				return pair->value;
#else
		Pair **ref=this->refs;
		for(int index=0; index<this->allocated; index++)
			for(Pair *pair=*ref++; pair; pair=pair->link)
				if(callback(String::Body(pair->key, pair->code), pair->value, info))
					return pair->value;
#endif
		return V(0);
	}
};
#else //HASH_CODE_CACHING

template<typename V> class HASH_STRING: public HASH<const String::Body,V>{};

#endif //HASH_CODE_CACHING

#ifndef HASH_ORDER
///    Auto-object used to temporarily substituting/removing string hash values
template <typename K, typename V>
class Temp_hash_value {
	HashString<V> &fhash;
	K fname;
	V saved_value;
public:
	Temp_hash_value(HashString<V>& ahash, K aname, V avalue) :
		fhash(ahash),
		fname(aname),
		saved_value(ahash.get(aname)) {
		fhash.put(aname, avalue);
	}
	~Temp_hash_value() { 
		fhash.put(fname, saved_value);
	}
};
#endif

#endif //PA_HASH_CLASS
