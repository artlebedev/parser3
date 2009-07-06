/**	@file
	Parser: @b mail class.
	relies on gmime library, by Jeffrey Stedfast <fejj@helixcode.com>

	Copyright(c) 2001-2009 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

static const char * const IDENT_VMAIL_C="$Date: 2009/07/06 08:48:24 $";

#include "pa_sapi.h"
#include "pa_vmail.h"
#include "pa_vstring.h"
#include "pa_request.h"
#include "pa_common.h"
#include "pa_charset.h"
#include "pa_charsets.h"
#include "pa_vdate.h"
#include "pa_vfile.h"
#include "pa_uue.h"

#ifdef WITH_MAILRECEIVE
extern "C" {
#include "gmime.h"
}

#include "pa_charsets.h"
#endif

// defines

#define RAW_NAME "raw"

// internals

enum PartType {
	P_TEXT,
	P_HTML,
	P_FILE,
	P_MESSAGE,

	P_TYPES_COUNT
};

static const char* const part_name_begins[P_TYPES_COUNT]={"text", "html", "file", "message"};

// defines for statics

#define FORMAT_NAME "format"
#define CHARSET_NAME "charset"
#define CID_NAME "content-id"

// statics

static const String format_name(FORMAT_NAME);
static const String charset_name(CHARSET_NAME);
static const String cid_name(CID_NAME);

// consts

const int MAX_CHARS_IN_HEADER_LINE=500;

// VMail

extern Methoded* mail_base_class;

VMail::VMail(): VStateless_class(0, mail_base_class) {}

#ifdef WITH_MAILRECEIVE

#define EXCEPTION_VALUE "x-exception"

static const String& maybeUpperCase(Charset& source_charset, 
				    const String& src, bool toUpperCase) {
	return toUpperCase?src.change_case(source_charset, String::CC_UPPER):src;
}

static void putReceived(Charset& source_charset, 
			HashStringValue& received, 
			const char* must_clone_name, Value* value, 
			bool nameToUpperCase=false) {

	if(must_clone_name && value) {
		received.put(
			maybeUpperCase(source_charset, 
				*new String(pa_strdup(must_clone_name)), nameToUpperCase),
			value);
	}
}

static void putReceived(Charset& source_charset, 
			HashStringValue& received, 
			const char* must_clone_name, const char* must_clone_value, 
			bool nameToUpperCase=false) {
	if(must_clone_name && must_clone_value) {

		String::C must_clone_value_cstr(must_clone_value, strlen(must_clone_value));
		//must_clone_value_cstr=Charset::transcode(must_clone_value_cstr, UTF8_charset, source_charset);

		putReceived(source_charset,
			received, 
			pa_strdup(must_clone_name), 
			new VString(*new String(pa_strdup(must_clone_value_cstr.str))),
			nameToUpperCase
		);
	}
}

static void putReceived(HashStringValue& received, const char* must_clone_name, time_t value) {
	if(must_clone_name)
		received.put(
			String::Body(pa_strdup(must_clone_name)), 
			new VDate(value)
		);
}

#ifndef DOXYGEN
struct MimeHeaderField2received_info {
	Charset* source_charset;
	HashStringValue* received;
};
#endif
static void MimeHeaderField2received(const char* must_clone_name, const char* must_clone_value, gpointer data) {
	MimeHeaderField2received_info& info=*static_cast<MimeHeaderField2received_info *>(data);

	putReceived(*info.source_charset, 
		*info.received, 
		must_clone_name, must_clone_value, true/*nameInUpperCase*/);
}

static void parse(Request& r, GMimeStream *stream, HashStringValue& received);

#ifndef DOXYGEN
struct MimePart2body_info {
	Request* r;
	HashStringValue* body;
	int partCounts[P_TYPES_COUNT];
};
#endif
static void MimePart2body(GMimePart *part, gpointer data) {
	MimePart2body_info& info=*static_cast<MimePart2body_info *>(data);
	Charset& source_charset=info.r->charsets.source();

	if(const GMimeContentType *type=g_mime_part_get_content_type(part)) {
		if(g_mime_content_type_is_type(type, "multipart", "*"))
			return; // skipping frames

		PartType partType;
		if(part->disposition) // inline/attachment
			partType=P_FILE;
		else if(g_mime_content_type_is_type(type, "text", "plain"))
			partType=P_TEXT;
		else if(g_mime_content_type_is_type(type, "text", "html"))
			partType=P_HTML;
		else if(g_mime_content_type_is_type(type, "message", "*"))
			partType=P_MESSAGE;
		else
			partType=P_FILE;
		
		// partName
		const char* partNameNumbered;
		char partNameBuf[MAX_STRING];
		const char* partNameStart=part_name_begins[partType];
		int partNo=++info.partCounts[partType];
		snprintf(partNameBuf, MAX_STRING, "%s%d", partNameStart, partNo);
		partNameNumbered=partNameBuf;
		
		// $.partX[ 
		VHash* vpartX(new VHash);  
		putReceived(source_charset, *info.body, partNameNumbered, vpartX);
		if(partNo==1)
			putReceived(source_charset, *info.body, partNameStart, vpartX);
			
		HashStringValue& partX=vpartX->hash();
		{
			// $.raw[
			VHash* vraw(new VHash);  putReceived(source_charset, partX, RAW_NAME, vraw);
			MimeHeaderField2received_info hfr_info={&source_charset, &vraw->hash()};
			g_mime_header_foreach(part->headers, MimeHeaderField2received, &hfr_info);
		}
		const char* content_filename=0;
		{
			// $.content-type[ 
			VHash* vcontent_type(new VHash);  putReceived(source_charset, partX, "content-type", vcontent_type);
			HashStringValue& content_type=vcontent_type->hash(); 
			{
				// $.value[text/plain] 
				char value[MAX_STRING];
				snprintf(value, MAX_STRING, "%s/%s", 
					type->type?type->type:"x-unknown",
					type->subtype?type->subtype:"x-unknown");
				putReceived(source_charset, content_type, VALUE_NAME, value);
			}
			GMimeParam *param=type->params;
			while(param) {
				// $.charset[windows-1251]  && co
				putReceived(source_charset, content_type, param->name, param->value, true);
				if(strcasecmp(param->name, "name")==0)
					content_filename=param->value;
				param=param->next;
			}
		}
		// $.description
		putReceived(source_charset, partX, "description", part->description);
		// $.content-id
		putReceived(source_charset, partX, "content-id", part->content_id);
		// $.content-md5
		putReceived(source_charset, partX, "content-md5", part->content_md5);
		// $.content-location
		putReceived(source_charset, partX, "content-location", part->content_location);
		
		// @todo GMimePart:
		//   GMimePartEncodingType encoding;
		//   GMimeDisposition *disposition;
		if(part->disposition) {
			GMimeParam *param=part->disposition->params;
			while(param) {
				// $.charset[windows-1251]  && co
				if(strcasecmp(param->name, "filename")==0)
					content_filename=param->value;
				param=param->next;
			}
		}

		// MESSAGE
		if(partType==P_MESSAGE) {
			if(part->content)
				if(GMimeStream *stream=part->content->stream)
					parse(*info.r, stream, partX);
		} else {
			// $.value[string|file]
			size_t buf_len;
			const char* local_buf=(const char*)g_mime_part_get_content(part, &buf_len);
			if(partType==P_FILE) {
				VFile* vfile(new VFile);
				char *pooled_buf=pa_strdup(local_buf, buf_len);
				VString* vcontent_type=content_filename?
					new VString(info.r->mime_type_of(content_filename)):0;
				vfile->set(true/*tainted*/, pooled_buf, buf_len, content_filename, vcontent_type);
				putReceived(source_charset, partX, VALUE_NAME, vfile);
			} else {
				// P_TEXT, P_HTML
				putReceived(source_charset, partX, VALUE_NAME,(const char*)local_buf);
			}
		}
	}
}

int gmt_offset() {
#if defined(HAVE_TIMEZONE) && defined(HAVE_DAYLIGHT)
	return timezone+(daylight?60*60*(timezone<0?-1:timezone>0?+1:0):0);
#else
	time_t t=time(0);
	tm *tm=localtime(&t);
#if defined(HAVE_TM_GMTOFF)
	return tm->tm_gmtoff;
#elif defined(HAVE_TM_TZADJ)
	return tm->tm_tzadj;
#else
#error neither HAVE_TIMEZONE&HAVE_DAYIGHT nor HAVE_TM_GMTOFF nor HAVE_TM_TZADJ defined
#endif
#endif
}

static void parse(Request& r, GMimeStream *stream, HashStringValue& received) {
	Charset& source_charset=r.charsets.source();

	GMimeMessage *message=g_mime_parser_construct_message(stream);
	try {
		const GMimeMessageHeader *messageHeader=message->header;
		if(!messageHeader)
			return;

		// firstly user-defined strings go
		//  user headers
		{
			// $.raw[
			VHash* vraw(new VHash);  putReceived(source_charset, received, RAW_NAME, vraw);
			MimeHeaderField2received_info hfr_info={&source_charset, &vraw->hash()};
			g_mime_header_foreach(messageHeader->headers, MimeHeaderField2received, &hfr_info);
		}

		// maybe-todo-recipients
		// x(messageHeader->recipients)

		//  secondly standard headers&body go
		//  standard header
		// .from
		putReceived(source_charset, received, "from", messageHeader->from);
		// .reply-to
		putReceived(source_charset, received, "reply-to", messageHeader->reply_to);
		// .to
		// @todo: messageHeader->recipients
		// .subject
		putReceived(source_charset, received, "subject", messageHeader->subject);
		// .date(date+gmt_offset)
		int tt_offset = 
			((messageHeader->gmt_offset / 100) *(60 * 60)) 
			+(messageHeader->gmt_offset % 100) * 60;			

		putReceived(received, "date", 
			messageHeader->date // local sender
			-tt_offset // move local sender to GMT sender
			-gmt_offset() // move GMT sender to our local time
		);
		// .message-id
		putReceived(source_charset, received, "message-id", messageHeader->message_id);

		// .body[part/parts
		GMimePart *part=message->mime_part;
		MimePart2body_info info={&r, &received, {0}};
		g_mime_part_foreach(part, MimePart2body, &info);

		// normal unref
		g_mime_object_unref(GMIME_OBJECT(message));
	} catch(const Exception& e) {
		// abnormal unref
		g_mime_object_unref(GMIME_OBJECT(message));
		putReceived(source_charset, received, VALUE_NAME, "<exception occured while parsing message>");
		putReceived(source_charset, received, EXCEPTION_VALUE, e.comment());
	} catch(...) {
		// abnormal unref
		g_mime_object_unref(GMIME_OBJECT(message));
		putReceived(source_charset, received, VALUE_NAME, "<exception occured while parsing message>");
	}
}
#endif



void VMail::fill_received(Request& 
#ifdef WITH_MAILRECEIVE
						  r
#endif
						  ) {
	// store letter to received
#ifdef WITH_MAILRECEIVE
	if(r.request_info.mail_received) {
		// init
		g_mime_init(0);
		//	g_mime_init(GMIME_INIT_FLAG_UTF8);

		// create stream with CRLF filter
		GMimeStream *stream = g_mime_stream_fs_new(fileno(stdin));
		GMimeStream *istream = g_mime_stream_filter_new_with_stream(stream);
		GMimeFilter *filter = g_mime_filter_crlf_new(GMIME_FILTER_CRLF_DECODE, GMIME_FILTER_CRLF_MODE_CRLF_ONLY);
		g_mime_stream_filter_add(GMIME_STREAM_FILTER(istream), filter);
		g_mime_stream_unref(stream);
		stream = istream;
		try {
			// parse incoming stream
			parse(r, stream, vreceived.hash());
			// normal stream free 
			g_mime_stream_unref(stream);
		} catch(const Exception& e) {
			// abnormal stream free 
			g_mime_stream_unref(stream);
			Charset& source_charset=r.charsets.source();
			HashStringValue& received=vreceived.hash();
			putReceived(source_charset, received, VALUE_NAME, "<exception occured while parsing messages>");
			putReceived(source_charset, received, EXCEPTION_VALUE, e.comment());
		} catch(...) {
			// abnormal stream free 
			g_mime_stream_unref(stream);
			rethrow;
		}
	}
#endif
}

typedef int (*string_contains_char_which_check)(int);
static bool string_contains_char_which(const char* string, string_contains_char_which_check check) {
	while(char c=*string++) {
		if(check((unsigned char)c))
			return true;
	}
	return false;
}
static char *trimBoth(char *s) {
	// sanity check
	if(!s)
		return 0;

	// trim head whitespace
	while(*s && isspace((unsigned char)*s))
		s++;
	// trim tail whitespace
	char *tail=s+strlen(s);
	if(tail>s) {
		do {
			--tail;
			if(isspace((unsigned char)*tail))
				*tail=0;
		} while(tail>s);
	}
	// return it
	return s;
}
static void extractEmail(String& result, char *email) {
	email=trimBoth(email);
	result.append_help_length(email, 0, String::L_TAINTED);

	/*
		http://www.faqs.org/rfcs/rfc822.html

		addr-spec   =  local-part "@" domain        ; global address
	
		local-part  =  word *("." word)             ; uninterpreted case-preserved
		word        =  atom / quoted-string

		domain      =  sub-domain *("." sub-domain)
		sub-domain  =  domain-ref / domain-literal
		domain-ref  =  atom                         ; symbolic reference

        domain-literal << ignoring for now
		quoted-string in word << ignoring for now

		atom        =  1*<any CHAR except specials, SPACE and CTLs>  << the ONLY to check

		specials    =  "(" / ")" / "<" / ">" / "@"  ; Must be in quoted-
                 /  "," / ";" / ":" / "\" / <">  ;  string, to use
                 /  "." / "[" / "]"              ;  within a word.

	*/
	const char* exception_type="email.format";
	if(strpbrk(email, "()<>,;:\\\"[]"/*specials minus @ and . */))
		throw Exception(exception_type,
			&result,
			"email contains bad characters (specials)");
	if(string_contains_char_which(email, (string_contains_char_which_check)isspace))
		throw Exception(exception_type,
			&result,
			"email contains bad characters (whitespace)");
	if(string_contains_char_which(email, (string_contains_char_which_check)iscntrl))
		throw Exception(exception_type,
			&result,
			"email contains bad characters (control)");
	if(result.is_empty())
		throw Exception(exception_type,
			0,
			"email is empty");
}

static const String& extractEmails(const String& string) {
	char *emails=string.cstrm();
	String& result=*new String;
	while(char *email=lsplit(&emails, ',')) {
		rsplit(email, '>');
		if(char *in_brackets=lsplit(email, '<'))
			email=in_brackets;
		if(!result.is_empty())
			result<<",";
		extractEmail(result, email);
	}

	return result;
}

#ifndef DOXYGEN
struct Store_message_element_info {
	Request_charsets& charsets;
	String& header;
	const String* & from;
	bool extract_to; String* & to;
	const String* errors_to;
	bool mime_version_specified;
	ArrayValue* parts[P_TYPES_COUNT];
	int parts_count;
	bool backward_compatibility;
	Value* content_type;
	bool had_content_disposition;

	Store_message_element_info(
		Request_charsets& acharsets,
		String& aheader,
		const String* & afrom,
		bool aextract_to, String* & ato
		):
		
		charsets(acharsets),
		header(aheader),
		from(afrom),
		extract_to(aextract_to), to(ato),
		errors_to(0),
		mime_version_specified(false),
		parts_count(0),
		backward_compatibility(false), content_type(0),
		had_content_disposition(false){
	}
};
#endif
static void store_message_element(HashStringValue::key_type raw_element_name, 
				  HashStringValue::value_type element_value, 
				  Store_message_element_info *info) {
	const String& low_element_name=String(raw_element_name, String::L_TAINTED).change_case(
		info->charsets.source(), String::CC_LOWER);

	// exclude internals
	if(low_element_name==MAIL_OPTIONS_NAME
		|| low_element_name==CHARSET_NAME
		|| low_element_name==VALUE_NAME
		|| low_element_name==RAW_NAME
		|| low_element_name==FORMAT_NAME
		|| low_element_name==NAME_NAME
		|| low_element_name==CID_NAME)
		return;

	// grep parts
	for(int pt=0; pt<P_TYPES_COUNT; pt++) {
		if(low_element_name.starts_with(part_name_begins[pt])) {
			// check that $.message# '#' is digit
			size_t start_len=strlen(part_name_begins[pt]);
			if(low_element_name.length()>start_len) {
				const char* at_num=low_element_name.mid(start_len, start_len+1).cstr();
				if(!isdigit((unsigned char)*at_num))
					continue;
			}
			*info->parts[pt]+=element_value;
			info->parts_count++;
			return;
		}
	}

	// fetch some special headers
	if(low_element_name=="from")
		info->from=&extractEmails(element_value->as_string());
	if(low_element_name==CONTENT_DISPOSITION_NAME)
		info->had_content_disposition=true;
	if(info->extract_to) { // defined only when SMTP used, see mail.C [collecting info for RCPT to-s]
		bool is_to=low_element_name=="to" ;
		bool is_cc=low_element_name=="cc" ;
		bool is_bcc=low_element_name=="bcc" ;
		if(is_to||is_cc||is_bcc) {
			if(!info->to)
				info->to=new String;
			else
				*info->to << ",";
			*info->to << extractEmails(element_value->as_string());
		}

		if(is_bcc) // blinding it
			return;
	}
	if(low_element_name=="errors-to")
		info->errors_to=&extractEmails(element_value->as_string());	
	if(low_element_name=="mime-version")
		info->mime_version_specified=true;

	// has content type?
	if(low_element_name==CONTENT_TYPE_NAME) {
		info->content_type=element_value;
		if(info->backward_compatibility)
			return;
	}

	// preparing header line
	const String& source_line=attributed_meaning_to_string(*element_value,
		String::L_PASS_APPENDED/*does not matter, would cstr(AS_IS) right away*/);
	if(source_line.is_empty())
		return; // we don't need empty headers here [used in clearing content-disposition]

	const char* source_line_cstr=source_line.cstr();
	String::C mail=Charset::transcode(
		String::C(source_line_cstr, source_line.length()),
		info->charsets.source(), 
		info->charsets.mail());
	String& mail_line=*new String;
	if(low_element_name=="to"
		|| low_element_name=="cc" 
		|| low_element_name=="bcc") 
	{
		// never wrap address lines, mailer can not handle wrapped properly
		mail_line.append_strdup(mail.str, mail.length, String::L_MAIL_HEADER);
	} else {
		while(mail.length) {
			bool too_long=mail.length>MAX_CHARS_IN_HEADER_LINE;
			size_t length=too_long
				? MAX_CHARS_IN_HEADER_LINE
				: mail.length;

			mail_line.append_strdup(mail.str, length, String::L_MAIL_HEADER);
			mail.length-=length;

			if(too_long)
				mail_line << "\n "; // break header and continue it on next line
		}	
	}

	// append header line
	info->header 
		<< raw_element_name 
		<< ":" << mail_line.cstr(String::L_UNSPECIFIED, 0, &info->charsets)
		<< "\n";
}

static const String& file_value_to_string(Request& r, Value* send_value) {
	String& result=*new String;

	VFile* vfile;
	const String* file_name=0;
	Value* vformat=0;
	Value* vcid=0;
	const String* dummy_from;
	String* dummy_to;
	Store_message_element_info info(r.charsets, 
		result, dummy_from, false, dummy_to);
	if(HashStringValue *send_hash=send_value->get_hash()) { // hash
		send_hash->for_each<Store_message_element_info*>(store_message_element, &info);

		// $.value
		if(Value* value=send_hash->get(value_name))
			vfile=value->as_vfile(String::L_AS_IS);
		else
			throw Exception(PARSER_RUNTIME,
				0,
				"file part has no $value");

		// $.format
		vformat=send_hash->get(format_name);

		// $.content-id
		vcid=send_hash->get(cid_name);

		// $.name
		if(Value* vfile_name=send_hash->get(name_name)) // $name specified 
			file_name=&vfile_name->as_string();
	} else  // must be VFile then
		vfile=send_value->as_vfile(String::L_AS_IS);

	if(!file_name)
		file_name=&vfile->fields().get(name_name)->as_string();

	const char* file_name_cstr=file_name->cstr();

	// content-type: application/octet-stream
	result << HTTP_CONTENT_TYPE ": " << r.mime_type_of(file_name_cstr) << "; name=\"" << file_name_cstr << "\"\n";

	if(!info.had_content_disposition) {
		// $.content-disposition wasn't specified
		result
			<< HTTP_CONTENT_DISPOSITION ": "
			<< ( vcid ? CONTENT_DISPOSITION_INLINE : CONTENT_DISPOSITION_ATTACHMENT )
			<< "; "
			<< CONTENT_DISPOSITION_FILENAME_NAME"=\"" << file_name_cstr << "\"\n";
	}

	if(vcid)
		result << CID_NAME ": <" << vcid->as_string() << ">\n"; // @todo: value must be escaped as %hh

	const String* type=vformat?&vformat->as_string():0;
	if(!type/*default = uue*/ || *type=="uue") {
		result << CONTENT_TRANSFER_ENCODING_NAME ": x-uuencode\n" << "\n";
		pa_uuencode(result, *file_name, *vfile);
	} else {
		if(*type=="base64") {
			result << CONTENT_TRANSFER_ENCODING_NAME ": base64\n" << "\n";
			result << pa_base64_encode(vfile->value_ptr(), vfile->value_size());
		} else {
			// for now
			throw Exception(PARSER_RUNTIME,
				type,
				"unknown attachment encode format");
		}
	}
	
	return result;
}

static const String& text_value_to_string(Request& r,
					  PartType pt, Value* send_value,
					  Store_message_element_info& info) {
	String& result=*new String;

	Value* text_value;
	Value* content_transfer_encoding=0;
	if(HashStringValue* send_hash=send_value->get_hash()) {
		// $.USER-HEADERS
		info.content_type=0; info.backward_compatibility=false; // reset
		send_hash->for_each<Store_message_element_info*>(store_message_element, &info);
		// $.value
		text_value=send_hash->get(value_name);
		if(!text_value)
			throw Exception(PARSER_RUNTIME,
				0,
				"%s part has no $" VALUE_NAME, part_name_begins[pt]);
		content_transfer_encoding=send_hash->get(content_transfer_encoding_name);
	} else
		text_value=send_value;

	if(!info.content_type) {
		result 
			<< HTTP_CONTENT_TYPE ": text/" << (pt==P_TEXT?"plain":"html")
			<< "; charset=" << info.charsets.mail().NAME()
			<< "\n";
	}
	if(!content_transfer_encoding)
		result << CONTENT_TRANSFER_ENCODING_NAME << ": 8bit\n";

	// header|body separator
	result << "\n"; 

	// body
	const String* body;
	switch(pt) {
	case P_TEXT:
		body=&text_value->as_string();
		break;
	case P_HTML: 
		{
			Temp_lang temp_lang(r, String::Language(String::L_HTML | String::L_OPTIMIZE_BIT));
			if(text_value->get_junction())
				body=&r.process_to_string(*text_value);
			else {
				throw Exception(PARSER_RUNTIME,
					0,
					"html part value must be code");
			}

			break;
		}
	default:
		throw Exception(0,
			0,
			"unhandled part type #%d", pt);
	}
	if(body) {
		Request_charsets charsets(r.charsets.source(), r.charsets.mail()/*uri!*/, r.charsets.mail());
		const char* body_cstr=strdup(body->cstr(String::L_UNSPECIFIED, 0, &charsets));  // body
		String::C mail=Charset::transcode(
			String::C(body_cstr, strlen(body_cstr)),
			r.charsets.source(),
			r.charsets.mail()/*always set - either mail.charset or $request:charset*/);
		///@todo
		result.append_know_length(mail.str, mail.length, String::L_CLEAN);
	}

	return result;
};

/// @todo files and messages in order (file, file2, ...)
const String& VMail::message_hash_to_string(Request& r,
					    HashStringValue* message_hash, int level, 
					    const String* & from, bool extract_to, String* & to) {
	
	if(!message_hash)
		throw Exception(PARSER_RUNTIME,
			0,
			"message must be hash");

	String& result=*new String;

	if(Value* vrecodecharset_name=message_hash->get(charset_name))
		r.charsets.set_mail(charsets.get(vrecodecharset_name->as_string()
			.change_case(r.charsets.source(), String::CC_UPPER)));
	else
		r.charsets.set_mail(r.charsets.source());
	// no big deal that we leave it set. they wont miss this point which would reset it

	Store_message_element_info info(r.charsets, 
		result, from, extract_to, to);
	{
		// for backward compatibilyty $.body+$.content-type -> 
		// $.text[$.value[] $.content-type[]]

		for(int pt=0; pt<P_TYPES_COUNT; pt++)
			info.parts[pt]=new ArrayValue(1);

		Value* body=message_hash->get("body");
		if(body) {
			message_hash->remove("body");
			info.backward_compatibility=true;
		}		
		message_hash->for_each<Store_message_element_info*>(store_message_element, &info);

		if(body) {
			VHash& text_part=*new VHash(); 
			HashStringValue& hash=text_part.hash();
			hash.put(value_name, body);
			if(info.content_type)
				hash.put(content_type_name, info.content_type);
			
			*info.parts[P_TEXT]+=&text_part;
			info.parts_count++;
		}

		if(!info.errors_to)
			result << "errors-to: postmaster\n"; // errors-to: default
		if(!info.mime_version_specified)
			result << "MIME-Version: 1.0\n"; // MIME-Version: default
	}

	int textCount=info.parts[P_TEXT]->count();
	if(textCount>1)
		throw Exception(PARSER_RUNTIME,
			0,
			"multiple text parts not supported, use file part");
	int htmlCount=info.parts[P_HTML]->count();
	if(htmlCount>1)
		throw Exception(PARSER_RUNTIME,
			0,
			"multiple html parts not supported, use file part");


	bool multipart=info.parts_count>1;
	bool alternative=textCount && htmlCount;
	// header
	char *boundary=0;
	if(multipart) {
		boundary=new(PointerFreeGC) char[MAX_NUMBER];
		snprintf(boundary, MAX_NUMBER-5/*lEvEl*/, "lEvEl%d", level);

		bool is_inline = false;
		{
			ArrayValue& files=*info.parts[P_FILE];
			for(size_t i=0; i<files.count(); i++) {
				HashStringValue* file;
				if((file=files.get(i)->get_hash()) && file->get(cid_name)){
					is_inline = true;
					break;
				}
			}
		}
		
		result << HTTP_CONTENT_TYPE ": " << ( is_inline ? HTTP_CONTENT_TYPE_MULTIPART_RELATED : HTTP_CONTENT_TYPE_MULTIPART_MIXED ) << ";";

		// multi-part
		result 
			 << " boundary=\"" << boundary << "\"\n"
				"\n"
				"This is a multi-part message in MIME format.";
	}

	// alternative or not
	{
		if(alternative) {
			result << "\n\n--" << boundary << "\n" // intermediate boundary
				HTTP_CONTENT_TYPE ": multipart/alternative; boundary=\"ALT" << boundary << "\"\n";
		} 
		for(int i=0; i<2; i++) {
			PartType pt=i==0?P_TEXT:P_HTML;
			if(info.parts[pt]->count()) {
				if(alternative)
					result << "\n\n--ALT" << boundary << "\n"; // intermediate boundary
				else if(boundary)
					result << "\n\n--" << boundary << "\n";  // intermediate boundary
				result << text_value_to_string(r, pt, info.parts[pt]->get(0), info);
			}
		}
		if(alternative)
			result << "\n\n--ALT" << boundary << "--\n";
	}

	// files
	{
		ArrayValue& files=*info.parts[P_FILE];
		for(size_t i=0; i<files.count(); i++) {
			if(boundary)
				result << "\n\n--" << boundary << "\n";  // intermediate boundary
			result << file_value_to_string(r, files.get(i));
		}
	}

	// messages
	{
		ArrayValue& messages=*info.parts[P_MESSAGE];
		for(size_t i=0; i<messages.count(); i++) {
			if(boundary)
				result << "\n\n--" << boundary << "\n";  // intermediate boundary
			
			const String* dummy_from;
			String* dummy_to;
			result << message_hash_to_string(r, messages.get(i)->get_hash(), level+1,
				dummy_from, false, dummy_to);
		}
	}
	
	// tailer
	if(boundary)
		result << "\n\n--" << boundary << "--\n"; // finish boundary

	// return
	return result;
}


Value* VMail::get_element(const String& aname, Value& aself, bool looking_up) {
	// $fields
#ifdef WITH_MAILRECEIVE
	if(aname==MAIL_RECEIVED_ELEMENT_NAME)
		return &vreceived;
#endif

	// $CLASS,$method
	if(Value* result=VStateless_class::get_element(aname, aself, looking_up))
		return result;

	return 0;
}

#if defined(WITH_MAILRECEIVE) && defined(_MSC_VER)
#	define GNOME_LIBS "../../../../win32/gnome"
#	pragma comment(lib, GNOME_LIBS "/glib/lib/libglib-1.3-11.lib")
#	ifdef _DEBUG
#		pragma comment(lib, GNOME_LIBS "/gmime-x.x.x/Debug/libgmime.lib")
#	else
#		pragma comment(lib, GNOME_LIBS "/gmime-x.x.x/Release/libgmime.lib")
#	endif
#endif
