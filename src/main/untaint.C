/** @file
	Parser: String class part: untaint mechanizm.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: untaint.C,v 1.15 2001/03/25 08:52:37 paf Exp $
*/

#include "pa_config_includes.h"

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_table.h"

#define escape(cases) \
	{ \
		const char *src=row->item.ptr; \
		for(int size=row->item.size; size--; src++) \
			switch(*src) { \
				cases \
			} \
	}
#define to_char(a, c)  case a: *dest++=c; break
#define _default  default: *dest++=*src; break
#define to_string(a, b, bsize)  \
		case a: \
			strncpy(dest, b, bsize); \
			dest+=bsize; \
		break
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
    if(strchr(" ,:", c))
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
	Table *typo_table=static_cast<Table *>(pool().tag());

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
				escape(
					to_char(' ', '_');
					encode(need_file_encode, '-');
				);
				break;
			case UL_URI:
				// tainted, untaint language: uri
				escape(
					to_char(' ', '+');
					encode(need_uri_encode, '%');
				);
				break;
			case UL_HEADER:
				// tainted, untaint language: header
				escape(
					encode(need_header_encode, '%');
				);
				break;
			case UL_TABLE: 
				// tainted, untaint language: table
				escape(
					to_char('\t', ' ');
					to_char('\n', ' ');
					_default;
				);
				break;
			case UL_SQL:
				// tainted, untaint language: sql
				// TODO: зависимость от sql сервера
				memset(dest, '?', row->item.size); 
				dest+=row->item.size;
				break;
			case UL_JS:
				escape(
					to_string('"', "\\\"", 2);
					to_string('\'', "\\'", 2);
					to_string('\n', "\\n", 2);
					to_string('\\', "\\\\", 2);
					to_string('\xFF', "\\\xFF", 2);
					_default;
				);
				break;
			case UL_HTML:
				escape(
					to_string('&', "&amp;", 5);
					to_string('>', "&gt;", 4);
					to_string('<', "&lt;",4);
					to_string('"', "&quot;",6);
					to_char('\t', ' ');
					//TODO: XSLT to_string('\'', "&apos;", 6)
					_default;
				);
				break;
			case UL_HTML_TYPO: {
				// tainted, untaint language: html-typo
				char *html=(char *)malloc(size()*6/*"&quot;" the longest possible*/+1);
				size_t html_size;
				{ // local dest
					char *dest=html;
					escape(
						to_string('&', "&amp;", 5);
						to_string('>', "&gt;", 4);
						to_string('<', "&lt;",4);
						to_string('"', "&quot;",6);
						to_char('\t', ' ');
						to_string('\r', "", 0); // todo: check  mac & linux browsers' textarea
						to_string('\n', "\\n", 2);  // convinient name for typo match
						//TODO: XSLT to_string('\'', "&apos;", 6)
						_default;
					);
					*dest=0;
					html_size=dest-html;
				}
				// typo table replacements
				if(typo_table) {
					const char *src=html;
					do {
						// there is a row where first column starts 'src'
						if(Table::Item *item=typo_table->first_that(typo_present, src)) {
							// get a=>b values
							const String& a=*static_cast<Array *>(item)->get_string(0);
							const String& b=*static_cast<Array *>(item)->get_string(1);
							// empty 'a' check
							if(a.size()==0) {
								pool().set_tag(0); // avoid recursion
								THROW(0, 0,
									&a,
									"typo table first column elements must not be empty");
							}
							// overflow check:
							//   b allowed to be max UNTAINT_TIMES_BIGGER then a
							if(b.size()>UNTAINT_TIMES_BIGGER*a.size()) {
								pool().set_tag(0); // avoid recursion
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
				} else {
					memcpy(dest, html, html_size);
					dest+=html_size;
				}
				break;
				}
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
	return dest;
}
