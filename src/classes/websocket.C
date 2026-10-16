/** @file
	Parser: @b websocket parser class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#include "pa_vmethod_frame.h"
#include "pa_request.h"
#include "pa_vhash.h"
#include "pa_vint.h"
#include "pa_vvoid.h"
#include "pa_vfile.h"
#include "pa_vresponse.h"
#include "pa_charsets.h"
#include "pa_websocket.h"

volatile const char * IDENT_WEBSOCKET_C="$Id: websocket.C,v 1.1 2026/10/16 13:55:16 moko Exp $" IDENT_PA_WEBSOCKET_H;

// class

class MWebsocket: public Methoded {
public:
	MWebsocket();
};

// global variable

DECLARE_CLASS_VAR(websocket, new MWebsocket);

// defines

const size_t MAX_MESSAGE_SIZE_DEFAULT=10*0x400*0x400;
#define MAX_CONTROL_SIZE 125
#define MAX_REASON_SIZE (MAX_CONTROL_SIZE-2)

static const String::Body status_name_upper("STATUS");
static const String::Body upgrade_name_upper("UPGRADE");
static const String::Body connection_name_upper("CONNECTION");
static const String::Body accept_name_upper("SEC-WEBSOCKET-ACCEPT");

// methods

static WebSocket& get_websocket(Request& r) {
	if(!r.websocket)
		throw Exception("websocket", 0, "websocket is available in httpd mode only");
	return *r.websocket;
}

static WebSocket& get_accepted_websocket(Request& r) {
	WebSocket& result=get_websocket(r);
	if(!result.accepted)
		throw Exception("websocket", 0, "connection is not accepted, use ^websocket:accept[] first");
	return result;
}

// text goes in UTF-8 and the string is in the source charset, tainted data is untainted as in the response
static String::C to_utf8(Request& r, const String& string) {
	String::Body body=string.cstr_to_string_body_untaint(r.response.lang, r.connection(false), &r.charsets);
	return Charset::transcode(String::C(body.cstr(), body.length()), r.charsets.source(), pa_UTF8_charset);
}

static void _accept(Request& r, MethodParams&) {
	WebSocket& websocket=get_websocket(r);
	if(websocket.accepted)
		throw Exception("websocket", 0, "connection is accepted already");
	if(r.response.started)
		throw Exception("websocket", 0, "unable to accept: headers are already sent");

	const char* accept_key=pa_websocket_accept_key(
		r.request_info.method,
		SAPI::Env::get(r.sapi_info, "HTTP_UPGRADE"),
		SAPI::Env::get(r.sapi_info, "HTTP_CONNECTION"),
		SAPI::Env::get(r.sapi_info, "HTTP_SEC_WEBSOCKET_KEY"),
		SAPI::Env::get(r.sapi_info, "HTTP_SEC_WEBSOCKET_VERSION"));

	// $response:fields, such as sec-websocket-protocol, are sent too
	HashStringValue& fields=r.response.fields();
	fields.put(status_name_upper, new VInt(101));
	fields.put(upgrade_name_upper, new VString(*new String("websocket")));
	fields.put(connection_name_upper, new VString(*new String("Upgrade")));
	fields.put(accept_name_upper, new VString(*new String(accept_key)));
	r.start(VResponse::WEBSOCKET);

	websocket.start(MAX_MESSAGE_SIZE_DEFAULT);
}

static void _receive(Request& r, MethodParams& params) {
	WebSocket& websocket=get_accepted_websocket(r);

	double timeout=params.as_double(0, "timeout must be expression", r);
	if(timeout<0)
		throw Exception(PARSER_RUNTIME, 0, "timeout must not be negative");

	WebSocket::Message message;
	if(!websocket.receive(timeout*1000 < INT_MAX ? (int)(timeout*1000) : INT_MAX, message)) {
		r.write(*VVoid::get());
		return;
	}

	// the data is copied as the buffer is reused
	const char* data=pa_strdup(message.data, message.size);

	VHash& result=*new VHash;
	HashStringValue& fields=result.hash();

	if(message.opcode==WebSocket::BINARY) {
		fields.put("type", new VString(*new String("binary")));
		VFile& file=*new VFile;
		file.set_binary(true/*tainted*/, data, message.size);
		fields.put("data", &file);
	} else {
		String::C text=Charset::transcode(String::C(data, message.size), pa_UTF8_charset, r.charsets.source());
		fields.put("type", new VString(*new String(message.opcode==WebSocket::TEXT ? "text" : "close")));
		fields.put("data", new VString(*new String(text, String::L_TAINTED)));
		if(message.opcode==WebSocket::CLOSE)
			fields.put("code", new VInt(message.code));
	}

	r.write(result);
}

static void _send(Request& r, MethodParams& params) {
	WebSocket& websocket=get_accepted_websocket(r);

	if(VFile* file=dynamic_cast<VFile*>(&params[0])) {
		websocket.send(WebSocket::BINARY, file->value_ptr(), file->value_size());
	} else {
		String::C text=to_utf8(r, params.as_string(0, "message must be string or file"));
		websocket.send(WebSocket::TEXT, text.str, text.length);
	}
}

static void _ping(Request& r, MethodParams& params) {
	WebSocket& websocket=get_accepted_websocket(r);

	String::C data;
	if(params.count())
		data=to_utf8(r, params.as_string(0, "data must be string"));
	if(data.length>MAX_CONTROL_SIZE)
		throw Exception(PARSER_RUNTIME, 0, "data is too long, it must be up to %d bytes", MAX_CONTROL_SIZE);
	websocket.send(WebSocket::PING, data.str, data.length);
}

static void _close(Request& r, MethodParams& params) {
	WebSocket& websocket=get_accepted_websocket(r);

	int code=params.count()>0 ? params.as_int(0, "code must be int", r) : WebSocket::NORMAL;
	if(!WebSocket::valid_status(code))
		throw Exception(PARSER_RUNTIME, 0, "code(%d) is not valid", code);

	String::C reason;
	if(params.count()>1)
		reason=to_utf8(r, params.as_string(1, "reason must be string"));
	if(reason.length>MAX_REASON_SIZE)
		throw Exception(PARSER_RUNTIME, 0, "reason is too long, it must be up to %d bytes", MAX_REASON_SIZE);

	websocket.close(code, reason.str ? reason.str : "", reason.length);
}

// constructor

MWebsocket::MWebsocket(): Methoded("websocket") {
	// ^accept[]
	add_native_method("accept", Method::CT_STATIC, _accept, 0, 0);

	// ^receive(timeout in seconds), returns message hash
	add_native_method("receive", Method::CT_STATIC, _receive, 1, 1);

	// ^send[string or file]
	add_native_method("send", Method::CT_STATIC, _send, 1, 1);

	// ^ping[] or ^ping[data]
	add_native_method("ping", Method::CT_STATIC, _ping, 0, 1);

	// ^close[] or ^close(code) or ^close(code;reason)
	add_native_method("close", Method::CT_STATIC, _close, 0, 2);
}
