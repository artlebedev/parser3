/*
  $Id: pa_hash.h,v 1.19 2001/03/07 11:14:11 paf Exp $
*/

/*


*/

#ifndef PA_HASH_H
#define PA_HASH_H

#include <stddef.h>

#include "pa_pool.h"
#include "pa_types.h"
#include "pa_string.h"

class Hash : virtual public Pooled {
public:

	typedef String Key;
	typedef void Value;

public:

	Hash(Pool& apool,bool) : Pooled(apool) { construct(apool, true); }
	Hash(Pool& apool) : Pooled(apool) { construct(apool, false); }

	// useful generic hash function
	static uint generic_code(uint aresult, const char *start, uint allocated);

	// put a [value] under the [key], return existed or not
	/*SYNCHRONIZED*/ bool put(const Key& key, Value *value);

	// get associated [value] by the [key]
	/*SYNCHRONIZED*/ Value *get(const Key& key) const;

	// put a [value] under the [key] if that [key] existed, return existed or not
	/*SYNCHRONIZED*/ bool put_replace(const Key& key, Value *value);

	// put a [value] under the [key] if that [key] NOT existed, return existed or not
	/*SYNCHRONIZED*/ bool put_dont_replace(const Key& key, Value *value);

	// put all 'src' values if NO with same key existed
	/*SYNCHRONIZED*/ void merge_dont_replace(const Hash& src);

	void put(const Key& key, int     value) { put(key, reinterpret_cast<Value *>(value)); }
	void put(const Key& key, String *value) { put(key, static_cast<Value *>(value)); }

	int get_int(const Key& key) { return reinterpret_cast<int>(get(key)); }
	String *get_string(const Key& key) { return static_cast<String *>(get(key)); }

	int size() { return used; }

protected:

	void construct(Pool& apool, bool athread_safe);

private:

	// expand when these %% of allocated exausted
	enum {
		THRESHOLD_PERCENT=75
	};

	// am I thread-safe?
	bool thread_safe;

	// the index of [allocated] in [allocates]
	int allocates_index;

	// possible [allocates]. prime numbers
	static uint allocates[];
	static int allocates_count;

	// number of allocated pairs
	int allocated;

	// helper: expanding when used == threshold
	int threshold;

	// used pairs
	int used;

	// main storage
	class Pair {
		friend Hash;

		uint code;
		const Key key;
		Value *value;
		Pair *link;
		
		void *operator new(size_t allocated, Pool& apool);

		Pair(uint acode, const Key& akey, Value *avalue, Pair *alink) :
			code(acode),
			key(akey),
			value(avalue),
			link(alink) {}
	} **refs;

	// filled to threshold: needs expanding
	bool full() { return used==threshold; }

	// allocate larger buffer & rehash
	void expand();

private: //disabled

	//Hash(Hash&) {}
	Hash& operator = (const Hash&) { return *this; }
};

#endif
