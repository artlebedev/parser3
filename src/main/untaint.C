/** @file
	Parser: String class part: untaint mechanizm.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_UNTAINT_C="$Date: 2002/08/01 11:41:19 $";

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_table.h"
#include "pa_globals.h"
#include "pa_sql_connection.h"
#include "pa_dictionary.h"
#include "pa_common.h"
#include "pa_charset.h"

//#define DEBUG_STRING_APPENDS_VS_EXPANDS

#ifdef DEBUG_STRING_APPENDS_VS_EXPANDS
ulong string_string_shortcut_economy=0;
#endif

#define escape(action) \
	{ \
		const char *src=row->item.ptr; \
		for(int size=row->item.size; size--; src++) \
			action \
	}
#define _default  default: *dest++=*src; break
#define encode(need_encode_func, prefix)  \
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
	// theoretical problem with, for instance, "_2B" and "." fragments, 
	// they would yield the same 
	// because need_file_encode('_')=false
	// but we need to delete such files somehow, getting names from ^index

    if((c>='0') &&(c<='9') ||(c>='A') &&(c<='Z') ||(c>='a') &&(c<='z')) 
		return false;

    return !strchr(
		" _./()-"
#ifdef WIN32
		":\\~"
#endif
		, c);
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
	"HTML"      ///< HTML code (for editing)
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

//#include "pa_sapi.h"
/** 
	appends other String,
	marking all tainted pieces of it with @a lang.
	or marking ALL pieces of it with a @a lang when @a forced to,
	and propagating OPTIMIZE language bit.

    using architecture advantage: after string-to-string-append string never modified.
	algorithm:
	if no language-change specified and src not yet appended to some other string[last_chunk!=0]
		shrinking dest last_chunk[preparing it for linking],
		///shrinking src last_chunk[preparing it to be linked, consequent dest.appends would go there],
		linking[dest.last_chunk = src.head.chunk]
	if some language-change specified or src already appended to some other string[last_chunk==0]
		cloning pieces.
*/
String& String::append(const String& src, uchar lang, bool forced) {
	if(!last_chunk) // growth stopped [we're appended as string to somebody]
		throw Exception(0,
			this,
			"string growth stopped (append string)");

	if(src.is_empty())
		return *this;

	// without language-chage, not-appended-before, big[not fitting our tail] string?
	if(lang==UL_PASS_APPENDED 
		&& src.last_chunk
		&& (uint(&last_chunk->rows[last_chunk->count]-append_here) < src.used_rows())) {
#ifdef DEBUG_STRING_APPENDS_VS_EXPANDS
		string_string_shortcut_economy+=src.used_rows()*sizeof(String::Chunk::Row);
#endif

		// using fact: 
		// src.head.chunk.count initally equeals this.head.chunk.count and shrinks-only, 
		// so can't be more than this.head.chunk.count, 
		// which means that we know that 
		// src.head.chunk would fit into this.head.chunk
		if(is_empty()) { // our head.chunk is empty
			// they have more than head.chunk? we need all head.chunk : we need only filled-part of head.chunk
			Chunk *src_head_link=src.head.chunk.rows[src.head.chunk.count].link;
			size_t head_count=src_head_link?src.head.chunk.count:(src.append_here-src.head.chunk.rows);
			// "your head.chunk is my head.chunk"
			memcpy(head.chunk.rows, src.head.chunk.rows, sizeof(Chunk::Row)*(head_count));
			if(src_head_link) {
				// "your body is my body"
				head.chunk.rows[head.chunk.count=head_count].link=src_head_link;
				// "your last_chunk is mine now"
				last_chunk=src.last_chunk;
				// "your append_here is mine now"
				append_here=src.append_here;
			} else {
				// "your last_chunk is mine now"
				last_chunk=&head.chunk;
				// "your append_here is recalc-mine now"
				append_here=head.chunk.rows+head_count;
			}
		} else { // our head.chunk contains something
			// "chopping off my tail-reserve"
			last_chunk->count=append_here-last_chunk->rows;
			// "you is my tail"
			append_here->link=&src.head.chunk;
			// "your last_chunk is mine now"
			last_chunk=src.last_chunk;
			// "your append_here is mine now"
			append_here=src.append_here;
		}

		// stop-growing mark
		src.last_chunk=0;
		return *this;
	}

	// manually unrolled code to avoid do{if(const)} constructs
	if(forced) 
		STRING_SRC_FOREACH_ROW(
			APPEND(row->item.ptr, row->item.size, 
				lang, //forcing passed lang
				row->item.origin.file, row->item.origin.line);
		)
	else if(lang==UL_PASS_APPENDED) 
		STRING_SRC_FOREACH_ROW(
			APPEND(row->item.ptr, row->item.size, 
				row->item.lang, // passing item's lang
				row->item.origin.file, row->item.origin.line);
		)
	else if(lang&UL_OPTIMIZE_BIT) // main idea here
		// tainted piece would get OPTIMIZED bit from 'lang'
		// clean piece would be marked OPTIMIZED manually
		// pieces with determined languages [not tainted|clean] would retain theirs langs
		STRING_SRC_FOREACH_ROW(
			APPEND(row->item.ptr, row->item.size, 
				row->item.lang==UL_TAINTED?lang:(
					row->item.lang==UL_CLEAN?UL_CLEAN|UL_OPTIMIZE_BIT: // ORing with OPTIMIZED flag
						row->item.lang
				), 
				row->item.origin.file, row->item.origin.line);
		)
	else
		STRING_SRC_FOREACH_ROW(
			APPEND(row->item.ptr, row->item.size, 
				row->item.lang==UL_TAINTED?lang:row->item.lang,
				row->item.origin.file, row->item.origin.line);
		);
/*
			for(Chunk::Row *row=last_chunk->rows; row<append_here; row++)
				if(row->link==(void*)0xcdcdcdcd)
					_asm int 3;*/
	return *this;
}

size_t String::cstr_bufsize(Untaint_lang lang,
							SQL_Connection *connection,
							Charset *buf_charset) const {
	size_t dest=1; // for terminating 0
	STRING_FOREACH_ROW(
		uchar to_lang=lang==UL_UNSPECIFIED?row->item.lang:lang;

		switch(to_lang & ~UL_OPTIMIZE_BIT) {
		case UL_CLEAN:
		case UL_TAINTED:
		case UL_AS_IS:
			// clean piece

			// tainted piece, but undefined untaint language
			// for VString.as_double of tainted values
			// for ^process{body} evaluation

			// tainted, untaint language: as-is
			dest+=row->item.size;
			break;
		case UL_FILE_SPEC:
			// tainted, untaint language: file [name]
			dest+=row->item.size*3/* worst: Z->%XX */;
			break;
		case UL_URI:
			// tainted, untaint language: uri
			dest+=row->item.size*6*3/* worst utf8 x worst Z->%XX */;
			break;
		case UL_HTTP_HEADER:
			// tainted, untaint language: http-field-content-text
			dest+=row->item.size*3/* worst: Z->%XX */;
			break;
		case UL_MAIL_HEADER:
			// tainted, untaint language: mail-header
			if(buf_charset) {
				// Subject: Re: parser3: =?koi8-r?Q?=D3=C5=CD=C9=CE=C1=D2?=
				dest+=
					row->item.size*3+
					buf_charset->name().size()+MAX_STRING/* worst: =?charset?Q?=%XX?= */;
			} else
				dest+=row->item.size;
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
	);
	return dest;
}

char *String::store_to(char *dest, Untaint_lang lang, 
					   SQL_Connection *connection,
					   Charset *store_to_charset,
					   const char *store_to_charset_name) const {
	// WARNING:
	//	 before any changes check cstr_bufsize first!!!
	bool whitespace=true;
	STRING_FOREACH_ROW(
		uchar to_lang=lang==UL_UNSPECIFIED?row->item.lang:lang;

		char *start=dest;

		switch(to_lang & ~UL_OPTIMIZE_BIT) {
		case UL_CLEAN:
		case UL_TAINTED:
		case UL_AS_IS:
			// clean piece

			// tainted piece, but undefined untaint language
			// for VString.as_double of tainted values
			// for ^process{body} evaluation

			// tainted, untaint language: as-is
			memcpy(dest, row->item.ptr, row->item.size); 
			dest+=row->item.size;
			break;
		case UL_FILE_SPEC:
			// tainted, untaint language: file [name]
			escape(
				encode(need_file_encode, '_');
			);
			break;
		case UL_URI:
			// tainted, untaint language: uri
			const void *client_ptr;
			size_t client_size;
			Charset::transcode(pool(), 
				pool().get_source_charset(), row->item.ptr, row->item.size,
				pool().get_client_charset(), client_ptr, client_size);
			{
				const char *src=(const char *)client_ptr;
				for(int size=client_size; size--; src++) 
					switch(*src) {
						case ' ': to_char('+');  break;
						default: encode(need_uri_encode, '%');
					};
			}
			break;
		case UL_HTTP_HEADER:
			// tainted, untaint language: http-field-content-text
			escape(switch(*src) {
				case ' ': to_char('+');  break;
				default: encode(need_uri_encode, '%');
			});
			break;
		case UL_MAIL_HEADER:
			// tainted, untaint language: mail-header
			if(store_to_charset && store_to_charset_name) {
				const void *mail_ptr;
				size_t mail_size;
				Charset::transcode(pool(), 
					pool().get_source_charset(), row->item.ptr, row->item.size,
					*store_to_charset, mail_ptr, mail_size);

				// Subject: Re: parser3: =?koi8-r?Q?=D3=C5=CD=C9=CE=C1=D2?=
				const char *src=(const char *)mail_ptr;
				bool to_quoted_printable=false;
				for(int size=mail_size; size--; src++) {
					if((*src & 0x80)  // starting quote-printable-encoding on first 8bit char
						|| (to_quoted_printable && (*src=='?' || *src=='=')) // additionally encoding '?' and '|'
						) {
						if(!to_quoted_printable) {
							dest+=sprintf(dest, "=?%s?Q?", store_to_charset_name);
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
				throw Exception(0,
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
		default:
			throw Exception(0, 
				this, 
				"unknown untaint language #%d", 
					static_cast<int>(row->item.lang)); // sould never
			break; // never
		}

		if(to_lang & UL_OPTIMIZE_BIT) { 
			// optimizing whitespace
			char *stop=dest;  dest=start;
			for(char *src=start; src<stop; src++)
				switch(*src) {
				// of all consequent white space chars leaving only first one
				case ' ': case '\r': case '\n': case '\t':
					if(!whitespace) {
						*dest++=*src;
						whitespace=true;
					}
					break;
				default:
					whitespace=false;
					*dest++=*src;
					break;
				};
		} else // piece without optimization
			whitespace=false;
	);

	return dest;
}

char *String::cstr_debug_origins() const {
	//_asm int 3;
	char *result=(char *)malloc(size()+used_rows()*MAX_STRING*2);
	char *dest=result;
	
	STRING_FOREACH_ROW(
IFNDEF_NO_STRING_ORIGIN(
		if(row->item.origin.file)
			dest+=sprintf(dest, ORIGIN_FILE_LINE_FORMAT,
				row->item.origin.file,
				1+row->item.origin.line);
		else
			dest+=sprintf(dest, "<unknown>");
);
		uchar show_lang=row->item.lang & ~UL_OPTIMIZE_BIT;
		if(show_lang>=sizeof(String_Untaint_lang_name)/sizeof(String_Untaint_lang_name[0]))
			throw Exception(0, 
				this, 
				"unknown untaint language #%d", 
					static_cast<int>(show_lang)); // sould never

		dest+=sprintf(dest, "#%s%s: ",
			String_Untaint_lang_name[show_lang],
			row->item.lang & UL_OPTIMIZE_BIT?".O":"");
		char *dest_after_origins=dest;

		memcpy(dest, row->item.ptr, row->item.size); 
		dest+=row->item.size;

		remove_crlf(dest_after_origins, dest);
		to_char('\n');
	);

	*dest=0;
	return result;
}
