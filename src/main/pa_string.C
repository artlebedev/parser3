/*
  $Id: pa_string.C,v 1.8 2001/01/27 12:04:53 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_hash.h"

void *String::operator new(size_t size, Pool *apool) {
	return apool->malloc(size);
}

void String::construct(Pool *apool) {
	pool=apool;
	head.count=curr_chunk_rows=CR_PREALLOCATED_COUNT;
	append_here=head.rows;
	head.preallocated_link=0;
	link_row=&head.rows[curr_chunk_rows];
	fused_rows=fsize=0;
}

void String::expand() {
	curr_chunk_rows+=curr_chunk_rows*CR_GROW_PERCENT/100;
	Chunk *chunk=static_cast<Chunk *>(
		pool->malloc(sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
	chunk->count=curr_chunk_rows;
	link_row->link=chunk;
	append_here=chunk->rows;
	link_row=&chunk->rows[curr_chunk_rows];
	link_row->link=0;
}

String::String(String& src) {
	pool=src.pool;
	head.count=CR_PREALLOCATED_COUNT;
	
	int src_used_rows=src.used_rows();
	if(src_used_rows<=head.count) {
		// new rows fit into preallocated area
		curr_chunk_rows=head.count;
		memcpy(head.rows, src.head.rows, sizeof(Chunk::Row)*src_used_rows);
		append_here=&head.rows[src_used_rows];
		head.preallocated_link=0;
		link_row=&head.rows[curr_chunk_rows];
	} else {
		// warning: 
		//   heavy relies on the fact 
		//   that preallocated area is the same for all strings
		//
		// info:
		//   allocating only enough mem to fit src string rows
		//   next append would allocate a new chunk
		//
		// new rows don't fit into preallocated area: splitting into two chunks
		// preallocated chunk src to constructing head
		memcpy(head.rows, src.head.rows, sizeof(Chunk::Row)*head.count);
		// remaining rows into new_chunk
		curr_chunk_rows=src_used_rows-head.count;
		Chunk *new_chunk=static_cast<Chunk *>(
			pool->malloc(sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
		new_chunk->count=curr_chunk_rows;
		head.preallocated_link=new_chunk;
		append_here=link_row=&new_chunk->rows[curr_chunk_rows];

		Chunk *old_chunk=src.head.preallocated_link; 
		Chunk::Row *new_rows=new_chunk->rows;
		int rows_left_to_copy=curr_chunk_rows;
		while(true) {
			int old_count=old_chunk->count;
			Chunk *next_chunk=old_chunk->rows[old_count].link;
			if(next_chunk) {
				// not last source chunk
				// taking it all
				memcpy(new_rows, old_chunk->rows, sizeof(Chunk::Row)*old_count);
				new_rows+=old_count;
				rows_left_to_copy-=old_count;

				old_chunk=next_chunk;
			} else {
				// the last source chunk
				// taking only those rows of chunk that _left_to_copy
				memcpy(new_rows, old_chunk->rows, sizeof(Chunk::Row)*rows_left_to_copy);
				break;
			}
		}
	}
	link_row->link=0;
	fused_rows=src_used_rows;
	fsize=src.fsize;
}

String& String::operator += (char *src) {
	if(chunk_is_full())
		expand();

	append_here->item.ptr=src;
	fsize+=append_here->item.size=strlen(src);
	append_here++; fused_rows++;

	return *this;
}

char *String::c_str() {
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

uint String::hash_code() {
	uint result=0;

	Chunk *chunk=&head; 
	do {
		Chunk::Row *row=chunk->rows;
		for(int i=0; i<chunk->count; i++) {
			if(row==append_here)
				goto break2;

			result=Hash::generic_code(result, row->item.ptr, row->item.size);
			row++;
		}
		chunk=row->link;
	} while(chunk);
break2:
	return result;
}

bool String::operator == (String& src) {
	if(size() != src.size())
		return false;

	// FIX: 0 approach!
	// use: in Hash it's "this" that has less chunks
	if(head.rows[0].item.size==src.head.rows[0].item.size)
		if(memcmp(head.rows[0].item.ptr, src.head.rows[0].item.ptr, head.rows[0].item.size)==0)
			return true;
	return false;
}
