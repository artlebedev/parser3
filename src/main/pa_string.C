#include <string.h>

#include "pa_pool.h"

void *String::operator new(size_t size, Pool *apool) {
	return apool->alloc(size);
}

void String::construct(Pool *apool, int achunk_items) {
	pool=apool;
	chunk_items=achunk_items;

	head_chunk=static_cast<Chunk *>(
		pool->calloc(sizeof(Chunk::Row)*chunk_items+sizeof(Chunk *)));
	append_here=&head_chunk->first;
	chunk_link_row=&head_chunk->first+chunk_items;
}

void String::expand() {
	chunk_link_row->link=static_cast<Chunk *>(
		pool->calloc(sizeof(Chunk::Row)*chunk_items+sizeof(Chunk *)));
	append_here=&chunk_link_row->link->first;
	chunk_link_row=&chunk_link_row->link->first+chunk_items;
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
	for(Chunk::Row *row=&head_chunk->first; row; row=&row->link->first)
		for(int i=0; i<chunk_items; i++) {
			if(row==append_here)
				goto break2;

			result+=row->item.size;
			row++;
		}
break2:
	return result;
}

char *String::c_str() {
	char *result=static_cast<char *>(pool->alloc(size()+1));

	char *copy_here=result;
	for(Chunk::Row *row=&head_chunk->first; row; row=&row->link->first)
		for(int i=0; i<chunk_items; i++) {
			if(row==append_here)
				goto break2;

			memcpy(copy_here, row->item.ptr, row->item.size);
			copy_here+=row->item.size;
			row++;
		}
break2:
	*copy_here=0;
	return result;
}

