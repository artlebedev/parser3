/*
  $Id: pa_array.C,v 1.12 2001/01/30 13:07:31 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_array.h"

void *Array::operator new(size_t size, Pool& apool) {
	return apool.malloc(size);
}

Array::Array(Pool& apool, int initial_rows) :
	pool(apool) {
	head=tail=static_cast<Chunk *>(
		pool.malloc(sizeof(int)+sizeof(Chunk::Row)*initial_rows+sizeof(Chunk *)));
	head->count=initial_rows;
	append_here=head->rows;
	link_row=&head->rows[initial_rows];
	link_row->link=0;
	fused_rows=0;

	cache_chunk_base=0;
	cache_chunk=head;
}

void Array::expand(int chunk_rows) {
	Chunk *chunk=tail=static_cast<Chunk *>(
		pool.malloc(sizeof(int)+sizeof(Chunk::Row)*chunk_rows+sizeof(Chunk *)));
	chunk->count=chunk_rows;
	link_row->link=chunk;
	append_here=chunk->rows;
	link_row=&chunk->rows[chunk_rows];
	link_row->link=0;
}


Array& Array::operator += (const Item *src) {
	if(chunk_is_full())
		expand(tail->count*CR_GROW_PERCENT/100);

	append_here->item=src;
	append_here++; fused_rows++;

	return *this;
}

const Array::Item *Array::get(int index) const {
	if(!(index>=0 && index<size())) {
		pool.exception().raise(0, 0, 0, 
			"Array::get(%d) out of range [0..%d]", index, size()-1);
		return 0;
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

Array& Array::append_array(const Array& src) {
	int src_used_rows=src.fused_rows;
	int last_chunk_rows_left=link_row-append_here;
	
	// our last chunk too small for src to fit?
	if(src_used_rows>last_chunk_rows_left) {
		// shrink last chunk to used rows
		tail->count-=last_chunk_rows_left;
		link_row=append_here;

		// append new src_used_rows-ed chunk 
		expand(src_used_rows);
	}

	Chunk *src_chunk=src.head; 
	Chunk::Row *dest_rows=append_here;
	int rows_left_to_copy=src_used_rows;
	while(true) {
		int src_count=src_chunk->count;
		Chunk *next_chunk=src_chunk->rows[src_count].link;
		if(next_chunk) {
			// not last source chunk
			// taking it all
			memcpy(dest_rows, src_chunk->rows, sizeof(Chunk::Row)*src_count);
			dest_rows+=src_count;
			rows_left_to_copy-=src_count;
			
			src_chunk=next_chunk;
		} else {
			// the last source chunk
			// taking only those rows of chunk that _left_to_copy
			memcpy(dest_rows, src_chunk->rows, sizeof(Chunk::Row)*rows_left_to_copy);
			break;
		}
	}
	append_here+=src_used_rows;
	fused_rows+=src_used_rows;

	return *this;
}
