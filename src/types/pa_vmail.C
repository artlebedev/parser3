/**	@file
	Parser: @b mail class.
	relies on gmime library, by Jeffrey Stedfast <fejj@helixcode.com>

	Copyright(c) 2001, 2002 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

static const char* IDENT_VMAIL_C="$Date: 2002/09/10 08:41:00 $";

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

static const char * const part_name_starts[P_TYPES_COUNT]={"text", "html", "file", "message"};

// VMail

extern Methoded *mail_base_class;

VMail::VMail(Pool& apool) : VStateless_class(apool, 0, mail_base_class),
	vreceived(apool) {
}

#ifdef WITH_MAILRECEIVE

static const String& maybeUpperCase(Pool& pool, const String& src, bool toUpperCase) {
	return toUpperCase?src.change_case(pool, String::CC_UPPER):src;
}

static void UTF8toSource(Pool& pool, const char *source_body, size_t source_content_length,
						 const void *& dest_body, size_t& dest_content_length) {
	if(source_body) {
		if(!source_content_length)
			source_content_length=strlen(source_body);
		Charset::transcode(pool,
									   *utf8_charset, source_body, source_content_length,
									   pool.get_source_charset(), dest_body, dest_content_length);
	} else {
		dest_body=0;
		dest_content_length=0;
	}
}

static void putReceived(Hash& received, const char *name, Value *value, bool nameToUpperCase=false) {
	Pool& pool=received.pool();
	if(name && value) {
		received.put(
			maybeUpperCase(pool, String::OnPool(pool, name, 0, true/*tainted*/), nameToUpperCase),
			value);
	}
}

static void putReceived(Hash& received, const char *name, const char *value, size_t value_size=0, bool nameToUpperCase=false) {
	if(value) {
		Pool& pool=received.pool();

		const void *value_dest_body;
		size_t value_dest_content_length;
		// UTF8toSource(pool, value, value_size, value_dest_body, value_dest_content_length);
		value_dest_body=value;
		value_dest_content_length=value_size;
		
		putReceived(received, name, 
			new(pool) VString(
				String::OnPool(pool,(const char *)value_dest_body, value_dest_content_length, true/*tainted*/)));
	}
}

static void putReceived(Hash& received, const char *name, time_t value) {
	Pool& pool=received.pool();
	if(name)
		received.put(String::OnPool(pool, name, 0, true/*tainted*/), new(pool) VDate(pool, value));
}

static void MimeHeaderField2received(const char *name, const char *value, gpointer data) {
	Hash& received=*static_cast<Hash *>(data);

	putReceived(received, name, value, 0, true/*nameInUpperCase*/);
}

static void parse(Request& r, GMimeStream *stream, Hash& received);

#ifndef DOXYGEN
struct MimePart2bodyInfo {
	Request *r;
	Hash *body;
	int partCounts[P_TYPES_COUNT];
};
#endif
/// @test why no copy to global in P_HTML putReceived?
static void MimePart2body(GMimePart *part,
						  gpointer data) {
	MimePart2bodyInfo& i=*static_cast<MimePart2bodyInfo *>(data);
	Pool& pool=i.body->pool();

	if(const GMimeContentType *type=g_mime_part_get_content_type(part)) {
		if(g_mime_content_type_is_type(type, "multipart", "*"))
			return; // skipping frames

		PartType partType;
		if(g_mime_content_type_is_type(type, "text", "plain"))
			partType=P_TEXT;
		else if(g_mime_content_type_is_type(type, "text", "html"))
			partType=P_HTML;
		else if(g_mime_content_type_is_type(type, "message", "*"))
			partType=P_MESSAGE;
		else
			partType=P_FILE;
		
		// partName
		const char *partName;
		char partNameBuf[MAX_STRING];
		const char *partNameStart=part_name_starts[partType];
		if(int partNo=i.partCounts[partType]++) {
			snprintf(partNameBuf, MAX_STRING, "%s%d", partNameStart, partNo);
			partName=partNameBuf;
		} else
			partName=partNameStart;
		
		// $.partX[ 
		VHash& vpartX=*new(pool) VHash(pool);  putReceived(*i.body, partName, &vpartX);
		Hash& partX=vpartX.hash(0);			
		{
			// $.raw[
			VHash& vraw=*new(pool) VHash(pool);  putReceived(partX, RAW_NAME, &vraw);
			g_mime_header_foreach(part->headers, MimeHeaderField2received, &vraw.hash(0));
		}
		const char *content_filename=0;
		{
			// $.content-type[ 
			VHash& vcontent_type=*new(pool) VHash(pool);  putReceived(partX, "content-type", &vcontent_type);
			Hash& content_type=vcontent_type.hash(0); 
			{
				// $.value[text/plain] 
				char value[MAX_STRING];
				snprintf(value, MAX_STRING, "%s/%s", 
					type->type?type->type:"x-unknown",
					type->subtype?type->subtype:"x-unknown");
				putReceived(content_type, VALUE_NAME, value);
			}
			GMimeParam *param=type->params;
			while(param) {
				// $.charset[windows-1251]  && co
				putReceived(content_type, param->name, param->value, true);
				if(strcasecmp(param->name, "name")==0)
					content_filename=param->value;
				param=param->next;
			}
		}
		// $.description
		putReceived(partX, "description", part->description);
		// $.content-id
		putReceived(partX, "content-id", part->content_id);
		// $.content-md5
		putReceived(partX, "content-md5", part->content_md5);
		// $.content-location
		putReceived(partX, "content-location", part->content_location);
		
		// todo GMimePart:
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
					parse(*i.r, stream, partX);
		} else {
			// $.value[string|file]
			size_t buf_len;
			const void *local_buf=g_mime_part_get_content(part, &buf_len);
			if(partType==P_FILE) {
				VFile& vfile=*new(pool) VFile(pool);
				char *global_buf=(char *)pool.malloc(buf_len);
				memcpy(global_buf, local_buf, buf_len);
				VString *vcontent_type=content_filename?
					new(pool) VString(i.r->mime_type_of(content_filename)):0;
				vfile.set(true/*tainted*/, global_buf, buf_len, content_filename, vcontent_type);
				putReceived(partX, VALUE_NAME, &vfile);
			} else {
				// P_TEXT, P_HTML
				putReceived(partX, VALUE_NAME,(const char*)local_buf, buf_len);
			}
		}
	}
}

static void parse(Request& r, GMimeStream *stream, Hash& received) {
	Pool& pool=received.pool();

	GMimeMessage *message=g_mime_parser_construct_message(stream);
	try {
		const GMimeMessageHeader *messageHeader=message->header;
		if(!messageHeader)
			return;

		// firstly user-defined strings go
		//  user headers
		{
			// $.raw[
			VHash& vraw=*new(pool) VHash(pool);  putReceived(received, RAW_NAME, &vraw);
			g_mime_header_foreach(messageHeader->headers, MimeHeaderField2received, &vraw.hash(0));
		}

		// maybe-todo-recipients
		// x(messageHeader->recipients)

		//  secondly standard headers&body go
		//  standard header
		// .from
		putReceived(received, "from", messageHeader->from);
		// .reply-to
		putReceived(received, "reply-to", messageHeader->reply_to);
		// .to
		// todo: messageHeader->recipients
		// .subject
		putReceived(received, "subject", messageHeader->subject);
		// .date(date+gmt_offset)
		int tt_offset = 
			((messageHeader->gmt_offset / 100) *(60 * 60)) 
			+(messageHeader->gmt_offset % 100) * 60;			
		putReceived(received, "date", 
			messageHeader->date // local sender
			-tt_offset // move local sender to GMT sender
			-(timezone+(daylight?60*60*sign(timezone):0)) // move GMT sender to our local time
		);
		// .message-id
		putReceived(received, "message-id", messageHeader->message_id);

		// .body[part/parts
		GMimePart *part=message->mime_part;
		const GMimeContentType *type=g_mime_part_get_content_type(part);
		MimePart2bodyInfo info={&r, &received};
		g_mime_part_foreach(part, MimePart2body, &info);

		// normal unref
		g_mime_object_unref(GMIME_OBJECT(message));
	} catch(...) {
		// abnormal unref
		g_mime_object_unref(GMIME_OBJECT(message));
	}
}
#endif



void VMail::fill_received(Request& r) {
	// store letter to received
#ifdef WITH_MAILRECEIVE
	if(r.info.mail_received) {
		// init
		g_mime_init(GMIME_INIT_FLAG_UTF8);

		// create stream with CRLF filter
		GMimeStream *stream = g_mime_stream_fs_new(fileno(stdin));
		GMimeStream *istream = g_mime_stream_filter_new_with_stream(stream);
		GMimeFilter *filter = g_mime_filter_crlf_new(GMIME_FILTER_CRLF_DECODE, GMIME_FILTER_CRLF_MODE_CRLF_ONLY);
		g_mime_stream_filter_add(GMIME_STREAM_FILTER(istream), filter);
		g_mime_stream_unref(stream);
		stream = istream;
		try {
			// parse incoming stream
			parse(r, stream, vreceived.hash(0));
			// normal stream free 
			g_mime_stream_unref(stream);
		} catch(...) {
			// abnormal stream free 
			g_mime_stream_unref(stream);
		}
	}
#endif
}

#ifndef DOXYGEN
struct Store_message_element_info {
	Charset *charset;
	String *header;
	const String **from, **to;
	const String *errors_to;
	Array *parts[P_TYPES_COUNT];
	int parts_count;
	bool has_content_type;
};
#endif
typedef int (*string_contains_char_which_check)(int);
static bool string_contains_char_which(const char *string, string_contains_char_which_check check) {
	while(char c=*string++) {
		if(check(c))
			return true;
	}
	return false;
}
const String& extractEmail(const String& string) {
	Pool& pool=string.pool();

	char *email=string.cstr();
	lsplit(email, '>'); lsplit(email, '\x0D');lsplit(email, '\x0A');
	char *next=rsplit(email, '<');
	if(next) email=next;

	String& result=*new(pool) String(pool);
	result.APPEND_TAINTED(email, 0, string.origin().file, string.origin().line);

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
	const char *exception_type="email.format";
	if(strpbrk(email, "()<>,;:\\\"[]"/*specials minus @ and . */))
		throw Exception(exception_type,
			&result,
			"email contains characters (specials)");
	if(string_contains_char_which(email, (string_contains_char_which_check)isspace))
		throw Exception(exception_type,
			&result,
			"email contains characters (whitespace)");
	if(string_contains_char_which(email, (string_contains_char_which_check)iscntrl))
		throw Exception(exception_type,
			&result,
			"email contains characters (control)");
	if(result.is_empty())
		throw Exception(exception_type,
			&string,
			"email is empty");

	return result;
}
static void store_message_element(const Hash::Key& raw_element_name, Hash::Val *aelement_value, 
								  void *info) {
	Value& element_value=*static_cast<Value *>(aelement_value);
	const String& low_element_name=raw_element_name.change_case(raw_element_name.pool(), String::CC_LOWER);
	Store_message_element_info& i=*static_cast<Store_message_element_info *>(info);

	// exclude internals
	if(low_element_name==CHARSET_NAME
		|| low_element_name==VALUE_NAME
		|| low_element_name==RAW_NAME
		|| low_element_name=="date")
		return;

	// grep parts
	for(int pt=0; pt<P_TYPES_COUNT; pt++) {
		if(low_element_name.starts_with(part_name_starts[pt])) {
			*i.parts[pt]+=&element_value;
			i.parts_count++;
			return;
		}
	}

	// fetch some special headers
	if(i.from && low_element_name=="from")
		*i.from=&extractEmail(element_value.as_string());
	if(i.to && low_element_name=="to")
		*i.to=&extractEmail(element_value.as_string());
	if(low_element_name=="errors-to")
		i.errors_to=&extractEmail(element_value.as_string());	

	// append header line
	*i.header << 
		raw_element_name << ":" << 
		attributed_meaning_to_string(element_value, String::UL_MAIL_HEADER).
			cstr(String::UL_UNSPECIFIED, 0, i.charset, i.charset?i.charset->name().cstr():0) << 
		"\n";

	// has content type?
	if(low_element_name==CONTENT_TYPE_NAME)
		i.has_content_type=true;
}

static const String& file_value_to_string(Request& r, const String *source, 
										 Value& send_value) {
	Pool& pool=r.pool();
	const VFile *vfile;
	const String *file_name;
	Value *vformat;
	if(Hash *send_hash=send_value.get_hash(source)) { // hash
		// $.value
		if(Value *value=static_cast<Value *>(send_hash->get(*value_name)))
			vfile=value->as_vfile(String::UL_AS_IS);
		else
			throw Exception("parser.runtime",
				source,
				"file part has no $value");

		// $.format
		vformat=static_cast<Value *>(send_hash->get(*new(pool) String(pool, "format")));

		// $.name
		if(Value *vfile_name=static_cast<Value *>(send_hash->get(
			*new(pool) String(pool, "name")))) // specified $name
			file_name=&vfile_name->as_string();
	} else {  // must be VFile
		vfile=send_value.as_vfile(String::UL_AS_IS);
		file_name=&static_cast<Value *>(vfile->fields().get(*name_name))->as_string();
		vformat=0;
	}
	const char *file_name_cstr=file_name->cstr();

	String& result=*new(pool) String(pool);

	// content-type: application/octet-stream
	result << "content-type: " << r.mime_type_of(file_name_cstr) 
		<< "; name=\"" << file_name_cstr << "\"\n";
	// content-disposition: attachment; filename="user_file_name"
	result << "content-disposition: attachment; filename=\"" << file_name_cstr << "\"\n";

	const String *type=vformat?&vformat->as_string():0;
	if(!type/*default = uue*/ || *type=="uue") {
		pa_uuencode(result, file_name_cstr, *vfile);
	} else // for now
		throw Exception("parser.runtime",
			type,
			"unknown attachment encode format");
	
	return result;
}

static const String& text_value_to_string(Request& r, const String *source, 
								 PartType pt, Value& send_value,
								 Store_message_element_info& info) {
	Pool& pool=r.pool();
	String& result=*new(pool) String(pool);

	Value *text_value;
	if(Hash *send_hash=send_value.get_hash(source)) {
		// $.USER-HEADERS
		info.has_content_type=false; // reset
		send_hash->for_each(store_message_element, &info);
		// $.value
		text_value=static_cast<Value *>(send_hash->get(*value_name));
		if(!text_value)
			throw Exception("parser.runtime",
				source,
				"%s part has no $" VALUE_NAME, part_name_starts[pt]);
	} else
		text_value=&send_value;

	if(!info.has_content_type) {
		result << "content-type: text/" << (pt==P_TEXT?"plain":"html");
		if(info.charset)
			result << "; charset=" << info.charset->name();
		result << "\n";
	}

	// header|body separator
	result << "\n"; 

	// body
	switch(pt) {
	case P_TEXT:
		result<<text_value->as_string();
		break;
	case P_HTML: 
		{
			Temp_lang temp_lang(r, String::UL_HTML | String::UL_OPTIMIZE_BIT);
			if(Junction *junction=text_value->get_junction())
				result << r.process_to_string(*text_value);
			else
				throw Exception("parser.runtime",
					source,
					"html part value must be code");

			break;
		}
	}

	return result;
};

/// @todo files and messages in order (file, file2, ...)
const String& VMail::message_hash_to_string(Request& r, const String *source,
											Hash *message_hash, int level, 
											const String **from, const String **to) {
	if(!message_hash)
		throw Exception("parser.runtime",
			source,
			"message must be hash");

	String& result=*NEW String(pool());

	Charset *charset;
	if(Value *vrecodecharset_name=static_cast<Value *>(message_hash->get(*charset_name)))
		charset=&charsets->get_charset(vrecodecharset_name->as_string());
	else
		charset=&pool().get_source_charset();

	Store_message_element_info info={
		charset,
		&result,
		from, to
	};
	{
		if(from)
			*from=0;
		if(to)
			*to=0;
		for(int pt=0; pt<P_TYPES_COUNT; pt++)
			info.parts[pt]=NEW Array(pool());
		message_hash->for_each(store_message_element, &info);
		if(!info.errors_to)
			result << "errors-to: postmaster\n"; // errors-to: default
	}

	int textCount=info.parts[P_TEXT]->size();
	if(textCount>1)
		throw Exception("parser.runtime",
			source,
			"multiple text parts not supported, use file part");
	int htmlCount=info.parts[P_HTML]->size();
	if(htmlCount>1)
		throw Exception("parser.runtime",
			source,
			"multiple html parts not supported, use file part");


	bool multipart=info.parts_count>1;
	bool alternative=textCount && htmlCount;
	// header
	char *boundary=0;
	if(multipart) {
		boundary=(char *)malloc(MAX_NUMBER);
		snprintf(boundary, MAX_NUMBER-5/*lEvEl*/, "lEvEl%d", level);
		// multi-part
		result << "content-type: multipart/mixed; boundary=\"" << boundary << "\"\n";
		result << "\n" 
			"This is a multi-part message in MIME format.";
	}

	// alternative or not
	{
		if(alternative) {
			result << "\n\n--" << boundary << "\n"; // intermediate boundary
			result << "content-type: multipart/alternative; boundary=\"ALT" << boundary << "\"\n";
		} 
		for(int i=0; i<2; i++) {
			PartType pt=i==0?P_TEXT:P_HTML;
			if(info.parts[pt]->size()) {
				if(alternative)
					result << "\n\n--ALT" << boundary << "\n"; // intermediate boundary
				else if(boundary)
					result << "\n\n--" << boundary << "\n";  // intermediate boundary
				result << text_value_to_string(r, source, pt, 
					*static_cast<Value *>(info.parts[pt]->get(0)), info);
			}
		}
		if(alternative)
			result << "\n\n--ALT" << boundary << "--\n";
	}

	// files
	{
		Array& files=*info.parts[P_FILE];
		for(int i=0; i<files.size(); i++) {
			if(boundary)
				result << "\n\n--" << boundary << "\n";  // intermediate boundary
			result << file_value_to_string(r, source, *static_cast<Value *>(files.get(i)));
		}
	}

	// messages
	{
		Array& messages=*info.parts[P_MESSAGE];
		for(int i=0; i<messages.size(); i++) {
			if(boundary)
				result << "\n\n--" << boundary << "\n";  // intermediate boundary
			
			result << message_hash_to_string(r, source,
				static_cast<Value *>(messages.get(i))->get_hash(source), 
				level+1);
		}
	}
	
	// tailer
	if(boundary)
		result << "\n\n--" << boundary << "--\n"; // finish boundary

	// return
	return result;
}


Value *VMail::get_element(const String& aname, Value *aself, bool looking_up) {
	// $fields
#ifdef WITH_MAILRECEIVE
	if(aname==MAIL_RECEIVED_ELEMENT_NAME)
		return &vreceived;
#endif

	// $CLASS,$method
	if(Value *result=VStateless_class::get_element(aname, aself, looking_up))
		return result;

	return 0;
}

#if defined(WITH_MAILRECEIVE) && defined(_MSC_VER)
#	define GNOME_LIBS "/parser3project/win32mailreceive/win32/gnome"
#	pragma comment(lib, GNOME_LIBS "/glib/lib/libglib-1.3-11.lib")
#	ifdef _DEBUG
#		pragma comment(lib, GNOME_LIBS "/gmime-x.x.x/Debug/libgmime.lib")
#	else
#		pragma comment(lib, GNOME_LIBS "/gmime-x.x.x/Release/libgmime.lib")
#	endif
#endif
