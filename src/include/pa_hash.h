/*
  $Id: pa_hash.h,v 1.9 2001/01/29 12:13:14 paf Exp $
*/

/*


*/

#ifndef PA_HASH_H
#define PA_HASH_H

#include <stddef.h>

#include "pa_types.h"
#include "pa_string.h"

class Pool;

class Hash {
public:

	typedef String Key;
	typedef void Value;

public:

	// useful generic hash function
	static uint generic_code(uint aresult, char *start, uint size);

	// put a [value] under the [key]
	/*SYNCHRONIZED*/ void put(Key& key, Value *value);

	// get associated [value] by the [key]
	/*SYNCHRONIZED*/ Value* get(Key& key);

private:
	friend Pool;

	// expand when these %% of size exausted
	enum {
		THRESHOLD_PERCENT=75
	};

	// the pool I'm allocated on
	Pool *pool;

	// am I thread-safe?
	bool thread_safe;

	// the index of [size] in [sizes]
	int size_index;

	// possible [sizes]. prime numbers
	static uint sizes[];
	static int sizes_count;

	// number of allocated pairs
	int size;

	// helper: expanding when used == threshold
	int threshold;

	// used pairs
	int used;

	// main storage
	class Pair {
		friend Hash;

		uint code;
		Key key;
		Value *value;
		Pair *link;
		
		void *operator new(size_t size, Pool *apool);

		Pair(uint acode, Key& akey, Value *avalue, Pair *alink) :
			code(acode),
			key(akey),
			value(avalue),
			link(alink) {}
	} **refs;

	// new&constructors made private to enforce factory manufacturing at pool
	void *operator new(size_t size, Pool *apool);

	Hash(Pool *apool, bool athread_safe);

	// filled to threshold: needs expanding
	bool full() { return used==threshold; }

	// allocate larger buffer & rehash
	void expand();

private: //disabled

	Hash& operator = (Hash& src) { return *this; }
	Hash(Hash& src) {}
};

#endif
