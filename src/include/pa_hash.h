/** @file
	Parser: hash class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_hash.h,v 1.55 2002/03/28 14:26:48 paf Exp $
*/

#ifndef PA_HASH_H
#define PA_HASH_H

#include "pa_pool.h"
#include "pa_types.h"
#include "pa_string.h"

/** 
	Pooled hash.

	Automatically rehashed when almost full.
	Contains no 0 values. 
		get returning 0 means there were no such.
		"put value 0" means "remove"
*/
class Hash : public Pooled {
public:

	typedef String Key; ///< hash Key type. longing for templates
	typedef void Val; ///< hash Val type. longing for templates

	/// for_each iterator function type
	typedef void (*For_each_func)(const Key& key, Val *value, void *info);

	/// for_each iterator function type
	typedef void (*For_each_func_refed)(const Key& key, Val *& value, void *info);

	/// first_that iterator function type
	typedef void *(*First_that_func)(const Key& key, Val *value, void *info);

public:

	Hash(Pool& apool) : Pooled(apool) { 
		construct_new(); 
	}

	Hash(const Hash& source) : Pooled(source.pool()){
		construct_copy(source);
	}

	/// useful generic hash function
	static uint generic_code(uint aresult, const char *start, uint allocated);

	/// put a [value] under the [key] @returns existed or not
	bool put(const Key& key, Val *value);

	/// remove the [key] @returns existed or not
	bool remove(const Key& key);
/*
	/// dirty hack to allow constant items storage. I long for Hash<const Val*>
	bool put(const Key& key, const Val *value) {
		return put(key, const_cast<Val *>(value)); 
	}
*/
	/// get associated [value] by the [key]
	Val *get(const Key& key) const;

	/// put a [value] under the [key] if that [key] existed @returns existed or not
	bool put_replace(const Key& key, Val *value);

	/// put a [value] under the [key] if that [key] NOT existed @returns existed or not
	bool put_dont_replace(const Key& key, Val *value);

	/// put all 'src' values if NO with same key existed
	void merge_dont_replace(const Hash& src);

	void put(const Key& key, int     value) { put(key, reinterpret_cast<Val *>(value)); }
	void put(const Key& key, const String *value) { 
		put(key, static_cast<Val *>(const_cast<String *>(value))); 
	}

	//@{
	/// handy get, longing for Hash<int>, Hash<String *>
	int get_int(const Key& key) const { return reinterpret_cast<int>(get(key)); }
	const String *get_string(const Key& key) const { return static_cast<String *>(get(key)); }
	//@}

	/// number of elements in hash
	int size() const { return count; }

	/// iterate over all not zero elements
	void for_each(For_each_func func, void *info=0) const;

	/// iterate over all not zero elements, passing references
	void for_each(For_each_func_refed func, void *info=0) const;

	/// iterate over all elements until condition
	void *first_that(First_that_func func, void *info=0) const;

	/// remove all elements
	void clear();

protected:

	void construct_new();
	void construct_copy(const Hash& source);

private:

	/// expand when these %% of allocated exausted
	enum {
		THRESHOLD_PERCENT=75
	};

	/// the index of [allocated] in [allocates]
	int allocates_index;

	/// possible [allocates]. prime numbers
	static uint allocates[];
	static int allocates_count;

	/// number of allocated pairs
	int allocated;

	/// helper: expanding when used == threshold
	int threshold;

	/// used pairs
	int used;

	/// stored pairs total (including those by links)
	int count;

	/// pair storage
	class Pair {
		friend class Hash;

		uint code;
		const Key& key;
		Val *value;
		Pair *link;
		
		void *operator new(size_t allocated, Pool& apool);

		Pair(uint acode, const Key& akey, Val *avalue, Pair *alink) :
			code(acode),
			key(akey),
			value(avalue),
			link(alink) {}
	} **refs;

	/// filled to threshold: needs expanding
	bool full() { return used==threshold; }

	/// allocate larger buffer & rehash
	void expand();

private: //disabled

	Hash& operator = (const Hash&) { return *this; }
};

///	Auto-object used for temporarily substituting/removing hash values
class Temp_hash_value {
	Hash& fhash;
	const Hash::Key& fname;
	Hash::Val *saved_value;
public:
	Temp_hash_value(Hash& ahash, const Hash::Key& aname, Hash::Val *avalue) : 
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
