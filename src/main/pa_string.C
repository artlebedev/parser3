/** @file
	Parser: string class. @see untasize_t.C.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_string.C,v 1.59 2001/04/02 15:59:56 paf Exp $
*/

#include "pa_config_includes.h"

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_common.h"

// String

String::String(Pool& apool, const char *src, bool tasize_ted) :
	Pooled(apool) {
	last_chunk=&head;
	head.count=CR_PREALLOCATED_COUNT;
	append_here=head.rows;
	head.preallocated_link=0;
	link_row=&head.rows[head.count];
	fused_rows=fsize=0;

	if(src)
		if(tasize_ted)
			APPEND_TAINTED(src, 0, 0, 0);
		else
			APPEND_CONST(src);
}

void String::expand() {
	size_t new_chunk_count=last_chunk->count+last_chunk->count*CR_GROW_PERCENT/100;
	last_chunk=static_cast<Chunk *>(
		malloc(sizeof(size_t)+sizeof(Chunk::Row)*new_chunk_count+sizeof(Chunk *)));
	last_chunk->count=new_chunk_count;
	link_row->link=last_chunk;
	append_here=last_chunk->rows;
	link_row=&last_chunk->rows[last_chunk->count];
	link_row->link=0;
}

String::String(const String& src) :	Pooled(src.pool()) {
	head.count=CR_PREALLOCATED_COUNT;
	
	size_t src_used_rows=src.fused_rows;
	if(src_used_rows<=head.count) {
		// all new rows fit size_to preallocated area
		size_t curr_chunk_rows=head.count;
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
		// new rows don't fit size_to preallocated area: splitting size_to two chunks
		// preallocated chunk src to constructing head
		memcpy(head.rows, src.head.rows, sizeof(Chunk::Row)*head.count);
		// remaining rows size_to new_chunk
		size_t curr_chunk_rows=src_used_rows-head.count;
		Chunk *new_chunk=static_cast<Chunk *>(
			malloc(sizeof(size_t)+sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *)));
		new_chunk->count=curr_chunk_rows;
		head.preallocated_link=new_chunk;
		append_here=link_row=&new_chunk->rows[new_chunk->count];

		Chunk *old_chunk=src.head.preallocated_link; 
		Chunk::Row *new_rows=new_chunk->rows;
		size_t rows_left_to_copy=new_chunk->count;
		while(true) {
			size_t old_count=old_chunk->count;
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

String& String::append(const String& src, Untaint_lang lang, bool forced) {
	size_t src_used_rows=src.fused_rows;
	size_t dst_free_rows=link_row-append_here;
	
	if(src_used_rows<=dst_free_rows) {
		// all new rows fit size_to last chunk
		memcpy(append_here, src.head.rows, sizeof(Chunk::Row)*src_used_rows);
		set_lang(append_here, lang, forced, src_used_rows);
		append_here+=src_used_rows;
	} else {
		// not all new rows fit size_to last chunk: shrinking it to used part,
		size_t used_rows=last_chunk->count-dst_free_rows;
		//size_t *countp=append_here
		link_row=&last_chunk->rows[last_chunk->count=used_rows];
		//   allocating only enough mem to fit src string rows
		//   next append would allocate a new chunk
		last_chunk=static_cast<Chunk *>(
			malloc(sizeof(size_t)+sizeof(Chunk::Row)*src_used_rows+sizeof(Chunk *)));
		last_chunk->count=src_used_rows;
		link_row->link=last_chunk;
		append_here=link_row=&last_chunk->rows[src_used_rows];

		const Chunk *old_chunk=&src.head; 
		Chunk::Row *new_rows=last_chunk->rows;
		size_t rows_left_to_copy=src_used_rows;
		while(true) {
			size_t old_count=old_chunk->count;
			Chunk *next_chunk=old_chunk->rows[old_count].link;
			if(next_chunk) {
				// not last source chunk
				// taking it all
				memcpy(new_rows, old_chunk->rows, sizeof(Chunk::Row)*old_count);
				set_lang(new_rows, lang, forced, old_count);
				new_rows+=old_count;
				rows_left_to_copy-=old_count;

				old_chunk=next_chunk;
			} else {
				// the last source chunk
				// taking only those rows of chunk that _left_to_copy
				memcpy(new_rows, old_chunk->rows, sizeof(Chunk::Row)*rows_left_to_copy);
				set_lang(new_rows, lang, forced, rows_left_to_copy);
				break;
			}
		}
		link_row->link=0;
	}
	fused_rows+=src_used_rows;
	fsize+=src.fsize;

	return *this;
}
void String::set_lang(Chunk::Row *row, Untaint_lang lang, bool forced, size_t size) {
	if(lang==UL_PASS_APPENDED)
		return;

	while(size--) {
		Untaint_lang& item_lang=(row++)->item.lang;
		if(item_lang==UL_YES || forced) // tasize_ted? need untasize_t language assignment
			item_lang=lang;  // assign untasize_t language
	}
}

/*void String::change_lang(Untaint_lang lang) {
	Chunk *chunk=&head; 
	do {
		Chunk::Row *row=chunk->rows;
		for(size_t i=0; i<chunk->count; i++) {
			if(row==append_here)
				goto break2;

			row->item.lang=lang;
			row++;
		}
		chunk=row->link;
	} while(chunk);
break2:
	return;
}
*/
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
	append_here->item.lang=lang;
#ifndef NO_STRING_ORIGIN
	append_here->item.origin.file=file;
	append_here->item.origin.line=line;
#endif
	append_here++; fused_rows++;

	return *this;
}

uint String::hash_code() const {
	uint result=0;

	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(size_t i=0; i<chunk->count; i++) {
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

int String::cmp(int& partial, const String& src, size_t this_offset) const {
	partial=-1;
	this_offset=min(this_offset, size()-1);

	const Chunk *a_chunk=&head;
	const Chunk *b_chunk=&src.head;
	const Chunk::Row *a_row=a_chunk->rows;
	const Chunk::Row *b_row=b_chunk->rows;
	size_t a_offset=this_offset;
	size_t b_offset=0;
	Chunk::Row *a_end=append_here;
	Chunk::Row *b_end=src.append_here;
	size_t a_countdown=a_chunk->count;
	size_t b_countdown=b_chunk->count;
	bool a_break=false;
	bool b_break=false;
	size_t result;
	for(size_t pos=0; true; pos+=a_row->item.size) {
		a_break=a_row==a_end;
		b_break=b_row==b_end;
		if(a_break || b_break)
			break;

		if(pos+a_row->item.size > this_offset) {
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

			if(!b_countdown) {
				b_chunk=b_row->link;
				b_row=b_chunk->rows;
				b_countdown=b_chunk->count;
			}
		} else {
			a_row++; a_countdown--; a_offset-=a_row->item.size;
		}

		if(!a_countdown) {
			a_chunk=a_row->link;
			a_row=a_chunk->rows;
			a_countdown=a_chunk->count;
		}
	}
	if(a_break==b_break) { // ended simultaneously
		partial=0; return 0;
	} else if(a_break) { // first bytes equal, but a ended before b
		partial=1; return -1;
	} else {
		partial=2; return +1;
	}
}

int String::cmp(int& partial, const char* b_ptr, size_t src_size, 
				size_t this_offset) const {
	partial=-1;
	size_t b_size=src_size?src_size:b_ptr?strlen(b_ptr):0;
	this_offset=min(this_offset, size()-1);

	const Chunk *a_chunk=&head;
	const Chunk::Row *a_row=a_chunk->rows;
	size_t a_offset=this_offset;
	size_t b_offset=0;
	Chunk::Row *a_end=append_here;
	size_t a_countdown=a_chunk->count;
	bool a_break=false;
	bool b_break=false;
	for(size_t pos=0; true; pos+=a_row->item.size) {
		a_break=a_row==a_end;
		if(a_break || b_break)
			break;

		if(pos+a_row->item.size > this_offset) {
			int size_diff=
				(a_row->item.size-a_offset)-
				(b_size-b_offset);
			
			if(size_diff==0) { // a has same size as b
				if(size_t result=memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, 
					a_row->item.size-a_offset)!=0)
					return result;
				a_row++; a_countdown--; a_offset=0;
				b_break=true;
			} else if (size_diff>0) { // a longer
				if(size_t result=memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, 
					b_size-b_offset)!=0)
					return result;
				a_offset+=b_size-b_offset;
				b_break=true;
			} else { // b longer
				if(size_t result=memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, 
					a_row->item.size-a_offset)!=0)
					return result;
				b_offset+=a_row->item.size-a_offset;
				a_row++; a_countdown--; a_offset=0;
			}
		} else {
			a_row++; a_countdown--; a_offset-=a_row->item.size;
		}

		if(!a_countdown) {
			a_chunk=a_row->link;
			a_row=a_chunk->rows;
			a_countdown=a_chunk->count;
		}
	}
	if(a_break==b_break) { // ended simultaneously
		partial=0; return 0;
	} else if(a_break) { // first bytes equal, but a ended before b
		partial=1; return -1;
	} else {
		partial=2; return +1;
	}
}

#ifndef NO_STRING_ORIGIN
const Origin& String::origin() const { 
	if(!fused_rows)
		THROW(0, 0, 
			0,
			"String::origin() of empty string called");
	
	// determining origin by last appended piece
	// because first one frequently constant. 
	// ex: ^load[/file] "document_root" + "/file"
	return append_here[-1].item.origin; 
}
#endif

String& String::piece(size_t start, size_t finish) const {
	start=max(0, start);
	finish=min(size(), finish);

	String& result=*NEW String(pool());

	size_t pos=0;
	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(size_t i=0; i<chunk->count; pos+=row->item.size, i++, row++) {
			if(row==append_here)
				goto break2;

			if(start>=pos) { // started now or already?
				size_t item_finish=pos+row->item.size;
				bool started=start < item_finish; // started now?
				bool finished=finish < item_finish; // finished now?
				size_t offset=started?start-pos:0;
				size_t size=finished?finish-pos:row->item.size;
				result.APPEND(
					row->item.ptr+offset, size-offset, 
					row->item.lang,
					row->item.origin.file, row->item.origin.line);
				if(finished)
					goto break2;
			}
		}
		chunk=row->link;
	} while(chunk);
break2:
	return result;
}

int String::pos(const String& substr, size_t result) const {
	for(; result<size(); result++) {
		int partial; cmp(partial, substr, result);
		if(
			partial==0 || // full match
			partial==2) // 'substr' starts 'this'+'result'
			return result;
	}
	
	return -1;
}

int String::pos(const char *substr, size_t result) const {
	for(; result<size(); result++) {
		int partial; cmp(partial, substr, 0, result);
		if(
			partial==0 || // full match
			partial==2) // 'substr' starts 'this'+'result'
			return result;
	}
	
	return -1;
}