/** @file
	Parser: @b amqp class decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_VAMQP_H
#define PA_VAMQP_H

#define IDENT_PA_VAMQP_H "$Id: pa_vamqp.h,v 1.2 2026/04/25 13:38:46 moko Exp $"

#include "classes.h"
#include "pa_vstateless_object.h"

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
	VAmqp(): fconnection(0), fchannel(0) {}
	~VAmqp() {}

	amqp_connection_state_t fconnection;
	int fchannel;
	bool fstop;

	amqp_connection_state_t connection() {
		if(!fconnection)
			throw Exception(PARSER_RUNTIME, 0, "using uninitialized amqp object");
		return fconnection;
	}

	int channel() {
		if(!fchannel)
			throw Exception(PARSER_RUNTIME, 0, "using uninitialized amqp object channel");
		return fchannel;
	}

#endif // WITH_AMQP
};

#endif


