#include <string.h>

#include "pa_pool.h"

void *String::operator new(size_t size, Pool *apool) {
	return apool->alloc(size);
}

void String::construct(Pool *apool) {
	pool=apool;
	head.count=curr_chunk_rows=CR_PREALLOCATED_COUNT;
	append_here=head.first;
	head.preallocated_link=0;
	link_row=&head.first[curr_chunk_rows];
}

void String::expand() {
	curr_chunk_rows=curr_chunk_rows*100/CR_GROW_PERCENT;
	Chunk *chunk=static_cast<Chunk *>(
		pool->calloc(sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
	chunk->count=curr_chunk_rows;
	link_row->link=chunk;
	append_here=chunk->first;
	link_row=&chunk->first[curr_chunk_rows];
}

String& String::operator += (char *src) {
	if(chunk_is_full())
		expand();

	append_here->item.ptr=src;
	append_here->item.size=strlen(src);
	append_here++;

	return *this;
}

size_t String::size() {
	int result=0;
	Chunk *chunk=&head; 
	do {
		Chunk::Row *row=chunk->first;
		for(int i=0; i<chunk->count; i++) {
			if(row==append_here)
				goto break2;

			result+=row->item.size;
			row++;
		}
		chunk=row->link;
	} while(chunk);
break2:
	return result;
}

char *String::c_str() {
	char *result=static_cast<char *>(pool->alloc(size()+1));

	char *copy_here=result;
	Chunk *chunk=&head; 
	do {
		Chunk::Row *row=chunk->first;
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

