/** @file
	Parser: @b amqp parser class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_varray.h"
#include "pa_vbool.h"
#include "pa_vvoid.h"
#include "pa_vamqp.h"
#include "pa_os.h"
#include "pa_globals.h"

#ifdef WITH_AMQP
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_ssl_socket.h>
#include <amqp_framing.h>
#include <stdlib.h>
#include <string.h>
#endif

volatile const char * IDENT_AMQP_C="$Id: amqp.C,v 1.17 2026/09/15 19:54:26 moko Exp $" IDENT_PA_VAMQP_H;

class MAmqp: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VAmqp(); }
public:
	MAmqp();
};

DECLARE_CLASS_VAR(amqp, new MAmqp);

#ifdef WITH_AMQP

static void status_check(VAmqp& self, int ret, const char *detail=""){
	if(ret == AMQP_STATUS_OK)
		return;

	self.fstate = VAmqp::CONNECTION_DEAD;

	const char* error_str = amqp_error_string2(ret);
	if(error_str) {
		throw Exception("amqp", 0, "%sfailed: %s", detail, error_str);
	} else {
		throw Exception("amqp", 0, "%sfailed: error %d", detail, ret);
	}
}

static void check(VAmqp& self, amqp_rpc_reply_t rr, const char *detail=""){
	if(rr.reply_type == AMQP_RESPONSE_NORMAL)
		return;

	self.fstate = (rr.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION && rr.reply.id == AMQP_CHANNEL_CLOSE_METHOD) ? VAmqp::CHANNEL_DEAD : VAmqp::CONNECTION_DEAD;

	// Extract error message from reply
	const char* error_msg = 0;
	size_t error_len = 0;
	if(rr.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION) {
		if(rr.reply.id == AMQP_CHANNEL_CLOSE_METHOD) {
			amqp_channel_close_t *m = (amqp_channel_close_t *)rr.reply.decoded;
			if(m->reply_text.len > 0 && m->reply_text.bytes) {
				error_msg = (const char*)m->reply_text.bytes;
				error_len = m->reply_text.len;
			}
		} else if(rr.reply.id == AMQP_CONNECTION_CLOSE_METHOD) {
			amqp_connection_close_t *m = (amqp_connection_close_t *)rr.reply.decoded;
			if(m->reply_text.len > 0 && m->reply_text.bytes) {
				error_msg = (const char*)m->reply_text.bytes;
				error_len = m->reply_text.len;
			}
		}
	}

	if(error_msg) {
		throw Exception("amqp", 0, "%sfailed: %.*s", detail, (int)error_len, error_msg);
	} else if(rr.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION) {
		status_check(self, rr.library_error, detail);
	}

	throw Exception("amqp", 0, "%sfailed", detail);
}

static void amqp_connect(VAmqp& self, Request& r, HashStringValue* options) {
	const char* host_c = "localhost";
	int port = 5672;
	const char* user_c = "guest";
	const char* pass_c = "guest";
	const char* vhost_c = "/";
	const char* locale_c = "en_US";
	int heartbeat = 30; // seconds
	const char* tls_ca = 0;
	const char* tls_cert = 0;
	const char* tls_key = 0;
	bool tls_specified = false;
	bool tls_verify = true;
	int reconnect_interval_sec = 0;

	if(options){
		for(HashStringValue::Iterator i(*options); i; i.next()){
			String::Body key=i.key();
			Value* value=i.value();
			if(key=="host"){
				host_c=value->as_string().cstr();
			} else if(key=="port"){
				port=r.process(*value).as_int();
			} else if(key=="user"){
				user_c=value->as_string().cstr();
			} else if(key=="password"){
				pass_c=value->as_string().cstr();
			} else if(key=="vhost"){
				vhost_c=value->as_string().cstr();
			} else if(key=="locale"){
				locale_c=value->as_string().cstr();
			} else if(key=="heartbeat"){
				heartbeat=r.process(*value).as_int();
			} else if(key=="auto_reconnect"){
				reconnect_interval_sec=r.process(*value).as_int();
			} else if(key=="tls"){
				tls_specified = true;
				if(HashStringValue* tls_options=value->get_hash()){
					for(HashStringValue::Iterator t(*tls_options); t; t.next()){
						String::Body tkey=t.key();
						Value* tval=t.value();
						if(tkey=="ca"){
							tls_ca=tval->as_string().cstr();
						} else if(tkey=="cert"){
							tls_cert=tval->as_string().cstr();
						} else if(tkey=="key"){
							tls_key=tval->as_string().cstr();
						} else if(tkey=="verify"){
							tls_verify=r.process(*tval).as_bool();
						} else
							throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
					}
				}
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}

	amqp_connection_state_t conn = amqp_new_connection();
	amqp_socket_t* socket = 0;

	if(tls_specified) {
		socket = amqp_ssl_socket_new(conn);
		if(!socket)
			throw Exception("amqp", 0, "failed to create SSL socket");

		// Set CA certificate if provided
		if(tls_ca)
			if(amqp_ssl_socket_set_cacert(socket, tls_ca))
				throw Exception("amqp", 0, "failed to set CA certificate");

		// Set client certificate and key if provided
		if(tls_cert && tls_key) {
			if(amqp_ssl_socket_set_key(socket, tls_cert, tls_key))
				throw Exception("amqp", 0, "failed to set client certificate/key");
		} else if(tls_cert || tls_key) {
			throw Exception("amqp", 0, "both cert and key must be specified for TLS");
		}

		// If CA is provided, peer verification will use it
		amqp_ssl_socket_set_verify_peer(socket, tls_verify && tls_ca);
		// If verify=true, enable hostname verification
		amqp_ssl_socket_set_verify_hostname(socket, tls_verify);
	} else {
		socket = amqp_tcp_socket_new(conn);
		if(!socket)
			throw Exception("amqp", 0, "failed to create TCP socket");
	}

	status_check(self, amqp_socket_open(socket, host_c, port), tls_specified ? "open SSL socket " : "open TCP socket ");

	amqp_rpc_reply_t rlogin = amqp_login(conn, vhost_c, 0, 131072, heartbeat, AMQP_SASL_METHOD_PLAIN, user_c, pass_c);
	if(rlogin.reply_type != AMQP_RESPONSE_NORMAL){
		amqp_destroy_connection(conn);
		check(self, rlogin, "login ");
	}

	int channel = 1;
	amqp_channel_open(conn, channel);
	amqp_rpc_reply_t ropen = amqp_get_rpc_reply(conn);
	if(ropen.reply_type != AMQP_RESPONSE_NORMAL){
		amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
		amqp_destroy_connection(conn);
		check(self, ropen, "open channel ");
	}

	if(self.fconnection)
		amqp_destroy_connection(self.fconnection);

	self.fconnection = conn;
	self.fchannel = channel;
	self.fstate = VAmqp::ALIVE;
	self.freconnect_interval = reconnect_interval_sec;
}

// repairs a channel-level error cheaply: reopen on the same connection
static void amqp_repair_channel(VAmqp& self) {
	amqp_channel_close_ok_t close_ok;
	amqp_send_method(self.fconnection, self.fchannel, AMQP_CHANNEL_CLOSE_OK_METHOD, &close_ok);
	amqp_channel_open(self.fconnection, self.fchannel);
	check(self, amqp_get_rpc_reply(self.fconnection), "channel repair ");
	self.fstate = VAmqp::ALIVE;
}

void VAmqp::ensure_connected() {
	if(fstate == ALIVE)
		return;

	if(fstate == CHANNEL_DEAD){
		amqp_repair_channel(*this);
		return;
	}

	// CONNECTION_DEAD
	if(freconnect_interval <= 0)
		throw Exception("amqp", 0, "connection is dead (auto_reconnect is not enabled)");

	pa_sleep(freconnect_interval, 0);

	amqp_connect(*this, pa_thread_request(), fcreate_options);
}

#endif // WITH_AMQP

static void _create(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);

#ifdef WITH_AMQP
	self.fcreate_options = params.count()>0 ? params.as_hash(0) : 0;
	amqp_connect(self, r, self.fcreate_options);
#else
	(void)params; (void)self;
	throw Exception("amqp", 0, "compiled without amqp support");
#endif // WITH_AMQP
}

#ifdef WITH_AMQP

#define AMQP_STRING(s,l) new String(String::C(pa_strdup((const char*)(s), (l)), (l)))
#define AMQP_VSTRING(s,l) new VString(*AMQP_STRING(s,l))

// broker-defined arguments/headers (x-message-ttl, x-dead-letter-exchange, ...)
// Type is taken from how the value was assigned: [] -> string, () -> number, (true|false) -> bool
static amqp_table_t amqp_build_arguments_table(Request& r, HashStringValue* arguments) {
	amqp_table_t table = amqp_empty_table;
	size_t count = arguments ? arguments->count() : 0;
	if(!count)
		return table;

	amqp_table_entry_t* entries = (amqp_table_entry_t*)pa_malloc(count * sizeof(amqp_table_entry_t));
	size_t i = 0;
	for(HashStringValue::Iterator it(*arguments); it; it.next()){
		entries[i].key = amqp_cstring_bytes(it.key().cstr());

		Value& value = r.process(*it.value());
		if(value.is_string() || value.is_void()){
			entries[i].value.kind = AMQP_FIELD_KIND_UTF8;
			entries[i].value.value.bytes = amqp_cstring_bytes(value.as_string().cstr());
		} else if(value.is_bool()){
			entries[i].value.kind = AMQP_FIELD_KIND_BOOLEAN;
			entries[i].value.value.boolean = value.as_bool();
		} else {
			entries[i].value.kind = AMQP_FIELD_KIND_I64;
			entries[i].value.value.i64 = (int64_t)value.as_wint();
		}
		i++;
	}

	table.num_entries = (int)count;
	table.entries = entries;
	return table;
}

// reverse of amqp_build_arguments_table - decodes a broker-provided field table (e.g. message headers) back into a hash.
static VHash* amqp_table_to_hash(amqp_table_t& table);

static Value* amqp_field_to_value(amqp_field_value_t& f) {
	switch(f.kind){
		case AMQP_FIELD_KIND_UTF8:
		case AMQP_FIELD_KIND_BYTES:
			return AMQP_VSTRING(f.value.bytes.bytes, f.value.bytes.len);
		case AMQP_FIELD_KIND_BOOLEAN:
			return &VBool::get(f.value.boolean);
		case AMQP_FIELD_KIND_I8: return new VInt((pa_wint)f.value.i8);
		case AMQP_FIELD_KIND_U8: return new VInt((pa_wint)f.value.u8);
		case AMQP_FIELD_KIND_I16: return new VInt((pa_wint)f.value.i16);
		case AMQP_FIELD_KIND_U16: return new VInt((pa_wint)f.value.u16);
		case AMQP_FIELD_KIND_I32: return new VInt((pa_wint)f.value.i32);
		case AMQP_FIELD_KIND_U32: return new VInt((pa_wint)f.value.u32);
		case AMQP_FIELD_KIND_I64: return new VInt((pa_wint)f.value.i64);
		case AMQP_FIELD_KIND_U64: return new VInt((pa_wint)f.value.u64);
		case AMQP_FIELD_KIND_TIMESTAMP: return new VInt((pa_wint)f.value.u64);
		case AMQP_FIELD_KIND_F32: return new VDouble((double)f.value.f32);
		case AMQP_FIELD_KIND_F64: return new VDouble(f.value.f64);
		case AMQP_FIELD_KIND_DECIMAL: {
			double divisor=1;
			for(uint8_t i=0; i<f.value.decimal.decimals; i++) divisor*=10;
			return new VDouble(f.value.decimal.value/divisor);
		}
		case AMQP_FIELD_KIND_TABLE:
			return amqp_table_to_hash(f.value.table);
		case AMQP_FIELD_KIND_ARRAY: {
			VArray* result=new VArray();
			ArrayValue& av=result->array();
			for(int i=0; i<f.value.array.num_entries; i++)
				av+=amqp_field_to_value(f.value.array.entries[i]);
			return result;
		}
		case AMQP_FIELD_KIND_VOID:
			return VVoid::get();
		default:
			return VString::empty();
	}
}

static VHash* amqp_table_to_hash(amqp_table_t& table) {
	VHash* result=new VHash;
	HashStringValue* h=result->get_hash();
	for(int i=0; i<table.num_entries; i++){
		amqp_table_entry_t& e=table.entries[i];
		h->put(*AMQP_STRING(e.key.bytes, e.key.len), amqp_field_to_value(e.value));
	}
	return result;
}

static void _publish(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const String &msg=params.as_string(0, "msg must be string");
	const char* exchange_c = ""; // default exchange
	const char* routing_key_c = 0;
	bool mandatory=false;

	amqp_basic_properties_t props;
	props._flags = 0;

	if(params.count()>1){
		if(HashStringValue* options=params.as_hash(1)){
			for(HashStringValue::Iterator i(*options); i; i.next()){
				String::Body key=i.key();
				Value* value=i.value();
				if(key=="exchange"){
					exchange_c=value->as_string().cstr();
				} else if(key=="routing_key"){
					routing_key_c=value->as_string().cstr();
				} else if(key=="queue"){
					routing_key_c=value->as_string().cstr();
				} else if(key=="mandatory"){
					mandatory=r.process(*value).as_bool();
				} else if(key=="content_type"){
					const char* v=value->as_string().cstr();
					props.content_type=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_CONTENT_TYPE_FLAG;
				} else if(key=="content_encoding"){
					const char* v=value->as_string().cstr();
					props.content_encoding=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_CONTENT_ENCODING_FLAG;
				} else if(key=="delivery_mode"){
					uint8_t dm=(uint8_t)value->as_int();
					props.delivery_mode=dm;
					props._flags|=AMQP_BASIC_DELIVERY_MODE_FLAG;
				} else if(key=="priority"){
					uint8_t pr=(uint8_t)value->as_int();
					props.priority=pr;
					props._flags|=AMQP_BASIC_PRIORITY_FLAG;
				} else if(key=="correlation_id"){
					const char* v=value->as_string().cstr();
					props.correlation_id=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_CORRELATION_ID_FLAG;
				} else if(key=="reply_to"){
					const char* v=value->as_string().cstr();
					props.reply_to=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_REPLY_TO_FLAG;
				} else if(key=="expiration"){
					const char* v=value->as_string().cstr();
					props.expiration=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_EXPIRATION_FLAG;
				} else if(key=="message_id"){
					const char* v=value->as_string().cstr();
					props.message_id=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_MESSAGE_ID_FLAG;
				} else if(key=="timestamp"){
					uint64_t ts=(uint64_t)value->as_double();
					props.timestamp=ts;
					props._flags|=AMQP_BASIC_TIMESTAMP_FLAG;
				} else if(key=="type"){
					const char* v=value->as_string().cstr();
					props.type=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_TYPE_FLAG;
				} else if(key=="user_id"){
					const char* v=value->as_string().cstr();
					props.user_id=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_USER_ID_FLAG;
				} else if(key=="app_id"){
					const char* v=value->as_string().cstr();
					props.app_id=amqp_cstring_bytes(v);
					props._flags|=AMQP_BASIC_APP_ID_FLAG;
				} else if(key=="headers"){
					props.headers=amqp_build_arguments_table(r, value->get_hash());
					props._flags|=AMQP_BASIC_HEADERS_FLAG;
				} else
					throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			}
		}
	}

	if(!routing_key_c)
		throw Exception("amqp", 0, "routing_key or queue must be specified");

	amqp_bytes_t body;
	body.len = msg.length();
	body.bytes=(void*)msg.cstr();

	int ret = amqp_basic_publish(self.connection(), self.channel(), amqp_cstring_bytes(exchange_c), amqp_cstring_bytes(routing_key_c), mandatory, 0, &props, body);

	status_check(self, ret, "publish ");
}

static void _release(Request& r, MethodParams&) {
	VAmqp& self=GET_SELF(r, VAmqp);
	if(self.fconnection){
		amqp_connection_state_t conn=self.fconnection;
		amqp_channel_close(conn, self.fchannel, AMQP_REPLY_SUCCESS);
		amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
		amqp_destroy_connection(conn);
		self.fconnection=0;
		self.fchannel=0;
		self.fstate=VAmqp::CONNECTION_DEAD;
	}
}

// to allow both [] and () call syntax
static uint64_t as_delivery_tag(MethodParams& params, int index=0) {
	const String &tag_s=params.as_string(index, "delivery tag must not be code");
	return pa_atoul(tag_s.cstr());
}

static void _ack(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	int ret = amqp_basic_ack(self.connection(), self.channel(), as_delivery_tag(params), 0);
	status_check(self, ret, "ack ");
}

static void _nack(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	uint64_t tag=as_delivery_tag(params);
	bool requeue=false;
	if(params.count()>1){
		if(HashStringValue* options=params.as_hash(1)){
			for(HashStringValue::Iterator i(*options); i; i.next()){
				if(i.key()=="requeue"){
					requeue=r.process(*i.value()).as_bool();
				} else
					throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			}
		}
	}
	int ret = amqp_basic_nack(self.connection(), self.channel(), tag, 0, requeue);
	status_check(self, ret, "nack ");
}

static void _qos(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	uint16_t prefetch_count=0;
	if(params.count()>0){
		if(HashStringValue* options=params.as_hash(0)){
			for(HashStringValue::Iterator i(*options); i; i.next()){
				if(i.key()=="prefetch_count"){
					int pc=r.process(*i.value()).as_int();
					prefetch_count= pc<0 ? 0 : (pc>65535 ? 65535 : (uint16_t)pc);
				} else
					throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			}
		}
	}
	amqp_basic_qos(self.connection(), self.channel(), 0, prefetch_count, 0);
	check(self, amqp_get_rpc_reply(self.connection()), "qos ");
}

static void _reject(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	uint64_t tag=as_delivery_tag(params);
	bool requeue=true; // by default return to queue
	if(params.count()>1){
		if(HashStringValue* options = params.as_hash(1)){
			for(HashStringValue::Iterator i(*options); i; i.next()){
				if(i.key() == "requeue"){
					requeue=r.process(*i.value()).as_bool();
				} else
					throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			}
		}
	}
	int ret = amqp_basic_reject(self.connection(), self.channel(), tag, requeue);
	status_check(self, ret, "reject ");
}

static void _declare(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const char* exchange_c = 0;
	const char* queue_c = 0;
	const char* type_c = "direct";
	bool passive=false, durable=false, auto_delete=false, internal=false, exclusive=false;
	amqp_table_t arguments=amqp_empty_table;
	if(HashStringValue* options=params.as_hash(0)){
		for(HashStringValue::Iterator i(*options); i; i.next()){
			String::Body key=i.key();
			Value* value=i.value();
			if(key=="exchange"){
				exchange_c=value->as_string().cstr();
			} else if(key=="queue"){
				queue_c=value->as_string().cstr();
			} else if(key=="type"){
				type_c=value->as_string().cstr();
			} else if(key=="passive"){
				passive=r.process(*value).as_bool();
			} else if(key=="durable"){
				durable=r.process(*value).as_bool();
			} else if(key=="auto_delete"){
				auto_delete=r.process(*value).as_bool();
			} else if(key=="internal"){
				internal=r.process(*value).as_bool();
			} else if(key=="exclusive"){
				exclusive=r.process(*value).as_bool();
			} else if(key=="arguments"){
				arguments=amqp_build_arguments_table(r, value->get_hash());
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}
	if(!exchange_c && !queue_c)
		throw Exception("amqp", 0, "exchange or queue must be specified");

	if(exchange_c){
		amqp_exchange_declare(self.connection(), self.channel(), amqp_cstring_bytes(exchange_c), amqp_cstring_bytes(type_c), passive, durable, auto_delete, internal, arguments);
		check(self, amqp_get_rpc_reply(self.connection()));
	}

	if(queue_c){
		amqp_queue_declare_ok_t *ok = amqp_queue_declare(self.connection(), self.channel(), *queue_c ? amqp_cstring_bytes(queue_c) : amqp_empty_bytes, passive, durable, exclusive, auto_delete, arguments);
		check(self, amqp_get_rpc_reply(self.connection()));
		if(!*queue_c && ok){
			r.write(*AMQP_STRING(ok->queue.bytes, ok->queue.len));
		}
	}
}

static void _delete(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const char* exchange_c = 0;
	const char* queue_c = 0;
	bool if_unused=false, if_empty=false;
	if(HashStringValue* options=params.as_hash(0)){
		for(HashStringValue::Iterator i(*options); i; i.next()){
			String::Body key=i.key();
			Value* value=i.value();
			if(key=="exchange"){
				exchange_c=value->as_string().cstr();
			} else if(key=="queue"){
				queue_c=value->as_string().cstr();
			} else if(key=="if_unused"){
				if_unused=r.process(*value).as_bool();
			} else if(key=="if_empty"){
				if_empty=r.process(*value).as_bool();
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}
	if(!exchange_c && !queue_c)
		throw Exception("amqp", 0, "exchange or queue must be specified");

	if(exchange_c){
		amqp_exchange_delete(self.connection(), self.channel(), amqp_cstring_bytes(exchange_c), if_unused);
		check(self, amqp_get_rpc_reply(self.connection()));
	}

	if(queue_c){
		amqp_queue_delete(self.connection(), self.channel(), amqp_cstring_bytes(queue_c), if_unused, if_empty);
		check(self, amqp_get_rpc_reply(self.connection()));
	}
}

static void _bind(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const char* exchange_c=0;
	const char* queue_c=0;
	const char* routing_key_c="";
	if(HashStringValue* options=params.as_hash(0)){
		for(HashStringValue::Iterator i(*options); i; i.next()){
			String::Body key=i.key();
			Value* value=i.value();
			if(key=="exchange"){
				exchange_c=value->as_string().cstr();
			} else if(key=="queue"){
				queue_c=value->as_string().cstr();
			} else if(key=="routing_key"){
				routing_key_c=value->as_string().cstr();
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}
	if(!exchange_c || !queue_c) throw Exception("amqp", 0, "exchange and queue are required");
	amqp_queue_bind(self.connection(), self.channel(), amqp_cstring_bytes(queue_c), amqp_cstring_bytes(exchange_c), amqp_cstring_bytes(routing_key_c), amqp_empty_table);
	check(self, amqp_get_rpc_reply(self.connection()));
}

static void _unbind(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const char* exchange_c=0;
	const char* queue_c=0;
	const char* routing_key_c="";
	if(HashStringValue* options=params.as_hash(0)){
		for(HashStringValue::Iterator i(*options); i; i.next()){
			String::Body key=i.key();
			Value* value=i.value();
			if(key=="exchange"){
				exchange_c=value->as_string().cstr();
			} else if(key=="queue"){
				queue_c=value->as_string().cstr();
			} else if(key=="routing_key"){
				routing_key_c=value->as_string().cstr();
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}
	if(!exchange_c || !queue_c) throw Exception("amqp", 0, "exchange and queue are required");
	amqp_queue_unbind(self.connection(), self.channel(), amqp_cstring_bytes(queue_c), amqp_cstring_bytes(exchange_c), amqp_cstring_bytes(routing_key_c), amqp_empty_table);
	check(self, amqp_get_rpc_reply(self.connection()));
}

static void _purge(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const char* queue_c = 0;
	if(HashStringValue* options=params.as_hash(0)){
		for(HashStringValue::Iterator i(*options); i; i.next()){
			String::Body key=i.key();
			Value* value=i.value();
			if(key=="queue"){
				queue_c=value->as_string().cstr();
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}
	if(!queue_c)
		throw Exception("amqp", 0, "queue must be specified");

	amqp_queue_purge_ok_t *ok = amqp_queue_purge(self.connection(), self.channel(), amqp_cstring_bytes(queue_c));
	check(self, amqp_get_rpc_reply(self.connection()));
	r.write(*new VInt(ok ? ok->message_count : 0));
}

static void _info(Request& r, MethodParams& params) {
	VAmqp& self = GET_SELF(r, VAmqp);
	const char* queue_c = 0;

	if (HashStringValue* options=params.as_hash(0)) {
		for (HashStringValue::Iterator i(*options); i; i.next()) {
			String::Body key=i.key();
			Value* value=i.value();
			if(key=="queue"){
				queue_c=value->as_string().cstr();
			} else {
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			}
		}
	}

	if (!queue_c)
		throw Exception("amqp", 0, "queue must be specified");

	amqp_queue_declare_ok_t* ok = amqp_queue_declare(self.connection(), self.channel(), amqp_cstring_bytes(queue_c), /*passive*/ 1, 0, 0, 0, amqp_empty_table);
	check(self, amqp_get_rpc_reply(self.connection()));

	Value& result=*new VHash;
	if(ok){
		result.put_element(*new String("queue"), AMQP_VSTRING(ok->queue.bytes, ok->queue.len));
		result.put_element(*new String("messages"), new VInt(ok->message_count));
		result.put_element(*new String("consumers"), new VInt(ok->consumer_count));
	}
	r.write(result);
}

static VHash *amqp_message_hash(amqp_envelope_t &envelope) {
	VHash *result=new VHash;
	HashStringValue* h=result->get_hash();
	h->put("msg", AMQP_VSTRING(envelope.message.body.bytes, envelope.message.body.len));
	h->put("delivery_tag", new VInt(envelope.delivery_tag));
	h->put("consumer_tag", AMQP_VSTRING(envelope.consumer_tag.bytes, envelope.consumer_tag.len));
	h->put("exchange", AMQP_VSTRING(envelope.exchange.bytes, envelope.exchange.len));
	h->put("routing_key", AMQP_VSTRING(envelope.routing_key.bytes, envelope.routing_key.len));
	h->put("redelivered", &VBool::get(envelope.redelivered));

	// properties are only put when the publisher actually set them (per the wire's own _flags bitmask) -
	// same handful of allocations as the fields above, just conditional on what's actually present
	amqp_basic_properties_t& props=envelope.message.properties;
	if(props._flags & AMQP_BASIC_CONTENT_TYPE_FLAG)
		h->put("content_type", AMQP_VSTRING(props.content_type.bytes, props.content_type.len));
	if(props._flags & AMQP_BASIC_CONTENT_ENCODING_FLAG)
		h->put("content_encoding", AMQP_VSTRING(props.content_encoding.bytes, props.content_encoding.len));
	if(props._flags & AMQP_BASIC_HEADERS_FLAG)
		h->put("headers", amqp_table_to_hash(props.headers));
	if(props._flags & AMQP_BASIC_DELIVERY_MODE_FLAG)
		h->put("delivery_mode", new VInt(props.delivery_mode));
	if(props._flags & AMQP_BASIC_PRIORITY_FLAG)
		h->put("priority", new VInt(props.priority));
	if(props._flags & AMQP_BASIC_CORRELATION_ID_FLAG)
		h->put("correlation_id", AMQP_VSTRING(props.correlation_id.bytes, props.correlation_id.len));
	if(props._flags & AMQP_BASIC_REPLY_TO_FLAG)
		h->put("reply_to", AMQP_VSTRING(props.reply_to.bytes, props.reply_to.len));
	if(props._flags & AMQP_BASIC_EXPIRATION_FLAG)
		h->put("expiration", AMQP_VSTRING(props.expiration.bytes, props.expiration.len));
	if(props._flags & AMQP_BASIC_MESSAGE_ID_FLAG)
		h->put("message_id", AMQP_VSTRING(props.message_id.bytes, props.message_id.len));
	if(props._flags & AMQP_BASIC_TIMESTAMP_FLAG)
		h->put("timestamp", new VInt((pa_wint)props.timestamp));
	if(props._flags & AMQP_BASIC_TYPE_FLAG)
		h->put("type", AMQP_VSTRING(props.type.bytes, props.type.len));
	if(props._flags & AMQP_BASIC_USER_ID_FLAG)
		h->put("user_id", AMQP_VSTRING(props.user_id.bytes, props.user_id.len));
	if(props._flags & AMQP_BASIC_APP_ID_FLAG)
		h->put("app_id", AMQP_VSTRING(props.app_id.bytes, props.app_id.len));
	if(props._flags & AMQP_BASIC_CLUSTER_ID_FLAG)
		h->put("cluster_id", AMQP_VSTRING(props.cluster_id.bytes, props.cluster_id.len));

	return result;
}

// timeout<0 - wait forever (NULL), timeout==0 - don't wait (poll), timeout>0 - wait up to timeout seconds
static struct timeval* amqp_wait_tv(struct timeval& tv, int timeout){
	if(timeout<0)
		return NULL;
	tv.tv_sec=timeout;
	tv.tv_usec=0;
	return &tv;
}

// amqp_consume_message() returns AMQP_STATUS_UNEXPECTED_STATE when it read a frame other than a deliver while polling.
// Per its own documented contract, read that pending frame to find out what it actually was:
//  - a basic.return for an earlier mandatory publish that found no route: benign, doesn't close anything;
//    drain the returned message (header+body, still pending right behind it) and retry - the real
//    message (if any) is still coming
//  - anything else: a genuine channel/connection close (typically after a protocol violation) - build a
//    reply so check() can classify it correctly (cheap channel repair vs full reconnect)
static amqp_rpc_reply_t amqp_consume_message_checked(amqp_connection_state_t conn, int channel, amqp_envelope_t* envelope, struct timeval* tv) {
	for(;;) {
		amqp_rpc_reply_t res = amqp_consume_message(conn, envelope, tv, 0);
		if(res.reply_type != AMQP_RESPONSE_LIBRARY_EXCEPTION || res.library_error != AMQP_STATUS_UNEXPECTED_STATE)
			return res;

		amqp_frame_t frame;
		if(amqp_simple_wait_frame(conn, &frame) != AMQP_STATUS_OK || frame.frame_type != AMQP_FRAME_METHOD)
			return res; // give up, let the original UNEXPECTED_STATE be treated as fatal

		if(frame.payload.method.id == AMQP_BASIC_RETURN_METHOD) {
			amqp_message_t message;
			if(amqp_read_message(conn, channel, &message, 0).reply_type == AMQP_RESPONSE_NORMAL)
				amqp_destroy_message(&message);
			continue;
		}

		res.reply_type = AMQP_RESPONSE_SERVER_EXCEPTION;
		res.reply.id = frame.payload.method.id;
		res.reply.decoded = frame.payload.method.decoded;
		return res;
	}
}

static void _consume(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const char* queue_c=0;
	const char* consumer_tag_c=0;
	bool ack=true, exclusive=false;
	int count=1; bool count_specified=false;
	int timeout=0; bool timeout_specified=false;
	Junction* callback=0;

	if(HashStringValue* options=params.as_hash(0)){
		for(HashStringValue::Iterator i(*options); i; i.next()){
			String::Body key=i.key();
			Value* value=i.value();
			if(key=="callback"){
				callback=value->get_junction();
			} else if(key=="queue"){
				queue_c=value->as_string().cstr();
			} else if(key=="consumer_tag"){
				consumer_tag_c=value->as_string().cstr();
			} else if(key=="ack"){
				ack=r.process(*value).as_bool();
			} else if(key=="exclusive"){
				exclusive=r.process(*value).as_bool();
			} else if(key=="count"){
				count=r.process(*value).as_int();
				count_specified=true;
			} else if(key=="timeout"){
				timeout=r.process(*value).as_int();
				timeout_specified=true;
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}

	if(!queue_c) throw Exception("amqp", 0, "queue must be specified");
	if(count==0) throw Exception("amqp", 0, "count must not be zero");

	// listen mode (callback): count/timeout unlimited by default - runs until stop_consume/timeout/error
	// pull mode (no callback): count(1)/timeout(0) by default - returns whatever is ready right now
	if(!count_specified)
		count = callback ? -1 : 1;
	if(!timeout_specified)
		timeout = callback ? -1 : 0;

	amqp_basic_consume_ok_t *ok = amqp_basic_consume(self.connection(), self.channel(), amqp_cstring_bytes(queue_c),
		consumer_tag_c ? amqp_cstring_bytes(consumer_tag_c) : amqp_empty_bytes,
		0 /*no_local*/, !ack, exclusive, amqp_empty_table);
	check(self, amqp_get_rpc_reply(self.connection()));

	// amqp_maybe_release_buffers() recycles ok->consumer_tag
	const char* consumer_tag_copy = ok ? pa_strdup((const char*)ok->consumer_tag.bytes, ok->consumer_tag.len) : 0;

	struct timeval tv;

	if(callback){
		self.fstop=false;
		for(int received=0; !self.fstop && (count<0 || received<count); received++){
			amqp_envelope_t envelope;
			memset(&envelope, 0, sizeof(envelope));
			amqp_maybe_release_buffers(self.connection());
			// timeout is the max idle gap between messages, re-armed on every wait
			amqp_rpc_reply_t res = amqp_consume_message_checked(self.connection(), self.channel(), &envelope, amqp_wait_tv(tv, timeout));
			if(res.reply_type == AMQP_RESPONSE_NORMAL){
				VHash *vh=amqp_message_hash(envelope);
				Value *params_cb[]={vh};
				METHOD_FRAME_ACTION(*callback->method, r.method_frame, callback->self, {
					frame.store_params(params_cb, 1);
					r.call(frame);
				});
				amqp_destroy_envelope(&envelope);
			} else if(res.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION && res.library_error == AMQP_STATUS_TIMEOUT) {
				break; // idle too long - stop cleanly
			} else {
				check(self, res, "consume ");
			}
		}
	} else {
		VArray& result=*new VArray();
		ArrayValue& result_array=result.array();

		for(int received=0; count<0 || received<count; received++){
			amqp_envelope_t envelope;
			memset(&envelope, 0, sizeof(envelope));
			amqp_maybe_release_buffers(self.connection());
			// timeout applies only to the first message of the batch;
			// the rest is drained with a non-blocking poll until nothing more is ready
			amqp_rpc_reply_t res = amqp_consume_message_checked(self.connection(), self.channel(), &envelope, amqp_wait_tv(tv, received==0 ? timeout : 0));
			if(res.reply_type == AMQP_RESPONSE_NORMAL){
				result_array+=amqp_message_hash(envelope);
				amqp_destroy_envelope(&envelope);
			} else if(res.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION && res.library_error == AMQP_STATUS_TIMEOUT) {
				break; // nothing (more) ready - return what we have
			} else {
				check(self, res, "consume ");
			}
		}
		r.write(result);
	}

	if(ok){
		amqp_basic_cancel(self.connection(), self.channel(), amqp_cstring_bytes(consumer_tag_copy));
		check(self, amqp_get_rpc_reply(self.connection()));
	}
}

static void _stop_consume(Request& r, MethodParams&) {
	VAmqp& self=GET_SELF(r, VAmqp);
	self.fstop=true;
}

#endif // WITH_AMQP

// constructor
MAmqp::MAmqp(): Methoded("amqp") {
	add_native_method("create", Method::CT_DYNAMIC, _create, 0, 1);
#ifdef WITH_AMQP
	add_native_method("publish", Method::CT_DYNAMIC, _publish, 1, 2);
	add_native_method("release", Method::CT_DYNAMIC, _release, 0, 0);
	add_native_method("ack", Method::CT_DYNAMIC, _ack, 1, 1);
	add_native_method("nack", Method::CT_DYNAMIC, _nack, 1, 2);
	add_native_method("reject", Method::CT_DYNAMIC, _reject, 1, 2);
	add_native_method("qos", Method::CT_DYNAMIC, _qos, 0, 1);
	add_native_method("declare", Method::CT_DYNAMIC, _declare, 1, 1);
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 1, 1);
	add_native_method("bind", Method::CT_DYNAMIC, _bind, 1, 1);
	add_native_method("unbind", Method::CT_DYNAMIC, _unbind, 1, 1);
	add_native_method("purge", Method::CT_DYNAMIC, _purge, 1, 1);
	add_native_method("info", Method::CT_DYNAMIC, _info, 1, 1);
	add_native_method("consume", Method::CT_DYNAMIC, _consume, 1, 1);
	add_native_method("stop_consume", Method::CT_DYNAMIC, _stop_consume, 0, 0);
#endif // WITH_AMQP
}
