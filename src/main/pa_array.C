/** @file
	Parser: array class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_array.C,v 1.46 2002/02/08 07:27:46 paf Exp $
*/

#include "pa_pool.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_common.h"

void Array::construct_new(int initial_rows) {
	head=tail=static_cast<Chunk *>(
		malloc(sizeof(int)+sizeof(Chunk::Row)*initial_rows+sizeof(Chunk *), 19));
	head->count=initial_rows;
	append_here=head->rows;
	link_row=&head->rows[initial_rows];
	link_row->link=0;
	fused_rows=0;
}

Array::Array(Pool& apool, int initial_rows) : Pooled(apool) {
	construct_new(max(initial_rows, CR_INITIAL_ROWS_DEFAULT));
}

Array::Array(const Array& source, int offset) : Pooled(source.pool()) {
	construct_new(source.size());
	append_array(source, offset);
}

void Array::expand(int chunk_rows) {
	Chunk *chunk=tail=static_cast<Chunk *>(
		malloc(sizeof(int)+sizeof(Chunk::Row)*chunk_rows+sizeof(Chunk *), 2));
	chunk->count=chunk_rows;
	link_row->link=chunk;
	append_here=chunk->rows;
	link_row=&chunk->rows[chunk_rows];
	link_row->link=0;
}


Array& Array::operator += (Item *src) {
	if(chunk_is_full())
		expand(tail->count+CR_GROW_COUNT);

	append_here->item=src;
	append_here++; fused_rows++;

	return *this;
}

Array::Item *Array::get(int index) const {
	if(!(index>=0 && index<size())) {
		throw Exception(0, 0, 0, 
			"Array::get(%d) out of range [0..%d]", index, size()-1);
		return 0; // never
	}

	int base=0;
	Chunk *chunk=head;

	// navigate to chunk with "index" row
	while(!(index>=base && index<base+chunk->count)) {
		int count=chunk->count;
		base+=count;
		chunk=chunk->rows[count].link;
	}

	return chunk->rows[index-base].item;
}

void Array::put(int index, Item *item) {
	if(!(index>=0 && index<size())) {
		throw Exception(0, 0, 0, 
			"Array::put(%d) out of range [0..%d]", index, size()-1);
		return; // never
	}

	int base=0;
	Chunk *chunk=head;

	// navigate to chunk with "index" row
	while(!(index>=base && index<base+chunk->count)) {
		int count=chunk->count;
		base+=count;
		chunk=chunk->rows[count].link;
	}

	chunk->rows[index-base].item=item;
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

void Array::for_each(For_each_func func, void *info) const {
	Chunk *chunk=head;
	while(true) {
		if(chunk==tail) { // last chunk?
			for(Chunk::Row *row=chunk->rows; row!=append_here; row++)
				(*func)(row->item, info);
			break;
		} else {
			int count=chunk->count;
			for(int i=0; i<count; i++)
				(*func)(chunk->rows[i].item, info);
			chunk=chunk->rows[count].link;
		}
	}
}

/*void Array::for_each(For_each_func_storage func, void *info) {
	Chunk *chunk=head;
	while(true) {
		if(chunk==tail) { // last chunk?
			for(Chunk::Row *row=chunk->rows; row!=append_here; row++)
				(*func)(&row->item, info);
			break;
		} else {
			int count=chunk->count;
			for(int i=0; i<count; i++)
				(*func)(&chunk->rows[i].item, info);
			chunk=chunk->rows[count].link;
		}
	}
}
*/

/*void Array::for_each(For_each_func_const func, const void *info) const {
	Chunk *chunk=head;
	while(true) {
		if(chunk==tail) { // last chunk?
			for(Chunk::Row *row=chunk->rows; row!=append_here; row++)
				(*func)(row->item, info);
			break;
		} else {
			int count=chunk->count;
			for(int i=0; i<count; i++)
				(*func)(chunk->rows[i].item, info);
			chunk=chunk->rows[count].link;
		}
	}
}
*/
void* Array::first_that(Item_that_func_const func, const void *info) const {
	Chunk *chunk=head;
	while(true) {
		if(chunk==tail) { // last chunk?
			for(Chunk::Row *row=chunk->rows; row!=append_here; row++)
				if(void *result=(*func)(row->item, info))
					return result;
			break;
		} else {
			int count=chunk->count;
			for(int i=0; i<count; i++) {
				Item* item=chunk->rows[i].item;
				if(void *result=(*func)(item, info))
					return result;
			}
			chunk=chunk->rows[count].link;
		}
	}
	return 0;
}

void* Array::first_that(Item_that_func func, void *info) const {
	Chunk *chunk=head;
	while(true) {
		if(chunk==tail) { // last chunk?
			for(Chunk::Row *row=chunk->rows; row!=append_here; row++)
				if(void *result=(*func)(row->item, info))
					return result;
			break;
		} else {
			int count=chunk->count;
			for(int i=0; i<count; i++) {
				Item* item=chunk->rows[i].item;
				if(void *result=(*func)(item, info))
					return result;
			}
			chunk=chunk->rows[count].link;
		}
	}
	return 0;
}
