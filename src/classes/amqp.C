/** @file
	Parser: @b amqp parser class.

	Copyright (c) 2001-2025 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_varray.h"
#include "pa_vbool.h"
#include "pa_vamqp.h"

#ifdef WITH_AMQP
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_ssl_socket.h>
#include <amqp_framing.h>
#include <stdlib.h>
#include <string.h>
#endif

volatile const char * IDENT_AMQP_C="$Id: amqp.C,v 1.11 2026/01/09 03:30:39 moko Exp $" IDENT_PA_VAMQP_H;

class MAmqp: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VAmqp(); }
public:
	MAmqp();
};

DECLARE_CLASS_VAR(amqp, new MAmqp);

#ifdef WITH_AMQP

static void status_check(int ret, const char *detail=""){
	if(ret == AMQP_STATUS_OK)
		return;

	const char* error_str = amqp_error_string2(ret);
	if(error_str) {
		throw Exception("amqp", 0, "%sfailed: %s", detail, error_str);
	} else {
		throw Exception("amqp", 0, "%sfailed: error %d", detail, ret);
	}
}

static void check(amqp_rpc_reply_t rr, const char *detail=""){
	if(rr.reply_type == AMQP_RESPONSE_NORMAL)
		return;

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
		status_check(rr.library_error, detail);
	}

	throw Exception("amqp", 0, "%sfailed", detail);
}

#endif // WITH_AMQP


static void _create(Request& r, MethodParams& params) {
VAmqp& self=GET_SELF(r, VAmqp);

#ifdef WITH_AMQP
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

	if(params.count()>0){
		if(HashStringValue* options=params.as_hash(0)){
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
	
	status_check(amqp_socket_open(socket, host_c, port), tls_specified ? "open SSL socket " : "open TCP socket ");

	amqp_rpc_reply_t rlogin = amqp_login(conn, vhost_c, 0, 131072, heartbeat, AMQP_SASL_METHOD_PLAIN, user_c, pass_c);
	if(rlogin.reply_type != AMQP_RESPONSE_NORMAL){
		amqp_destroy_connection(conn);
		check(rlogin, "login ");
	}

	int channel = 1;
	amqp_channel_open(conn, channel);
	amqp_rpc_reply_t ropen = amqp_get_rpc_reply(conn);
	if(ropen.reply_type != AMQP_RESPONSE_NORMAL){
		amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
		amqp_destroy_connection(conn);
		check(ropen, "open channel ");
	}

	self.fconnection = conn;
	self.fchannel = channel;
#else
	(void)params; (void)self;
	throw Exception("amqp", 0, "compiled without amqp support");
#endif // WITH_AMQP
}

#ifdef WITH_AMQP

#define AMQP_STRING(s,l) new String(String::C(pa_strdup((const char*)(s), (l)), (l)))
#define AMQP_VSTRING(s,l) new VString(*AMQP_STRING(s,l))

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
/*					if(HashStringValue* hh=pval->get_hash()){
						size_t count=hh->count();
						amqp_table_entry_t* entries=count ? new amqp_table_entry_t[count] : 0;
						size_t idx=0;
						for(HashStringValue::Iterator hi(*hh); hi; hi.next()){
							String::Body hkey=hi.key();
							const char* hv=hi.value()->as_string().cstr();
							entries[idx].key=amqp_cstring_bytes(hkey.cstr());
							entries[idx].value.kind=AMQP_FIELD_KIND_UTF8;
							entries[idx].value.value.bytes=amqp_cstring_bytes(hv);
							idx++;
						}
						props.headers.num_entries=(int)count;
						props.headers.entries=entries;
						props._flags|=AMQP_BASIC_HEADERS_FLAG;
					}
*/				} else
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

	if(ret!=AMQP_STATUS_OK)
		throw Exception("amqp", 0, "publish failed");

	// free temporary headers entries if allocated
	if(props._flags & AMQP_BASIC_HEADERS_FLAG){
//		delete [] props.headers.entries;
	}
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
	}
}

static void _ack(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	double tag=params.as_double(0, "delivery tag must be number", r);
	int ret = amqp_basic_ack(self.connection(), self.channel(), (uint64_t)tag, 0);
	if(ret!=AMQP_STATUS_OK)
		throw Exception("amqp", 0, "ack failed");
}

static void _nack(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const String &tag_s=params.as_string(0, "delivery tag must not be code");
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
	int ret = amqp_basic_nack(self.connection(), self.channel(), pa_atoul(tag_s.cstr()), 0, requeue);
	if(ret!=AMQP_STATUS_OK)
		throw Exception("amqp", 0, "nack failed");
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
	amqp_basic_qos_ok_t *ret = amqp_basic_qos(self.connection(), self.channel(), 0, prefetch_count, 0);
	if(!ret)
		throw Exception("amqp", 0, "qos failed");
}

static void _reject(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const String &tag_s=params.as_string(0, "delivery tag must not be code");
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
	int ret = amqp_basic_reject(self.connection(), self.channel(), pa_atoul(tag_s.cstr()), requeue);
	if(ret!=AMQP_STATUS_OK)
		throw Exception("amqp", 0, "reject failed");
}

static void _declare(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const char* exchange_c = 0;
	const char* queue_c = 0;
	const char* type_c = "direct";
	bool passive=false, durable=false, auto_delete=true, internal=false, exclusive=false;
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
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}
	if(!exchange_c && !queue_c)
		throw Exception("amqp", 0, "exchange or queue must be specified");

	if(exchange_c){
		amqp_exchange_declare(self.connection(), self.channel(), amqp_cstring_bytes(exchange_c), amqp_cstring_bytes(type_c), passive, durable, auto_delete, internal, amqp_empty_table);
		check(amqp_get_rpc_reply(self.connection()));
	}

	if(queue_c){
		amqp_queue_declare_ok_t *ok = amqp_queue_declare(self.connection(), self.channel(), *queue_c ? amqp_cstring_bytes(queue_c) : amqp_empty_bytes, passive, durable, exclusive, auto_delete, amqp_empty_table);
		check(amqp_get_rpc_reply(self.connection()));
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
		check(amqp_get_rpc_reply(self.connection()));
	}

	if(queue_c){
		amqp_queue_delete(self.connection(), self.channel(), amqp_cstring_bytes(queue_c), if_unused, if_empty);
		check(amqp_get_rpc_reply(self.connection()));
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
	check(amqp_get_rpc_reply(self.connection()));
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
	check(amqp_get_rpc_reply(self.connection()));
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
	check(amqp_get_rpc_reply(self.connection()));
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
	check(amqp_get_rpc_reply(self.connection()));

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
	return result;
}

static void _consume(Request& r, MethodParams& params) {
	VAmqp& self=GET_SELF(r, VAmqp);
	const char* queue_c=0;
	const char* consumer_tag_c=0;
	bool no_ack=true, exclusive=false;
	int count=1;
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
			} else if(key=="no_ack"){
				no_ack=r.process(*value).as_bool();
			} else if(key=="exclusive"){
				exclusive=r.process(*value).as_bool();
			} else if(key=="count"){
				count=r.process(*value).as_int();
			} else
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}

	if(!queue_c) throw Exception("amqp", 0, "queue must be specified");

	amqp_basic_consume_ok_t *ok = amqp_basic_consume(self.connection(), self.channel(), amqp_cstring_bytes(queue_c),
		consumer_tag_c ? amqp_cstring_bytes(consumer_tag_c) : amqp_empty_bytes,
		0 /*no_local*/, no_ack, exclusive, amqp_empty_table);
	check(amqp_get_rpc_reply(self.connection()));

	if(callback){
		self.fstop=false;
		while(!self.fstop){
			amqp_envelope_t envelope;
			memset(&envelope, 0, sizeof(envelope));
			amqp_maybe_release_buffers(self.connection());
			amqp_rpc_reply_t res = amqp_consume_message(self.connection(), &envelope, NULL, 0);
			if(res.reply_type == AMQP_RESPONSE_NORMAL){
				VHash *vh=amqp_message_hash(envelope);
				Value *params_cb[]={vh};
				METHOD_FRAME_ACTION(*callback->method, r.method_frame, callback->self, {
					frame.store_params(params_cb, 1);
					r.call(frame);
				});
				amqp_destroy_envelope(&envelope);
			} else if(res.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION && res.library_error == AMQP_STATUS_TIMEOUT) {
				continue;
			} else {
				break;
			}
		}
	} else {
		VArray& result=*new VArray();
		ArrayValue& result_array=result.array();

		for(int i=0; i<count; i++){
			amqp_envelope_t envelope;
			memset(&envelope, 0, sizeof(envelope));
			amqp_maybe_release_buffers(self.connection());
			amqp_rpc_reply_t res = amqp_consume_message(self.connection(), &envelope, NULL, 0);
			if(res.reply_type == AMQP_RESPONSE_NORMAL){
				result_array+=amqp_message_hash(envelope);
				amqp_destroy_envelope(&envelope);
			} else {
				check(res);
			}
		}
		r.write(result);
	}

	if(ok){
		amqp_basic_cancel(self.connection(), self.channel(), ok->consumer_tag);
		check(amqp_get_rpc_reply(self.connection()));
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
