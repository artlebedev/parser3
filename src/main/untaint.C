/** @file
	Parser: String class part: untaint mechanizm.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

volatile const char * IDENT_UNTAINT_C="$Id: untaint.C,v 1.176 2020/12/15 17:10:37 moko Exp $";


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
#include "../lib/cord/include/ec.h"
}

#include "pa_sql_connection.h"

// defines

#undef CORD_ec_append
// redefining to intercept flushes and implement whitespace optimization
// of all consequent white space chars leaving only first one
#define CORD_ec_append(x, c) \
    {  \
	bool skip=false; \
	if(optimize) switch(c) { \
	case ' ': case '\n': case '\t': \
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

#define escape_fragment(action) \
	for(; fragment_length--; CORD_next(info->pos)) { \
		char c=CORD_pos_fetch(info->pos); \
		action \
	}

#define encode(need_encode_func, prefix, otherwise)  \
	if(need_encode_func(c)) { \
		CORD_ec_append(info->result, prefix); \
		to_hex(c); \
	} else \
		CORD_ec_append(info->result, otherwise);

#define to_hex(c) \
	{ \
		CORD_ec_append(info->result, hex_digits[((unsigned char)c) >> 4]); \
		CORD_ec_append(info->result, hex_digits[((unsigned char)c) & 0x0F]); \
	}

#define to_char(c)  { CORD_ec_append(info->result, c); whitespace=false; }
#define to_string(s)  { CORD_ec_append_cord(info->result, (CORD)(s)); whitespace=false; }
#define _default CORD_ec_append(info->result, c)

inline bool need_file_encode(unsigned char c){
	// russian letters and space ENABLED
	// encoding only these...
	return strchr(
			"*?\"<>|"
#ifndef WIN32
			":\\"
#endif
			, c)!=0;
}

inline bool need_uri_encode(unsigned char c){
	return !(pa_isalnum(c) || strchr("_-./*", c));
}

inline bool need_regex_escape(unsigned char c){
	return strchr("\\^$.[]|()?*+{}-", c)!=0;
}

inline bool need_parser_code_escape(unsigned char c){
	return strchr("^$;@()[]{}:#\"", c)!=0;
}

// String

/*
HTTP-header    = field-name ":" [ field-value ] CRLF

field-name     = token
field-value    = *( field-content | LWS )

field-content  = <the OCTETs making up the field-value
                        and consisting of either *TEXT or combinations
                        of token, tspecials, and quoted-string>


token          = 1*<any CHAR except CTLs or tspecials>
word           = token | quoted-string
quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
qdtext         = <any TEXT except <">>
quoted-pair    = "\" CHAR

OCTET          = <any 8-bit sequence of data>
CHAR           = <any US-ASCII character (octets 0 - 127)>

tspecials      = "(" | ")" | "<" | ">" | "@"
               | "," | ";" | ":" | "\" | <">
               | "/" | "[" | "]" | "?" | "="
               | "{" | "}" | SP | HT

SP             = <US-ASCII SP, space (32)>
HT             = <US-ASCII HT, horizontal-tab (9)>

LWS            = [CRLF] 1*( SP | HT )
TEXT           = <any OCTET except CTLs, but including LWS>
CTL            = <any US-ASCII control character (octets 0 - 31) and DEL (127)>


  if(strchr("()<>@,;:\\\"/[]?={} \t", *ptr))
*/
inline bool need_quote_http_header(const char* ptr, size_t size) {
	for(; size--; ptr++)
		if(strchr(";\\\"= \t" /* excluded ()<>@, :/ ? []{} */, *ptr))
			return true;
	return false;
}

#ifndef DOXYGEN
struct Append_fragment_info {
	String::Language lang;
	String::Languages* dest_languages;
	size_t dest_body_plan_length;
};
#endif
int append_fragment_optimizing(char alang, size_t asize, Append_fragment_info* info) {
	const String::Language lang=(String::Language)(unsigned char)alang;
	// main idea here:
	// tainted piece would get OPTIMIZED bit from 'lang'
	// clean piece would be marked OPTIMIZED manually
	// pieces with determined languages [not tainted|clean] would retain theirs langs
	info->dest_languages->append(info->dest_body_plan_length, 
		lang==String::L_TAINTED?
			info->lang
			:lang==String::L_CLEAN?
				(String::Language)(String::L_CLEAN|String::L_OPTIMIZE_BIT) // ORing with OPTIMIZED flag
				:lang,
		asize);
	info->dest_body_plan_length+=asize;

	return 0; // 0=continue
}
int append_fragment_nonoptimizing(char alang, size_t asize, Append_fragment_info* info) {
	const String::Language lang=(String::Language)(unsigned char)alang;
	// The core idea: tainted pieces got marked with context's lang
	info->dest_languages->append(info->dest_body_plan_length, 
		lang==String::L_TAINTED?
			info->lang
			:lang,
		asize);
	info->dest_body_plan_length+=asize;

	return 0; // 0=continue
}


/** 
	appends to other String without language change
*/

String& String::append_to(String& dest) const {
	if(is_empty())
		return dest;

	// first: fragment infos
	dest.langs.appendHelper(dest.body, langs, body);

	// next: letters
	dest.body<<body;

	ASSERT_STRING_INVARIANT(dest);
	return dest;
}

/** 
	appends to other String,
	marking all tainted pieces of it with @a lang.
	or marking ALL pieces of it with a @a lang when @a forced to,
	and propagating OPTIMIZE language bit.
*/
String& String::append_to(String& dest, Language ilang, bool forced) const {
	if(is_empty())
		return dest;

	// first: fragment infos
	
	if(forced) //forcing passed lang?
		dest.langs.appendHelper(dest.body, ilang, body);
	else {
		if(langs.opt.is_not_just_lang){
			Append_fragment_info info={ilang, &dest.langs, dest.body.length()};
			langs.for_each(body, ilang&L_OPTIMIZE_BIT?
				append_fragment_optimizing
				:append_fragment_nonoptimizing, &info);
		} else {
			Language lang=langs.opt.lang;
			// see append_fragment_* for explanation
			if(ilang&L_OPTIMIZE_BIT){
				dest.langs.appendHelper(dest.body,
					lang==String::L_TAINTED?
						ilang
						:lang==String::L_CLEAN?
							(String::Language)(String::L_CLEAN|String::L_OPTIMIZE_BIT)
							:lang,
					body);
			} else {
				dest.langs.appendHelper(dest.body, lang==String::L_TAINTED ? ilang:lang, body);
			}
		}
	}

	// next: letters
	dest.body<<body;

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
	return (pa_isalnum((unsigned char)c) || strchr("!*+-/", c));
}
inline bool addr_spec_soon(const char *src) {
	for(char c; (c=*src); src++)
		if(c=='<')
			return true;
		else if(!(c==' ' || c=='\t'))
			return false;
	return false;
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
		long avail=CORD_pos_chars_left(pos);
		if(avail<=0) {
			CORD_next(pos);
			if(!--n)
				break;
		} else if((size_t)avail<n) {
			CORD_pos_advance(pos, avail);
			n-=avail;
		} else { // avail>=n
			CORD_pos_advance(pos, n);
			break;
		}
	}
}

#ifndef DOXYGEN
struct Cstr_to_string_body_block_info {
	// input
	String::Language lang;
	SQL_Connection* connection;
	const Request_charsets* charsets;
	const String::Body* body;

	// output
	CORD_ec result;

	// private
	CORD_pos pos;
	size_t fragment_begin;
	bool whitespace;
	const char* exception;
};
#endif

// @todo: replace info->body->mid with something that uses info->pos
int cstr_to_string_body_block(String::Language to_lang, size_t fragment_length, Cstr_to_string_body_block_info* info) {
	bool& whitespace=info->whitespace;
	size_t fragment_end=info->fragment_begin+fragment_length;
	//fprintf(stderr, "%d, %d =%s=\n", to_lang, fragment_length, info->body->cstr());
	
	bool optimize=(to_lang & String::L_OPTIMIZE_BIT)!=0;
	if(!optimize)
		whitespace=false;
		
	switch(to_lang & ~String::L_OPTIMIZE_BIT) {
	case String::L_CLEAN:
	case String::L_TAINTED:
	case String::L_AS_IS:
		// clean piece

		// tainted piece, but undefined untaint language
		// for VString.as_double of tainted values
		// for ^process{body} evaluation

		// tainted, untaint language: as-is
		ec_append(info->result, optimize, whitespace, info->pos, fragment_length);
		break;
	case String::L_FILE_SPEC:
		// tainted, untaint language: file [name]
		{
			escape_fragment(
				encode(need_file_encode, '_', c); 
			);
		}
		break;
	case String::L_URI:
		// tainted, untaint language: uri
		{
			const char *fragment_str=info->body->mid(info->fragment_begin, fragment_length).cstr();
			// skip source [we use recoded version]
			pa_CORD_pos_advance(info->pos, fragment_length);
			String::C output(fragment_str, fragment_length);
			if(info->charsets)
				output=Charset::transcode(output, info->charsets->source(), info->charsets->client());

			char c;
			for(const char* src=output.str; (c=*src++); )
				encode(need_uri_encode, '%', c);
		}
		break;
	case String::L_HTTP_HEADER:
		// tainted, untaint language: http-field-content-text
		escape_fragment(switch(c) {
			case '\n': 
			case '\r': to_string(" ");  break;
			default: _default; break;
		});
		break;
	case String::L_MAIL_HEADER:
		// tainted, untaint language: mail-header
		// http://www.ietf.org/rfc/rfc2047.txt
		if(info->charsets) {
			size_t mail_size;
			const char *mail_ptr=
				info->body->mid(info->fragment_begin, mail_size=fragment_length).cstr();
			// skip source [we use recoded version]
			pa_CORD_pos_advance(info->pos, mail_size);

			const char* charset_name=info->charsets->mail().NAME().cstr();

			// Subject: Re: parser3: =?koi8-r?Q?=D3=C5=CD=C9=CE=C1=D2?=
			bool to_quoted_printable=false;

			bool email=false;
			uchar c;
			for(const char* src=mail_ptr; c=(uchar)*src; src++) {
				if(c=='\r' || c=='\n')
					c=' ';
				if(to_quoted_printable && (c==',' || c == '"' || addr_spec_soon(src))) {
					email=c=='<';
					to_string("?=");
					to_quoted_printable=false;
				}
				//RFC   + An 'encoded-word' MUST NOT appear in any portion of an 'addr-spec'.
				if(!email && (
					( !to_quoted_printable && (c & 0x80 || (c == ' ' && src == mail_ptr) ) )  // starting quote-printable-encoding on first 8bit char or leading space (issue #123)
					|| ( to_quoted_printable && !mail_header_char_valid_within_Qencoded(c) )
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
			ec_append(info->result, optimize, whitespace, info->pos, fragment_length);
		break;
	case String::L_SQL:
		// tainted, untaint language: sql
		if(info->connection) {
			const char *fragment_str=info->body->mid(info->fragment_begin, fragment_length).cstr();
			// skip source [we use recoded version]
			pa_CORD_pos_advance(info->pos, fragment_length);

			to_string(info->connection->quote(fragment_str, fragment_length));
		} else {
			info->exception="untaint in SQL language failed - no connection specified";
			info->fragment_begin=fragment_end;
			return 1; // stop processing. can't throw exception here
		}
		break;
	case String::L_JS:
		escape_fragment(switch(c) {
			case '\n': to_string("\\n");  break;
			case '"': to_string("\\\"");  break;
			case '\'': to_string("\\'");  break;
			case '\\': to_string("\\\\");  break;
			case '\xFF': to_string("\\\xFF");  break;
			case '\r': to_string("\\r");  break;
			default: _default; break;
		});
		break;
	case String::L_XML:
		// [2]    Char    ::=    #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF] 
		escape_fragment(switch(c) {
			case '\x20':
			case '\x9':
			case '\xA':
			case '\xD': // this is usually removed on input
				_default; 
				break;
			case '&': to_string("&amp;");  break;
			case '>': to_string("&gt;");  break;
			case '<': to_string("&lt;");  break;
			case '"': to_string("&quot;");  break;
			case '\'': to_string("&apos;");  break;
			default: 
				if(((unsigned char)c)<0x20) {
					// fixing it, so that libxml would not result
					// in fatal error parsing text
					// though it really violates standard.
					// to indicate there were an error
					// replace bad char not to it's code, 
					// which we can do,
					// but rather to '!' to show that input were actually
					// invalid.
					// life: shows that MSIE can somehow garble form values
					// so that they contain these chars.
					to_char('!'); 
				} else {
					_default; 
				}
				break;
		});
		break;
	case String::L_HTML:
		escape_fragment(switch(c) {
			case '&': to_string("&amp;");  break;
			case '>': to_string("&gt;");  break;
			case '<': to_string("&lt;");  break;
			case '"': to_string("&quot;");  break;
			default: _default; break;
		});
		break;
	case String::L_REGEX:
		// tainted, untaint language: regex
		escape_fragment(
			if(need_regex_escape(c))
				to_char('\\')
			_default;
		);
		break;
	case String::L_JSON:
		// tainted, untaint language: json <http://json.org/>
		// escape '"' '\' '/' '\n' '\t' '\r' '\b' '\f' chars and escape chars as \uXXXX if output charset != UTF-8
		{
			if(info->charsets==NULL || info->charsets->client().isUTF8()){
				// escaping to \uXXXX is not needed
				escape_fragment(switch((unsigned char)c) {
					case '\n': to_string("\\n");  break;
					case '"' : to_string("\\\""); break;
					case '\\': to_string("\\\\"); break;
					case '/' : to_string("\\/");  break;
					case '\t': to_string("\\t");  break;
					case '\r': to_string("\\r");  break;
					case '\b': to_string("\\b");  break;
					case '\f': to_string("\\f");  break;
					case 0xE2: // \u2028 and \u2029 (line/paragraph separators), check bug #1023
						if(info->charsets && info->charsets->source().isUTF8() && fragment_length>=2){
							CORD_next(info->pos);
							char c1=CORD_pos_fetch(info->pos);
							CORD_next(info->pos);
							char c2=CORD_pos_fetch(info->pos);
							if((unsigned char)c1 == 0x80 && ((unsigned char)c2 >= 0xA8 && (unsigned char)c2 <= 0xAF)){
								to_string("\\u20");
								to_hex(((unsigned char)c2-0x80));
							} else {
								CORD_ec_append(info->result, c);
								CORD_ec_append(info->result, c1);
								CORD_ec_append(info->result, c2);
							}
							fragment_length-=2;
						} else {
							_default;
						}
						break;
					default: 
						if((unsigned char)c < 0x20){
							to_string("\\u00");
							to_hex(c);
						} else {
							_default;
						}
						break;
				});
			} else {
				const char *fragment_str=info->body->mid(info->fragment_begin, fragment_length).cstr();
				// skip source [we use recoded version]
				pa_CORD_pos_advance(info->pos, fragment_length);
				to_string(Charset::escape_JSON(String::C(fragment_str, fragment_length), info->charsets->source()).str);
			}
		}
		break;
	case String::L_HTTP_COOKIE:
		// tainted, untaint language: cookie (3.3.0 and higher: %uXXXX in UTF-8)
		if(info->charsets) {
			const char *fragment_str=info->body->mid(info->fragment_begin, fragment_length).cstr();
			// skip source [we use recoded version]
			pa_CORD_pos_advance(info->pos, fragment_length);
			to_string(Charset::escape(String::C(fragment_str, fragment_length), info->charsets->source()).str);
		} else
			ec_append(info->result, optimize, whitespace, info->pos, fragment_length);
		break;
	case String::L_PARSER_CODE:
		// for auto-untaint in process
		escape_fragment(
			if(need_parser_code_escape(c))
				to_char('^');
			_default;
		);
		break;
	default:
		SAPI::die("unknown untaint language #%d", static_cast<int>(to_lang)); // should never
		break; // never
	}

	info->fragment_begin=fragment_end;

	return 0; // 0=continue
}


String::Body String::cstr_to_string_body_taint(Language lang, SQL_Connection* connection, const Request_charsets *charsets) const {
	if(is_empty())
		return String::Body();

	Cstr_to_string_body_block_info info;
	// input
	info.lang=lang;
	info.connection=connection;
	info.charsets=charsets;
	info.body=&body;
	// output
	CORD_ec_init(info.result);
	// private
	body.set_pos(info.pos, 0);
	info.fragment_begin=0;
	info.exception=0;
	info.whitespace=true;

	cstr_to_string_body_block(lang, length(), &info);

	if(info.exception)
		throw Exception(0,
			0,
			info.exception);

	return String::Body(CORD_ec_to_cord(info.result));
}

int cstr_to_string_body_block_untaint(char alang, size_t fragment_length, Cstr_to_string_body_block_info* info){
	const String::Language lang=(String::Language)(unsigned char)alang;
	// see append_fragment_* for explanation
	if(info->lang&String::L_OPTIMIZE_BIT)
		return cstr_to_string_body_block(
			lang==String::L_TAINTED?
				info->lang
				:lang==String::L_CLEAN?
					(String::Language)(String::L_CLEAN|String::L_OPTIMIZE_BIT)
					:lang,
			fragment_length, info);
	else
		return cstr_to_string_body_block(lang==String::L_TAINTED ? info->lang:lang, fragment_length, info);
}

String::Body String::cstr_to_string_body_untaint(Language lang, SQL_Connection* connection, const Request_charsets *charsets) const {
	if(is_empty())
		return String::Body();

	Cstr_to_string_body_block_info info;
	// input
	info.lang=lang;
	info.connection=connection;
	info.charsets=charsets;
	info.body=&body;
	// output
	CORD_ec_init(info.result);
	// private
	body.set_pos(info.pos, 0);
	info.fragment_begin=0;
	info.exception=0;
	info.whitespace=true;

	langs.for_each(body, cstr_to_string_body_block_untaint, &info);

	if(info.exception)
		throw Exception(0,
			0,
			info.exception);

	return String::Body(CORD_ec_to_cord(info.result));
}

const char* String::untaint_and_transcode_cstr(Language lang, const Request_charsets *charsets) const {
	if(charsets && &charsets->source() != &charsets->client()){
		// Note: L_URI is allready transcoded during untaint, but transcode does not affect %XX
		return Charset::transcode(cstr_to_string_body_untaint(lang, 0, charsets), charsets->source(), charsets->client()).cstr();
	} else
		return cstr_to_string_body_untaint(lang, 0, charsets).cstr();
}
