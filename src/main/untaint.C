/** @file
	Parser: String class part: untaint mechanizm.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: untaint.C,v 1.10 2001/03/20 06:45:19 paf Exp $
*/

#include <string.h>

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"

#define escape(cases) \
	{ \
		const char *ptr=row->item.ptr; \
		for (int size=row->item.size; size--; ptr++) \
			switch(*ptr) { \
				cases \
			} \
	}
#define escape_value(a, c)  case a: *copy_here++=c; break
#define escape_default  default: *copy_here++=*ptr; break
#define escape_subst(a, b, bsize)  \
		case a: \
			strncpy(copy_here, b, bsize); \
			copy_here+=bsize; \
		break
#define escape_encode(need_encode_func, prefix)  \
		default: \
			if(need_encode_func(*ptr)) { \
				static const char *hex="0123456789ABCDEF"; \
				char chunk[3]={prefix}; \
				chunk[1]=hex[((unsigned char)*ptr)/0x10]; \
				chunk[2]=hex[((unsigned char)*ptr)%0x10]; \
				strncpy(copy_here, chunk, 3);  copy_here+=3; \
			} else \
				*copy_here++=*ptr; \
			break

inline bool need_file_encode(unsigned char c){
    if ((c>='0') && (c<='9') || (c>='A') && (c<='Z') || (c>='a') && (c<='z')) 
		return false;

    return !strchr("./\\", c);
}
inline bool need_uri_encode(unsigned char c){
    if ((c>='0') && (c<='9') || (c>='A') && (c<='Z') || (c>='a') && (c<='z')) 
		return false;

    return !strchr("_-./", c);
}
inline bool need_header_encode(unsigned char c){
    if(strchr(" ,:", c))
		return false;

	return need_uri_encode(c);
}

// String

/// @todo optimize whitespaces for all but 'html'
char *String::cstr() const {
	char *result=(char *)malloc(size()*UNTAINT_TIMES_BIGGER+1);

	char *copy_here=result;
	const Chunk *chunk=&head; 
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
			case FILE_NAME:
				// tainted, untaint language: file [name]
				escape(
					escape_value(' ', '_');
					escape_encode(need_file_encode, '-');
				);
				break;
			case URI:
				// tainted, untaint language: uri
				escape(
					escape_value(' ', '+');
					escape_encode(need_uri_encode, '%');
				);
				break;
			case HEADER:
				// tainted, untaint language: header
				escape(
					escape_encode(need_header_encode, '%');
				);
				break;
			case TABLE: 
				escape(
					escape_value('\t', ' ');
					escape_value('\n', ' ');
					escape_default;
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
					escape_subst('"', "\\\"", 2);
					escape_subst('\'', "\\'", 2);
					escape_subst('\n', "\\n", 2);
					escape_subst('\r', "\\r", 2);
					escape_subst('\\', "\\\\", 2);
					escape_subst('я', "\\я", 2);
					escape_default;
				);
				break;
			case HTML:
				escape(
					escape_subst('&', "&amp;", 5); // BEFORE consequent relpaces yelding '&'
					escape_subst('>', "&gt;", 4);
					escape_subst('<', "&lt;",4);
					escape_subst('"', "&quot;",6);
					escape_value('\t', ' ');
					//TODO: XSLT escape_subst('\'', "&apos;", 6)
					escape_default;
				);
				break;
			case HTML_TYPO: 
				// tainted, untaint language: html-typo
				escape(
					escape_subst('&', "&amp;", 5); // BEFORE consequent relpaces yelding '&'
					escape_subst('>', "&gt;", 4);
					escape_subst('<', "&lt;",4);
					escape_subst('"', "&quot;",6);
					escape_value('\t', ' ');
					//TODO: $MAIN:html-type table replace, max length(b)==UNTAINT_TIMES_BIGGER*length(a)
					escape_default;
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
