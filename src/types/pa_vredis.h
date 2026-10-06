/** @file
	Parser: @b redis class decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_VREDIS_H
#define PA_VREDIS_H

#define IDENT_PA_VREDIS_H "$Id: pa_vredis.h,v 1.1 2026/10/06 23:34:31 moko Exp $"

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_common.h"
#include "pa_os.h"

#ifdef WITH_REDIS
#include <hiredis/hiredis.h>
#endif

// defines
#define VREDIS_TYPE "redis"
#define MAX_REDIS_COMMAND_PARAMS 10000

// externs
extern Methoded *redis_class;

#ifdef WITH_REDIS
/// classes/redis.C: the native code of ^r.command_name[arguments...]
void redis_command_method(Request& r, MethodParams& params);

class Temp_redis_reply {
	redisReply* freply;
public:
	Temp_redis_reply(redisReply* areply): freply(areply) {}
	~Temp_redis_reply() { if(freply) freeReplyObject(freply); }
};
#endif

class VRedis: public VStateless_object {
public:
	// value
	override const char* type() const { return VREDIS_TYPE; }
	override VStateless_class *get_class() { return redis_class; }

#ifdef WITH_REDIS
#ifdef FEATURE_GET_ELEMENT4CALL
	override Value* get_element4call(const String& aname) {
#else
	override Value* get_element(const String& aname) {
#endif
		if(Value* result=VStateless_object::get_element(aname))
			return result;

		Method* method=new Method(
			Method::CT_DYNAMIC,
			0, MAX_REDIS_COMMAND_PARAMS,
			0/*params_names*/, 0/*locals_names*/,
			0/*parser_code*/, redis_command_method, false/*all_vars_local*/
#ifdef OPTIMIZE_RESULT
			, Method::RO_USE_WCONTEXT
#endif
#ifdef OPTIMIZE_CALL
			, Method::CO_WITHOUT_WCONTEXT
#endif
			);
		method->name=&aname;
		return method->get_vjunction(*this);
}

public: // usage

	VRedis(): fcontext(0), fuser(0), fpassword(0), fdb(0), fprotocol(3), fauto_reconnect(0) {}

	redisContext* fcontext;
	const char* fuser;
	const char* fpassword;
	int fdb;
	int fprotocol;
	int fauto_reconnect; // 0 = disabled, N>0 = sleeps N sec before reconnecting

	/// HELLO or AUTH, then SELECT: on open and on reconnect
	void handshake() {
		if(fprotocol==3) {
			const char* argv[5]={"HELLO", "3"};
			int argc=2;
			if(fpassword) {
				argv[argc++]="AUTH";
				argv[argc++]=fuser ? fuser : "default";
				argv[argc++]=fpassword;
			}
			void_command(argc, argv);
		} else if(fpassword) {
			const char* argv[3]={"AUTH"};
			int argc=1;
			if(fuser)
				argv[argc++]=fuser;
			argv[argc++]=fpassword;
			void_command(argc, argv);
		}
		if(fdb) {
			const char* argv[2]={"SELECT", pa_itoa(fdb)};
			void_command(2, argv);
		}
	}

	/// sends the command and returns the reply, which the caller frees
	redisReply* command(int argc, const char** argv, const size_t* lengths=0) {
		redisReply* reply=(redisReply*)redisCommandArgv(context(), argc, argv, lengths);
		if(!reply)
			throw Exception("redis", 0, "%s failed: %s", argv[0], fcontext->errstr);
		return reply;
	}

	/// a command whose reply is only checked for an error
	void void_command(int argc, const char** argv) {
		redisReply* reply=command(argc, argv);
		Temp_redis_reply temp_reply(reply);
		if(reply->type==REDIS_REPLY_ERROR)
			throw Exception("redis", 0, "%s failed: %s", argv[0], reply->str);
	}

private:

	/// reconnects first if the connection is broken and auto_reconnect is enabled
	redisContext* context() {
		if(!fcontext)
			throw Exception("redis", 0, "using unopened or released redis object");
		if(fcontext->err) {
			if(fauto_reconnect <= 0)
				throw Exception("redis", 0, "connection is broken (auto_reconnect is not enabled): %s", fcontext->errstr);
			pa_sleep(fauto_reconnect, 0);
			if(redisReconnect(fcontext) != REDIS_OK)
				throw Exception("redis", 0, "reconnect failed: %s", fcontext->errstr);
			handshake();
		}
		return fcontext;
	}

#endif // WITH_REDIS
};

#endif
