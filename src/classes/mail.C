/** @file
	Parser: @b mail parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: mail.C,v 1.4 2001/04/07 13:48:35 paf Exp $
*/

#include "pa_config_includes.h"

#include "_mail.h"
#include "pa_common.h"
#include "pa_request.h"

#ifdef WIN32
#	include "smtp/smtp.h"
#endif

// global var

VStateless_class *mail_class;

// consts

// methods

struct Mail_info {
	String *attribute_to_exclude;
	String *header;
	const String **from, **to;
};

static void add_header_attribute(const Hash::Key& aattribute, Hash::Val *ameaning, 
								 void *info) {

	Value& lmeaning=*static_cast<Value *>(ameaning);
	Mail_info& mi=*static_cast<Mail_info *>(info);

	// exclude one attribute [body]
	if(aattribute==*mi.attribute_to_exclude)
		return;

	// fetch from & to from header for SMTP
	if(mi.from && aattribute=="from")
		*mi.from=&lmeaning.as_string();
	if(mi.to && aattribute=="to")
		*mi.to=&lmeaning.as_string();

	// append header line
	*mi.header+=aattribute;
	*mi.header+=":";
	*mi.header+=attributed_meaning_to_string(lmeaning, String::UL_MAIL_HEADER);
	*mi.header+="\n";
}
struct Seq_item {
	const String *part_number;
	Value *part_value;
};
static void add_part(const Hash::Key& part_number, Hash::Val *part_value, 
					 void *info) {
	Seq_item **seq_ref=static_cast<Seq_item **>(info);
	(**seq_ref).part_number=&part_number;
	(**seq_ref).part_value=static_cast<Value *>(part_value);
	(*seq_ref)++;
}
static double key_of_part(const void *item) {
	const char *cstr=static_cast<const Seq_item *>(item)->part_number->cstr();
	char *error_pos;
	return strtod(cstr, &error_pos);
}
static int sort_cmp_string_double_value(const void *a, const void *b) {
	double va=key_of_part(a);
	double vb=key_of_part(b);
	if(va<vb)
		return -1;
	else if(va>vb)
		return +1;
	else 
		return 0;
}
static const String& letter_hash_to_string(Request& r, const String& method_name, 
										   Hash& letter_hash, int level,
										   const String **from, const String **to) {
	Pool& pool=r.pool();

	// prepare header: 'hash' without "body"
	String& result=*new(pool) String(pool);
	Mail_info mail_info={
		/*excluding*/ body_name,
		&result,
		from, to
	};
	letter_hash.for_each(add_header_attribute, &mail_info);

	if(Value *body_element=static_cast<Value *>(letter_hash.get(*body_name))) {
		if(Hash *body_hash=body_element->get_hash()) {
			char *boundary=(char *)pool.malloc(MAX_NUMBER);
			snprintf(boundary, MAX_NUMBER-6/*level_*/, "level_%d", level);
			// multi-part
			((result+=
				"content-type: multipart/mixed;\n"
				"    boundary=\"----=")+=boundary)+="\"\n"
				"\n"
				"This is a multi-part message in MIME format.";

			// body parts..
			// ..collect
			Seq_item *seq=(Seq_item *)malloc(sizeof(Seq_item)*body_hash->size());
			Seq_item *seq_ref=seq;  body_hash->for_each(add_part, &seq_ref);
			// ..sort
			_qsort(seq, body_hash->size(), sizeof(Seq_item), 
				sort_cmp_string_double_value);
			// ..insert in 'seq' order
			for(int i=0; i<body_hash->size(); i++) {
				// intermediate boundary
				((result+="\n------=")+=boundary)+="\n";

				if(Hash *part_hash=seq[i].part_value->get_hash())
					result+=letter_hash_to_string(r, method_name, *part_hash, 
						level+1, 0, 0);
				else
					PTHROW(0, 0,
						seq[i].part_number,
						"part is not hash");
			}

			// finish boundary
			((result+="\n------=")+=boundary)+="--\n";
		} else {
			result+="\n"; // header|body separator
			result+=body_element->as_string();  // body
		}
	} else 
		PTHROW(0, 0,
			&method_name,
			"has no $body");

	return result;
}


/// @test unix ver
static void sendmail(Request& r, const String& method_name, 
					 const String& letter, 
					 const String *from, const String *to) {
	Pool& pool=r.pool();

#ifdef WIN32
	if(!from)
		PTHROW(0, 0,
			&method_name,
			"not specified 'from'");
	if(!to)
		PTHROW(0, 0,
			&method_name,
			"not specified 'to'");

	char *letter_cstr=letter.cstr();
	SMTP& smtp=*new(pool) SMTP(pool, method_name);
	const String *server_port;
	// $MAIN:MAIL.SMTP[mail.design.ru]
	if(r.mail && (server_port=r.mail->get_string(*new(pool) String(pool, "SMTP")))) {
		char *server=server_port->cstr();
		char *port=rsplit(server, ':');
		if(!port)
			port="25";

		smtp.Send(server, port, letter_cstr, from->cstr(), to->cstr());
	} else
		PTHROW(0, 0,
			&method_name,
			"$MAIN:MAIL.SMTP not defined");
#else
	PTHROW(0, 0,
		&method_name,
		"todo");
#endif
}

static void _send(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value& vhash=*static_cast<Value *>(params->get(0));
	// forcing [this body type]
	r.fail_if_junction_(true, vhash, method_name, "message must not be code");

	Hash *hash=vhash.get_hash();
	if(!hash)
		PTHROW(0, 0,
			&method_name,
			"message must be hash");

	const String *from, *to;
	const String& letter=letter_hash_to_string(r, method_name, *hash, 0, &from, &to);

	sendmail(r, method_name, letter, from, to);
}

// initialize
void initialize_mail_class(Pool& pool, VStateless_class& vclass) {
	// ^mail:send{hash}
	vclass.add_native_method("send", Method::CT_STATIC, _send, 1, 1);
}
