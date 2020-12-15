/** @file
	Parser: @b mail parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"
#include "pa_vmethod_frame.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_exec.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_uue.h"
#include "pa_vmail.h"

#include "smtp.h"

volatile const char * IDENT_MAIL_C="$Id: mail.C,v 1.135 2020/12/15 17:10:29 moko Exp $";

// defines

#define MAIL_CLASS_NAME "mail"
#define SENDMAIL_NAME "sendmail"

// consts

const int ATTACHMENT_WEIGHT=100;

// class

class MMail: public Methoded {
public: // Methoded
	bool used_directly() { return false; }
	void configure_user(Request& r);

public:
	MMail();
};

// global variable

DECLARE_CLASS_VAR(mail, new MMail);

// defines for statics

#define MAIL_NAME "MAIL"

// statics
	
static const String mail_name(MAIL_NAME);
static const String mail_sendmail_name(SENDMAIL_NAME);

// helpers

static void sendmail(
			Value* 
#ifndef WIN32
			vmail_conf
#endif
			, Value* smtp_server_port,
			const String& message, 
			const String* from,
			const String* to,
			const String* 
#ifndef WIN32
			 options
#endif
			 ) {
	const char* exception_type="email.format";
	if(!from) // we use in sendmail -f {from} && SMTP MAIL from: {from}
		throw Exception(exception_type, 0, "parameter does not specify 'from' header field");

	const char* message_cstr=message.untaint_cstr(String::L_AS_IS);

	if(smtp_server_port) {
		if(!to) // we use only in SMTP RCPT to: {to}
			throw Exception(exception_type, 0, "parameter does not specify 'to' header field");

		SMTP smtp;
		char* server=smtp_server_port->as_string().cstrm();
		const char* port=rsplit(server, ':');
		if(!port)
			port="25";

		smtp.Send(server, port, message_cstr, from->cstrm(), to->cstrm());
		return;
	}

#ifdef WIN32
	// win32 without SMTP server configured
	throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":" MAIL_NAME ".SMTP not defined");
#else
	// unix
	// $MAIN:MAIL.sendmail["/usr/sbin/sendmail -t -i -f postmaster"] default
	// $MAIN:MAIL.sendmail["/usr/lib/sendmail -t -i  -f postmaster"] default

	String* sendmail_command=new String;
	if(vmail_conf) {
#ifdef PA_FORCED_SENDMAIL
		throw Exception(PARSER_RUNTIME,
			0,
			"Parser was configured with --with-sendmail=" PA_FORCED_SENDMAIL
			" key, to change sendmail you should reconfigure and recompie it");
#else
		if(Value* sendmail_value=vmail_conf->get_hash()->get(mail_sendmail_name))
			*sendmail_command<<sendmail_value->as_string();
		else
			throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":" MAIL_NAME "." SENDMAIL_NAME " not defined");
#endif
	} else {
#ifdef PA_FORCED_SENDMAIL
		*sendmail_command<<PA_FORCED_SENDMAIL;
#else
		String* test=new String("/usr/sbin/sendmail");
		if(!file_executable(*test))
			test=new String("/usr/lib/sendmail");
		*sendmail_command<<*test;
		*sendmail_command<<" -t -i -f postmaster";
#endif
	}
	if(options)
		*sendmail_command<<" "<<*options;

	// we know sendmail_command here, should replace "postmaster" with "$from" from message
	size_t at_postmaster=sendmail_command->pos("postmaster");
	if(at_postmaster!=STRING_NOT_FOUND) {
		String& reconstructed=sendmail_command->mid(0, at_postmaster);
		reconstructed << *from;
		reconstructed << sendmail_command->mid(at_postmaster+10/*postmaster*/, sendmail_command->length());
		sendmail_command=&reconstructed;
	}

	// execute it
	ArrayString argv;
	const String* file_spec;
	size_t after_file_spec=sendmail_command->pos(' ');
	if(after_file_spec==STRING_NOT_FOUND || after_file_spec==0)
		file_spec=sendmail_command;
	else {
		file_spec=&sendmail_command->mid(0, after_file_spec);
		sendmail_command->split(argv, after_file_spec+1, " ", String::L_AS_IS);
	}

	if(!file_executable(*file_spec))
		throw Exception("email.send",
			file_spec, 
			"is not executable."
#ifdef PA_FORCED_SENDMAIL
			" Use configure key \"--with-sendmail=appropriate sendmail command\""
#else
			" Set $" MAIN_CLASS_NAME ":" MAIL_NAME "." SENDMAIL_NAME " to appropriate sendmail command"
#endif
		);

	PA_exec_result exec=pa_exec(
		// forced_allow
#ifdef PA_FORCED_SENDMAIL
		true
#else
		false
#endif
		, *file_spec,
		0 /* pass env */,
		argv,
		String::C(message_cstr, strlen(message_cstr)));

	if(exec.status || exec.err.length())
		throw Exception("email.send", 0, "'%s' reported problem: %s (%d)", file_spec->cstr(), exec.err.length() ? exec.err.cstr() : "UNKNOWN", exec.status);
#endif //WIN32
}

// methods

static void _send(Request& r, MethodParams& params) {
	HashStringValue* hash=params.as_hash(0, "message");
	if(!hash || !hash->count())
		return;
	// todo@ check if enough options are specified.
	// now ^mail:send[^hash::create[]] and ^mail:send[$.print-debug(1)] "work".

	const String* soptions=0;
	if(Value* voptions=hash->get(MAIL_OPTIONS_NAME))
		soptions=&voptions->as_string();

	bool print_debug=false;
	if(Value* vdebug=hash->get(MAIL_DEBUG_NAME))
		print_debug=vdebug->as_bool();

	Value* vmail_conf=static_cast<Value*>(r.classes_conf.get(mail_class->type()));
	Value* smtp_server_port=0;
	if(vmail_conf) {
		// $MAIN:MAIL.SMTP[mail.yourdomain.ru[:port]]
		smtp_server_port=vmail_conf->get_hash()->get("SMTP");
	}

	const String* from=0;
	String* to=0;
	const String& message = GET_SELF(r, VMail).message_hash_to_string(r, hash, from, smtp_server_port ? true : false /*send by SMTP=strip to?*/, to);

	if(print_debug)
		r.write(message);
	else
		sendmail(vmail_conf, smtp_server_port, message, from, to, soptions);
}

// constructor & configurator

MMail::MMail(): Methoded(MAIL_CLASS_NAME) {
	// ^mail:send{hash}
	add_native_method("send", Method::CT_STATIC, _send, 1, 1);
}

void MMail::configure_user(Request& r) {

	// $MAIN:MAIL[$SMTP[mail.design.ru]]
	if(Value* mail_element=r.main_class.get_element(mail_name)) {
		if(mail_element->get_hash())
			r.classes_conf.put(type(), mail_element);
		else
			if( !mail_element->is_string() )
				throw Exception(PARSER_RUNTIME, 0, "$" MAIL_CLASS_NAME ":" MAIL_NAME " is not hash");
	}
}
