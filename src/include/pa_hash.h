/*
  $Id: pa_hash.h,v 1.1 2001/01/27 10:02:59 paf Exp $
*/

/*


*/

#ifndef PA_HASH_H
#define PA_HASH_H

#include <stddef.h>

#include "pa_types.h"
#include "pa_string.h"

class Pool;

typedef String Key;
typedef void Value;

class Hash {
public:

private:
	friend Pool;

	// expand when there would be used this %% of size
	enum {
		THRESHOLD_PERCENT=75
	};

	// the pool I'm allocated on
	Pool *pool;

	// the index of size in sizes
	int size_index;

	// possible sizes. prime numbers
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
		uint code;
		Key key;
		Value *value;
		Pair *link;
		Pair(uint acode, Key& akey, Value *avalue) :
			code(acode),
			key(akey),
			value(avalue),
			link(0) {}
	} **pair_refs;

	// new&constructors made private to enforce factory manufacturing at pool
	static void *operator new(size_t size, Pool *apool);

	Hash(Pool *apool);

	bool full() {
		return used==threshold;
	}
	void expand();

public:

	Hash() { /* never */ }
	static uint generic_code(uint aresult, char *start, uint size);
	void put(Key& key, Value *value);
	Value* get(Key& key);
};

#endif
