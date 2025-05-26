/** @file
	Parser: hash class decl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)

	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_HASH_H
#define PA_HASH_H

#define IDENT_PA_HASH_H "$Id: pa_hash.h,v 1.107 2025/05/26 00:52:15 moko Exp $"

#include "pa_memory.h"
#include "pa_types.h"
#include "pa_string.h"

const int HASH_ALLOCATES_COUNT=29;

// nearest primes to 5 * 2^n
static uint Hash_allocates[HASH_ALLOCATES_COUNT]={
	5, 11, 19, 41, 79, 163, 317, 641, 1279, 2557, 5119,
	10243, 20479, 40961, 81919, 163841, 327673, 655357,
	1310719, 2621447, 5242883, 10485767, 20971529, 41943049,
	83886091, 167772161, 335544323, 671088637, 1342177283};

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
#undef HASH_NEW_PAIR
#undef HASH_ORDER_CLEAR
#undef HASH_FOR_EACH

#define HASH OrderedHash
#define HASH_STRING OrderedHashString
#define HASH_NEW_PAIR(code, key, value) *ref=new Pair(code, key, value, *ref, this->last); this->last=&((*ref)->next)
#define HASH_ORDER_CLEAR() first=0; last=&first

#define HASH_FOR_EACH \
	for(Pair *pair=this->first; pair; pair=pair->next)

#else

#define HASH Hash
#define HASH_STRING HashString
#define HASH_NEW_PAIR(code, key, value) *ref=new Pair(code, key, value, *ref)
#define HASH_ORDER_CLEAR()

#define HASH_FOR_EACH \
	Pair **ref=this->refs; \
	for(int index=0; index<this->allocated; index++) \
		for(Pair *pair=*ref++; pair; pair=pair->link)

#endif

#define HASH_PUT_CSTR(h, cstr, v) \
	{ static const String::Body body_##__LINE__(cstr); (h).put(body_##__LINE__, (v)); }

template<typename K, typename V> class HASH: public PA_Object {
protected:
	class Pair;
public:
	typedef K key_type;
	typedef V value_type;

	HASH() { 
		allocated=Hash_allocates[allocates_index=0];
		fpairs_count=fused_refs=0;
		refs=new(PointerGC) Pair*[allocated];
		HASH_ORDER_CLEAR();
	}

	HASH(const HASH& source) {
		allocates_index=source.allocates_index;
		allocated=source.allocated;
		fused_refs=source.fused_refs;
		fpairs_count=source.fpairs_count;
		refs=new(PointerGC) Pair*[allocated];
		// clone & rehash
#ifdef HASH_ORDER
		HASH_ORDER_CLEAR();
		for(Pair *pair=source.first; pair; pair=pair->next)
		{
			uint index=pair->code%allocated;
			Pair **ref=&refs[index];
			HASH_NEW_PAIR(pair->code, pair->key, pair->value);
		}
#else
		for(int i=0; i<source.allocated; i++)
			for(Pair *pair=source.refs[i]; pair; pair=pair->link)
			{
				Pair **ref=&refs[i];
				HASH_NEW_PAIR(pair->code, pair->key, pair->value);
			}
#endif
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
		operator delete[](refs);
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
		HASH_NEW_PAIR(code, key, value);
		fpairs_count++;
		return false;
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

#ifdef HASH_ORDER
	String::Body first_key() const {
#ifdef HASH_CODE_CACHING
		return (first) ? String::Body(first->key, first->code) : String::Body();
#else
		return (first) ? first->key : String::Body();
#endif
	}

	V first_value() const {
		return (first) ? first->value : V(0);
	}

	inline Pair* last_pair() const {
		return (fpairs_count) ? (Pair*)((char *)last - offsetof(Pair, next)) : NULL;
	}

	String::Body last_key() const {
		if(Pair* pair = last_pair()) {
#ifdef HASH_CODE_CACHING
			return String::Body(pair->key, pair->code);
#else
			return pair->key;
#endif
		} else {
			return String::Body();
		}
	}

	V last_value() const {
		if(Pair* pair = last_pair())
			return pair->value;
		return NULL;
	}

	void order_clear() {
		HASH_ORDER_CLEAR();
	}

	void order_next(Pair* pair) {
		pair->prev=last;
		pair->next=0;
		*last=pair;
		last=&(pair->next);
	}
#endif //HASH_ORDER

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
		HASH_NEW_PAIR(code, key, value);
		fpairs_count++;
		return false;
	}

	/// put all 'src' values if NO with same key existed
	void merge_dont_replace(const HASH& src) {
#ifdef HASH_ORDER
		for(Pair *pair=src.first; pair; pair=pair->next)
#else
		for(int i=0; i<src.allocated; i++)
			for(Pair *pair=src.refs[i]; pair; pair=pair->link)
#endif
				put_dont_replace(pair->key, pair->value);
	}

	/// number of elements in hash
	int count() const { return fpairs_count; }

	/// iterate over all pairs
	template<typename I> void for_each(void callback(K, V, I), I info) const {
		HASH_FOR_EACH
			callback(pair->key, pair->value, info);
	}

	/// iterate over all pairs
	template<typename I> void for_each_ref(void callback(K, V&, I), I info) const {
		HASH_FOR_EACH
			callback(pair->key, pair->value, info);
	}

	/// iterate over all pairs until condition becomes true, return that element
	template<typename I> V first_that(bool callback(K, V, I), I info) const {
		HASH_FOR_EACH
			if(callback(pair->key, pair->value, info))
				return pair->value;
		return V(0);
	}

	/// remove all elements
	void clear() {
		memset(refs, 0, sizeof(*refs)*allocated);
		fpairs_count=fused_refs=0;
		HASH_ORDER_CLEAR();
	}

protected:

	/// the index of [allocated] in [Hash_allocates]
	int allocates_index;

	/// number of allocated pairs
	int allocated;

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

	/// filled to threshold (THRESHOLD_PERCENT=75), needs expanding
	bool is_full() { return fused_refs + allocated/4 >= allocated; }

	/// allocate larger buffer & rehash
	void expand() {
		int old_allocated=allocated;
		Pair **old_refs=refs;

		if (allocates_index<HASH_ALLOCATES_COUNT-1) allocates_index++;
		// allocated bigger refs array
		allocated=Hash_allocates[allocates_index];
		refs=new(PointerGC) Pair*[allocated];

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

		operator delete[](old_refs);
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
		uint index=code % this->allocated;
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
		HASH_NEW_PAIR(code, key, value);
		this->fpairs_count++;
		return false;
	}

	bool put(const char *str, V value) {
		return put(String::Body(str), value);
	}

	/// remove the [key] @returns existed or not
	bool remove(K str) {
		CORD key=str.get_cord();
		uint code=str.get_hash_code();
		uint index=code % this->allocated;
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
		uint index=code % this->allocated;
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
		uint index=code % this->allocated;
		for(Pair *pair=this->refs[index]; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0)
				return pair->value;

		return V(0);
	}

	/// get associated [value] by the [key], optimized
	V get(const char *key) const {
		uint code=0;
		if(key && *key){
			generic_hash_code(code, key);
		} else {
			key=0;
		}
		uint index=code % this->allocated;
		for(Pair *pair=this->refs[index]; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,(CORD)key)==0)
				return pair->value;

		return V((const char *)0);
	}

	/// put a [value] under the [key] if that [key] existed @returns existed or not
	bool put_replaced(K str, V value) {
		if(!value) {
			remove(str);
			return false;
		}

		CORD key=str.get_cord();
		uint code=str.get_hash_code();
		uint index=code % this->allocated;
		for(Pair *pair=this->refs[index]; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key, replacing
				pair->value=value;
				return true;
			}

		// proper pair not found 
		return false;
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
		uint index=code % this->allocated;
		Pair **ref=&this->refs[index];
		for(Pair *pair=*ref; pair; pair=pair->link)
			if(pair->code==code && CORD_cmp(pair->key,key)==0) {
				// found a pair with the same key, NOT replacing
				return true;
			}

		// proper pair not found -- create&link_in new pair
		if(!*ref) // root cell were fused_refs?
			this->fused_refs++; // not, we'll use it and record the fact
		HASH_NEW_PAIR(code, key, value);
		this->fpairs_count++;
		return false;
	}

	/// rename $.from[] to $.to[]
	void rename(K from, K to) {
		CORD key_from=from.get_cord();
		uint code_from=from.get_hash_code();
		uint index_from=code_from % this->allocated;

		CORD key_to=to.get_cord();
		uint code_to=to.get_hash_code();
		uint index_to=code_to % this->allocated;

		if(code_from == code_to && CORD_cmp(key_from, key_to)==0)
			return;

		for(Pair **ref=&this->refs[index_from]; *ref; ref=&(*ref)->link){
			Pair *pair=*ref;
			if(pair->code==code_from && CORD_cmp(pair->key,key_from)==0) {
				// found a pair with the required key

				// remove it from the from key first
				Pair *next=pair->link;
				*ref=next;

				// to simplify code
				remove(to);

				// change pair key
				pair->code=code_to;
				(CORD &)(pair->key)=key_to;

				// link to the to key, hash order left intact
				if(!(pair->link=this->refs[index_to])) // root was unused?
					this->fused_refs++; // we've used it and record the fact
				this->refs[index_to]=pair;
				return;
			}
		}
	}

	/// put all 'src' values if NO with same key existed
	void merge_dont_replace(const HASH_STRING& src) {
#ifdef HASH_ORDER
		for(Pair *pair=src.first; pair; pair=pair->next)
#else
		for(int i=0; i<src.allocated; i++)
			for(Pair *pair=src.refs[i]; pair; pair=pair->link)
#endif
				put_dont_replace(String::Body(pair->key, pair->code), pair->value);
	}

	/// iterate over all pairs
	template<typename I> void for_each(void callback(K, V, I), I info) const {
		HASH_FOR_EACH
			callback(String::Body(pair->key, pair->code), pair->value, info);
	}

	/// iterate over all pairs
	template<typename I> void for_each_ref(void callback(K, V&, I), I info) const {
		HASH_FOR_EACH
			callback(String::Body(pair->key, pair->code), pair->value, info);
	}

	/// iterate over all pairs until condition becomes true, return that element
	template<typename I> V first_that(bool callback(K, V, I), I info) const {
		HASH_FOR_EACH
			if(callback(String::Body(pair->key, pair->code), pair->value, info))
				return pair->value;
		return V(0);
	}

#else //HASH_CODE_CACHING

template<typename V> class HASH_STRING: public HASH<const String::Body,V>{
public:
	typedef typename HASH<const String::Body,V>::Pair Pair;

#endif //HASH_CODE_CACHING

	/// simple hash iterator
	class Iterator {
		const HASH_STRING<V>& fhash;
		Pair *fcurrent;
#ifndef HASH_ORDER
		int i;
#endif
	public:
#ifdef HASH_ORDER
		Iterator(const HASH_STRING<V>& ahash): fhash(ahash) {
			fcurrent=fhash.first;
		}

		void next() {
			fcurrent=fcurrent->next;
		}
#else
		Iterator(const HASH_STRING<V>& ahash): fhash(ahash) {
			fcurrent=0;
			for(i=0; i<fhash.allocated; i++)
				if (fcurrent=fhash.refs[i])
					break;
		}

		void next() {
			if(fcurrent=fcurrent->link)
				return;
			for(i++; i<fhash.allocated; i++)
				if (fcurrent=fhash.refs[i])
					break;
		}
#endif

		operator bool () {
			return fcurrent != NULL;
		}

		String::Body key(){
#ifdef HASH_CODE_CACHING
			return String::Body(fcurrent->key, fcurrent->code);
#else
			return fcurrent->key;
#endif
		}

		V value(){
			return fcurrent->value;
		}

		Pair *pair(){
			return fcurrent;
		}
	};

#ifdef HASH_ORDER
	/// simple reverse hash iterator
	class ReverseIterator {
		const HASH_STRING<V>& fhash;
		Pair *fcurrent;
	public:
		ReverseIterator(const HASH_STRING<V>& ahash): fhash(ahash) {
			fcurrent=fhash.last_pair();
		}

		void prev() {
			fcurrent=(fcurrent->prev == &fhash.first) ? NULL : (Pair*)((char *)fcurrent->prev - offsetof(Pair, next));
		}

		operator bool () {
			return fcurrent != NULL;
		}

		String::Body key(){
#ifdef HASH_CODE_CACHING
			return String::Body(fcurrent->key, fcurrent->code);
#else
			return fcurrent->key;
#endif
		}

		V value(){
			return fcurrent->value;
		}

		Pair *pair(){
			return fcurrent;
		}
	};
#endif

};

#ifndef HASH_ORDER
///    Auto-object used to temporarily substituting/removing string hash values
template <typename H, typename V>
class Temp_hash_value {
	H *fhash;
	String::Body fname;
	V saved_value;
public:
	Temp_hash_value(H *ahash, String::Body aname, V avalue) : fhash(ahash), fname(aname) {
		if(fhash){
			saved_value=fhash->get(aname);
			fhash->put(aname, avalue);
		}
	}
	~Temp_hash_value() {
		if(fhash)
			fhash->put(fname, saved_value);
	}
};
#endif

#endif //PA_HASH_CLASS
