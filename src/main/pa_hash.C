/*
  $Id: pa_hash.C,v 1.1 2001/01/27 10:03:31 paf Exp $
*/

/*
	The prime numbers used from zend_hash.c,
	the part of Zend scripting engine library,
	Copyrighted (C) 1999-2000    Zend Technologies Ltd.
	http://www.zend.com/license/0_92.txt
	For more information about Zend please visit http://www.zend.com/
*/

#include "pa_pool.h"

/* Zend comment: Generated on an Octa-ALPHA 300MHz CPU & 2.5GB RAM monster */
uint Hash::sizes[]={
	5, 11, 19, 53, 107, 223, 463, 983, 1979, 3907, 7963, 
	16229, 32531, 65407, 130987, 262237, 524521, 1048793, 
	2097397, 4194103, 8388857, 16777447, 33554201, 67108961, 
	134217487, 268435697, 536870683, 1073741621, 2147483399};
int Hash::sizes_count=
	sizeof(sizes)/sizeof(uint);

void *Hash::operator new(size_t size, Pool *apool) {
	return apool->malloc(size);
}

Hash::Hash(Pool *apool) {
	pool=apool;
	
	size=sizes[size_index=0];
	threshold=size*100/THRESHOLD_PERCENT;
	used=0;
	pair_refs=static_cast<Pair **>(pool->calloc(sizeof(Pair *)*size));
}

void Hash::expand() {
	int new_size_index=size_index+1<sizes_count?size_index+1:sizes_count-1;
	int new_size=sizes[new_size_index];
	Pair **new_pair_refs=static_cast<Pair **>(pool->calloc(sizeof(Pair *)*new_size));

	// rehash

	size=new_size;  size_index=new_size_index;
	pair_refs=new_pair_refs;
}

uint Hash::generic_code(uint aresult, char *start, uint size) {
	return 0;
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

void Hash::put(Key& akey, Value *avalue) {
	if(full()) 
		expand();

	uint index=akey.hash_code()%size;
	Pair *pair=&pairs[index];
	if(pair->used) {
		Pair *prev_pair=pair;
		for(; pair; pair=pair->link) {
			if(pair->key==akey) { // found a pair with the same key
				pair->value=avalue;
				return;
			}
			prev_pair=pair;
		}

		// not found proper pair -- create&link_in new pair
		Pair *new_pair=static_cast<Pair *>(pool->malloc(sizeof(Pair)));
		new_pair->used=true;
		new_pair->key=akey;
		new_pair->value=avalue;
		new_pair->link=0;
		prev_pair->link=new_pair;	
	}
	pair->used=true;
	pair->key=akey;
	pair->value=avalue;
}

Value* Hash::get(Key& key) {
	uint code=akey.hash_code();
	uint index=code%size;
	for(Pair *pair=&pairs[index]; pair; pair=pair->link)
		if(pair->code==code && pair->key==key)
			return pair->value;
	
	return 0;
}
