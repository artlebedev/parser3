/** @file
	Parser: @b amqp class decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_VAMQP_H
#define PA_VAMQP_H

#define IDENT_PA_VAMQP_H "$Id: pa_vamqp.h,v 1.4 2026/09/20 11:38:43 moko Exp $"

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_common.h"

#ifdef WITH_AMQP
#include <amqp.h>
#endif

// defines
#define VAMQP_TYPE "amqp"

// externs
extern Methoded *amqp_class;

class VAmqp: public VStateless_object {
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

	VAmqp(): fconnection(0), fchannel(0), fstate(CONNECTION_DEAD), fcreate_options(0), freconnect_interval(0) {}
	~VAmqp() {}

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

#endif // WITH_AMQP
};

#endif


