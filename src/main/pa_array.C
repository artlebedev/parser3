/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_array.C,v 1.21 2001/03/10 16:34:39 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_array.h"
#include "pa_exception.h"

Array::Array(Pool& apool, int initial_rows) :
	Pooled(apool) {
	head=tail=static_cast<Chunk *>(
		malloc(sizeof(int)+sizeof(Chunk::Row)*initial_rows+sizeof(Chunk *)));
	head->count=initial_rows;
	append_here=head->rows;
	link_row=&head->rows[initial_rows];
	link_row->link=0;
	fused_rows=0;

	cache_chunk_base=0;
	cache_chunk=head;
}

void Array::expand(int chunk_rows) {
	if(chunk_rows<CR_INITIAL_ROWS_DEFAULT)
		chunk_rows=CR_INITIAL_ROWS_DEFAULT;

	Chunk *chunk=tail=static_cast<Chunk *>(
		malloc(sizeof(int)+sizeof(Chunk::Row)*chunk_rows+sizeof(Chunk *)));
	chunk->count=chunk_rows;
	link_row->link=chunk;
	append_here=chunk->rows;
	link_row=&chunk->rows[chunk_rows];
	link_row->link=0;
}


Array& Array::operator += (Item *src) {
	if(chunk_is_full())
		expand(tail->count*CR_GROW_PERCENT/100);

	append_here->item=src;
	append_here++; fused_rows++;

	return *this;
}

Array::Item *Array::get(int index) const {
	if(!(index>=0 && index<size())) {
		THROW(0, 0, 0, 
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

void Array::put(int index, Item *item) {
	if(!(index>=0 && index<size())) {
		THROW(0, 0, 0, 
			"Array::put(%d) out of range [0..%d]", index, size()-1);
		return;
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

	cache_chunk->rows[index-cache_chunk_base].item=item;
}

Array& Array::append_array(const Array& src, int offset) {
	int src_rows_to_copy=src.fused_rows-offset;
	int last_chunk_rows_left=link_row-append_here;
	
	// our last chunk too small for src to fit?
	if(src_rows_to_copy>last_chunk_rows_left) {
		// shrink last chunk to used rows
		tail->count-=last_chunk_rows_left;
		link_row=append_here;

		// append new src_used_rows-ed chunk 
		expand(src_rows_to_copy);
	}

	Chunk *src_chunk=src.head;
	Chunk::Row *dest_rows=append_here;
	int rows_left_to_copy=src.fused_rows;
	int rows_left_to_skip=offset;
	while(true) {
		int src_count=src_chunk->count;
		Chunk *next_chunk=src_chunk->rows[src_count].link;
		if(next_chunk) {
			// not last source chunk
			// taking it all
			int rows_to_copy_now=src_count-rows_left_to_skip;
			if(rows_to_copy_now>0)
				memcpy(dest_rows, src_chunk->rows+rows_left_to_skip, 
					sizeof(Chunk::Row)*rows_to_copy_now);
			else
				rows_left_to_skip-=src_count;

			dest_rows+=rows_to_copy_now;
			rows_left_to_copy-=src_count;
			src_chunk=next_chunk;
		} else {
			// the last source chunk
			// taking only those rows of chunk that _left_to_copy
			int rows_to_copy_now=rows_left_to_copy-rows_left_to_skip;
			if(rows_to_copy_now>0)
				memcpy(dest_rows, src_chunk->rows+rows_left_to_skip, 
					sizeof(Chunk::Row)*rows_to_copy_now);
			break;
		}
	}
	append_here+=src_rows_to_copy;
	fused_rows+=src_rows_to_copy;

	return *this;
}
