/*
  $Id: pa_string.C,v 1.35 2001/03/10 12:12:51 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"

// String

String::String(Pool& apool) :
	Pooled(apool) {
	last_chunk=&head;
	head.count=CR_PREALLOCATED_COUNT;
	append_here=head.rows;
	head.preallocated_link=0;
	link_row=&head.rows[head.count];
	fused_rows=fsize=0;
}

void String::expand() {
	int new_chunk_count=last_chunk->count+last_chunk->count*CR_GROW_PERCENT/100;
	last_chunk=static_cast<Chunk *>(
		malloc(sizeof(int)+sizeof(Chunk::Row)*new_chunk_count+sizeof(Chunk *)));
	last_chunk->count=new_chunk_count;
	link_row->link=last_chunk;
	append_here=last_chunk->rows;
	link_row=&last_chunk->rows[last_chunk->count];
	link_row->link=0;
}

String::String(const String& src) :
	Pooled(src.pool()) {
	head.count=CR_PREALLOCATED_COUNT;
	
	int src_used_rows=src.used_rows();
	if(src_used_rows<=head.count) {
		// all new rows fit into preallocated area
		int curr_chunk_rows=head.count;
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
		int curr_chunk_rows=src_used_rows-head.count;
		Chunk *new_chunk=static_cast<Chunk *>(
			malloc(sizeof(int)+sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
		new_chunk->count=curr_chunk_rows;
		head.preallocated_link=new_chunk;
		append_here=link_row=&new_chunk->rows[new_chunk->count];

		Chunk *old_chunk=src.head.preallocated_link; 
		Chunk::Row *new_rows=new_chunk->rows;
		int rows_left_to_copy=new_chunk->count;
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

String& String::append(const String& src, Untaint_lang lang) {
	int src_used_rows=src.used_rows();
	int dst_free_rows=link_row-append_here;
	
	if(src_used_rows<=dst_free_rows) {
		// all new rows fit into last chunk
		memcpy(append_here, src.head.rows, sizeof(Chunk::Row)*src_used_rows);
		set_lang(append_here, lang, src_used_rows);
		append_here+=src_used_rows;
	} else {
		// not all new rows fit into last chunk: shrinking it to used part,
		int used_rows=last_chunk->count-dst_free_rows;
		//int *countp=append_here
		link_row=&last_chunk->rows[last_chunk->count=used_rows];
		//   allocating only enough mem to fit src string rows
		//   next append would allocate a new chunk
		last_chunk=static_cast<Chunk *>(
			malloc(sizeof(int)+sizeof(Chunk::Row)*src_used_rows+sizeof(Chunk *)));
		last_chunk->count=src_used_rows;
		link_row->link=last_chunk;
		append_here=link_row=&last_chunk->rows[src_used_rows];

		const Chunk *old_chunk=&src.head; 
		Chunk::Row *new_rows=last_chunk->rows;
		int rows_left_to_copy=src_used_rows;
		while(true) {
			int old_count=old_chunk->count;
			Chunk *next_chunk=old_chunk->rows[old_count].link;
			if(next_chunk) {
				// not last source chunk
				// taking it all
				memcpy(new_rows, old_chunk->rows, sizeof(Chunk::Row)*old_count);
				set_lang(new_rows, lang, old_count);
				new_rows+=old_count;
				rows_left_to_copy-=old_count;

				old_chunk=next_chunk;
			} else {
				// the last source chunk
				// taking only those rows of chunk that _left_to_copy
				memcpy(new_rows, old_chunk->rows, sizeof(Chunk::Row)*rows_left_to_copy);
				set_lang(new_rows, lang, rows_left_to_copy);
				break;
			}
		}
		link_row->link=0;
	}
	fused_rows+=src_used_rows;
	fsize+=src.fsize;

	return *this;
}
void String::set_lang(Chunk::Row *row, Untaint_lang lang, size_t size) {
	if(lang==PASS_APPENDED)
		return;

	while(size--) {
		Untaint_lang& item_lang=(row++)->item.lang;
		if(item_lang==YES) // tainted? need untaint language assignment
			item_lang=lang;  // assign untaint language
	}
}

String& String::real_append(STRING_APPEND_PARAMS) {
	if(!src)
		return *this;
	if(!size)
		size=strlen(src);
	if(!size)
		return *this;

	if(chunk_is_full())
		expand();

	append_here->item.ptr=src;
	fsize+=append_here->item.size=size;
	append_here->item.lang=tainted?/*Untaint_lang::*/YES:Untaint_lang::NO;
#ifndef NO_STRING_ORIGIN
	append_here->item.origin.file=file;
	append_here->item.origin.line=line;
#endif
	append_here++; fused_rows++;

	return *this;
}

char *String::cstr() const {
	char *result=static_cast<char *>(malloc(size()+1));

	char *copy_here=result;
	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(int i=0; i<chunk->count; i++) {
			if(row==append_here)
				goto break2;

			switch(row->item.lang) {
			case NO:
			case YES: // for VString.get_double of tainted values
			case AS_IS: 
				memcpy(copy_here, row->item.ptr, row->item.size); 
				break;
			case HTML_TYPO: 
				memset(copy_here, '?', row->item.size); 
				break;
			default:
				THROW(0,0,
					this,
					"unknown untaint language #%d of %d piece", 
						static_cast<int>(row->item.lang),
						i);
			}
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

int String::cmp(const String& src) const {
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
	int result;
	while(true) {
		a_break=a_row==a_end;
		b_break=b_row==b_end;
		if(a_break || b_break)
			break;

		int size_diff=
			(a_row->item.size-a_offset)-
			(b_row->item.size-b_offset);

		if(size_diff==0) { // a has same size as b
			result=memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, a_row->item.size-a_offset);
			if(result)
				return result;
			a_row++; a_countdown--; a_offset=0;
			b_row++; b_countdown--; b_offset=0;
		} else if (size_diff>0) { // a longer
			result=memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, b_row->item.size-b_offset);
			if(result)
				return result;
			a_offset+=b_row->item.size-b_offset;
			b_row++; b_countdown--; b_offset=0;
		} else { // b longer
			result=memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, a_row->item.size-a_offset);
			if(result)
				return result;
			b_offset+=a_row->item.size-a_offset;
			a_row++; a_countdown--; a_offset=0;
		}

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
	if(a_break==b_break) // ended simultaneously
		result=0;
	else if(a_break) // first bytes equal, but a ended before b
		result=-1;
	else
		result=+1;
	return result;
}

bool String::operator == (char* b_ptr) const {
	size_t b_size=b_ptr?strlen(b_ptr):0;
	if(size() != b_size)
		return false;

	const Chunk *a_chunk=&head;
	const Chunk::Row *a_row=a_chunk->rows;
	int a_offset=0;
	int b_offset=0;
	Chunk::Row *a_end=append_here;
	int a_countdown=a_chunk->count;
	bool a_break=false;
	bool b_break=false;
	while(true) {
		int size_diff=
			(a_row->item.size-a_offset)-
			(b_size-b_offset);

		if(size_diff==0) { // a has same size as b
			if(memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, a_row->item.size-a_offset)!=0)
				return false;
			a_row++; a_countdown--; a_offset=0;
			b_break=true;
		} else if (size_diff>0) { // a longer
			if(memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, b_size-b_offset)!=0)
				return false;
			a_offset+=b_size-b_offset;
			b_break=true;
		} else { // b longer
			if(memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, a_row->item.size-a_offset)!=0)
				return false;
			b_offset+=a_row->item.size-a_offset;
			a_row++; a_countdown--; a_offset=0;
		}

		a_break=a_row==a_end;
		if(a_break || b_break)
			break;

		if(!a_countdown) {
			a_chunk=a_row->link;
			a_row=a_chunk->rows;
			a_countdown=a_chunk->count;
		}
	}
	return a_break==b_break;
}
