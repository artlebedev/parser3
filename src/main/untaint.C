/** @file
	Parser: String class part: untaint mechanizm.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: untaint.C,v 1.21 2001/03/25 10:30:43 paf Exp $
*/

#include "pa_config_includes.h"

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_table.h"

#define escape(action) \
	{ \
		const char *src=row->item.ptr; \
		for(int size=row->item.size; size--; src++) \
			action \
	}
#define _default  default: *dest++=*src; break
#define encode(need_encode_func, prefix)  \
		default: \
			if(need_encode_func(*src)) { \
				static const char *hex="0123456789ABCDEF"; \
				char chunk[3]={prefix}; \
				chunk[1]=hex[((unsigned char)*src)/0x10]; \
				chunk[2]=hex[((unsigned char)*src)%0x10]; \
				strncpy(dest, chunk, 3);  dest+=3; \
			} else \
				*dest++=*src; \
			break
#define to_char(c)  *dest++=c
#define to_string(b, bsize)  \
		strncpy(dest, b, bsize); \
		dest+=bsize; \

inline bool need_file_encode(unsigned char c){
    if((c>='0') &&(c<='9') ||(c>='A') &&(c<='Z') ||(c>='a') &&(c<='z')) 
		return false;

    return !strchr("./", c);
}
inline bool need_uri_encode(unsigned char c){
    if((c>='0') &&(c<='9') ||(c>='A') &&(c<='Z') ||(c>='a') &&(c<='z')) 
		return false;

    return !strchr("_-./", c);
}
inline bool need_header_encode(unsigned char c){
    if(strchr(" , :", c))
		return false;

	return need_uri_encode(c);
}

// String

static bool typo_present(Array::Item *value, const void *info) {
	Array *row=static_cast<Array *>(value);
	const char *src=static_cast<const char *>(info);

	int partial;
	row->get_string(0)->cmp(src, partial);
	return 
		partial==0 || // full match
		partial==1; // typo left column starts 'src'
}

/// @todo optimize whitespaces for all but 'html'
char *String::store_to(char *dest) const {
	// $MAIN:html-typo table
	Table *user_typo_table=static_cast<Table *>(pool().tag());
	Table *typo_table=user_typo_table?user_typo_table:default_typo_table;

	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(int i=0; i<chunk->count; i++) {
			if(row==append_here)
				goto break2;

			// WARNING:
			//	string can grow only UNTAINT_TIMES_BIGGER
			switch(row->item.lang) {
			case UL_NO:
				// clean piece
			case UL_YES:
				// tainted piece, but undefined untaint language
				// for VString.get_double of tainted values
				// for ^process{body} evaluation
			case UL_AS_IS:
				// tainted, untaint language: as-is
				memcpy(dest, row->item.ptr, row->item.size); 
				dest+=row->item.size;
				break;
			case UL_FILE_NAME:
				// tainted, untaint language: file [name]
				escape(switch(*src) {
					case ' ': to_char('_');  break;
					encode(need_file_encode, '-');
				});
				break;
			case UL_URI:
				// tainted, untaint language: uri
				escape(switch(*src) {
					case ' ': to_char('+');  break;
					encode(need_uri_encode, '%');
				});
				break;
			case UL_HEADER:
				// tainted, untaint language: header
				escape(switch(*src) {
					encode(need_header_encode, '%');
				});
				break;
			case UL_TABLE: 
				// tainted, untaint language: table
				escape(switch(*src) {
					case '\t': to_char(' ');  break;
					case '\n': to_char(' ');  break;
					_default;
				});
				break;
			case UL_SQL:
				// tainted, untaint language: sql
				// TODO: зависимость от sql сервера
				memset(dest, '?', row->item.size); 
				dest+=row->item.size;
				break;
			case UL_JS:
				escape(switch(*src) {
					case '"': to_string("\\\"", 2);  break;
					case '\'': to_string("\\'", 2);  break;
					case '\n': to_string("\\n", 2);  break;
					case '\\': to_string("\\\\", 2);  break;
					case '\xFF': to_string("\\\xFF", 2);  break;
					_default;
				});
				break;
			case UL_HTML:
				escape(switch(*src) {
					case '&': to_string("&amp;", 5);  break;
					case '>': to_string("&gt;", 4);  break;
					case '<': to_string("&lt;", 4);  break;
					case '"': to_string("&quot;", 6);  break;
					//TODO: XSLT case '\'': to_string("&apos;", 6);  break;
					_default;
				});
				break;
			case UL_HTML_TYPO: {
				// tainted, untaint language: html-typo
				char *html_for_typo=(char *)malloc(size()*2/* '\n' -> '\' 'n' */+1);
				// note:
				//   there still is a possibility that user 
				//   would not replace \n as she supposed to
				//   and rather replace \ and n into huge strings
				//   thus causing memory overrun
				//   this can be dealed by allocating *2 memory, but that's too expensive
				// todo parser4: fix that
				size_t html_for_typo_size;
				{ // local dest
					char *dest=html_for_typo;
					escape(switch(*src) {
						// convinient name for typo match "\n"
						case '\r': 
							if(typo_table) {
								*dest++='\\';  *dest++='n'; // \r -> \n
								if(src[1]=='\n') // \r\n -> remove \n
									src++;
							}
							break;
						case '\n': 
							if(typo_table)
								to_string("\\n", 2);
							break;
						//TODO: XSLT case '\'': to_string("&apos;", 6);  break;
						_default;
					});
					*dest=0;
					html_for_typo_size=dest-html_for_typo;
				}
				// typo table replacements
				const char *src=html_for_typo;
				do {
					// there is a row where first column starts 'src'
					if(Table::Item *item=typo_table->first_that(typo_present, src)) {
						// get a=>b values
						const String& a=*static_cast<Array *>(item)->get_string(0);
						const String& b=*static_cast<Array *>(item)->get_string(1);
						// empty 'a' | 'b' checks
						if(a.size()==0 || b.size()==0) {
							pool().set_tag(default_typo_table); // avoid recursion
							THROW(0, 0, 
								typo_table->origin_string(), 
								"typo table column elements must not be empty");
						}
						// overflow check:
						//   b allowed to be max UNTAINT_TIMES_BIGGER then a
						if(b.size()>UNTAINT_TIMES_BIGGER*a.size()) {
							pool().set_tag(default_typo_table); // avoid recursion
							THROW(0, 0, 
								&b, 
								"is %g times longer then '%s', "
								"while maximum, handled by Parser, is %d", 
								((double)b.size())/a.size(), 
								a.cstr(), 
								UNTAINT_TIMES_BIGGER);
						}
						
						// skip 'a' in 'src'
						src+=a.size();
						// write 'b' to 'dest'
						b.store_to(dest);
						dest+=b.size();
					} else
						*dest++=*src++;
				} while(*src);
				break;
				}
			default:
				THROW(0, 0, 
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
	return dest;
}
