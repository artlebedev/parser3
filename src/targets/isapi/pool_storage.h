/** @file
	Parser: ISAPI: pool storage class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pool_storage.h,v 1.8 2001/09/15 14:22:47 parser Exp $
*/

#ifndef PA_POOL_STORAGE_H
#define PA_POOL_STORAGE_H

#include "pa_config_includes.h"

/**
	Dumb pool allocations accounter
	
	@todo implement at least simple suballocations
*/
class Pool_storage {

	template<typename T> class List {
		friend Pool_storage;
	public:
		List(int preallocate) : 
			items((T *)::malloc(preallocate*sizeof(T))),
			allocated(0),
			used(0) {
			if(items) // successfully preallocated?
				allocated=preallocate;
		}

		~List() {
			::free(items);
		}

		bool add(T item) {
			if(full())
				if(!expand())
					return false;

			items[used++]=item;
			return true;
		}

	private:

		bool full() { return used==allocated; }
		bool expand() {
			size_t new_allocated=allocated*3/2;
			T *new_items=(T *)::realloc(items, new_allocated*sizeof(T));
			if(new_items) {
				items=new_items;
				allocated=new_allocated;
				return true;
			} else
				return false;			
		}

	private:

		T *items;
		size_t allocated;
		size_t used;

	};

	struct Cleanup_struct {
		void (*cleanup) (void *);
		void *data;
	};


public:

	Pool_storage() : 
		cleanups(100),
		allocations(10*0x400) {
	}

	void *malloc(size_t size) { 
		void *result=::malloc(size);
		if(result && !allocations.add(result)) {
			::free(result); result=0;
		}
		return result;
	}
	void *calloc(size_t size) { 
		void *result=::calloc(size, 1);
		if(result && !allocations.add(result)) {
			::free(result); result=0;
		}
		return result;
	}

	bool register_cleanup(void (*cleanup) (void *), void *data) {
		Cleanup_struct item={cleanup, data};
		return cleanups.add(item)!=0;
	}

	~Pool_storage() {
		size_t i;

		// cleanups first, because they use some object's memory pointers
		// Cleanup_structs 
		for(i=0; i<cleanups.used; i++) {
			Cleanup_struct& item=cleanups.items[i];
			item.cleanup(item.data);
		}

		// allocations
		for(i=0; i<allocations.used; i++)
			free(allocations.items[i]);
	}

private:

	List<Cleanup_struct> cleanups;
	List<void *> allocations;

};


#endif
