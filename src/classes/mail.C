/** @file
	Parser: @b mail parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: mail.C,v 1.26 2001/05/04 10:42:36 paf Exp $
*/

#include "pa_config_includes.h"

#ifdef _MSC_VER
#	include "smtp/smtp.h"
#endif

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_exec.h"

// defines

#define MAIL_CLASS_NAME "mail"

#define MAIL_NAME "MAIL"

// global variable

Methoded *mail_class;

// class

class MMail : public Methoded {
public:
	MMail(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
	void configure_user(Request& r);
private:
	String mail_name;
	String content_disposition_name;
	String content_disposition_filename_name;
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
		vfile=value->as_vfile(String::UL_AS_IS); // bad with html attaches. todo: solve
	else
		PTHROW(0, 0,
			&origin_string,
			"has no $value");

	const String *file_name;
	if(Value *vfile_name=static_cast<Value *>(attach_hash.get(
		*new(pool) String(pool, "file-name")))) // specified $file-name
		file_name=&vfile_name->as_string();
	else // no $file-name, VFile surely knows name
		file_name=&static_cast<Value *>(vfile->fields().get(*name_name))->as_string();
	const char *file_name_cstr=file_name->cstr(String::UL_FILE_NAME);

	String& result=*new(pool) String(pool);

	// content-type: application/octet-stream
	result << "content-type: " << r.mime_type_of(file_name_cstr) << "\n";
	// content-disposition: attachment; filename="user_file_name"
	result << "content-disposition: attachment; filename=\"" << file_name_cstr << "\"\n";

	const String& type=vtype->as_string();
	if(type=="uue") {
		uuencode(result, file_name_cstr, *vfile);
	} else 
		PTHROW(0, 0,
			&type,
			"unknown encode type");
	
	return result;
}


static bool find_content_type(const Hash::Key& aattribute, Hash::Val *ameaning, 
							  void *) {
	return StrEqNc(aattribute.cstr(), CONTENT_TYPE_NAME);
}
static bool find_content_type_charset(const Hash::Key& aattribute, Hash::Val *ameaning, 
									  void *) {
	return StrEqNc(aattribute.cstr(), "charset");
}

/// used by mail: _send / letter_hash_to_string / add_header_attribute
struct Mail_info {
	String *attribute_to_exclude;
	const char *charset;
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
	*mi.header << 
		aattribute << ":" << 
		attributed_meaning_to_string(lmeaning, String::UL_MAIL_HEADER).
			cstr(String::UL_UNSPECIFIED, 0, mi.charset) << 
		"\n";
}

/// used in mail: _send / letter_hash_to_string / add_part
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

	const char *charset=0;
	if(Value *content_type=
		static_cast<Value *>(letter_hash.first_that(find_content_type)))
		if(Hash *hash=content_type->get_hash())
			if(Value *content_type_charset=
				static_cast<Value *>(hash->first_that(find_content_type_charset)))
				charset=content_type_charset->as_string().cstr();

	*from=*to=0;
	Mail_info mail_info={
		/*excluding*/ body_name,
		charset,
		&result,
		from, to
	};
	letter_hash.for_each(add_header_attribute, &mail_info);

	if(Value *body_element=static_cast<Value *>(letter_hash.get(*body_name))) {
		if(Hash *body_hash=body_element->get_hash()) {
			char *boundary=(char *)pool.malloc(MAX_NUMBER);
			snprintf(boundary, MAX_NUMBER-5/*lEvEl*/, "lEvEl%d", level);
			// multi-part
			result << "content-type: multipart/mixed; boundary=\"" << boundary << "\"\n";
			result << "\n" 
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
				result << "\n--" << boundary << "\n";

				if(Hash *part_hash=seq[i].part_value->get_hash())
					if(seq[i].part_name->mid(0, 6/*attach*/)=="attach")
						result << attach_hash_to_string(r, *seq[i].part_name, *part_hash);
					else 
						result << letter_hash_to_string(r, method_name, *part_hash, 
							level+1, 0, 0);
				else
					PTHROW(0, 0,
						seq[i].part_name,
						"part is not hash");
			}

			// finish boundary
			result << "\n--" << boundary << "--\n";
		} else {
			result << 
				"\n" << // header|body separator
				body_element->as_string();  // body
		}
	} else 
		PTHROW(0, 0,
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
		PTHROW(0, 0,
			&method_name,
			"has no 'from' header specified");
	if(!to)
		PTHROW(0, 0,
			&method_name,
			"has no 'to' header specified");

	SMTP& smtp=*new(pool) SMTP(pool, method_name);
	Value *server_port;
	// $MAIN:MAIL.SMTP[mail.design.ru]
	if(mail_conf && 
		(server_port=static_cast<Value *>(mail_conf->get(
			*new(pool) String(pool, "SMTP"))))) {
		char *server=server_port->as_string().cstr();
		const char *port=rsplit(server, ':');
		if(!port)
			port="25";

		smtp.Send(server, port, letter_cstr, from->cstr(), to->cstr());
	} else
		PTHROW(0, 0,
			&method_name,
			"$"MAIN_CLASS_NAME":"MAIL_NAME".SMTP not defined");
#else
	// unix
	// $MAIN:MAIL.prog1["/usr/sbin/sendmail -t"] default
	// $MAIN:MAIL.prog2["/usr/lib/sendmail -t"] default
	if(mail_conf) {
		char no_cstr[MAX_NUMBER];
		for(int no=-2; ; no++) {
			const String *prog_string;
			switch(no) {
			case -2: prog_string=new(pool) String(pool, "/usr/sbin/sendmail -t"); break;
			case -1: prog_string=new(pool) String(pool, "/usr/lib/sendmail -t"); break;
			default: 
				{
					String prog_key(pool, "prog");
					snprintf(no_cstr, MAX_NUMBER, "%d", no);
					prog_key << no_cstr;
					if(Value *prog_value=static_cast<Value *>(mail_conf->get(prog_key)))
						prog_string=&prog_value->as_string();
					else
						if(no==0)
							continue;
						else
							PTHROW(0, 0,
								&method_name,
								"$"MAIN_CLASS_NAME":"MAIL_NAME".%s not defined", 
								prog_key.cstr());
				}
			}
			// we know prog_string here
			Array argv(pool);
			const String *file_spec;
			int after_file_spec=prog_string->pos(" ", 1);
			if(after_file_spec<=0)
				file_spec=prog_string;
			else {
				size_t pos_after=after_file_spec;
				file_spec=&prog_string->mid(0, pos_after);
				prog_string->split(argv, &pos_after, " ", 1, String::UL_CLEAN);
			}

			// skip unavailable default programs
			if(no<0 && !file_executable(*file_spec))
				continue;

			String in(pool, letter_cstr); String out(pool); String err(pool);
			int exit_status=pa_exec(*file_spec,
				0/*default env*/,
				&argv,
				in, out, err);
			if(exit_status || err.size())
				PTHROW(0, 0,
					&method_name,
					"'%s' reported problem: %s (%d)",
						file_spec->cstr(),
						err.size()?err.cstr():"UNKNOWN", 
						exit_status);
			break;
		}
	} else
		PTHROW(0, 0,
			&method_name,
			"$" MAIN_CLASS_NAME ":" MAIL_NAME " not defined");
#endif
}


// methods

static void _send(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& vhash=params->get_no_junction(0, "message must not be code");
	Hash *hash=vhash.get_hash();
	if(!hash)
		PTHROW(0, 0,
			&method_name,
			"message must be hash");

	const String *from, *to;
	const String& letter=letter_hash_to_string(r, method_name, *hash, 0, &from, &to);

//	r.write_assign_lang(*new(pool) VString(letter));
	sendmail(r, method_name, letter, from, to);
}

// constructor & configurator

MMail::MMail(Pool& apool) : Methoded(apool),
	mail_name(apool, MAIL_NAME),
	content_disposition_name(apool, CONTENT_DISPOSITION_NAME),
	content_disposition_filename_name(apool, CONTENT_DISPOSITION_FILENAME_NAME)
{
	set_name(*NEW String(pool(), MAIL_CLASS_NAME));

	/// ^mail:send{hash}
	add_native_method("send", Method::CT_STATIC, _send, 1, 1);
}

void MMail::configure_user(Request& r) {
	Pool& pool=r.pool();

	// $MAIN:MAIL[$SMTP[mail.design.ru]]
	if(Value *mail_element=r.main_class->get_element(mail_name))
		if(Hash *mail_conf=mail_element->get_hash())
			r.classes_conf.put(name(), mail_conf);
		else
			PTHROW(0, 0,
				0,
				"$" MAIL_CLASS_NAME ":" MAIL_NAME " is not hash");
}

// creator

Methoded *MMail_create(Pool& pool) {
	return mail_class=new(pool) MMail(pool);
}
