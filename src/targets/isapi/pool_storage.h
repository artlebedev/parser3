/** @file
	Parser: ISAPI: pool storage class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pool_storage.h,v 1.6 2001/09/15 13:20:22 parser Exp $
*/

#ifndef PA_POOL_STORAGE_H
#define PA_POOL_STORAGE_H

#include "pa_config_includes.h"

/**
	Dumb pool allocations accounter
	
	@todo implement at least simple suballocations
*/
class Pool_storage {

	class List {
		friend Pool_storage;
	public:
		List(int preallocate_elements, size_t aitem_size) : 
			items((char *)::malloc(preallocate_elements*aitem_size)), item_size(aitem_size),
			allocated_elements(0),
			used_elements(0) {
			if(items) // successfully preallocated?
				allocated_elements=preallocate_elements;
		}

		~List() {
			free(items);
		}

		void *add(void *item) {
			if(full())
				if(!expand())
					return 0;

			return memcpy(&items[(used_elements++)*item_size], item, item_size);
		}

	private:

		bool full() { return used_elements==allocated_elements; }
		bool expand() {
			size_t new_allocated=allocated_elements*3/2;
			char *new_items=(char *)::realloc(items, new_allocated*item_size);
			if(new_items) {
				items=new_items;
				allocated_elements=new_allocated;
				return true;
			} else
				return false;			
		}

	private:

		char *items; size_t item_size;
		size_t allocated_elements;
		size_t used_elements;

	};

	struct Cleanup_struct {
		void (*cleanup) (void *);
		void *data;
	};


public:

	Pool_storage() : 
		allocations(10*0x400, sizeof(void *)),
		cleanups(100, sizeof(Cleanup_struct)) {
	}

	void *malloc(size_t size) { return allocations.add(::malloc(size)); }
	void *calloc(size_t size) { return allocations.add(::calloc(size, 1)); }

	bool register_cleanup(void (*cleanup) (void *), void *data) {
		Cleanup_struct item={cleanup, data};
		return cleanups.add(&item)!=0;
	}

	~Pool_storage() {
		size_t i;

		// allocations
		for(i=0; i<allocations.used_elements; i++)
			free((void *)allocations.items[i*allocations.item_size]);

		// Cleanup_structs
		for(i=0; i<cleanups.used_elements; i++) {
			Cleanup_struct *item=(Cleanup_struct *)cleanups.items[i*cleanups.item_size];
			item->cleanup(item->data);
		}
	}

private:

	List allocations; // void *
	List cleanups; // Cleanup_struct

};


#endif
