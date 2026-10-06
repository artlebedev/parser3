/** @file
	Parser: @b redis parser class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#include "pa_common.h"
#include "pa_vredis.h"
#include "pa_request.h"
#include "pa_vmethod_frame.h"
#include "pa_varray.h"
#include "pa_vbool.h"
#include "pa_vfile.h"

volatile const char * IDENT_REDIS_C="$Id: redis.C,v 1.2 2026/10/06 23:54:23 moko Exp $" IDENT_PA_VREDIS_H;

// defines

#define DEFAULT_TIMEOUT 2 // seconds

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
	if(self.fcontext) {
		redisFree(self.fcontext);
		self.fcontext=0;
	}

	redisContext* context=redisConnectWithOptions(&options);
	if(!context)
		throw Exception("redis", 0, "connect failed: out of memory");
	if(context->err) {
		const char* error=pa_strdup(context->errstr);
		redisFree(context);
		throw Exception("redis", 0, "connect failed: %s", error);
	}

	self.fcontext=context;
	self.fuser=user;
	self.fpassword=password;
	self.fdb=db;
	self.fprotocol=protocol;
	self.fauto_reconnect=auto_reconnect;

	try {
		self.handshake();
	} catch(...) {
		redisFree(context);
		self.fcontext=0;
		throw;
	}
}

static void _release(Request& r, MethodParams&) {
	VRedis& self=GET_SELF(r, VRedis);

	if(self.fcontext) {
		redisFree(self.fcontext);
		self.fcontext=0;
	}
}

static Value* reply_value(redisReply& reply, bool binary, const String& command) {
	switch(reply.type) {
		case REDIS_REPLY_STRING:
		case REDIS_REPLY_STATUS:
		case REDIS_REPLY_VERB:
		case REDIS_REPLY_BIGNUM: {
			char* data=pa_strdup(reply.str, reply.len);
			if(binary) {
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
				array+=reply_value(*reply.element[i], binary, command);
			return &result;
		}
		case REDIS_REPLY_MAP: {
			VHash& result=*new VHash;
			for(size_t i=0; i+1<reply.elements; i+=2)
				result.hash().put(reply_value(*reply.element[i], false, command)->as_string(), reply_value(*reply.element[i+1], binary, command));
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
static void redis_command(Request& r, MethodParams& params, const String& command, size_t first, bool binary) {
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

	r.write(*reply_value(*reply, binary, command));
}

// ^r.call[command;arguments...]
static void _call(Request& r, MethodParams& params) {
	redis_command(r, params, params.as_string(0, "command must be string"), 1, false);
}

// ^r.call_binary[command;arguments...]: the strings of the reply are files
static void _call_binary(Request& r, MethodParams& params) {
	redis_command(r, params, params.as_string(0, "command must be string"), 1, true);
}

// ^r.command_name[arguments...]: the command is the name of the method called, see VRedis::get_element4call
void redis_command_method(Request& r, MethodParams& params) {
	redis_command(r, params, *r.get_method_frame()->method.name, 0, false);
}

#else

static void _open(Request&, MethodParams&) {
	throw Exception("redis", 0, "compiled without redis support");
}

#endif // WITH_REDIS

MRedis::MRedis(): Methoded("redis") {
	// ^redis::open[ $.host[localhost] $.port(6379) $.unix_socket[path] $.user[] $.password[] $.db(0) $.timeout(seconds, 0 is none) $.protocol(3) $.auto_reconnect(seconds) ]
	add_native_method("open", Method::CT_DYNAMIC, _open, 0, 1);

#ifdef WITH_REDIS
	// ^r.call[command;arguments...]
	add_native_method("call", Method::CT_DYNAMIC, _call, 1, MAX_REDIS_COMMAND_PARAMS);

	// ^r.call_binary[command;arguments...]
	add_native_method("call_binary", Method::CT_DYNAMIC, _call_binary, 1, MAX_REDIS_COMMAND_PARAMS);

	// ^r.release[]
	add_native_method("release", Method::CT_DYNAMIC, _release, 0, 0);
#endif // WITH_REDIS
}
