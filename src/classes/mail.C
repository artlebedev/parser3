/** @file
	Parser: @b mail parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: mail.C,v 1.8 2001/04/07 15:42:15 paf Exp $
*/

#include "pa_config_includes.h"

#ifdef WIN32
#	include "smtp/smtp.h"
#endif

#include "_mail.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"

// global var

VStateless_class *mail_class;

// helpers

///
static void uuencode(String& result, const char *file_name_cstr, const VFile& vfile) {
	// content-transfer-encoding: x-uuencode
	result+="content-transfer-encoding: x-uuencode\n";
	result+="\n";
	result+="todo";
}

/// ^mail:send[$attach[$type[uue|mime64] $value[DATA]]] 
static const String& attach_hash_to_string(Request& r, const String& origin_string, 
										   Hash& attach_hash) {
	Pool& pool=r.pool();

	Value *vtype=static_cast<Value *>(attach_hash.get(*new(pool) String(pool, "type")));
	if(!vtype)
		PTHROW(0, 0,
			&origin_string,
			"has no $type");

	const VFile *vfile;
	if(Value *value=static_cast<Value *>(attach_hash.get(*value_name)))
		vfile=value->as_vfile();
	else
		PTHROW(0, 0,
			&origin_string,
			"has no $value");

	const String *file_name;
	if(Value *vfile_name=static_cast<Value *>(attach_hash.get(
		*new(pool) String(pool, "file-name")))) // specified $file-name
		file_name=&vfile_name->as_string();
	else // no $file-name
		if(Value *vfile_name=static_cast<Value *>(vfile->fields().get(*name_name)))
			file_name=&vfile_name->as_string(); // VFile knows name
		else // vfile doesn't know name
			file_name=new(pool) String(pool, "noname.dat");
	const char *file_name_cstr=file_name->cstr(String::UL_FILE_NAME);

	String& result=*new(pool) String(pool);

	// content-disposition: attachment; filename="user_file_name"
	result+="content-disposition: attachment; filename=\"";
	result+=file_name_cstr;
	result+="\"\n";

	const String& type=vtype->as_string();
	if(type=="uue") {
		uuencode(result, file_name_cstr, *vfile);
	} else 
		PTHROW(0, 0,
			&type,
			"unknown encode type");
	
	return result;
}


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
	const String *part_name;
	Value *part_value;
};
static void add_part(const Hash::Key& part_name, Hash::Val *part_value, 
					 void *info) {
	Seq_item **seq_ref=static_cast<Seq_item **>(info);
	(**seq_ref).part_name=&part_name;
	(**seq_ref).part_value=static_cast<Value *>(part_value);
	(*seq_ref)++;
}
static double key_of_part(const void *item) {
	const char *cstr=static_cast<const Seq_item *>(item)->part_name->cstr();
	char *error_pos;
	return strtod(cstr, &error_pos);
}
static int sort_cmp_string_double_value(const void *a, const void *b) {
	double va=key_of_part(a);
	double vb=key_of_part(b);

	// 0 logically equals infinity. so that attachments would go last
	if(va==0)
		return +1;
	if(vb==0)
		return -1;

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
			snprintf(boundary, MAX_NUMBER, "%d", level);
			// multi-part
			((result+=
				"content-type: multipart/mixed; boundary=\"")+=boundary)+="\"\n"
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
				((result+="\n--")+=boundary)+="\n";

				if(Hash *part_hash=seq[i].part_value->get_hash())
					if(seq[i].part_name->mid(0, 6/*attach*/)=="attach")
						result+=attach_hash_to_string(r, *seq[i].part_name, *part_hash);
					else 
						result+=letter_hash_to_string(r, method_name, *part_hash, 
							level+1, 0, 0);
				else
					PTHROW(0, 0,
						seq[i].part_name,
						"part is not hash");
			}

			// finish boundary
			((result+="\n--")+=boundary)+="--\n";
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
	Value *server_port;
	// $MAIN:MAIL.SMTP[mail.design.ru]
	if(r.mail && 
		(server_port=static_cast<Value *>(r.mail->get(
			*new(pool) String(pool, "SMTP"))))) {
		char *server=server_port->as_string().cstr();
		char *port=rsplit(server, ':');
		if(!port)
			port=const_cast<char *>("25");

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


// methods

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

	r.write_assign_lang(*new(pool) VString(letter));
	//sendmail(r, method_name, letter, from, to);
}

// initialize
void initialize_mail_class(Pool& pool, VStateless_class& vclass) {
	// ^mail:send{hash}
	vclass.add_native_method("send", Method::CT_STATIC, _send, 1, 1);
}
