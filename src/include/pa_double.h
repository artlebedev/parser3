/*
  $Id: pa_double.h,v 1.1 2001/03/08 16:54:25 paf Exp $
*/

#ifndef PA_DOUBLE_H
#define PA_DOUBLE_H

#ifdef HAVE_CONFIG_H
#include "pa_config.h"
#endif

#include "pa_pool.h"

class Double : public Pooled {
public:

	Double(Pool& apool, double avalue) : Pooled(apool),
		value(avalue) {
	}

	bool operator < (const Double& src) const {	return value<src.value; }
	bool operator > (const Double& src) const {	return value>src.value; }
	bool operator <= (const Double& src) const { return value<=src.value; }
	bool operator >= (const Double& src) const { return value>=src.value; }
	bool operator == (const Double& src) const { return value==src.value; }
	bool operator != (const Double& src) const { return value!=src.value; }

public:

	double value;

private: //disabled

	Double& operator = (const Double&) { return *this; }

};

#endif
