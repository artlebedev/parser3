/** @file
	Parser: String class part: untaint mechanizm.

	Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_UNTAINT_C="$Date: 2003/07/24 11:31:24 $";


#include "pa_string.h"
#include "pa_hash.h"
#include "pa_exception.h"
#include "pa_table.h"
#include "pa_globals.h"
#include "pa_dictionary.h"
#include "pa_common.h"
#include "pa_charset.h"
#include "pa_request_charsets.h"
#include "pa_sapi.h"

extern "C" { // author forgot to do that
#include "ec.h"
}

#define PA_SQL

#ifdef PA_SQL
#include "pa_sql_connection.h"
#endif

// defines


#undef CORD_ec_append
// redefining to intercept flushes and implement whitespace optimization
// of all consequent white space chars leaving only first one
#define CORD_ec_append(x, c) \
    {  \
	bool skip=false; \
	if(optimize) switch(c) { \
	case ' ': case '\r': case '\n': case '\t': \
		if(whitespace) \
			skip=true; /*skipping subsequent*/ \
		else \
			whitespace=true; \
		break; \
	default: \
		whitespace=false; \
		break; \
	} \
	if(!skip) { \
		if ((x)[0].ec_bufptr == (x)[0].ec_buf + CORD_BUFSZ) { \
	  		CORD_ec_flush_buf(x); \
		} \
		*((x)[0].ec_bufptr)++ = (c); \
	} \
    }


#define escape(action) \
	for(; fragment_length--; CORD_next(pos)) { \
		char c=CORD_pos_fetch(pos); \
		action \
	}
#define _default  default: CORD_ec_append(result, c); break
#define encode(need_encode_func, prefix, otherwise)  \
	if(need_encode_func(c)) { \
		static const char* hex="0123456789ABCDEF"; \
		CORD_ec_append(result, prefix); \
		CORD_ec_append(result, hex[((unsigned char)c)/0x10]); \
		CORD_ec_append(result, hex[((unsigned char)c)%0x10]); \
	} else \
		CORD_ec_append(result, otherwise);
#define to_char(c)  CORD_ec_append(result, c)
#define to_string(s)  CORD_ec_append_cord(result, s)

inline bool need_file_encode(unsigned char c){
	// russian letters and space ENABLED
	// encoding only these...
	return strchr(
			  "*?'\"<>|"
#ifndef WIN32
			  ":\\"
#endif
			  , c)!=0;
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

static const char*  String_Untaint_lang_name[]={
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
inline bool need_quote_http_header(const char* ptr, size_t size) {
	for(; size--; ptr++)
		if(strchr(";\\\"= \t" /* excluded ()<>@, :/ ? []{} */, *ptr))
			return true;
	return false;
}


/** 
	appends to other String,
	marking all tainted pieces of it with @a lang.
	or marking ALL pieces of it with a @a lang when @a forced to,
	and propagating OPTIMIZE language bit.
*/
String& String::append_to(String& dest, Language lang, bool forced) const {
	if(is_empty())
		return dest;

	// first: letters
	dest.body<<body;

	// next: fragment infos
	
	if(lang==L_PASS_APPENDED) // without language-change?
		dest.fragments.append(fragments);
	else if(forced) //forcing passed lang?
		dest.fragments+=Fragment(lang, length());
	else if(lang&L_OPTIMIZE_BIT) {
		for(Array_iterator<ArrayFragment::element_type> i(fragments); i.has_next(); ) {
			const Fragment fragment=i.next();
			// main idea here:
			// tainted piece would get OPTIMIZED bit from 'lang'
			// clean piece would be marked OPTIMIZED manually
			// pieces with determined languages [not tainted|clean] would retain theirs langs
			dest.fragments+=Fragment(static_cast<Language>(
				fragment.lang==L_TAINTED?lang:(
					fragment.lang==L_CLEAN?L_CLEAN|L_OPTIMIZE_BIT: // ORing with OPTIMIZED flag
						fragment.lang)),
				fragment.length);
		}
	} else { // The core idea: tainted pieces got marked with context's lang
		for(Array_iterator<ArrayFragment::element_type> i(fragments); i.has_next(); ) {
			const Fragment fragment=i.next();
			dest.fragments+=Fragment(
				fragment.lang==L_TAINTED?lang:fragment.lang,
				fragment.length);
		}
	}

	ASSERT_STRING_INVARIANT(dest);
	return dest;
}

/** http://www.ietf.org/rfc/rfc2047.txt
RFC
(3) As a replacement for a 'word' entity within a 'phrase', for example,
    one that precedes an address in a From, To, or Cc header.  The ABNF
    definition for 'phrase' from RFC 822 thus becomes:

    phrase = 1*( encoded-word / word )

    In this case the set of characters that may be used in a "Q"-encoded
    'encoded-word' is restricted to: <upper and lower case ASCII
    letters, decimal digits, "!", "*", "+", "-", "/", "=", and "_"
    (underscore, ASCII 95.)>.  An 'encoded-word' that appears within a
    'phrase' MUST be separated from any adjacent 'word', 'text' or
    'special' by 'linear-white-space'.
...
   (2) The 8-bit hexadecimal value 20 (e.g., ISO-8859-1 SPACE) may be
       represented as "_" (underscore, ASCII 95.).  (This character may
       not pass through some internetwork mail gateways, but its use
       will greatly enhance readability of "Q" encoded data with mail
       readers that do not support this encoding.)  Note that the "_"
       always represents hexadecimal 20, even if the SPACE character
       occupies a different code position in the character set in use.

	paf: obviously, 
		without "=", or one could not differ "=E0" and "russian letter a"
		and without "_", or in would mean 0x20
*/
inline bool mail_header_char_valid_within_Qencoded(char c) {
	return c>='A' && c<='Z'
		|| c>='a' && c<='Z'
		|| c>='0' && c<='9'
		|| strchr("!*+-/", c);
}
/**
	RFC 
	Upper case should be used for hexadecimal digits "A" through "F"
	The 8-bit hexadecimal value 20 (e.g., ISO-8859-1 SPACE) 
	may be represented as "_" 
*/
inline bool mail_header_nonspace_char(char c) {
	return c != 0x20;
}
inline void ec_append(CORD_ec& result, bool& optimize, bool& whitespace, CORD_pos pos, size_t size) {
	while(size--) {
		CORD_ec_append(result, CORD_pos_fetch(pos));
		CORD_next(pos);
	}
}
inline void pa_CORD_pos_advance(CORD_pos pos, size_t n) {
	while(true) {
		size_t avail=CORD_pos_chars_left(pos);
		if(avail==0) {
			CORD_next(pos);
			if(!--n)
				break;
		} else if(avail<n) {
			CORD_pos_advance(pos, avail);
			n-=avail;
		} else { // avail>=n
			CORD_pos_advance(pos, n);
			break;
		}
	}
}
StringBody String::cstr_to_string_body(Language lang, 
		  SQL_Connection* connection,
		  const Request_charsets *charsets) const {
	CORD_ec result; CORD_ec_init(result);
	CORD_pos pos; body.set_pos(pos, 0);

	bool whitespace=true;

	size_t fragment_begin=0;
	size_t fragment_end;
	for(Array_iterator<Fragment> i(fragments); i.has_next(); fragment_begin=fragment_end) {
		const Fragment fragment=i.next();
		size_t fragment_length=fragment.length;
		fragment_end=fragment_begin+fragment_length;
		//fprintf(stderr, "%d, %d\n", fragment.lang, fragment.length);

		Language to_lang=lang==L_UNSPECIFIED?fragment.lang:lang;
		bool optimize=(to_lang & L_OPTIMIZE_BIT)!=0;
		if(!optimize)
			whitespace=false;
			
		switch(to_lang & ~L_OPTIMIZE_BIT) {
		case L_CLEAN:
		case L_TAINTED:
		case L_AS_IS:
			// clean piece

			// tainted piece, but undefined untaint language
			// for VString.as_double of tainted values
			// for ^process{body} evaluation

			// tainted, untaint language: as-is
			ec_append(result, optimize, whitespace, pos, fragment_length);
			break;
		case L_FILE_SPEC:
			// tainted, untaint language: file [name]
			escape(
				// Macintosh has problems with small Russian letter 'r'
				if( c=='\xF0' && charsets && charsets->source().NAME()=="WINDOWS-1251" ) {
					// fixing that letter for most common charset
					to_char('p');
				} else // fallback to default
					encode(need_file_encode, '_', c); 
			);
			break;
		case L_URI:
			// tainted, untaint language: uri
			{
				const char *fragment_str=body.mid(fragment_begin, fragment_length).cstr();
				// skip source [we use recoded version]
				pa_CORD_pos_advance(pos, fragment_length);
				String::C output(fragment_str, fragment_length);
				if(charsets) 
					output=Charset::transcode(output, 
						charsets->source(), 
						charsets->client());

				char c;
				for(const char* src=output.str; c=*src++; ) 
					encode(need_uri_encode, '%', c);
			}
			break;
		case L_HTTP_HEADER:
			// tainted, untaint language: http-field-content-text
			escape(
				encode(need_uri_encode, '%', c);
			);
			break;
		case L_MAIL_HEADER:
			// tainted, untaint language: mail-header
			// http://www.ietf.org/rfc/rfc2047.txt
			if(charsets) {
				size_t mail_size;
				const char *mail_ptr=
					body.mid(fragment_begin, mail_size=fragment_length).cstr();
				// skip source [we use recoded version]
				pa_CORD_pos_advance(pos, mail_size);

				const char* charset_name=charsets->mail().NAME().cstr();

				// Subject: Re: parser3: =?koi8-r?Q?=D3=C5=CD=C9=CE=C1=D2?=
				bool to_quoted_printable=false;

				bool email=false;
				uchar c;
				for(const char* src=mail_ptr; c=(uchar)*src++; ) {
					//RFC   + An 'encoded-word' MUST NOT appear in any portion of an 'addr-spec'.
					if(to_quoted_printable && (c==',' || c=='<')) {
						email=c=='<';
						to_string("?=");
						to_quoted_printable=false;
					}
					if(!email && (
						!to_quoted_printable && (c & 0x80)  // starting quote-printable-encoding on first 8bit char
						|| to_quoted_printable && !mail_header_char_valid_within_Qencoded(c)
						)) {
						if(!to_quoted_printable) {
							to_string("=?");
							to_string(charset_name);
							to_string("?Q?");
							to_quoted_printable=true;
						}
						encode(mail_header_nonspace_char, '=', '_'); 
					} else
						to_char(c);
					if(c=='>')
						email=false;
				}
				if(to_quoted_printable) // close
					to_string("?=");
			
			} else
				ec_append(result, optimize, whitespace, pos, fragment_length);
			break;
		case L_TABLE: 
			// tainted, untaint language: table
			escape(switch(c) {
				case '\t': to_char(' ');  break;
				case '\n': to_char(' ');  break;
				_default;
			});
			break;
#ifdef PA_SQL
		case L_SQL:
			// tainted, untaint language: sql
			if(connection) {
				const char *fragment_str=body.mid(fragment_begin, fragment_length).cstr();
				// skip source [we use recoded version]
				pa_CORD_pos_advance(pos, fragment_length);

				to_string(connection->quote(fragment_str, fragment_length));
			} else
				throw Exception(0,
					0,
					"untaint in SQL language failed - no connection specified");
			break;
#endif
		case L_JS:
			escape(switch(c) {
				case '"': to_string("\\\"");  break;
				case '\'': to_string("\\'");  break;
				case '\n': to_string("\\n");  break;
				case '\\': to_string("\\\\");  break;
				case '\xFF': to_string("\\\xFF");  break;
				_default;
			});
			break;
		case L_XML:
			escape(switch(c) {
				case '&': to_string("&amp;");  break;
				case '>': to_string("&gt;");  break;
				case '<': to_string("&lt;");  break;
				case '"': to_string("&quot;");  break;
				case '\'': to_string("&apos;");  break;
				_default;
			});
			break;
		case L_HTML:
			escape(switch(c) {
				case '&': to_string("&amp;");  break;
				case '>': to_string("&gt;");  break;
				case '<': to_string("&lt;");  break;
				case '"': to_string("&quot;");  break;
				_default;
			});
			break;
		default:
			SAPI::die("unknown untaint language #%d", 
				static_cast<int>(to_lang)); // should never
			break; // never
		}
	}

	return StringBody(CORD_ec_to_cord(result));
}
