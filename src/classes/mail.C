/** @file
	Parser: @b mail parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: mail.C,v 1.65 2002/06/12 12:50:32 paf Exp $
*/

#include "pa_config_includes.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_exec.h"
#include "pa_charsets.h"
#include "pa_charset.h"

#ifdef _MSC_VER
#	include "smtp/smtp.h"
#endif

// defines

#define MAIL_CLASS_NAME "mail"

#define MAIL_NAME "MAIL"

// global variable

Methoded *mail_class;

// consts

const int ATTACHMENT_WEIGHT=100;

// class

class MMail : public Methoded {
public:
	MMail(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
	void configure_user(Request& r);
private:
	String mail_name;
};

// helpers

// uuencode

static unsigned char uue_table[64] = {
  '`', '!', '"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', ':', ';', '<', '=', '>', '?',
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
  'X', 'Y', 'Z', '[', '\\',']', '^', '_'
};
static void uuencode(String& result, const char *file_name_cstr, const VFile& vfile) {
	//header
	result << "content-transfer-encoding: x-uuencode\n" << "\n";
	result << "begin 644 " << file_name_cstr << "\n";

	//body
	const unsigned char *in=(const unsigned char *)vfile.value_ptr();
	size_t in_length=vfile.value_size();

	int count=45;
	for(const unsigned char *itemp=in; itemp<(in+in_length); itemp+=count) {
		int index;	

		if((itemp+count)>(in+in_length)) 
			count=in_length-(itemp-in);

		char *buf=(char *)result.pool().malloc(MAX_STRING);
		char *optr=buf;
		
		/*
		* for UU and XX, encode the number of bytes as first character
		*/
		*optr++ = uue_table[count];
		
		for (index=0; index<=count-3; index+=3) {
			*optr++ = uue_table[itemp[index] >> 2];
			*optr++ = uue_table[((itemp[index  ] & 0x03) << 4) | (itemp[index+1] >> 4)];
			*optr++ = uue_table[((itemp[index+1] & 0x0f) << 2) | (itemp[index+2] >> 6)];
			*optr++ = uue_table[  itemp[index+2] & 0x3f];
		}
		
		/*
		* Special handlitempg for itempcomplete litempes
		*/
		if (index != count) {
			if (count - index == 2) {
				*optr++ = uue_table[itemp[index] >> 2];
				*optr++ = uue_table[((itemp[index  ] & 0x03) << 4) | 
					( itemp[index+1] >> 4)];
				*optr++ = uue_table[((itemp[index+1] & 0x0f) << 2)];
				*optr++ = uue_table[0];
			}
			else if (count - index == 1) {
				*optr++ = uue_table[ itemp[index] >> 2];
				*optr++ = uue_table[(itemp[index] & 0x03) << 4];
				*optr++ = uue_table[0];
				*optr++ = uue_table[0];
			}
		}
		/*
		* end of line
		*/
		*optr++ = '\n';	
		*optr = 0;
		result << buf;
	}
	
	//footer
	result.APPEND_AS_IS((const char *)uue_table, 1/* one char */, 0, 0) << "\n"
		"end\n";
}

/** ^mail:send[$attach[$type[uue|mime64] $value[DATA]]] 
	@todo solve - bad with html attaches
*/
static const String& attach_hash_to_string(Request& r, const String& origin_string, 
										   Hash& attach_hash) {
	Pool& pool=r.pool();

	Value *vformat=static_cast<Value *>(attach_hash.get(*new(pool) String(pool, "format")));

	const VFile *vfile;
	if(Value *value=static_cast<Value *>(attach_hash.get(*value_name)))
		vfile=value->as_vfile(String::UL_AS_IS); // bad with html attaches. todo: solve
	else
		throw Exception("parser.runtime",
			&origin_string,
			"has no $value");

	const String *file_name;
	if(Value *vfile_name=static_cast<Value *>(attach_hash.get(
		*new(pool) String(pool, "file-name")))) // specified $file-name
		file_name=&vfile_name->as_string();
	else // no $file-name, VFile surely knows name
		file_name=&static_cast<Value *>(vfile->fields().get(*name_name))->as_string();
	const char *file_name_cstr=file_name->cstr();

	String& result=*new(pool) String(pool);

	// content-type: application/octet-stream
	result << "content-type: " << r.mime_type_of(file_name_cstr) 
		<< "; name=\"" << file_name_cstr << "\"\n";
	// content-disposition: attachment; filename="user_file_name"
	result << "content-disposition: attachment; filename=\"" << file_name_cstr << "\"\n";

	const String *type=vformat?&vformat->as_string():0;
	if(!type/*default = uue*/ || *type=="uue") {
		uuencode(result, file_name_cstr, *vfile);
	} else // for now
		throw Exception("parser.runtime",
			type,
			"unknown attachment encode format");
	
	return result;
}


#ifndef DOXYGEN
struct Mail_info {
	Charset *charset; const char *content_charset_name;
	String *header;
	const String **from, **to;
};
#endif
static void add_header_attribute(const Hash::Key& aattribute, Hash::Val *ameaning, 
								 void *info) {

	Value& lmeaning=*static_cast<Value *>(ameaning);
	Mail_info& mi=*static_cast<Mail_info *>(info);

	// exclude one attribute [body]
	if(aattribute==BODY_NAME
		|| aattribute==CHARSET_NAME)
		return;

	// fetch from & to from header for SMTP
	if(mi.from && aattribute=="from")
		*mi.from=&lmeaning.as_string();
	if(mi.to && aattribute=="to")
		*mi.to=&lmeaning.as_string();

	// append header line
	*mi.header << 
		aattribute << ":" << 
		attributed_meaning_to_string(lmeaning, String::UL_MAIL_HEADER).
			cstr(String::UL_UNSPECIFIED, 0, mi.charset, mi.content_charset_name) << 
		"\n";
}

#ifndef DOXYGEN
struct Mail_seq_item {
	int weight;
	const String *name;
	Value *value;
};
#endif
static int get_part_name_weight(const Hash::Key& part_name) {
	const char *cstr=part_name.cstr();
	int offset=0;
	if(strncmp(cstr, "text", 4)==0) {
		cstr+=4;
	} else if(strncmp(cstr, "attach", 6)==0) {
		cstr+=6;
		offset=ATTACHMENT_WEIGHT;
	} else
		throw Exception("parser.runtime",
			&part_name,
			"is neither text# nor attach#");

	char *error_pos;
	return strtol(cstr, &error_pos, 10)+offset;
}
static void add_part(const Hash::Key& part_name, Hash::Val *part_value, 
					 void *info) {
	Mail_seq_item **seq_ref=static_cast<Mail_seq_item **>(info);
	(**seq_ref).weight=get_part_name_weight(part_name);
	(**seq_ref).name=&part_name;
	(**seq_ref).value=static_cast<Value *>(part_value);
	(*seq_ref)++;
}
static int key_of_part(const void *item) {
	return static_cast<const Mail_seq_item *>(item)->weight;
}
static int sort_cmp_string_double_value(const void *a, const void *b) {
	return key_of_part(a)-key_of_part(b);
}
static const String& letter_hash_to_string(Request& r, const String& method_name, 
										   Hash& letter_hash, int level,
										   const String **from, const String **to) {
	Pool& pool=r.pool();

	// prepare header: 'hash' without "body"
	String& result=*new(pool) String(pool);

	Charset *charset;
	if(Value *vrecodecharset_name=static_cast<Value *>(letter_hash.get(*charset_name)))
		charset=&charsets->get_charset(vrecodecharset_name->as_string());
	else
		charset=&pool.get_source_charset();

	const char *content_charset_name=0;
	if(Value *vcontent_type=static_cast<Value *>(letter_hash.get(*content_type_name)))
		if(Hash *hcontent_type=vcontent_type->get_hash(0))
			if(Value *vcontentcharset_name=static_cast<Value *>(hcontent_type->get(*charset_name)))
				content_charset_name=vcontentcharset_name->as_string().cstr();

	if(from)
		*from=0;
	if(to)
		*to=0;
	Mail_info mail_info={
		charset, content_charset_name,
		&result,
		from, to
	};
	letter_hash.for_each(add_header_attribute, &mail_info);

	if(Value *body_element=static_cast<Value *>(letter_hash.get(*body_name))) {
		if(Hash *body_hash=body_element->get_hash(&method_name)) {
			// body parts..
			// ..collect
			Mail_seq_item *seq=(Mail_seq_item *)pool.malloc(sizeof(Mail_seq_item)*body_hash->size());
			Mail_seq_item *seq_ref=seq;  body_hash->for_each(add_part, &seq_ref);
			// ..sort
			_qsort(seq, body_hash->size(), sizeof(Mail_seq_item), 
				sort_cmp_string_double_value);

			bool multipart=body_hash->size()>1;
			// header
			char *boundary=0;
			if(multipart) {
				boundary=(char *)pool.malloc(MAX_NUMBER);
				snprintf(boundary, MAX_NUMBER-5/*lEvEl*/, "lEvEl%d", level);
				// multi-part
				result << "content-type: multipart/mixed; boundary=\"" << boundary << "\"\n";
				result << "\n" 
					"This is a multi-part message in MIME format.";
			}

			// ..insert in 'seq' order
			for(int i=0; i<body_hash->size(); i++) {
				if(multipart) {
					// intermediate boundary
					result << "\n--" << boundary << "\n";
				}

				if(Hash *part_hash=seq[i].value->get_hash(&method_name))
					if(seq[i].weight>=ATTACHMENT_WEIGHT)
						result << attach_hash_to_string(r, *seq[i].name, *part_hash);
					else 
						result << letter_hash_to_string(r, method_name, *part_hash, 
							level+1, 0, 0);
				else
					throw Exception("parser.runtime",
						seq[i].name,
						"part is not hash");
			}

			// finish boundary
			if(multipart) {
				result << "\n--" << boundary << "--\n";
			}
		} else {
			result << 
				"\n"; // header|body separator

			const String& body=body_element->as_string();
			const void *body_ptr=body.cstr();  // body
			size_t body_size=body.size();  // body
			const void *mail_ptr;
			size_t mail_size;
			Charset::transcode(pool, 
				pool.get_source_charset(), body_ptr, body_size,
				*charset/*always set - either mail.charset or $request:charset*/, mail_ptr, mail_size);
			result.APPEND_CLEAN((const char*)mail_ptr, mail_size, 0, 0);
		}
	} else 
		throw Exception("parser.runtime",
			&method_name,
			"has no $body");

	return result;
}

static void sendmail(Request& r, const String& method_name, 
					 const String& letter, 
					 const String *from, const String *to) {
	Pool& pool=r.pool();

	char *letter_cstr=letter.cstr();
	Hash *mail_conf=static_cast<Hash *>(r.classes_conf.get(mail_class->name()));

#ifdef _MSC_VER
	if(!from)
		throw Exception("parser.runtime",
			&method_name,
			"has no 'from' header specified");
	if(!to)
		throw Exception("parser.runtime",
			&method_name,
			"has no 'to' header specified");

	SMTP& smtp=*new(pool) SMTP(pool, method_name);
	Value *server_port;
	// $MAIN:MAIL.SMTP[mail.yourdomain.ru[:port]]
	if(mail_conf && 
		(server_port=static_cast<Value *>(mail_conf->get(
			*new(pool) String(pool, "SMTP"))))) {
		char *server=server_port->as_string().cstr();
		const char *port=rsplit(server, ':');
		if(!port)
			port="25";

		smtp.Send(server, port, letter_cstr, from->cstr(), to->cstr());
	} else
		throw Exception("parser.runtime",
			&method_name,
			"$"MAIN_CLASS_NAME":"MAIL_NAME".SMTP not defined");
#else
	// unix
	// $MAIN:MAIL.sendmail["/usr/sbin/sendmail -t"] default
	// $MAIN:MAIL.sendmail["/usr/lib/sendmail -t"] default

	const String *sendmail_command;
#ifdef PA_FORCED_SENDMAIL
	sendmail_command=new(pool) String(pool, PA_FORCED_SENDMAIL);
#else
	const char *sendmailkey_cstr="sendmail";
	if(mail_conf) {
		if(Value *sendmail_value=static_cast<Value *>(mail_conf->get(String(pool, sendmailkey_cstr))))
			sendmail_command=&sendmail_value->as_string();
		else
			throw Exception("parser.runtime",
				&method_name,
				"$"MAIN_CLASS_NAME":"MAIL_NAME".%s not defined", 
				sendmailkey_cstr);
	} else {
		String *test=new(pool) String(pool, "/usr/sbin/sendmail");
		if(!file_executable(*test))
			test=new(pool) String(pool, "/usr/lib/sendmail");
		test->APPEND_CONST(" -t");
		sendmail_command=test;
	}
#endif

	// we know sendmail_command here
	Array argv(pool);
	const String *file_spec;
	int after_file_spec=sendmail_command->pos(" ", 1);
	if(after_file_spec<=0)
		file_spec=sendmail_command;
	else {
		size_t pos_after=after_file_spec;
		file_spec=&sendmail_command->mid(0, pos_after++);
		sendmail_command->split(argv, &pos_after, " ", 1, String::UL_AS_IS);
	}

	if(!file_executable(*file_spec))
		throw Exception(0,
			file_spec, 
			"is not executable."
#ifdef PA_FORCED_SENDMAIL
			" Use configure key \"--with-sendmail=appropriate sendmail command\""
#else
			" Set $"MAIN_CLASS_NAME":"MAIL_NAME".%s with appropriate sendmail command", 
				sendmailkey_cstr
#endif
		);


	String in(pool, letter_cstr); String out(pool); String err(pool);
	int exit_status=pa_exec(
		// forced_allow
#ifdef PA_FORCED_SENDMAIL
		true
#else
		false
#endif
		, *file_spec,
		0/*default env*/,
		&argv,
		in, out, err);
	if(exit_status || err.size())
		throw Exception(0,
			&method_name,
			"'%s' reported problem: %s (%d)",
				file_spec->cstr(),
				err.size()?err.cstr():"UNKNOWN", 
				exit_status);
#endif
}


// methods

static void _send(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& vhash=params->as_no_junction(0, "message must not be code");
	Hash *hash=vhash.get_hash(&method_name);
	if(!hash)
		throw Exception("parser.runtime",
			&method_name,
			"message must be hash");

	const String *from, *to;
	const String& letter=letter_hash_to_string(r, method_name, *hash, 0, &from, &to);

	sendmail(r, method_name, letter, from, to);
}

// constructor & configurator

MMail::MMail(Pool& apool) : Methoded(apool, MAIL_CLASS_NAME),
	mail_name(apool, MAIL_NAME)
{
	// ^mail:send{hash}
	add_native_method("send", Method::CT_STATIC, _send, 1, 1);
}

void MMail::configure_user(Request& r) {
	Pool& pool=r.pool();

	// $MAIN:MAIL[$SMTP[mail.design.ru]]
	if(Value *mail_element=r.main_class->get_element(mail_name))
		if(Hash *mail_conf=mail_element->get_hash(0))
			r.classes_conf.put(name(), mail_conf);
		else if(!mail_element->get_string())
			throw Exception("parser.runtime",
				0,
				"$" MAIN_CLASS_NAME ":" MAIL_NAME " is not hash");
}

// creator

Methoded *MMail_create(Pool& pool) {
	return mail_class=new(pool) MMail(pool);
}
