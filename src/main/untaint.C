/** @file
	Parser: String class part: untaint mechanizm.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: untaint.C,v 1.76 2001/11/16 13:51:14 paf Exp $
*/

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_table.h"
#include "pa_globals.h"
#include "pa_sql_connection.h"
#include "pa_dictionary.h"
#include "pa_common.h"

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
				memcpy(dest, chunk, 3);  dest+=3; \
			} else \
				*dest++=*src; \
			break
#define to_char(c)  *dest++=c
#define to_string(b, bsize)  \
		memcpy(dest, b, bsize); \
		dest+=bsize; \

inline bool need_file_encode(unsigned char c){
    if((c>='0') &&(c<='9') ||(c>='A') &&(c<='Z') ||(c>='a') &&(c<='z')) 
		return false;

    return !strchr(
#ifdef WIN32
		":\\~"
#endif
		"./()_-", c);
}
inline bool need_uri_encode(unsigned char c){
    if((c>='0') &&(c<='9') ||(c>='A') &&(c<='Z') ||(c>='a') &&(c<='z')) 
		return false;

    return !strchr("_-./", c);
}
inline bool need_http_header_encode(unsigned char c){
    if(strchr(" , :", c))
		return false;

	return need_uri_encode(c);
}

//

static const char * String_Untaint_lang_name[]={
	"U", ///< zero value handy for hash lookup @see untaint_lang_name2enum
	"C", ///< clean
	"T",  ///< tainted, untaint language as assigned later 
	// untaint languages. assigned by ^untaint[lang]{...}
	"P",
		/**<
			leave language built into string being appended.
			just a flag, that value not stored
		*/
	"A",     ///< leave all characters intact
	"F", ///< file specification
	"H",    ///< ext in HTTP response header
	"M",    ///< text in mail header
	"URI",       ///< text in uri
	"T",     ///< ^table:set body
	"SQL",       ///< ^table:sql body
	"JS",        ///< JavaScript code
	"XML",		///< ^dom:set xml
	"HTML",      ///< HTML code (for editing)
	"UHTML", ///< HTML code with USER chars
};


// String

/*

HTTP-header    = field-name ":" [ field-value ] CRLF

       field-name     = token
       field-value    = *( field-content | LWS )

       field-content  = <the OCTETs making up the field-value
                        and consisting of either *TEXT or combinations
                        of token, tspecials, and quoted-string>



word           = token | quoted-string

token          = 1*<any CHAR except CTLs or tspecials>



tspecials      = "(" | ")" | "<" | ">" | "@"
                      | "," | ";" | ":" | "\" | <">
                      | "/" | "[" | "]" | "?" | "="
                      | "{" | "}" | SP | HT

SP             = <US-ASCII SP, space (32)>
HT             = <US-ASCII HT, horizontal-tab (9)>

LWS            = [CRLF] 1*( SP | HT )
TEXT           = <any OCTET except CTLs,
                        but including LWS>

quoted-pair    = "\" CHAR

  if(strchr("()<>@,;:\\\"/[]?={} \t", *ptr))
*/
inline bool need_quote_http_header(const char *ptr, size_t size) {
	for(; size--; ptr++)
		if(strchr(";\\\"= \t" /* excluded ()<>@, :/ ? []{} */, *ptr))
			return true;
	return false;
}

/// @test UL_OPTIMIZED_HTML optimize
size_t String::cstr_bufsize(Untaint_lang lang,
							SQL_Connection *connection,
							const char *charset) const {
	size_t dest=1;
	bool whitespace=true;
	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(uint i=0; i<chunk->count; i++, row++) {
			if(row==append_here)
				goto break2;

			Untaint_lang to_lang=lang==UL_UNSPECIFIED?(Untaint_lang)row->item.lang:lang;

			switch(to_lang) {
			case UL_CLEAN:
				// clean piece
				{ // optimizing whitespace
					escape(switch(*src) {
						case ' ': case '\n': case '\t':
							if(!whitespace) {
								dest++;
								whitespace=true;
							}
							break;
						default:
							whitespace=false;
							dest++;
							break;
					});
				}
				break;
			case UL_TAINTED:
				// tainted piece, but undefined untaint language
				// for VString.as_double of tainted values
				// for ^process{body} evaluation
			case UL_AS_IS:
				// tainted, untaint language: as-is
				dest+=row->item.size;
				break;
			case UL_FILE_SPEC:
				// tainted, untaint language: file [name]
				dest+=row->item.size*3/* worst: Z->%XX */;
				break;
			case UL_URI:
				// tainted, untaint language: uri
				dest+=row->item.size*3/* worst: Z->%XX */;
				break;
			case UL_HTTP_HEADER:
				// tainted, untaint language: http-field-content-text
				dest+=row->item.size*3/* worst: Z->%XX */;
				break;
			case UL_MAIL_HEADER:
				// tainted, untaint language: mail-header
				if(charset) {
					// Subject: Re: parser3: =?koi8-r?Q?=D3=C5=CD=C9=CE=C1=D2?=
					dest+=row->item.size*3+MAX_STRING/* worst: =?charset?Q?=%XX?= */;
				} else {
					dest+=row->item.size;
				}
				break;
			case UL_TABLE: 
				// tainted, untaint language: table
				dest+=row->item.size;
				break;
			case UL_SQL:
				// tainted, untaint language: sql
				if(connection)
					dest+=connection->quote(0, row->item.ptr, row->item.size);
				break;
			case UL_JS:
				escape(switch(*src) {
					case '"': case '\'': case '\n': case '\\': case '\xFF':
						dest+=2;  break;
					default: 
						dest++;  break;
				});
				break;
			case UL_XML:
				escape(switch(*src) {
					case '&': case '>': case '<': case '"': case '\'': 
						dest+= 6;  break;
					default: 
						dest++;  break;
				});
				break;
			case UL_HTML:
			case UL_OPTIMIZED_HTML:
				escape(switch(*src) {
					case '&': 
					case '>': 
					case '<': 
					case '"': 
						dest+=6;  break;
					default: 
						dest++;  break;
				});
				break;
			}

			if((lang==UL_UNSPECIFIED?row->item.lang:lang)!=UL_CLEAN)
				whitespace=false;
		}
		chunk=row->link;
	} while(chunk);

break2:
	return dest;
}

/// @test UL_OPTIMIZED_HTML optimize
char *String::store_to(char *dest, Untaint_lang lang, 
					   SQL_Connection *connection,
					   const char *charset) const {
	// WARNING:
	//	 before any changes check cstr_bufsize first!!!
	bool whitespace=true;
	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(uint i=0; i<chunk->count; i++, row++) {
			if(row==append_here)
				goto break2;

			Untaint_lang to_lang=lang==UL_UNSPECIFIED?(Untaint_lang)row->item.lang:lang;

			switch(to_lang) {
			case UL_CLEAN:
				// clean piece
				{ // optimizing whitespace
					escape(switch(*src) {
						case ' ': case '\n': case '\t':
							if(!whitespace) {
								*dest++=*src;
								whitespace=true;
							}
							break;
						default:
							whitespace=false;
							*dest++=*src;
							break;
					});
				}
				break;
			case UL_TAINTED:
				// tainted piece, but undefined untaint language
				// for VString.as_double of tainted values
				// for ^process{body} evaluation
			case UL_AS_IS:
				// tainted, untaint language: as-is
				memcpy(dest, row->item.ptr, row->item.size); 
				dest+=row->item.size;
				break;
			case UL_FILE_SPEC:
				// tainted, untaint language: file [name]
				escape(switch(*src) {
					case ' ': to_char('_');  break;
					encode(need_file_encode, '+');
				});
				break;
			case UL_URI:
				// tainted, untaint language: uri
				escape(switch(*src) {
					case ' ': to_char('+');  break;
					encode(need_uri_encode, '%');
				});
				break;
			case UL_HTTP_HEADER:
				// tainted, untaint language: http-field-content-text
				escape(switch(*src) {
					case ' ': to_char('+');  break;
					encode(need_uri_encode, '%');
				});
				break;
			case UL_MAIL_HEADER:
				// tainted, untaint language: mail-header
				if(charset) {
					// Subject: Re: parser3: =?koi8-r?Q?=D3=C5=CD=C9=CE=C1=D2?=
					const char *src=row->item.ptr; 
					bool to_quoted_printable=false;
					for(int size=row->item.size; size--; src++) {
						if(*src & 0x80) {
							if(!to_quoted_printable) {
								dest+=sprintf(dest, "=?%.15s?Q?", charset);
								to_quoted_printable=true;
							}
							dest+=sprintf(dest, "=%02X", *src & 0xFF);
						} else {
							*dest++=*src;						
						}
					}
					if(to_quoted_printable) // close
						dest+=sprintf(dest, "?=");
				} else {
					memcpy(dest, row->item.ptr, row->item.size); 
					dest+=row->item.size;
				}
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
				if(connection)
					dest+=connection->quote(dest, row->item.ptr, row->item.size);
				else
					throw Exception(0, 0,
						this,
						"untaint in SQL language failed - no connection specified");
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
			case UL_XML:
				escape(switch(*src) {
					case '&': to_string("&amp;", 5);  break;
					case '>': to_string("&gt;", 4);  break;
					case '<': to_string("&lt;", 4);  break;
					case '"': to_string("&quot;", 6);  break;
					case '\'': to_string("&apos;", 6);  break;
					_default;
				});
				break;
			case UL_HTML:
			case UL_OPTIMIZED_HTML:
				escape(switch(*src) {
					case '&': to_string("&amp;", 5);  break;
					case '>': to_string("&gt;", 4);  break;
					case '<': to_string("&lt;", 4);  break;
					case '"': to_string("&quot;", 6);  break;
					_default;
				});
				break;
			default:
				throw Exception(0, 0, 
					this, 
					"unknown untaint language #%d of %d piece", 
						static_cast<int>(row->item.lang), 
						i); // never
				break; // never
			}

			if((lang==UL_UNSPECIFIED?row->item.lang:lang)!=UL_CLEAN)
				whitespace=false;
		}
		chunk=row->link;
	} while(chunk);

break2:
	return dest;
}

char *String::cstr_debug_origins() const {
	char *result=(char *)malloc(size()+used_rows()*MAX_STRING*2);
	char *dest=result;
	
	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(uint i=0; i<chunk->count; i++, row++) {
			if(row==append_here)
				goto break2;

#ifndef NO_STRING_ORIGIN
			if(row->item.origin.file)
				dest+=sprintf(dest, ORIGIN_FILE_LINE_FORMAT,
					row->item.origin.file,
					1+row->item.origin.line);
			else
				dest+=sprintf(dest, "<unknown>");
#endif
			dest+=sprintf(dest, "#%s: ",
				String_Untaint_lang_name[row->item.lang]);
			char *dest_after_origins=dest;

			memcpy(dest, row->item.ptr, row->item.size); 
			dest+=row->item.size;

			remove_crlf(dest_after_origins, dest);
			to_char('\n');
		}
		chunk=row->link;
	} while(chunk);

break2:
	*dest=0;
	return result;
}
