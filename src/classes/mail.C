/** @file
	Parser: @b mail parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_MAIL_C="$Date: 2002/12/26 14:01:46 $";

#include "pa_config_includes.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_exec.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_uue.h"

#ifdef _MSC_VER
#	include "smtp/smtp.h"
#endif

// defines

#define MAIL_CLASS_NAME "mail"

#define MAIL_NAME "MAIL"

// consts

const int ATTACHMENT_WEIGHT=100;

// global variable

Methoded *mail_base_class;

// class

class MMail : public Methoded {
public:
	MMail(Pool& pool);
public: // Methoded
	bool used_directly() { return false; }
	void configure_user(Request& r);
private:
	String mail_name;
};

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
		pa_uuencode(result, file_name_cstr, *vfile);
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
	const String **from;
	String **to;
	const String *errors_to;
	bool mime_version_specified;
};
#endif

String& extractEmails(const String& string);
static void add_header_attribute(const Hash::Key& raw_element_name, Hash::Val *aelement_value, 
								 void *info) {
	Value& element_value=*static_cast<Value *>(aelement_value);
	const String& low_element_name=raw_element_name.change_case(raw_element_name.pool(), String::CC_LOWER);
	Mail_info& mi=*static_cast<Mail_info *>(info);

	// exclude one attribute [body]
	if(low_element_name==BODY_NAME
		|| low_element_name==CHARSET_NAME)
		return;

	// fetch from & to from header for SMTP
	if(mi.from && low_element_name=="from")
		*mi.from=&extractEmails(element_value.as_string());
	if(mi.to && low_element_name=="to")
		*mi.to=&extractEmails(element_value.as_string());
	if(low_element_name=="errors-to")
		mi.errors_to=&extractEmails(element_value.as_string());
	if(low_element_name=="mime-version")
		mi.mime_version_specified=true;

	// append header line
	*mi.header << 
		low_element_name << ":" << 
		attributed_meaning_to_string(element_value, String::UL_MAIL_HEADER, true).
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
static const String& message_hash_to_string(Request& r, const String& method_name, 
										   Hash& message_hash, int level,
										   const String **from, String **to) {
	Pool& pool=r.pool();

	// prepare header: 'hash' without "body"
	String& result=*new(pool) String(pool);

	Charset *charset;
	if(Value *vrecodecharset_name=static_cast<Value *>(message_hash.get(*charset_name)))
		charset=&charsets->get_charset(vrecodecharset_name->as_string());
	else
		charset=&pool.get_source_charset();

	const char *content_charset_name=0;
	if(Value *vcontent_type=static_cast<Value *>(message_hash.get(*content_type_name)))
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
	message_hash.for_each(add_header_attribute, &mail_info);
	if(!mail_info.errors_to)
		result << "errors-to: postmaster\n"; // errors-to: default
	if(!mail_info.mime_version_specified)
		result << "MIME-Version: 1.0\n"; // MIME-Version: default

	if(Value *body_element=static_cast<Value *>(message_hash.get(*body_name))) {
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
						result << message_hash_to_string(r, method_name, *part_hash, 
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
			const char *body_ptr=body.cstr(String::UL_UNSPECIFIED);  // body
			size_t body_size=strlen(body_ptr);  // body
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
					 const String& message, 
					 const String *from, const String *to) {
	Pool& pool=r.pool();

	char *message_cstr=message.cstr(String::UL_UNSPECIFIED);
	Hash *mail_conf=static_cast<Hash *>(r.classes_conf.get(mail_base_class->name()));

	const char *exception_type="email.format";
	if(!from) // we use in sendmail -f {from} && SMTP MAIL from: {from}
		throw Exception(exception_type,
			&method_name,
			"parameter does not specify 'from' header field");

#ifdef _MSC_VER
	if(!to) // we use only in SMTP RCPT to: {to}
		throw Exception(exception_type,
			&method_name,
			"parameter does not specify 'to' header field");

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

		smtp.Send(server, port, message_cstr, from->cstr(), to->cstr());
	} else
		throw Exception("parser.runtime",
			&method_name,
			"$"MAIN_CLASS_NAME":"MAIL_NAME".SMTP not defined");
#else
	// unix
	// $MAIN:MAIL.sendmail["/usr/sbin/sendmail -t -i -f postmaster"] default
	// $MAIN:MAIL.sendmail["/usr/lib/sendmail -t -i  -f postmaster"] default

	const String *sendmail_command;
	const char *sendmailkey_cstr="sendmail";
	if(mail_conf) {
#ifdef PA_FORCED_SENDMAIL
		throw Exception("parser.runtime",
			&method_name,
			"Parser was configured with --with-sendmail="PA_FORCED_SENDMAIL
			" key, to change sendmail you should reconfigure and recompie it");
#else
		if(Value *sendmail_value=static_cast<Value *>(mail_conf->get(String(pool, sendmailkey_cstr))))
			sendmail_command=&sendmail_value->as_string();
		else
			throw Exception("parser.runtime",
				&method_name,
				"$"MAIN_CLASS_NAME":"MAIL_NAME".%s not defined", 
				sendmailkey_cstr);
#endif
	} else {
#ifdef PA_FORCED_SENDMAIL
		sendmail_command=new(pool) String(pool, PA_FORCED_SENDMAIL);
#else
		String *test=new(pool) String(pool, "/usr/sbin/sendmail");
		if(!file_executable(*test))
			test=new(pool) String(pool, "/usr/lib/sendmail");
		test->APPEND_CONST(" -t -i -f postmaster");
		sendmail_command=test;
#endif
	}

	// we know sendmail_command here, should replace "postmaster" with "$from" from message
	int at_postmaster=sendmail_command->pos("postmaster");
	if(at_postmaster>0) {
		String& reconstructed=sendmail_command->mid(0, at_postmaster);
		reconstructed << *from;
		reconstructed << sendmail_command->mid(at_postmaster+10/*postmaster*/, sendmail_command->size());
		sendmail_command=&reconstructed;
	}

	// execute it
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
		throw Exception("email.send",
			file_spec, 
			"is not executable."
#ifdef PA_FORCED_SENDMAIL
			" Use configure key \"--with-sendmail=appropriate sendmail command\""
#else
			" Set $"MAIN_CLASS_NAME":"MAIL_NAME".%s to appropriate sendmail command", 
				sendmailkey_cstr
#endif
		);


	String in(pool, message_cstr); String out(pool); String err(pool);
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
		throw Exception("email.send",
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

	const String *from=0;
	String *to=0;
	String **to_param=
#ifdef _MSC_VER
		&to
#else
		0
#endif
		;
	const String& message=hash->get(*body_name)/*old format*/?
		message_hash_to_string(r, method_name, *hash, 0, &from, to_param) : // old
		static_cast<VMail *>(r.get_self())->message_hash_to_string(r, &method_name, hash, 0, &from, to_param); // new

	//r.write_pass_lang(message);
	sendmail(r, method_name, message, from, to);
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
	if(Value *mail_element=r.main_class.get_element(mail_name, r.main_class, false))
		if(Hash *mail_conf=mail_element->get_hash(0))
			r.classes_conf.put(name(), mail_conf);
		else
			if( !mail_element->is_string() )
				throw Exception("parser.runtime",
					0,
					"$" MAIL_CLASS_NAME ":" MAIL_NAME " is not hash");
}

// creator

Methoded *MMail_create(Pool& pool) {
	return mail_base_class=new(pool) MMail(pool);
}
