/** @file
	Parser: ISAPI: pool storage class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pool_storage.h,v 1.4 2001/04/03 07:32:49 paf Exp $
*/

#ifndef PA_POOL_STORAGE_H
#define PA_POOL_STORAGE_H

#include "pa_config_includes.h"

/**
	Dumb pool allocations accounter
	
	@todo implement at least simple suballocations
*/
class Pool_storage {
public:
	
	void **ptrs;
	size_t size;
	size_t used;

	Pool_storage(size_t preallocate=10*0x400) : 
		ptrs((void **)::malloc(preallocate*sizeof(void *))),
		size(0),
		used(0) {
		if(ptrs) // successfully preallocated?
			size=preallocate;
	}

	~Pool_storage() {
		return;
		for(size_t i=0; i<used; i++)
			free(ptrs[i]);
		free(ptrs);
	}

	void *malloc(size_t size) {
		if(full())
			if(!expand())
				return 0;

		return ptrs[used++]=::malloc(size);
	}
	void *calloc(size_t size) {
		if(full())
			if(!expand())
				return 0;

		return ptrs[used++]=::calloc(size, 1);
	}

private:

	bool full() { return used==size; }
	bool expand() {
		size_t new_size=size*3/2;
		void **new_ptrs=(void **)::realloc(ptrs, new_size*sizeof(void *));
		if(new_ptrs) {
			ptrs=new_ptrs;
			size=new_size;
			return true;
		} else
			return false;			
	}
};


#endif
