/*
  $Id: pa_string.C,v 1.24 2001/02/14 14:15:37 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"

// String

String::String(Pool& apool) :
	Pooled(apool) {
	head.count=curr_chunk_rows=CR_PREALLOCATED_COUNT;
	append_here=head.rows;
	head.preallocated_link=0;
	link_row=&head.rows[curr_chunk_rows];
	fused_rows=fsize=0;
}

void String::expand() {
	curr_chunk_rows+=curr_chunk_rows*CR_GROW_PERCENT/100;
	Chunk *chunk=static_cast<Chunk *>(
		pool.malloc(sizeof(int)+sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
	chunk->count=curr_chunk_rows;
	link_row->link=chunk;
	append_here=chunk->rows;
	link_row=&chunk->rows[curr_chunk_rows];
	link_row->link=0;
}

String::String(const String& src) :
	Pooled(src.pool) {
	head.count=CR_PREALLOCATED_COUNT;
	
	int src_used_rows=src.used_rows();
	if(src_used_rows<=head.count) {
		// all new rows fit into preallocated area
		curr_chunk_rows=head.count;
		memcpy(head.rows, src.head.rows, sizeof(Chunk::Row)*src_used_rows);
		append_here=&head.rows[src_used_rows];
		link_row=&head.rows[curr_chunk_rows];
	} else {
		// warning: 
		//   heavily relies on the fact 
		//   "preallocated area is the same for all strings"
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
			pool.malloc(sizeof(int)+sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
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
/*
String(const String_iterator& begin, const String_iterator& end) {
	;//TODO
}
*/

String& String::real_append(STRING_APPEND_PARAMS) {
	if(!src)
		return *this;
	int len=strlen(src);
	if(!len)
		return *this;

	if(chunk_is_full())
		expand();

	append_here->item.ptr=src;
	fsize+=append_here->item.size=len;
#ifndef NO_STRING_ORIGIN
	append_here->item.origin.file=file;
	append_here->item.origin.line=line;
#endif
	append_here++; fused_rows++;

	return *this;
}

char *String::cstr() const {
	char *result=static_cast<char *>(pool.malloc(size()+1));

	char *copy_here=result;
	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
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

uint String::hash_code() const {
	uint result=0;

	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
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

bool String::operator == (const String& src) const {
	if(size() != src.size())
		return false;

	const Chunk *a_chunk=&head;
	const Chunk *b_chunk=&src.head;
	const Chunk::Row *a_row=a_chunk->rows;
	const Chunk::Row *b_row=b_chunk->rows;
	int a_offset=0;
	int b_offset=0;
	Chunk::Row *a_end=append_here;
	Chunk::Row *b_end=src.append_here;
	int a_countdown=a_chunk->count;
	int b_countdown=b_chunk->count;
	bool a_break=false;
	bool b_break=false;
	while(true) {
		int size_diff=
			(a_row->item.size-a_offset)-
			(b_row->item.size-b_offset);

		if(size_diff==0) { // a has same size as b
			if(memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, a_row->item.size-a_offset)!=0)
				return false;
			a_row++; a_countdown--; a_offset=0;
			b_row++; b_countdown--; b_offset=0;
		} else if (size_diff>0) { // a longer
			if(memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, b_row->item.size-b_offset)!=0)
				return false;
			a_offset+=b_row->item.size-b_offset;
			b_row++; b_countdown--; b_offset=0;
		} else { // b longer
			if(memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, a_row->item.size-a_offset)!=0)
				return false;
			b_offset+=a_row->item.size-a_offset;
			a_row++; a_countdown--; a_offset=0;
		}

		a_break=a_row==a_end;
		b_break=b_row==b_end;
		if(a_break || b_break)
			break;

		if(!a_countdown) {
			a_chunk=a_row->link;
			a_row=a_chunk->rows;
			a_countdown=a_chunk->count;
		}
		if(!b_countdown) {
			b_chunk=b_row->link;
			b_row=b_chunk->rows;
			b_countdown=b_chunk->count;
		}
	}
	return a_break==b_break;
}

String& String::append(const String_iterator& begin, const String_iterator& end) {
	//TODO
	return *this;
}

// Char_types

Char_types::Char_types() {
	memset(types, 0, sizeof(types));
}

void Char_types::set(char from, char to, int type) {
	memset(&types[static_cast<unsigned int>(from)], type, to-from+1);
}

// String_iterator 

String_iterator::String_iterator(String& astring) :	string(astring) {
	read_here=string.head.rows;
	position=string.size()==0?0:read_here->item.ptr;
	link_row=reinterpret_cast<String::Chunk::Row*>(string.head.preallocated_link);
}

char String_iterator::operator()() const {
	return position?*position:0;
}

void String_iterator::skip() {
	if(!position)
		return;

	if(++position==
		read_here->item.ptr+
		read_here->item.size) {

		// next row
		if(++read_here==string.append_here) {
			position=0;
			return;
		}
		if(read_here==link_row) {
			String::Chunk *chunk=link_row->link;
			if(!chunk)
				string.pool.exception().raise(0, 0,
					&string,
					"String_iterator::skip() missed "
					"read_here==string.append_here check");

			read_here=chunk->rows;
			link_row=&chunk->rows[chunk->count];
		}
		position=read_here->item.ptr;
	}
}

bool String_iterator::skip_to(char c) {
	if(!position)
		return false;

	while(true) {
		if(char *found=static_cast<char *>(
			memchr(position, c, read_here->item.size-(position-read_here->item.ptr)))) {
			position=found;
			return true;
		}

		// next row
		if(++read_here==string.append_here) {
			position=0;
			return false;
		}
		if(read_here==link_row) {
			String::Chunk *chunk=link_row->link;
			if(!chunk)
				string.pool.exception().raise(0, 0,
					&string,
					"String_iterator::skip_to(char) missed "
					"read_here==string.append_here check");

			read_here=chunk->rows;
			link_row=&chunk->rows[chunk->count];
		}
		position=read_here->item.ptr;
	}
}

int String_iterator::skip_to(Char_types& types) {
	if(!position)
		return false;

	while(true) {
		int countdown=read_here->item.size-(position-read_here->item.ptr);
		for(; countdown--; position++)
			if(int type=types.get(*position))
				return type;

		// next row
		if(++read_here==string.append_here) {
			position=0;
			return -1;
		}
		if(read_here==link_row) {
			String::Chunk *chunk=link_row->link;
			if(!chunk)
				string.pool.exception().raise(0, 0,
					&string,
					"String_iterator::skip_to(Char_type) missed "
					"read_here==string.append_here check");

			read_here=chunk->rows;
			link_row=&chunk->rows[chunk->count];
		}
		position=read_here->item.ptr;
	}
}
