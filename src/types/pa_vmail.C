/**	@file
	Parser: @b mail class.
	relies on gmime library, by Jeffrey Stedfast <fejj@helixcode.com>

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

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

volatile const char * IDENT_PA_VMAIL_C="$Id: pa_vmail.C,v 1.114 2015/08/28 16:26:02 moko Exp $" IDENT_PA_VMAIL_H;

#ifdef WITH_MAILRECEIVE
extern "C" {
#include "gmime/gmime.h"
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

static const char* const part_name_begins[P_TYPES_COUNT] = {
	"text",
	"html",
	"file",
	"message"
};

// defines for statics

#define FORMAT_NAME "format"
#define CHARSET_NAME "charset"
#define CID_NAME "content-id"

#define CONTENT_TRANSFER_ENCODING_NAME "content-transfer-encoding"
#define CONTENT_TRANSFER_ENCODING_CAPITALIZED "Content-Transfer-Encoding"

// statics

static const String format_name(FORMAT_NAME);
static const String charset_name(CHARSET_NAME);
static const String cid_name(CID_NAME);

static const String content_transfer_encoding_name(CONTENT_TRANSFER_ENCODING_NAME);

// consts

const int MAX_CHARS_IN_HEADER_LINE=500;

// VMail

extern Methoded* mail_base_class;

VMail::VMail(): VStateless_class(0, mail_base_class) {}

#ifdef WITH_MAILRECEIVE

#define EXCEPTION_VALUE "x-exception"

static Charset* source_charset;

static const char *transcode(const char *value) {
	if(value && !source_charset->isUTF8()){
		String::C transcoded=Charset::transcode(String::C(value, strlen(value)), UTF8_charset, *source_charset);
		value=transcoded.str;
	}
	return value;
}

static void putReceived(HashStringValue& received, const char* name, Value* value, bool capitalizeName=false) {
	if(name && value)
		received.put(capitalizeName ? capitalize(pa_strdup(name)) : pa_strdup(name), value);
}

static void putReceived(HashStringValue& received, const char* name, const char* value, bool capitalizeName=false) {
	if(name && value)
		putReceived(received, name, new VString(*new String(pa_strdup(value), String::L_TAINTED)), capitalizeName);
}

static void putReceived(HashStringValue& received, const char* name, time_t value) {
	if(name)
		received.put(pa_strdup(name), new VDate(value) );
}

static void MimeHeaderField2received(const char* name, const char* value, gpointer data) {
	HashStringValue* received=static_cast<HashStringValue*>(data);
	putReceived(*received, name, value, true /*capitalizeName*/);
}

static void parse(Request& r, GMimeMessage *message, HashStringValue& received);

#ifndef DOXYGEN
struct MimePart2body_info {
	Request* r;
	HashStringValue* body;
	int partCounts[P_TYPES_COUNT];
};
#endif


static char *readStream(GMimeStream* gstream, size_t &length){
	length=MAX_STRING;
	char *result=(char*)pa_malloc_atomic(length+1);
	char *ptr=result;

	while(true) {
		size_t current_size=ptr-result;
		ssize_t todo_size=length-current_size;
		ssize_t received_size=g_mime_stream_read (gstream, ptr, todo_size);

		if(received_size<0)
			throw Exception(PARSER_RUNTIME, 0,"mail content stream read error");
		if(received_size==0)
			break;
		if(received_size==todo_size) {
			length=length*2;
			result=(char *)pa_realloc(result, length+1);
			ptr=result+current_size+received_size;
		} else {
			ptr+=received_size;
		}
	}

	length=ptr-result;
	result[length]='\0';
	return result;
}

static void MimePart2body(GMimeObject *parent, GMimeObject *part, gpointer data) {
	MimePart2body_info& info=*static_cast<MimePart2body_info *>(data);

	// skipping message/partial & frames
	if (GMIME_IS_MESSAGE_PARTIAL (part) || GMIME_IS_MULTIPART (part))
		return;
	
	if (GMimeContentType *type=g_mime_object_get_content_type(part)) {
		PartType partType=P_FILE;
		
		if (GMIME_IS_MESSAGE_PART(part)){
			partType=P_MESSAGE;
		} else {
			const char *disposition=g_mime_object_get_disposition(part);
			if(!disposition || strcmp(disposition, GMIME_DISPOSITION_ATTACHMENT)){
				if(g_mime_content_type_is_type(type, "text", "plain"))
					partType=P_TEXT;
				else if(g_mime_content_type_is_type(type, "text", "html"))
					partType=P_HTML;
			}
		}

		// partName
		int partNumber=++info.partCounts[partType];
		const char *partName=part_name_begins[partType];
		
		char partNameNumbered[MAX_STRING];
		snprintf(partNameNumbered, MAX_STRING, "%s%d", partName, partNumber);

		// $.partN[
		VHash* vpartHash(new VHash);
		if(partNumber==1)
			putReceived(*info.body, partName, vpartHash);
		putReceived(*info.body, partNameNumbered, vpartHash);

		HashStringValue& partHash=vpartHash->hash();

		// $.raw[
		VHash* vraw(new VHash);
		putReceived(partHash, RAW_NAME, vraw);
		g_mime_header_list_foreach(part->headers, MimeHeaderField2received, &vraw->hash());

		// $.content-type[
		VHash* vcontent_type(new VHash);
		putReceived(partHash, "content-type", vcontent_type);

		// $.value[text/plain]
		char value[MAX_STRING];
		snprintf(value, MAX_STRING, "%s/%s", type->type ? type->type : "x-unknown", type->subtype ? type->subtype : "x-unknown");
		putReceived(vcontent_type->hash(), VALUE_NAME, value);

		const GMimeParam *param=g_mime_content_type_get_params(type);
		while(param) {
			// $.charset[windows-1251]  && co
			putReceived(vcontent_type->hash(), g_mime_param_get_name(param), transcode(g_mime_param_get_value(param)), true /*capitalizeName*/);
			param=g_mime_param_next(param);
		}

		if (GMIME_IS_MESSAGE_PART (part)) {
			/* message/rfc822, $.raw[] will be overwitten */
			GMimeMessage *message = g_mime_message_part_get_message ((GMimeMessagePart *) part);
			parse(*info.r, message, partHash);
		} else {
			GMimePart *gpart = (GMimePart *)part;

			putReceived(partHash, "description", g_mime_part_get_content_description(gpart));
			putReceived(partHash, "content-id", g_mime_part_get_content_id(gpart));
			putReceived(partHash, "content-md5", g_mime_part_get_content_md5(gpart));
			putReceived(partHash, "content-location", g_mime_part_get_content_location(gpart));

			// $.value[string|file]
			if(GMimeDataWrapper* gcontent=g_mime_part_get_content_object(gpart)){
				GMimeStream* gstream=g_mime_stream_filter_new(g_mime_data_wrapper_get_stream(gcontent));
				
				if(GMimeFilter* filter=g_mime_filter_basic_new(g_mime_part_get_content_encoding(gpart), false))
					g_mime_stream_filter_add(GMIME_STREAM_FILTER(gstream), filter);
				
				size_t length;
				
				if(partType==P_FILE) {
					char *content=readStream(gstream, length);
					const char* content_filename=transcode(g_mime_part_get_filename(gpart));
					VFile* vfile(new VFile);
					vfile->set_binary(true/*tainted*/, content, length, new String(content_filename), content_filename ? new VString(info.r->mime_type_of(content_filename)) : 0);
					putReceived(partHash, VALUE_NAME, vfile);
				} else {
					// P_TEXT, P_HTML
					if(Value *charset=vcontent_type->hash().get("Charset"))
						if(GMimeFilter* filter=g_mime_filter_charset_new(charset->get_string()->cstr(), source_charset->NAME_CSTR()))
							g_mime_stream_filter_add(GMIME_STREAM_FILTER(gstream), filter);
					
					char *content=readStream(gstream, length);
					putReceived(partHash, VALUE_NAME,new VString(*new String(content)));
				}
			}
		}
	}
}

static void parse(Request& r, GMimeMessage *message, HashStringValue& received) {
	try {
		// firstly user-defined strings go
		//  user headers
		{
			// $.raw[
			VHash* vraw(new VHash);  putReceived( received, RAW_NAME, vraw);
			g_mime_header_list_foreach(g_mime_object_get_header_list(GMIME_OBJECT(message)), MimeHeaderField2received, &vraw->hash());
		}

		//  secondly standard headers
		putReceived(received, "message-id", g_mime_message_get_message_id(message));
		putReceived(received, "from", transcode(g_mime_message_get_sender(message)));

		const char *msg_to=internet_address_list_to_string(g_mime_message_get_recipients(message, GMIME_RECIPIENT_TYPE_TO), false);
		putReceived(received, "to", transcode(msg_to));
		const char *msg_cc=internet_address_list_to_string(g_mime_message_get_recipients(message, GMIME_RECIPIENT_TYPE_CC), false);
		putReceived(received, "cc", transcode(msg_cc));

		putReceived(received, "reply-to", transcode(g_mime_message_get_reply_to(message)));
		putReceived(received, "subject", transcode(g_mime_message_get_subject(message)));

		// @todo: g_mime_message_get_recipients(message)
		
		// .date(time_t in UTC)
		time_t date;
		g_mime_message_get_date(message, &date, 0);
		putReceived(received, "date", date);

		// .body[part/parts
		MimePart2body_info info={&r, &received, {0}};
		g_mime_message_foreach(message, MimePart2body, &info);

	} catch(const Exception& e) {
		putReceived(received, VALUE_NAME, "<exception occured while parsing message>");
		putReceived(received, EXCEPTION_VALUE, e.comment());
	} catch(...) {
		putReceived(received, VALUE_NAME, "<exception occured while parsing message>");
	}
}

void VMail::fill_received(Request& r) {
	if(r.request_info.mail_received) {
		source_charset=&r.charsets.source();
		g_mime_init(0);

		// create stream with CRLF filter
		GMimeStream *stream = g_mime_stream_filter_new(g_mime_stream_file_new(stdin));
		g_mime_stream_filter_add(GMIME_STREAM_FILTER(stream), g_mime_filter_crlf_new(false, false));
		try {
			// parse incoming message
			GMimeMessage *message=g_mime_parser_construct_message(g_mime_parser_new_with_stream(stream));
			parse(r, message, vreceived.hash());
			g_object_unref(GMIME_OBJECT(message));
		} catch(const Exception& e) {
			HashStringValue& received=vreceived.hash();
			putReceived(received, VALUE_NAME, "<exception occured while parsing message>");
			putReceived(received, EXCEPTION_VALUE, e.comment());
		} catch(...) {
			// abnormal stream free
			g_object_unref(stream);
			rethrow;
		}
		g_object_unref(stream);
		
		g_mime_shutdown();
	}
}

#else // WITH_MAILRECEIVE
void VMail::fill_received(Request&){}
#endif // WITH_MAILRECEIVE

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
		|| low_element_name==CID_NAME
		|| low_element_name==MAIL_DEBUG_NAME)
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
	if(low_element_name==CONTENT_DISPOSITION)
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
		<< capitalize(raw_element_name.cstr())
		<< ": " << mail_line.untaint_cstr(String::L_AS_IS, 0, &info->charsets)
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
	Store_message_element_info info(r.charsets, result, dummy_from, false, dummy_to);

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

	const char* file_name_cstr;
	const char* quoted_file_name_cstr;
	{
		Request_charsets charsets(r.charsets.source(), r.charsets.mail()/*uri!*/, r.charsets.mail());
		file_name_cstr=file_name->untaint_and_transcode_cstr(String::L_FILE_SPEC, &charsets);
		quoted_file_name_cstr=String(file_name_cstr).taint_cstr(String::L_MAIL_HEADER, 0, &charsets);
	}

	// Content-Type: application/octet-stream
	result
		<< HTTP_CONTENT_TYPE_CAPITALIZED ": "
		<< r.mime_type_of(file_name_cstr)
		<< "; name=\""
		<< quoted_file_name_cstr
		<< "\"\n";

	if(!info.had_content_disposition) // $.Content-Disposition wasn't specified by user
		result
			<< CONTENT_DISPOSITION_CAPITALIZED ": "
			<< ( vcid ? CONTENT_DISPOSITION_INLINE : CONTENT_DISPOSITION_ATTACHMENT )
			<< "; "
			<< CONTENT_DISPOSITION_FILENAME_NAME"=\"" << quoted_file_name_cstr << "\"\n";

	if(vcid)
		result
			<< "Content-Id: <"
			<< vcid->as_string()
			<< ">\n"; // @todo: value must be escaped as %hh

	const String* type=vformat?&vformat->as_string():0;
	if(!type/*default*/ || *type=="base64") {
		result << CONTENT_TRANSFER_ENCODING_CAPITALIZED ": base64\n\n";
		result << pa_base64_encode(vfile->value_ptr(), vfile->value_size());
	} else {
		if(*type=="uue") {
			result << CONTENT_TRANSFER_ENCODING_CAPITALIZED ": x-uuencode\n\n";
			result << pa_uuencode((const unsigned char*)vfile->value_ptr(), vfile->value_size(), file_name_cstr);
		} else
			throw Exception(PARSER_RUNTIME,
				type,
				"unknown attachment encode format");
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
		info.content_type=0;
		info.backward_compatibility=false; // reset
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
			<< HTTP_CONTENT_TYPE_CAPITALIZED ": text/" << (pt==P_TEXT?"plain":"html")
			<< "; charset=" << info.charsets.mail().NAME()
			<< "\n";
	}
	if(!content_transfer_encoding)
		result << CONTENT_TRANSFER_ENCODING_CAPITALIZED << ": 8bit\n";

	// header|body separator
	result << "\n"; 

	// body
	const String* body;
	switch(pt) {
	case P_TEXT:
		{
			body=&text_value->as_string();
			break;
		}
	case P_HTML: 
		{
			Temp_lang temp_lang(r, String::Language(String::L_HTML | String::L_OPTIMIZE_BIT));
			if(text_value->get_junction())
				body=&r.process_to_string(*text_value);
			else
				throw Exception(PARSER_RUNTIME,
					0,
					"html part value must be code");

			break;
		}
	default:
		throw Exception(0,
			0,
			"unhandled part type #%d", pt);
	}
	if(body) {
		Request_charsets charsets(r.charsets.source(), r.charsets.mail()/*uri!*/, r.charsets.mail());
		const char* body_cstr=body->untaint_and_transcode_cstr(String::L_AS_IS, &charsets);
		result.append_know_length(body_cstr, strlen(body_cstr), String::L_CLEAN);
	}

	return result;
}

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
			result << "Errors-To: postmaster\n"; // errors-to: default
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
		
		result << HTTP_CONTENT_TYPE_CAPITALIZED ": " << ( is_inline ? HTTP_CONTENT_TYPE_MULTIPART_RELATED : HTTP_CONTENT_TYPE_MULTIPART_MIXED ) << ";";

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
				HTTP_CONTENT_TYPE_CAPITALIZED ": multipart/alternative; boundary=\"ALT" << boundary << "\"\n";
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


Value* VMail::get_element(const String& aname) {
	// $fields
#ifdef WITH_MAILRECEIVE
	if(aname==MAIL_RECEIVED_ELEMENT_NAME)
		return &vreceived;
#endif

	// $method
	if(Value* result=VStateless_class::get_element(aname))
		return result;

	return bark("%s field not found", &aname);
}
