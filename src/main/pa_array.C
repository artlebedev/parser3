/*
  $Id: pa_array.C,v 1.1 2001/01/27 15:00:04 paf Exp $
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
	append_here=head->rows;
	link_row=&head->rows[curr_chunk_rows];
	link_row->link=0;
	fused_rows=0;
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