/*
  $Id: pa_array.C,v 1.3 2001/01/27 15:45:24 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"

void *Array::operator new(size_t size, Pool *apool) {
	return apool->malloc(size);
}

void Array::construct(Pool *apool, int initial_rows) {
	pool=apool;
	curr_chunk_rows=initial_rows;
	head=static_cast<Chunk *>(
		pool->malloc(sizeof(int)+sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
	head->count=curr_chunk_rows;
	append_here=head->rows;
	link_row=&head->rows[curr_chunk_rows];
	link_row->link=0;
	fused_rows=0;

	cache_chunk_base=0;
	cache_chunk=head;
}

void Array::expand() {
	curr_chunk_rows+=curr_chunk_rows*CR_GROW_PERCENT/100;
	Chunk *chunk=static_cast<Chunk *>(
		pool->malloc(sizeof(int)+sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
	chunk->count=curr_chunk_rows;
	link_row->link=chunk;
	append_here=chunk->rows;
	link_row=&chunk->rows[curr_chunk_rows];
	link_row->link=0;
}


Array& Array::operator += (Item src) {
	if(chunk_is_full())
		expand();

	append_here->item=src;
	append_here++; fused_rows++;

	return *this;
}

/*
char *Array::c_str() {
	char *result=static_cast<char *>(pool->malloc(size()+1));

	char *copy_here=result;
	Chunk *chunk=&head; 
	do {
		Chunk::Row *row=chunk->rows;
		for(int i=0; i<chunk->count; i++) {
			if(row==append_here)
				goto break2;

			memcpy(copy_here, row->item.ptr, row->item.size);
			copy_here+=row->item.size;
			row++;
		}
		chunk=row->link;
	} while(chunk);
break2:
	*copy_here=0;
	return result;
}
*/
/*
void Array::put(int index, Item item) {
}

Array::Item Array::get(int index) {
}
*/

Array::Item& Array::operator [] (int index) {
	if(!(index>=0 && index<size())) {
		// FIX: some sort of thread-global error
		Item *result=0;
		return *result;
	}

	// if they ask index to the left of cached position, forget cache
	if(index<cache_chunk_base) {
		cache_chunk_base=0;
		cache_chunk=head;
	}

	// navigate to chunk with "index" row
	while(!(index>=cache_chunk_base && index<cache_chunk_base+cache_chunk->count)) {
		int count=cache_chunk->count;
		cache_chunk_base+=count;
		cache_chunk=cache_chunk->rows[count].link;
	}

	return cache_chunk->rows[index-cache_chunk_base].item;
}