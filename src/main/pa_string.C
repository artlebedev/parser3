/** @file
	Parser: string class. @see untasize_t.C.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_string.C,v 1.130 2001/12/13 10:24:12 paf Exp $
*/

#include "pa_config_includes.h"

#include "pcre.h"
#include "internal.h"

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_array.h"
#include "pa_globals.h"
#include "pa_table.h"
#include "pa_dictionary.h"

String::String(Pool& apool, const char *src, size_t src_size, bool tainted) :
	Pooled(apool) {
	last_chunk=&head;
	head.count=CR_PREALLOCATED_COUNT;
	append_here=head.rows;
	head_link=0;
	link_row=&head.rows[head.count];

	if(src)
		if(tainted)
			APPEND_TAINTED(src, src_size, 0, 0);
		else
			APPEND_CLEAN(src, src_size, 0, 0);
}

String::String(const String& src) :	
	Pooled(src.pool()) {
	head.count=CR_PREALLOCATED_COUNT;
	
	uint src_used_rows=src.used_rows();
	if(src_used_rows<=head.count) {
		// all new rows fit size_to preallocated area
		last_chunk=&head;
		uint curr_chunk_rows=head.count;
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
		uint curr_chunk_rows=src_used_rows-head.count;
		last_chunk=static_cast<Chunk *>(
			malloc(sizeof(uint)+sizeof(Chunk::Row)*curr_chunk_rows+sizeof(Chunk *), 9));
		last_chunk->count=curr_chunk_rows;
		head_link=last_chunk;
		append_here=link_row=&last_chunk->rows[last_chunk->count];

		Chunk *old_chunk=src.head_link; 
		Chunk::Row *new_rows=last_chunk->rows;
		uint rows_left_to_copy=last_chunk->count;
		while(true) {
			uint old_count=old_chunk->count;
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
	src_used_rows;
}

size_t  String::size() const {
	size_t result=0;
	STRING_FOREACH_ROW(
			result+=row->item.size;
	);
break2:
	return result;
}

/// @todo not very optimal
uint String::used_rows() const {
	uint result=0;
	STRING_FOREACH_ROW(
		result++;
	);
break2:
	return result;
}
void String::expand() {
	uint new_chunk_count=last_chunk->count+CR_GROW_COUNT;
	if(new_chunk_count>MAX_USHORT)
		new_chunk_count=MAX_USHORT;

	last_chunk=static_cast<Chunk *>(
		malloc(sizeof(uint)+sizeof(Chunk::Row)*new_chunk_count+sizeof(Chunk *), 10));
	last_chunk->count=new_chunk_count;
	link_row->link=last_chunk;
	append_here=last_chunk->rows;
	link_row=&last_chunk->rows[last_chunk->count];
	link_row->link=0;
}

String& String::real_append(STRING_APPEND_PARAMS) {
	if(!src)
		return *this;
	if(!size)
		size=strlen(src);
	if(!size)
		return *this;

	while(size>MAX_USHORT) {
		if(chunk_is_full())
			expand();

		append_here->item.ptr=src;
		append_here->item.size=MAX_USHORT;
		append_here->item.lang=lang;
#ifndef NO_STRING_ORIGIN
		append_here->item.origin.file=file;
		append_here->item.origin.line=line;
#endif
		append_here++;

		src+=MAX_USHORT;
		size-=MAX_USHORT;
	}

	if(chunk_is_full())
		expand();

	append_here->item.ptr=src;
	append_here->item.size=size;
	append_here->item.lang=lang;
#ifndef NO_STRING_ORIGIN
	append_here->item.origin.file=file;
	append_here->item.origin.line=line;
#endif
	append_here++;

	return *this;
}

char String::first_char() const {
	if(!used_rows())
		throw Exception(0, 0,
			this,
			"getting first char of empty string");

	return *head.rows[0].item.ptr;
}

uint String::hash_code() const {
	uint result=0;
	STRING_FOREACH_ROW(
			result=Hash::generic_code(result, row->item.ptr, row->item.size);
	);
break2:
	return result;
}

/// @todo move 'lang' skipping to pos
int String::cmp(int& partial, const String& src, 
				size_t this_offset, Untaint_lang lang) const {
	partial=-1;
	size_t a_size=size();
	this_offset=min(this_offset, a_size-1);

	const Chunk *a_chunk=&head;
	const Chunk *b_chunk=&src.head;
	const Chunk::Row *a_row=a_chunk->rows;
	const Chunk::Row *b_row=b_chunk->rows;
	size_t a_offset=this_offset;
	size_t b_offset=0;
	Chunk::Row *a_end=append_here;
	Chunk::Row *b_end=src.append_here;
	uint a_countdown=a_chunk->count;
	uint b_countdown=b_chunk->count;
	int result;
	size_t pos=0; 

	bool a_break=a_size==0;
	bool b_break=src.size()==0;
	if(!(a_break || b_break)) while(true) {
		if(pos+a_row->item.size > this_offset) {
			if(lang!=UL_UNSPECIFIED && a_row->item.lang!=lang) 
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
			if(b_break=b_row==b_end) {
				a_break=a_row==a_end;
				break;			
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

		if(a_break=a_row==a_end) {
			b_break=b_row==b_end;
			break;
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
	size_t a_size=size();
	size_t b_size=src_size?src_size:b_ptr?strlen(b_ptr):0;
	this_offset=min(this_offset, a_size-1);

	const Chunk *a_chunk=&head;
	const Chunk::Row *a_row=a_chunk->rows;
	size_t a_offset=this_offset;
	size_t b_offset=0;
	Chunk::Row *a_end=append_here;
	uint a_countdown=a_chunk->count;
	size_t pos=0;

	bool a_break=a_size==0;
	bool b_break=b_size==0;
	if(!(a_break || b_break)) while(true) {
		if(pos+a_row->item.size > this_offset) {
			if(lang!=UL_UNSPECIFIED && a_row->item.lang!=lang) 
				return -1; // wrong lang -- bail out

			int size_diff=
				(a_row->item.size-a_offset)-
				(b_size-b_offset);
			
			if(size_diff==0) { // a has same size as b
				if(int result=memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, 
					a_row->item.size-a_offset)!=0)
					return result;
				pos+=a_row->item.size;
				a_row++; a_countdown--; a_offset=0;
				b_break=true;
			} else if (size_diff>0) { // a longer
				if(int result=memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, 
					b_size-b_offset)!=0)
					return result;
				a_offset+=b_size-b_offset;
				b_break=true;
			} else { // b longer
				if(int result=memcmp(a_row->item.ptr+a_offset, b_ptr+b_offset, 
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

		a_break=a_row==a_end;
		if(a_break || b_break)
			break;
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
	if(!used_rows()) {
		static const Origin empty_origin={"empty string"};
		return empty_origin;
	}
	
	// determining origin by last appended piece
	// because first one frequently constant. 
	// ex: ^load[/file] "document_root" + "/file"
	// when last peice is constant, 
	// ex: parser_root_auto_path{dynamic} / auto.p{const}
	// using first piece
	Origin& last_origin=append_here[-1].item.origin;
	return last_origin.file ? last_origin : head.rows[0].item.origin;
}
#endif

String& String::mid(size_t start, size_t finish) const {
	String& result=*NEW String(pool());

	start=max(0, start);
	finish=min(size(), finish);
	if(start==finish)
		return result;

	size_t pos=0;
	STRING_FOREACH_ROW(
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
		pos+=row->item.size;
	);
break2:
//	SAPI::log(pool(), "piece of '%s' from %d to %d is '%s'",
		//cstr(), start, finish, result.cstr());
	return result;
}

int String::pos(const String& substr, 
				int result, Untaint_lang lang) const {
	size_t self_size=size();
	for(; result<self_size; result++) {
		int partial; cmp(partial, substr, result, lang);
		if(
			partial==0 || // full match
			partial==2) // 'substr' starts 'this'+'result'
			return result;
	}
	
	return -1;
}

int String::pos(const char *substr, size_t substr_size, 
				int result, Untaint_lang lang) const {
	size_t self_size=size();
	for(; result<self_size; result++) {
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
	size_t self_size=size();
	if(delim_size) {
		size_t pos_after=pos_after_ref?*pos_after_ref:0;
		int pos_before;
		// while we have 'delim'...
		for(; (pos_before=pos(delim, delim_size, pos_after, lang))>=0 && limit; limit--) {
			result+=&mid(pos_after, pos_before);
			pos_after=pos_before+delim_size;
		}
		// last piece
		if(pos_after<self_size && limit) {
			result+=&mid(pos_after, self_size);
			pos_after=self_size;
		}
		if(pos_after_ref)
			*pos_after_ref=pos_after;
	} else { // empty delim
		result+=this;
		if(pos_after_ref)
			*pos_after_ref+=self_size;
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
			result+=&mid(pos_after, pos_before);
			pos_after=pos_before+delim.size();
		}
		// last piece
		if(pos_after<size() && limit) {
			result+=&mid(pos_after, size());
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

static void regex_options(char *options, int *result){
    struct Regex_option {
		char key;
		int clear, set;
		int *result;
    } regex_option[]={
		{'i', 0, PCRE_CASELESS, result}, // a=A
		{'s', 0, PCRE_DOTALL, result}, // \n\n$ [default]
		{'x', 0, PCRE_EXTENDED, result}, // whitespace in regex ignored
		{'m', PCRE_DOTALL, PCRE_MULTILINE, result}, // ^aaa\n$^bbb\n$
		{'g', 0, true, result+1}, // many rows
		{0},
    };
	result[0]=PCRE_EXTRA | PCRE_DOTALL;
	result[1]=0;

    if(options) 
		for(Regex_option *o=regex_option; o->key; o++) 
			if(
				strchr(options, o->key) || 
				strchr(options, toupper(o->key))) {
				*(o->result)&=~o->clear;
				*(o->result)|=o->set;
			}
}

/// @todo maybe need speedup: some option to remove pre/match/post string generation
bool String::match(const unsigned char *pcre_tables,
				   const String *aorigin,
				   const String& regexp, 
				   const String *options,
				   Table **table,
				   Row_action row_action, void *info,
				   bool *was_global) const { 

	if(!regexp.size())
		throw Exception(0, 0,
			aorigin,
			"regexp is empty");
	const char *pattern=regexp.cstr();
	const char *errptr;
	int erroffset;
    int option_bits[2];  regex_options(options?options->cstr():0, option_bits);
	if(was_global)
		*was_global=option_bits[1]!=0;
	pcre *code=pcre_compile(pattern, option_bits[0], 
		&errptr, &erroffset,
		pcre_tables);

	if(!code)
		throw Exception(0, 0,
			&regexp.mid(erroffset, regexp.size()),
			"regular expression syntax error - %s", errptr);
	
	int info_substrings=pcre_info(code, 0, 0);
	if(info_substrings<0) {
		pcre_free(code);
		throw Exception(0, 0,
			aorigin,
			"pcre_info error (%d)", 
				info_substrings);
	}

	int startoffset=0;
	const char *subject=cstr();
	int length=strlen(subject);
	int ovecsize;
	int *ovector=(int *)malloc(sizeof(int)*
		(ovecsize=(1/*match*/+info_substrings)*3), 11);

	{ // create table
		Array& columns=*NEW Array(pool());
		columns+=string_pre_match_name;
		columns+=string_match_name;
		columns+=string_post_match_name;
		for(int i=1; i<=info_substrings; i++) {
			char *column=(char *)malloc(MAX_NUMBER);
			snprintf(column, MAX_NUMBER, "%d", i);
			columns+=NEW String(pool(), column); // .i column name
		}
		*table=NEW Table(pool(), aorigin, &columns);
	}

	int exec_option_bits=0;
	while(true) {
		int exec_substrings=pcre_exec(code, 0,
			subject, length, startoffset,
			exec_option_bits, ovector, ovecsize);
		
		if(exec_substrings==PCRE_ERROR_NOMATCH) {
			pcre_free(code);
			row_action(**table, 0/*last time, no row*/, 0, 0, info);
			return option_bits[1]!=0; // global=true+table, not global=false
		}

		if(exec_substrings<0) {
			pcre_free(code);
			throw Exception(0, 0,
				aorigin,
				"regular expression execute error (%d)", 
					exec_substrings);
		}

		Array& row=*NEW Array(pool());
		row+=&mid(0, ovector[0]); // .prematch column value
		row+=&mid(ovector[0], ovector[1]); // .match
		row+=&mid(ovector[1], size()); // .postmatch
		
		for(int i=1; i<exec_substrings; i++) {
			// -1:-1 case handled peacefully by mid() itself
			row+=&mid(ovector[i*2+0], ovector[i*2+1]); // .i column value
		}
		
		row_action(**table, &row, startoffset, ovector[0], info);

		if(!option_bits[1] || startoffset==ovector[1]) { // not global | going to hang
			pcre_free(code);
			row_action(**table, 0/*last time, no row*/, 0, 0, info);
			return true;
		}
		startoffset=ovector[1];

/*
		if(option_bits[0] & PCRE_MULTILINE)
			exec_option_bits|=PCRE_NOTBOL; // start of subject+startoffset not BOL
*/
	}
}

String& String::change_case(Pool& pool, const unsigned char *tables, 
							Change_case_kind kind) const {
	String& result=*new(pool) String(pool);

	const unsigned char *a;
	const unsigned char *b;
	switch(kind) {
	case CC_UPPER:
		a=tables+lcc_offset;
		b=tables+fcc_offset;
		break;
	case CC_LOWER:
		a=tables+lcc_offset;
		b=0;
		break;
	default:
		throw Exception(0, 0, 
			this, 
			"unknown change case kind #%d", 
				static_cast<int>(kind)); // never
		a=b=0; // calm, compiler
		break; // never
	}	

	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(uint i=0; i<chunk->count; i++, row++) {
			if(row==append_here)
				goto break2;

			char *new_cstr=(char *)pool.malloc(row->item.size, 12);
			char *dest=new_cstr;
			const char *src=row->item.ptr; 
			for(int size=row->item.size; size--; src++) {
				unsigned char c=a[(unsigned char)*src];
				if(b)
					c=b[c];

				*dest++=(char)c;
			}
			
			result.APPEND(new_cstr, row->item.size, 
				row->item.lang,
				row->item.origin.file, row->item.origin.line);
		}
		chunk=row->link;
	} while(chunk);
break2:

	return result;
}

void String::join_chain(Pool& pool, 
					   uint& ai, const Chunk*& achunk, const Chunk::Row*& arow,
					   uchar& joined_lang, const char *& joined_ptr, size_t& joined_size) const {
	joined_lang=arow->item.lang;
	
	// calc size
	joined_size=0;
	{
		uint start_i=ai;
		const Chunk::Row *start_row=arow;
		const Chunk *chunk=achunk;
		do {
			const Chunk::Row *row=start_row;
			for(uint i=start_i; i<chunk->count; i++, row++) {
				if(row==append_here)
					goto break21;
				
				if(row->item.lang==joined_lang)
					joined_size+=row->item.size;
				else
					goto break21;
			}
			if(chunk=row->link) {
				start_i=0;
				start_row=chunk->rows;
			} else
				break;
		} while(true);
break21:;
	}

	// if one row, return simply itself
	if(joined_size==arow->item.size) {
		joined_ptr=arow->item.ptr;
		ai++; arow++;
		if(ai==achunk->count)
			achunk=arow->link;		
	} else {
		// join adjacent rows
		char *ptr=(char *)pool.malloc(joined_size,13);
		joined_ptr=ptr;
		uint start_i=ai;
		const Chunk::Row *start_row=arow;
		const Chunk *chunk=achunk;
		uint i;
		const Chunk::Row *row;
		do {
			row=start_row;
			for(i=start_i; i<chunk->count; i++, row++) {
				if(row==append_here)
					goto break22;
				
				if(row->item.lang==joined_lang) {
					memcpy(ptr, row->item.ptr, row->item.size);
					ptr+=row->item.size;
				} else
					goto break22;
			}
			if(chunk=row->link) {
				start_i=0;
				start_row=chunk->rows;
			} else
				break;
		} while(true);
break22:;
		
		// return joined rows
		ai=i;
		arow=row;
		achunk=chunk;
	}
}

String& String::reconstruct(Pool& pool) const {
	//_asm int 3;
	String& result=*new(pool) String(pool);
	const Chunk *chunk=&head; 
	while(true) {
		const Chunk::Row *row=chunk->rows;
		for(uint i=0; i<chunk->count; ) {
			if(row==append_here)
				goto break2;

			uchar joined_lang;
			const char *joined_ptr;
			size_t joined_size;
#ifndef NO_STRING_ORIGIN
			const char *joined_origin_file=row->item.origin.file;
			const size_t joined_origin_line=row->item.origin.line;
#endif
			join_chain(pool, i, chunk, row,
				joined_lang, joined_ptr, joined_size);

			result.APPEND(joined_ptr, joined_size, joined_lang,
				joined_origin_file, joined_origin_line);
			if(!chunk)
				goto break2;
		}
	}
break2:

	return result;
};

String& String::replace_in_reconstructed(Pool& pool, Dictionary& dict) const {
	//_asm int 3;
	String& result=*new(pool) String(pool);
	STRING_FOREACH_ROW(
		const char *src=row->item.ptr; 
		size_t src_size=row->item.size;
		char *new_cstr=(char *)pool.malloc((size_t)ceil(src_size*dict.max_ratio()), 14);
		char *dest=new_cstr;
		while(src_size) {
			// there is a row where first column starts 'src'
			if(Table::Item *item=dict.first_that_starts(src, src_size)) {
				// get a=>b values
				const String& a=*static_cast<Array *>(item)->get_string(0);
				const String& b=*static_cast<Array *>(item)->get_string(1);
				// skip 'a' in 'src' && reduce work size
				src+=a.size();  src_size-=a.size();
				// write 'b' to 'dest' && skip 'b' in 'dest'
				b.store_to(dest);  dest+=b.size();
			} else {
				// write a char to b && reduce work size
				*dest++=*src++;  src_size--;
			}
		}

		result.APPEND(new_cstr, dest-new_cstr, 
			row->item.lang,
			row->item.origin.file, row->item.origin.line);
	);
break2:
	return result;
}

String& String::replace(Pool& pool, Dictionary& dict) const {
	return reconstruct(pool).replace_in_reconstructed(pool, dict);
}

double String::as_double() const { 
	double result;
	const char *cstr;
	char buf[MAX_NUMBER];
	if(head.rows+1==append_here) {
		int size=min(head.rows[0].item.size, MAX_NUMBER-1);
		memcpy(buf, head.rows[0].item.ptr, size);
		buf[size]=0;
		cstr=buf;
	} else
		cstr=this->cstr();
	char *error_pos;
	// 0xABC
	if(cstr[0]=='0')
		if(cstr[1]=='x' || cstr[1]=='X')
			result=(double)(unsigned long)strtol(cstr, &error_pos, 0);
		else
			result=(double)strtod(cstr+1/*skip leading 0*/, &error_pos);
	else
		result=(double)strtod(cstr, &error_pos);

	if(*error_pos/*not EOS*/)
		throw Exception(0, 0,
			this,
			"invalid number (double)");

	return result;
}
int String::as_int() const { 
	int result;
	const char *cstr;
	char buf[MAX_NUMBER];
	if(head.rows+1==append_here) {
		int size=min(head.rows[0].item.size, MAX_NUMBER-1);
		memcpy(buf, head.rows[0].item.ptr, size);
		buf[size]=0;
		cstr=buf;
	} else
		cstr=this->cstr();
	char *error_pos;
	// 0xABC
	if(cstr[0]=='0')
		if(cstr[1]=='x' || cstr[1]=='X')
			result=(int)(unsigned long)strtol(cstr, &error_pos, 0);
		else
			result=(int)strtol(cstr+1/*skip leading 0*/, &error_pos, 0);
	else
		result=(int)strtol(cstr, &error_pos, 0);

	if(*error_pos/*not EOS*/)
		throw Exception(0, 0,
			this,
			"invalid number (int)");

	return result;
}

inline void ushort2uchars(ushort word, uchar& byte1, uchar& byte2) {
	byte1=word&0xFF;
	byte2=word>>8;
}
inline ushort uchars2ushort(uchar byte1, uchar byte2) {
	return (byte2<<8) | byte1;
}
/* @todo maybe network order worth spending some effort?
	don't bothering myself with network byte order,
	am not planning to be able to move resulting file across platforms
	for now
*/
void String::serialize(size_t prolog_size, void *& buf, size_t& buf_size) const {
	buf_size=
		prolog_size
		+used_rows()*(sizeof(uchar)+sizeof(ushort))
		+size();
	buf=malloc(buf_size,15);
	char *cur=(char *)buf+prolog_size;

	STRING_FOREACH_ROW(
		// lang
		memcpy(cur, &row->item.lang, sizeof(uchar));
		cur+=sizeof(uchar);
		// size
		uchar byte1; uchar byte2;
		ushort2uchars(row->item.size, byte1, byte2);
		memcpy(cur, &byte1, sizeof(uchar)); cur+=sizeof(uchar);
		memcpy(cur, &byte2, sizeof(uchar)); cur+=sizeof(uchar);
		// bytes
		memcpy(cur, row->item.ptr, row->item.size);
		cur+=row->item.size;
	);
break2:
	;
}
void String::deserialize(size_t prolog_size, void *buf, size_t buf_size, const char *file) {
	char *cur=(char *)buf+prolog_size;
	buf_size-=prolog_size;

	while(buf_size) {
		uchar lang=*(uchar *)(cur);
		
		ushort size=uchars2ushort(
			*(uchar*)(cur+sizeof(uchar)*1),
			*(uchar*)(cur+sizeof(uchar)*2)
		);

		const char *ptr=(const char*)(cur+sizeof(uchar)*3); 
		APPEND(ptr, size, lang, file, 0);

		size_t piece_size=sizeof(uchar)+sizeof(ushort)+size;
		cur+=piece_size;
		buf_size-=piece_size;
	}
}
