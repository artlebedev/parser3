/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: untaint.C,v 1.2 2001/03/12 21:54:20 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"

#define escape(cases) \
	{ \
		const char *ptr=row->item.ptr; \
		int size=row->item.size; \
		for (;*ptr;ptr++) \
			switch(*ptr) { \
				cases \
				default: *copy_here++=*ptr; break; \
			} \
	}
#define escape_value(a, c)  case a: *copy_here++=c; break;
#define escape_subst(a, b, bsize)  \
	case a: \
		{ \
			strncpy(copy_here, b, bsize); \
			copy_here+=bsize; \
		} \
		break;

// String

char *String::cstr() const {
	char *result=(char *)malloc(size()*UNTAINT_TIMES_BIGGER+1);

	char *copy_here=result;
	const Chunk *chunk=&head; 
	// TODO: оптимизировать whitespaces для всех, кроме 'html'
	do {
		const Chunk::Row *row=chunk->rows;
		for(int i=0; i<chunk->count; i++) {
			if(row==append_here)
				goto break2;

			// WARNING:
			//	string can grow only UNTAINT_TIMES_BIGGER
			switch(row->item.lang) {
			case NO:
				// clean piece
			case YES:
				// tainted piece, but undefined untaint language
				// for VString.get_double of tainted values
				// for ^process{body} evaluation
			case AS_IS:
				// tainted, untaint language: as-is
				memcpy(copy_here, row->item.ptr, row->item.size); 
				copy_here+=row->item.size;
				break;
			case TABLE: 
				escape(
					escape_value('\t', ' ')
					escape_value('\n', ' ')
				);
				break;
			case SQL:
				// tainted, untaint language: sql
				// TODO: зависимость от sql сервера
				memset(copy_here, '?', row->item.size); 
				copy_here+=row->item.size;
				break;
			case JS:
				escape(
					escape_subst('"', "\\\"", 2)
					escape_subst('\'', "\\'", 2)
					escape_subst('\n', "\\n", 2)
					escape_subst('\r', "\\r", 2)
					escape_subst('\\', "\\\\", 2)
					escape_subst('я', "\\я", 2)
				);
				break;
			case HTML:
				escape(
					escape_subst('&', "&amp;", 5) // BEFORE consequent relpaces yelding '&'
					escape_subst('>', "&gt;", 4)
					escape_subst('<', "&lt;",4)
					escape_subst('"', "&quot;",6)
					escape_value('\t', ' ')
					//TODO: XSLT escape_subst('\'', "&apos;", 6)
				);
				break;
			case HTML_TYPO: 
				// tainted, untaint language: html-typo
				escape(
					escape_subst('&', "&amp;", 5) // BEFORE consequent relpaces yelding '&'
					escape_subst('>', "&gt;", 4)
					escape_subst('<', "&lt;",4)
					escape_subst('"', "&quot;",6)
					escape_value('\t', ' ')
					//TODO: $MAIN:html-type table replace, max length(b)==UNTAINT_TIMES_BIGGER*length(a)
				);
				break;
			default:
				THROW(0,0,
					this,
					"unknown untaint language #%d of %d piece", 
						static_cast<int>(row->item.lang),
						i);
			}
			row++;
		}
		chunk=row->link;
	} while(chunk);
break2:
	*copy_here=0;
	return result;
}
