/** @file
	Parser: string class. @see untasize_t.C.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_string.C,v 1.62 2001/04/03 09:58:10 paf Exp $
*/

#include "pa_config_includes.h"

#include <locale.h>

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_array.h"
#include "pa_globals.h"
#include "pa_table.h"
#include "pa_threads.h"

#include "pcre.h"

//#include "pa_sapi.h"

// consts

const int MAX_MATCH_COLUMNS=20;

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
	const Chunk *chunk=&src.head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(size_t i=0; i<chunk->count; i++, row++) {
			if(row==src.append_here)
				goto break2;
			
			APPEND(row->item.ptr, row->item.size, 
				(lang!=UL_PASS_APPENDED && (row->item.lang==UL_TAINTED || forced))?lang:row->item.lang,
				row->item.origin.file, row->item.origin.line);
		}
		chunk=row->link;
	} while(chunk);
break2:
	return *this;
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

/// @todo move 'lang' skipping to pos
int String::cmp(int& partial, const String& src, 
				size_t this_offset, Untaint_lang lang) const {
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
	size_t pos=0; 
	while(true) {
		a_break=a_row==a_end;
		b_break=b_row==b_end;
		if(a_break || b_break)
			break;

		if(pos+a_row->item.size > this_offset) {
			if(lang!=UL_UNKNOWN && a_row->item.lang!=lang) 
				return -1; // wrong lang -- bail out

			int size_diff=
				(a_row->item.size-a_offset)-
				(b_row->item.size-b_offset);
			
			if(size_diff==0) { // a has same size as b
				result=memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, 
					a_row->item.size-a_offset);
				if(result)
					return result;
				pos+=a_row->item.size;
				a_row++; a_countdown--; a_offset=0;
				b_row++; b_countdown--; b_offset=0;
			} else if (size_diff>0) { // a longer
				result=memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, 
					b_row->item.size-b_offset);
				if(result)
					return result;
				a_offset+=b_row->item.size-b_offset;
				b_row++; b_countdown--; b_offset=0;
			} else { // b longer
				result=memcmp(a_row->item.ptr+a_offset, b_row->item.ptr+b_offset, 
					a_row->item.size-a_offset);
				if(result)
					return result;
				b_offset+=a_row->item.size-a_offset;
				pos+=a_row->item.size;
				a_row++; a_countdown--; a_offset=0;
			}
			
			if(!b_countdown) {
				b_chunk=b_row->link;
				b_row=b_chunk->rows;
				b_countdown=b_chunk->count;
			}
		} else {
			a_offset-=a_row->item.size;
			pos+=a_row->item.size;
			a_row++; a_countdown--; 
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

/// @todo move 'lang' skipping to pos
int String::cmp(int& partial, const char* b_ptr, size_t src_size, 
				size_t this_offset, Untaint_lang lang) const {
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
	size_t pos=0;
	while(true) {
		a_break=a_row==a_end;
		if(a_break || b_break)
			break;

		if(pos+a_row->item.size > this_offset) {
			if(lang!=UL_UNKNOWN && a_row->item.lang!=lang) 
				return -1; // wrong lang -- bail out

			int size_diff=
				(a_row->item.size-a_offset)-
				(b_size-b_offset);
			
			if(size_diff==0) { // a has same size as b
				if(size_t result=memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, 
					a_row->item.size-a_offset)!=0)
					return result;
				pos+=a_row->item.size;
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
				pos+=a_row->item.size;
				a_row++; a_countdown--; a_offset=0;
			}
		} else {
			a_offset-=a_row->item.size; 
			pos+=a_row->item.size;
			a_row++; a_countdown--; 
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
	if(start==finish)
		return *empty_string;

	String& result=*NEW String(pool());

	size_t pos=0;
	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(size_t i=0; i<chunk->count; pos+=row->item.size, i++, row++) {
			if(row==append_here)
				goto break2;

			size_t item_finish=pos+row->item.size;
			if(item_finish > start) { // started now or already?
				bool started=result.size()==0; // started now?
				bool finished=finish <= item_finish; // finished now?
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
//	SAPI::log(pool(), "piece of '%s' from %d to %d is '%s'",
		//cstr(), start, finish, result.cstr());
	return result;
}

int String::pos(const String& substr, 
				size_t result, Untaint_lang lang) const {
	for(; result<size(); result++) {
		int partial; cmp(partial, substr, result, lang);
		if(
			partial==0 || // full match
			partial==2) // 'substr' starts 'this'+'result'
			return result;
	}
	
	return -1;
}

int String::pos(const char *substr, size_t substr_size, 
				size_t result, Untaint_lang lang) const {
	for(; result<size(); result++) {
		int partial; cmp(partial, substr, substr_size, result, lang);
		if(
			partial==0 || // full match
			partial==2) // 'substr' starts 'this'+'result'
			return result;
	}
	
	return -1;
}

void String::split(Array& result, 
				   size_t* pos_after_ref, 
				   const char *delim, size_t delim_size, 
				   Untaint_lang lang, int limit) const {
	if(delim_size) {
		size_t pos_after=pos_after_ref?*pos_after_ref:0;
		int pos_before;
		// while we have 'delim'...
		for(; (pos_before=pos(delim, delim_size, pos_after, lang))>=0 && limit; limit--) {
			result+=&piece(pos_after, pos_before);
			pos_after=pos_before+delim_size;
		}
		// last piece
		if(pos_after<size() && limit) {
			result+=&piece(pos_after, size());
			pos_after=size();
		}
		if(pos_after_ref)
			*pos_after_ref=pos_after;
	} else { // empty delim
		result+=this;
		if(pos_after_ref)
			*pos_after_ref+=size();
	}
}

void String::split(Array& result, 
				   size_t* pos_after_ref, 
				   const String& delim, Untaint_lang lang, 
				   int limit) const {
	if(delim.size()) {
		size_t pos_after=pos_after_ref?*pos_after_ref:0;
		int pos_before;
		// while we have 'delim'...
		for(; (pos_before=pos(delim, pos_after, lang))>=0 && limit; limit--) {
			result+=&piece(pos_after, pos_before);
			pos_after=pos_before+delim.size();
		}
		// last piece
		if(pos_after<size() && limit) {
			result+=&piece(pos_after, size());
			pos_after=size();
		}
		if(pos_after_ref)
			*pos_after_ref=pos_after;
	} else { // empty delim
		result+=this;
		if(pos_after_ref)
			*pos_after_ref+=size();
	}
}

/// @test setlocale param to auto.p  @test pcre_malloc & pcre_free substs
bool String::match(const String *aorigin,
				   const String& regexp, 
				   const String& options,
				   Table **table) const { 
	SYNCHRONIZED(true);
	static const unsigned char *tables=0;
	if(!tables) {
		setlocale(LC_CTYPE, "ru");
		tables = pcre_maketables();
	}
	const char *pattern=regexp.cstr();
	const char *errptr;
	int erroffset;
	pcre *code=pcre_compile(pattern, 0, 
		&errptr, &erroffset,
		tables);

	if(!code)
		THROW(0, 0,
			&regexp.piece(erroffset, regexp.size()),
			errptr);
	
	int ovecsize;
	int *ovector=(int *)malloc(sizeof(int)*(ovecsize=(1/*.match*/+MAX_MATCH_COLUMNS)*3));
	const char *subject=cstr();
	int length=strlen(subject);
	int exec_result=pcre_exec(code, 0,
          subject, length, 0/*startoffset*/,
          0/*options*/, ovector, ovecsize);

	if(exec_result==PCRE_ERROR_NOMATCH) {
		*table=0;
		return false;
	}

	if(exec_result<0)
		THROW(0, 0,
			0,
			"pcre_exec failed");

	if(exec_result==0)
		THROW(0, 0,
			aorigin,
			"produced more substrings than maximum handled by Parser, which is %d",
				MAX_MATCH_COLUMNS);

	Array& columns=*NEW Array(pool());
	columns+=string_match_name; // .match column name
	Array& row=*NEW Array(pool());
	row+=&piece(ovector[0], ovector[1]); // match column value
	
	for(int i=1; i<exec_result; i++) {
		char *column=(char *)malloc(MAX_NUMBER);
		snprintf(column, MAX_NUMBER, "%d", i);
		columns+=NEW String(pool(), column); // .i column name
		row+=&piece(ovector[i*2+0], ovector[i*2+1]); // .i column value
	}
	
	*table=NEW Table(pool(), aorigin, &columns);
	(**table)+=&row;
	return true;
}
