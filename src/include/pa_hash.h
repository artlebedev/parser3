/*
  $Id: pa_hash.h,v 1.15 2001/02/21 11:10:02 paf Exp $
*/

/*


*/

#ifndef PA_HASH_H
#define PA_HASH_H

#include <stddef.h>

#include "pa_pool.h"
#include "pa_types.h"
#include "pa_string.h"

class Hash : public Pooled {
public:

	typedef String Key;
	typedef void Value;

public:

	Hash(Pool& apool,bool) : Pooled(apool) { construct(apool, true); }
	Hash(Pool& apool) : Pooled(apool) { construct(apool, false); }

	// useful generic hash function
	static uint generic_code(uint aresult, const char *start, uint size);

	// put a [value] under the [key]
	/*SYNCHRONIZED*/ void put(const Key& key, Value *value);

	// get associated [value] by the [key]
	/*SYNCHRONIZED*/ Value *get(const Key& key) const;

	void put(const Key& key, int     value) { put(key, reinterpret_cast<Value *>(value)); }
	void put(const Key& key, String *value) { put(key, static_cast<Value *>(value)); }

	int get_int(const Key& key) { return reinterpret_cast<int>(get(key)); }
	String *get_string(const Key& key) { return static_cast<String *>(get(key)); }

protected:

	void construct(Pool& apool, bool athread_safe);

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
		const Key key;
		Value *value;
		Pair *link;
		
		void *operator new(size_t size, Pool& apool);

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
