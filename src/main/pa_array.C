/*
  $Id: pa_array.C,v 1.5 2001/01/29 09:57:22 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"

void *Array::operator new(size_t size, Pool *apool) {
	return apool->malloc(size);
}

void Array::construct(Pool *apool, int initial_rows) {
	tail
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
	tail
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

Array& Array::operator += (Array& src) {
	int src_size=src.size();
	int last_chunk_rows_left=link_row-append_here;
	
	// our last chunk too small for src to fit?
	if(src_size>last_chunk_rows_left) {
		// shrink last chunk to used rows
		tail->count-=last_chunk_rows_left;
		link_row=append_here;

		// append new src_size-ed chunk 
		Chunk *chunk=static_cast<Chunk *>(
			pool->malloc(sizeof(int)+sizeof(Chunk::Row)*src_size+sizeof(Chunk *)));
		chunk->count=src_size;
		tail=link_row->link=chunk;
		append_here=chunk->rows;
		link_row=&chunk->rows[curr_chunk_rows];
		link_row->link=0;
	}

		Chunk *src_chunk=src.head; 
		Chunk::Row *dest_rows=append_here;
		int rows_left_to_copy=src_size;
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
	} else {
	}

	return *this;
}

void Array::remove(int index, int count=1) {
}
