/** @file
	Parser: CGI: pool storage class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pool_storage.h,v 1.4 2001/10/24 16:52:25 parser Exp $
*/

#ifndef PA_POOL_STORAGE_H
#define PA_POOL_STORAGE_H

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
		::free(items);
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
		cleanups(100) {
	}

	bool register_cleanup(void (*cleanup) (void *), void *data) {
		Cleanup_struct item={cleanup, data};
		return cleanups.add(item)!=0;
	}

	static void cleanup(Cleanup_struct& item) {
		item.cleanup(item.data);
	}

	~Pool_storage() {
		// cleanups first, because they use some object's memory pointers
		cleanups.for_each_reverse(cleanup);
	}

private:

	List<Cleanup_struct> cleanups;

};


#endif
