/*
  $Id: pa_bool.h,v 1.1 2001/03/08 16:54:25 paf Exp $
*/

#ifndef PA_BOOL_H
#define PA_BOOL_H

#ifdef HAVE_CONFIG_H
#include "pa_config.h"
#endif

#include "pa_pool.h"

class Bool : public Pooled {
public:

	Bool(Pool& apool, bool avalue) : Pooled(apool),
		value(avalue) {
	}

//	bool operator == (const Bool& src) const {

public:

	bool value;

private: //disabled

	Bool& operator = (const Bool&) { return *this; }

};

#endif
