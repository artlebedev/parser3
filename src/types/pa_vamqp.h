/** @file
	Parser: @b amqp class decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_VAMQP_H
#define PA_VAMQP_H

#define IDENT_PA_VAMQP_H "$Id: pa_vamqp.h,v 1.6 2026/10/09 21:10:38 moko Exp $"

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_common.h"
#include "pa_pool.h"

//for librabbitmq before 0.12
//#define PA_AMQP_COMPAT

#ifdef WITH_AMQP

#ifdef PA_AMQP_COMPAT

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_framing.h>
#ifdef WITH_AMQP_SSL
#include <amqp_ssl_socket.h>
#endif

#else

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <rabbitmq-c/framing.h>
#ifdef WITH_AMQP_SSL
#include <rabbitmq-c/ssl_socket.h>
#endif

#endif

#endif

// defines
#define VAMQP_TYPE "amqp"

// externs
extern Methoded *amqp_class;

class VAmqp: public VStateless_object, Pooled {
public:
	// value
	override const char* type() const { return VAMQP_TYPE; }
	override VStateless_class *get_class() { return amqp_class; }

#ifdef WITH_AMQP
public: // usage

	// ALIVE: usable right now
	// CHANNEL_DEAD: last operation got a channel-level error - reopening the channel (same connection) is required
	// CONNECTION_DEAD: last operation got a connection-level error - a full reconnect is required
	enum ConnState { ALIVE, CHANNEL_DEAD, CONNECTION_DEAD };

	VAmqp(Pool& apool): Pooled(apool), fconnection(0), fchannel(0), fstate(CONNECTION_DEAD), fcreate_options(0), freconnect_interval(0) {}

	/// called by the request pool at the end of the request
	override ~VAmqp() { release(); }

	amqp_connection_state_t fconnection;
	amqp_channel_t fchannel;
	bool fstop;
	ConnState fstate;
	HashStringValue* fcreate_options;
	int freconnect_interval; // 0 = auto-reconnect disabled, N>0 = enabled, sleeps N sec before each attempt

	void ensure_connected();

	amqp_connection_state_t connection() {
		if(!fconnection)
			throw Exception(PARSER_RUNTIME, 0, "using uninitialized amqp object");
		ensure_connected();
		return fconnection;
	}

	amqp_channel_t channel() {
		if(!fchannel)
			throw Exception(PARSER_RUNTIME, 0, "using uninitialized amqp object channel");
		ensure_connected();
		return fchannel;
	}

	void release() {
		if(!fconnection)
			return;
		if(fstate==ALIVE) { // a dead connection or channel is not closed politely
			amqp_channel_close(fconnection, fchannel, AMQP_REPLY_SUCCESS);
			amqp_connection_close(fconnection, AMQP_REPLY_SUCCESS);
		}
		amqp_destroy_connection(fconnection);
		fconnection=0;
		fchannel=0;
		fstate=CONNECTION_DEAD;
	}

#else
	VAmqp(Pool& apool): Pooled(apool) {}
#endif // WITH_AMQP
};

#endif


