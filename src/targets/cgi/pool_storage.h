/** @file
	Parser: CGI: pool storage class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_POOL_STORAGE_H
#define PA_POOL_STORAGE_H

static const char* IDENT_POOL_STORAGE_H="$Id: pool_storage.h,v 1.11 2002/08/01 11:26:52 paf Exp $";

#include "pa_config_includes.h"

/**
	Dumb pool allocations accounter
	
	@todo implement at least simple suballocations
*/
template<class T> class List {
public:
	List(int preallocate) : 
		items((T *)::malloc(preallocate*sizeof(T))),
		allocated(0),
		used(0) {
		if(items) // successfully preallocated?
			allocated=preallocate;
	}

	~List() {
		if(items)
    		    ::free(items);
		items=0;
		used=0;
	}

	bool add(T item) {
		if(full())
			if(!expand())
				return false;

		items[used++]=item;
		return true;
	}

	void for_each_reverse(void (*callback)(T& info)) {
		size_t top;

		for(top=used; top; )
			callback(items[--top]);
	}

size_t size() { return used; }
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

class Pool_storage {

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
//fprintf(stderr, "malloc: %d\n", size);
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

	static void cleanup(Cleanup_struct& item) {
		item.cleanup(item.data);
	}

	static void free(void *& item) {
		::free(item);  item=0;
	}

	~Pool_storage() {
	//__asm__("int3");
//		_asm int 3;
//fprintf(stderr, "cleanups: %d\n", cleanups.size());
		// cleanups first, because they use some object's memory pointers
		cleanups.for_each_reverse(cleanup);
		
//fprintf(stderr, "allocs: %d\n", allocations.size());
		// allocations
		allocations.for_each_reverse(free);
	}

private:

	List<Cleanup_struct> cleanups;
	List<void *> allocations;

};


#endif
