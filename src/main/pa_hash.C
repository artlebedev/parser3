/** @file
	Parser: hash class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_hash.C,v 1.36 2001/06/28 07:41:59 parser Exp $
*/
static char *RCSId="$Id: pa_hash.C,v 1.36 2001/06/28 07:41:59 parser Exp $"; 

/*
	The prime numbers used from zend_hash.c,
	the part of Zend scripting engine library,
	Copyrighted (C) 1999-2000    Zend Technologies Ltd.
	http://www.zend.com/license/0_92.txt
	For more information about Zend please visit http://www.zend.com/
*/

#include "pa_hash.h"

void *Hash::Pair::operator new(size_t size, Pool& apool) {
	return apool.malloc(size);
}

/* Zend comment: Generated on an Octa-ALPHA 300MHz CPU & 2.5GB RAM monster */
uint Hash::allocates[]={
	5, 11, 19, 53, 107, 223, 463, 983, 1979, 3907, 7963, 
	16229, 32531, 65407, 130987, 262237, 524521, 1048793, 
	2097397, 4194103, 8388857, 16777447, 33554201, 67108961, 
	134217487, 268435697, 536870683, 1073741621, 2147483399};
int Hash::allocates_count=
	sizeof(allocates)/sizeof(uint);


void Hash::construct(Pool& apool) {
	allocated=allocates[allocates_index=0];
	threshold=allocated*THRESHOLD_PERCENT/100;
	used=0;
	refs=static_cast<Pair **>(calloc(sizeof(Pair *)*allocated));
}

void Hash::expand() {
	int old_size=allocated;
	Pair **old_refs=refs;

	// allocated bigger refs array
	allocates_index=allocates_index+1<allocates_count?allocates_index+1:allocates_count-1;
	allocated=allocates[allocates_index];
	threshold=allocated*THRESHOLD_PERCENT/100;
	refs=static_cast<Pair **>(calloc(sizeof(Pair *)*allocated));

	// rehash
	Pair **old_ref=old_refs;
	for(int old_index=0; old_index<old_size; old_index++)
		for(Pair *pair=*old_ref++; pair; ) {
			Pair *linked_pair=pair->link;

			uint new_index=pair->code%allocated;
			Pair **new_ref=&refs[new_index];
			pair->link=*new_ref;
			*new_ref=pair;

			pair=linked_pair;
		}
}

uint Hash::generic_code(uint aresult, const char *start, uint allocated) {
	uint result=aresult, g;
	const char *end=start+allocated;

	while (start<end) {
		result=(result<<4)+*start++;
		if ((g=(result&0xF0000000))) {
			result=result^(g>>24);
			result=result^g;
		}
	}
	return result;
}

bool Hash::put(const Key& key, Val *value) {
	if(full()) 
		expand();

	uint code=key.hash_code();
	uint index=code%allocated;
	Pair **ref=&refs[index];
	for(Pair *pair=*ref; pair; pair=pair->link)
		if(pair->code==code && pair->key==key) {
			// found a pair with the same key
			pair->value=value;
			return true;
		}
	
	// proper pair not found -- create&link_in new pair
	if(!*ref) // root cell were used?
		used++; // not, we'll use it and record the fact
	*ref=NEW Pair(code, key, value, *ref);
	return false;
}

Hash::Val *Hash::get(const Key& key) const {
	uint code=key.hash_code();
	uint index=code%allocated;
	for(Pair *pair=refs[index]; pair; pair=pair->link)
		if(pair->code==code && pair->key==key)
			return pair->value;
	
	return 0;
}

bool Hash::put_replace(const Key& key, Val *value) {
	uint code=key.hash_code();
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

bool Hash::put_dont_replace(const Key& key, Val *value) {
	if(full()) 
		expand();

	uint code=key.hash_code();
	uint index=code%allocated;
	Pair **ref=&refs[index];
	for(Pair *pair=*ref; pair; pair=pair->link)
		if(pair->code==code && pair->key==key) {
			// found a pair with the same key, NOT replacing
			return true;
		}

	// proper pair not found -- create&link_in new pair
	*ref=NEW Pair(code, key, value, *ref);
	if(!*ref) // root cell were used?
		used++; // not, we'll use it and record the fact
	return false;
}

void Hash::merge_dont_replace(const Hash& src) {
	for(int i=0; i<src.allocated; i++)
		for(Pair *pair=src.refs[i]; pair; pair=pair->link)
			put_dont_replace(pair->key, pair->value);
	// MAY:optimize this.allocated==src.allocated case
}

void Hash::for_each(For_each_func func, void *info) const {
	Pair **ref=refs;
	for(int index=0; index<allocated; index++)
		for(Pair *pair=*ref++; pair; pair=pair->link)
			if(pair->value)
				(*func)(pair->key, pair->value, info);
}

Hash::Val* Hash::first_that(First_that_func func, void *info) const {
	Pair **ref=refs;
	for(int index=0; index<allocated; index++)
		for(Pair *pair=*ref++; pair; pair=pair->link)
			if(pair->value)
				if((*func)(pair->key, pair->value, info))
					return pair->value;
	return 0;
}

void Hash::clear() {
	memset(refs, 0, sizeof(*refs)*allocated);
	used=0;
}
