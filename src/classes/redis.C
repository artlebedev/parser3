/** @file
	Parser: @b redis parser class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_vredis.h"
#include "pa_request.h"
#include "pa_varray.h"
#include "pa_vbool.h"
#include "pa_vfile.h"

volatile const char * IDENT_REDIS_C="$Id: redis.C,v 1.6 2026/10/09 00:30:40 moko Exp $" IDENT_PA_VREDIS_H;

// defines

#define DEFAULT_TIMEOUT 2 // seconds
// ^r.f-get[key], ^r.f-call[GET;key]: the strings of the reply are files
#define FILE_PREFIX "f-"

class MRedis: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VRedis(); }
public:
	MRedis();
};

DECLARE_CLASS_VAR(redis, new MRedis);

#ifdef WITH_REDIS

static void _open(Request& r, MethodParams& params) {
	VRedis& self=GET_SELF(r, VRedis);

	const char* host="localhost";
	int port=6379;
	bool tcp_specified=false;
	const char* unix_socket=0;
	double timeout=DEFAULT_TIMEOUT;
	const char* user=0;
	const char* password=0;
	int db=0;
	int protocol=3;
	int auto_reconnect=0;
#ifdef WITH_REDIS_SSL
	bool tls_specified=false;
	const char* tls_ca=0;
	const char* tls_cert=0;
	const char* tls_key=0;
	bool tls_verify=true;
#endif

	if(params.count()>0)
		if(HashStringValue* options=params.as_hash(0)) {
			for(HashStringValue::Iterator i(*options); i; i.next()) {
				String::Body key=i.key();
				Value* value=i.value();
				if(key=="host") {
					host=value->as_string().cstr();
					tcp_specified=true;
				} else if(key=="port") {
					port=r.process(*value).as_int();
					tcp_specified=true;
				} else if(key=="unix_socket") {
					unix_socket=value->as_string().cstr();
				} else if(key=="timeout") {
					timeout=r.process(*value).as_double();
				} else if(key=="user") {
					user=value->as_string().cstr();
				} else if(key=="password") {
					password=value->as_string().cstr();
				} else if(key=="db") {
					db=r.process(*value).as_int();
				} else if(key=="protocol") {
					protocol=r.process(*value).as_int();
					if(protocol!=2 && protocol!=3)
						throw Exception(PARSER_RUNTIME, 0, "protocol must be 2 or 3");
				} else if(key=="auto_reconnect") {
					auto_reconnect=r.process(*value).as_int();
				} else if(key=="tls") {
#ifdef WITH_REDIS_SSL
					tls_specified=true;
					if(HashStringValue* tls_options=value->get_hash())
						for(HashStringValue::Iterator t(*tls_options); t; t.next()) {
							String::Body tkey=t.key();
							Value* tvalue=t.value();
							if(tkey=="ca") {
								tls_ca=tvalue->as_string().cstr();
							} else if(tkey=="cert") {
								tls_cert=tvalue->as_string().cstr();
							} else if(tkey=="key") {
								tls_key=tvalue->as_string().cstr();
							} else if(tkey=="verify") {
								tls_verify=r.process(*tvalue).as_bool();
							} else
								throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
						}
#else
					throw Exception("redis", 0, "compiled without redis SSL support");
#endif
				} else
					throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			}
		}

	if(unix_socket && tcp_specified)
		throw Exception(PARSER_RUNTIME, 0, "unix_socket can not be used with host or port");

	redisOptions options;
	memset(&options, 0, sizeof(options));
	if(unix_socket)
		REDIS_OPTIONS_SET_UNIX(&options, unix_socket);
	else
		REDIS_OPTIONS_SET_TCP(&options, host, port);

	struct timeval tv;
	if(timeout>0) { // 0 is none
		tv.tv_sec=(long)timeout;
		tv.tv_usec=(long)((timeout-tv.tv_sec)*1000000);
		options.connect_timeout=&tv;
		options.command_timeout=&tv;
	}

	// ^r.open[] of an opened object: close the old connection first
	self.release();

	self.fuser=user;
	self.fpassword=password;
	self.fdb=db;
	self.fprotocol=protocol;
	self.fauto_reconnect=auto_reconnect;

	try {
#ifdef WITH_REDIS_SSL
		if(tls_specified) {
			static bool ssl_initialized=false; // OpenSSL before 1.1 needs it once
			if(!ssl_initialized) {
				redisInitOpenSSL();
				ssl_initialized=true;
			}

			redisSSLOptions ssl_options;
			memset(&ssl_options, 0, sizeof(ssl_options));
			ssl_options.cacert_filename=tls_ca;
			ssl_options.cert_filename=tls_cert;
			ssl_options.private_key_filename=tls_key;
			ssl_options.server_name=host;
			// as amqp: the server certificate is checked only against the given ca
			ssl_options.verify_mode=tls_verify && tls_ca ? REDIS_SSL_VERIFY_PEER : REDIS_SSL_VERIFY_NONE;

			redisSSLContextError error=REDIS_SSL_CTX_NONE;
			if(!(self.fssl_context=redisCreateSSLContextWithOptions(&ssl_options, &error)))
				throw Exception("redis", 0, "SSL context failed: %s", redisSSLContextGetError(error));
		}
#endif

		if(!(self.fcontext=redisConnectWithOptions(&options)))
			throw Exception("redis", 0, "connect failed: out of memory");
		if(self.fcontext->err)
			throw Exception("redis", 0, "connect failed: %s", self.fcontext->errstr);

		self.handshake();
	} catch(...) {
		self.release();
		throw;
	}
}

static void _release(Request& r, MethodParams&) {
	GET_SELF(r, VRedis).release();
}

static Value* reply_value(redisReply& reply, bool as_file, const String& command) {
	switch(reply.type) {
		case REDIS_REPLY_STRING:
		case REDIS_REPLY_STATUS:
		case REDIS_REPLY_VERB:
		case REDIS_REPLY_BIGNUM: {
			char* data=pa_strdup(reply.str, reply.len);
			if(as_file) {
				VFile& file=*new VFile;
				file.set_binary(true/*tainted*/, data, reply.len);
				return &file;
			}
			return new VString(*new String(String::C(data, reply.len), String::L_TAINTED));
		}
		case REDIS_REPLY_INTEGER:
			if(reply.integer>=PA_WINT_MIN && reply.integer<=PA_WINT_MAX)
				return new VInt(reply.integer);
			// beyond the precise int range
			return new VString(*new String(pa_itoa(reply.integer)));
		case REDIS_REPLY_DOUBLE:
			return new VDouble(reply.dval);
		case REDIS_REPLY_BOOL:
			return &VBool::get(reply.integer!=0);
		case REDIS_REPLY_NIL:
			return VVoid::get();
		case REDIS_REPLY_ARRAY:
		case REDIS_REPLY_SET:
		case REDIS_REPLY_PUSH: {
			VArray& result=*new VArray(reply.elements);
			ArrayValue& array=result.array();
			for(size_t i=0; i<reply.elements; i++)
				array+=reply_value(*reply.element[i], as_file, command);
			return &result;
		}
		case REDIS_REPLY_MAP: {
			VHash& result=*new VHash;
			for(size_t i=0; i+1<reply.elements; i+=2)
				result.hash().put(reply_value(*reply.element[i], false, command)->as_string(), reply_value(*reply.element[i+1], as_file, command));
			return &result;
		}
		case REDIS_REPLY_ERROR:
			throw Exception("redis", &command, "%s", reply.str);
		case REDIS_REPLY_ATTR:
			return VVoid::get();
	}
	throw Exception("redis", &command, "unknown reply type %d", reply.type);
}

// a string, a number, a bool or a file
static void add_argument(Array<const char*>& argv, Array<size_t>& lengths, Value& value) {
	if(VFile* file=dynamic_cast<VFile*>(&value)) {
		argv+=file->value_ptr();
		lengths+=file->value_size();
	} else if(dynamic_cast<VBool*>(&value)) {
		// as redis itself encodes booleans in integer replies
		argv+=value.as_bool() ? "1" : "0";
		lengths+=1;
	} else if(dynamic_cast<VArray*>(&value) || dynamic_cast<VHash*>(&value)) {
		throw Exception(PARSER_RUNTIME, 0, "redis command argument can not be a nested array or a hash");
	} else {
		const char* string=value.as_string().cstr();
		argv+=string;
		lengths+=strlen(string);
	}
}

// arguments start at params[first]: strings, numbers, files, not nested arrays and hashes as key value pairs
static void redis_command(Request& r, MethodParams& params, const String& command, size_t first, bool as_file) {
	VRedis& self=GET_SELF(r, VRedis);

	Array<const char*> argv;
	Array<size_t> lengths;
	const char* command_cstr=command.cstr();
	argv+=command_cstr;
	lengths+=strlen(command_cstr);

	for(size_t i=first; i<params.count(); i++) {
		Value& param=params[i];
		if(VArray* array=dynamic_cast<VArray*>(&param)) {
			for(ArrayValue::Iterator e(array->array()); e; e.next())
				if(e.value())
					add_argument(argv, lengths, *e.value());
		} else if(VHash* hash=dynamic_cast<VHash*>(&param)) {
			for(HashStringValue::Iterator e(hash->hash()); e; e.next()) {
				const char* key=e.key().cstr();
				argv+=key;
				lengths+=strlen(key);
				add_argument(argv, lengths, *e.value());
			}
		} else
			add_argument(argv, lengths, param);
	}

	redisReply* reply=self.command((int)argv.count(), argv.ptr(0), lengths.ptr(0));
	Temp_redis_reply temp_reply(reply);

	r.write(*reply_value(*reply, as_file, command));
}

static void _call(Request& r, MethodParams& params) {
	redis_command(r, params, params.as_string(0, "command must be string"), 1, false);
}

static void _file_call(Request& r, MethodParams& params) {
	redis_command(r, params, params.as_string(0, "command must be string"), 1, true);
}

// ^r.command_name[arguments...]: the command is the name of the method called, see VRedis::get_element4call
void redis_command_method(Request& r, MethodParams& params) {
	const String& name=*r.get_method_frame()->method.name;
	if(name.starts_with(FILE_PREFIX)) {
		size_t prefix_length=strlen(FILE_PREFIX);
		if(name.length()==prefix_length)
			throw Exception(PARSER_RUNTIME, &name, "command name expected after " FILE_PREFIX);
		redis_command(r, params, name.mid(prefix_length, name.length()), 0, true);
	} else
		redis_command(r, params, name, 0, false);
}

#else

static void _open(Request&, MethodParams&) {
	throw Exception("redis", 0, "compiled without redis support");
}

#endif // WITH_REDIS

MRedis::MRedis(): Methoded("redis") {
	// ^redis::open[ $.host[localhost] $.port(6379) $.unix_socket[path] $.user[] $.password[] $.db(0) $.timeout(seconds, 0 is none) $.protocol(3) $.auto_reconnect(seconds)
	//	$.tls[ $.ca[path] $.cert[path] $.key[path] $.verify(true) ] ]
	add_native_method("open", Method::CT_DYNAMIC, _open, 0, 1);

#ifdef WITH_REDIS
	// ^r.call[command;arguments...]
	add_native_method("call", Method::CT_DYNAMIC, _call, 1, MAX_REDIS_COMMAND_PARAMS);

	// ^r.f-call[command;arguments...]
	add_native_method(FILE_PREFIX "call", Method::CT_DYNAMIC, _file_call, 1, MAX_REDIS_COMMAND_PARAMS);

	// ^r.release[]
	add_native_method("release", Method::CT_DYNAMIC, _release, 0, 0);
#endif // WITH_REDIS
}
