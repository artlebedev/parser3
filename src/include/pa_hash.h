/*
  $Id: pa_hash.h,v 1.12 2001/01/29 20:46:22 paf Exp $
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

	void *operator new(size_t size, Pool& apool);
	Hash(Pool& apool, bool athread_safe);

	// useful generic hash function
	static uint generic_code(uint aresult, const char *start, uint size);

	// put a [value] under the [key]
	/*SYNCHRONIZED*/ void put(Key& key, Value *value);

	// get associated [value] by the [key]
	/*SYNCHRONIZED*/ Value *get(Key& key);

	void put(Key& key, int     value) { put(key, reinterpret_cast<Value *>(value)); }
	void put(Key& key, String *value) { put(key, static_cast<Value *>(value)); }

	int get_int(Key& key) { return reinterpret_cast<int>(get(key)); }
	String *get_string(Key& key) { return static_cast<String *>(get(key)); }

protected:

	// the pool I'm allocated on
	Pool& pool;

private:

	// expand when these %% of size exausted
	enum {
		THRESHOLD_PERCENT=75
	};

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
		
		void *operator new(size_t size, Pool& apool);

		Pair(uint acode, Key& akey, Value *avalue, Pair *alink) :
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
