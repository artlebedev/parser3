/** @file
	Parser: String class part: untaint mechanizm.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: untaint.C,v 1.63 2001/09/26 10:32:26 parser Exp $
*/

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_table.h"
#include "pa_globals.h"
#include "pa_sql_connection.h"
#include "pa_dictionary.h"

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
	"F", ///< filename
	"H",    ///< text in HTTP response header
	"M",    ///< text in mail header
	"URI",       ///< text in uri
	"T",     ///< ^table:set body
	"SQL",       ///< ^table:sql body
	"JS",        ///< JavaScript code
	"HTML",      ///< HTML code (for editing)
	"UHTML", ///< HTML code with USER chars
};


// String

static bool typo_present(Array::Item *value, const void *info) {
	Array *row=static_cast<Array *>(value);
	const char *src=static_cast<const char *>(info);

	int partial;
	row->get_string(0)->cmp(partial, src);
	return 
		partial==0 || // full match
		partial==1; // typo left column starts 'src'
}

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

/** @todo maybe additional check "are all pieces are clean?" would be profitable?
	@todo fix potential forigins_mode buf overrun
*/
size_t String::cstr_bufsize(Untaint_lang lang) const {
	return (lang==UL_AS_IS?size():size()*UNTAINT_TIMES_BIGGER*(forigins_mode?10:1)) +1;
}

/** @todo fix theoretical \n mem overrun in TYPO replacements
*/
char *String::store_to(char *dest, Untaint_lang lang, 
					   SQL_Connection *connection,
					   const char *charset) const {
	// $MAIN:html-typo table
	Dictionary *user_typo_dict=static_cast<Dictionary *>(pool().tag());
	Dictionary *typo_dict=user_typo_dict?user_typo_dict:default_typo_dict;

	bool whitespace=true;
	const Chunk *chunk=&head; 
	do {
		const Chunk::Row *row=chunk->rows;
		for(size_t i=0; i<chunk->count; i++, row++) {
			if(row==append_here)
				goto break2;

			Untaint_lang to_lang=lang==UL_UNSPECIFIED?row->item.lang:lang;

			char *dest_before_origins=dest;

			if(forigins_mode) {
#ifndef NO_STRING_ORIGIN
				if(row->item.origin.file)
					dest+=sprintf(dest, "%s(%d)",
						row->item.origin.file,
						1+row->item.origin.line);
				else
					dest+=sprintf(dest, "unknown");
#endif
				dest+=sprintf(dest, "#%s: ",
					String_Untaint_lang_name[to_lang]);
			}
			char *dest_after_origins=dest;

			// WARNING:
			//	string can grow only UNTAINT_TIMES_BIGGER
			switch(to_lang) {
			case UL_CLEAN:
				// clean piece
				{ // optimizing whitespace
					const char *src=row->item.ptr; 
					for(int size=row->item.size; size--; src++)
						switch(*src) {
						case ' ': case '\n': case '\r': case '\t':
							if(!whitespace) {
								*dest++=*src;
								whitespace=true;
							}
							break;
						default:
							whitespace=false;
							*dest++=*src;
							break;
						}
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
				// tainted, untaint language: http-header
				if(need_quote_http_header(row->item.ptr, row->item.size)) {
					*dest++='\"';
					escape(switch(*src) {
						case '\"': to_string("\\\"", 2);  break;
						_default;
					});
					*dest++='\"';
				} else {
					memcpy(dest, row->item.ptr, row->item.size); 
					dest+=row->item.size;
				}
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
					THROW(0, 0,
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
				escape(switch(*src) {
					case '&': to_string("&amp;", 5);  break;
					case '>': to_string("&gt;", 4);  break;
					case '<': to_string("&lt;", 4);  break;
					case '"': to_string("&quot;", 6);  break;
					_default;
				});
				break;
			case UL_USER_HTML: {
				// tainted, untaint language: html-typo
				if(!typo_dict) // never, always has default
					THROW(0, 0,
						this,
						"untaint to user-html lang failed, no typo table");

				char *html_for_typo=
					(char *)malloc(row->item.size*2/* '\n' -> '\' 'n' */+1);
				// note:
				//   there still is a possibility that user 
				//   would not replace \n as she supposed to
				//   and rather replace \ and n into huge strings
				//   thus causing memory overrun
				//   this can be dealed by allocating *2 memory, but that's too expensive
				size_t html_for_typo_size;
				{ // local dest
					char *dest=html_for_typo;
					escape(switch(*src) {
						// convinient name for typo match "\n"
						case '\r': 
							to_string("\\n", 2); // \r -> "\n"
							if(size && src[1]=='\n') { // \r\n -> remove \n
								size--; src++;
							}
							break;
						case '\n': 
							to_string("\\n", 2);
							break;
						_default;
					});
					*dest=0;
					html_for_typo_size=dest-html_for_typo;
				}
				// typo table replacements
				const char *src=html_for_typo;
				do {
					// there is a row where first column starts 'src'
					if(Table::Item *item=typo_dict->first_that_starts(src)) {
						// get a=>b values
						const String& a=*static_cast<Array *>(item)->get_string(0);
						const String& b=*static_cast<Array *>(item)->get_string(1);
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
						// skip 'b' in 'dest'
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
						i); // never
				break; // never
			}

			if((lang==UL_UNSPECIFIED?row->item.lang:lang)!=UL_CLEAN)
				whitespace=false;

			if(forigins_mode)
				if(dest==dest_after_origins) // never moved==optimized space
					dest=dest_before_origins;
				else {
					for(char *p=dest_after_origins; p<dest; p++)
						if(*p=='\n')
							*p='|';

					to_char('\n');
				}
		}
		chunk=row->link;
	} while(chunk);
break2:
	return dest;
}
