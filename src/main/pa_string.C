/*
  $Id: pa_string.C,v 1.7 2001/01/27 10:02:59 paf Exp $
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
}

void String::expand() {
	curr_chunk_rows=curr_chunk_rows*100/CR_GROW_PERCENT;
	Chunk *chunk=static_cast<Chunk *>(
		pool->calloc(sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
	chunk->count=curr_chunk_rows;
	link_row->link=chunk;
	append_here=chunk->rows;
	link_row=&chunk->rows[curr_chunk_rows];
}

String::String(String& src) {/*
	int src_used_rows=src.used_rows(); {
		curr_chunk_rows=src_total_rows
			Chunk *chunk=static_cast<Chunk *>(
			pool->calloc(sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
		chunk->count=curr_chunk_rows;
		link_row->link=chunk;
		append_here=chunk->rows;
		link_row=&chunk->rows[curr_chunk_rows];
	}

	Chunk *chunk=&head; 
	do {
		result+=chunk->count;
		chunk=row->link;
	} while(chunk);
break2:
	return result;*/

}

String& String::operator = (String& src) {
	return *this;
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
		Chunk::Row *row=chunk->rows;
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

int String::used_rows() {
	int result=0;

	Chunk *chunk=&head; 
	do {
		int count=chunk->count;
		result+=count;
		chunk=chunk->rows[count].link;
	} while(chunk);

	result-=link_row-append_here;

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
	return false;
}
