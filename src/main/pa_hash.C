/*
  $Id: pa_hash.C,v 1.8 2001/01/29 20:10:32 paf Exp $
*/

/*
	The prime numbers used from zend_hash.c,
	the part of Zend scripting engine library,
	Copyrighted (C) 1999-2000    Zend Technologies Ltd.
	http://www.zend.com/license/0_92.txt
	For more information about Zend please visit http://www.zend.com/
*/

#include "pa_pool.h"
#include "pa_threads.h"

void *Hash::Pair::operator new(size_t size, Pool& apool) {
	return apool.malloc(size);
}

/* Zend comment: Generated on an Octa-ALPHA 300MHz CPU & 2.5GB RAM monster */
uint Hash::sizes[]={
	5, 11, 19, 53, 107, 223, 463, 983, 1979, 3907, 7963, 
	16229, 32531, 65407, 130987, 262237, 524521, 1048793, 
	2097397, 4194103, 8388857, 16777447, 33554201, 67108961, 
	134217487, 268435697, 536870683, 1073741621, 2147483399};
int Hash::sizes_count=
	sizeof(sizes)/sizeof(uint);

void *Hash::operator new(size_t size, Pool& apool) {
	return apool.malloc(size);
}

Hash::Hash(Pool& apool, bool athread_safe) :
	pool(apool),
	thread_safe(athread_safe) {
	
	size=sizes[size_index=0];
	threshold=size*THRESHOLD_PERCENT/100;
	used=0;
	refs=static_cast<Pair **>(pool.calloc(sizeof(Pair *)*size));
}

void Hash::expand() {
	int old_size=size;
	Pair **old_refs=refs;

	// allocated bigger refs array
	size_index=size_index+1<sizes_count?size_index+1:sizes_count-1;
	size=sizes[size_index];
	refs=static_cast<Pair **>(pool.calloc(sizeof(Pair *)*size));

	// rehash
	Pair **old_ref=old_refs;
	for(int old_index=0; old_index<old_size; old_index++)
		for(Pair *pair=*old_ref++; pair; ) {
			Pair *linked_pair=pair->link;

			uint new_index=pair->code%size;
			Pair **new_ref=&refs[new_index];
			pair->link=*new_ref;
			*new_ref=pair;

			pair=linked_pair;
		}
}

uint Hash::generic_code(uint aresult, char *start, uint size) {
	uint result=aresult, g;
	char *end=start+size;

	while (start<end) {
		result=(result<<4)+*start++;
		if ((g=(result&0xF0000000))) {
			result=result^(g>>24);
			result=result^g;
		}
	}
	return result;
}

void Hash::put(Key& key, Value *value) {  SYNCHRONIZED(thread_safe);
	if(full()) 
		expand();

	uint code=key.hash_code();
	uint index=code%size;
	Pair **ref=&refs[index];
	for(Pair *pair=*ref; pair; pair=pair->link)
		if(pair->code==code && pair->key==key) {
			// found a pair with the same key
			pair->value=value;
			return;
		}
	
	// proper pair not found -- create&link_in new pair
	*ref=new(pool) Pair(code, key, value, *ref);
}

Hash::Value *Hash::get(Key& key) {  SYNCHRONIZED(thread_safe);
	uint code=key.hash_code();
	uint index=code%size;
	for(Pair *pair=refs[index]; pair; pair=pair->link)
		if(pair->code==code && pair->key==key)
			return pair->value;
	
	return 0;
}
