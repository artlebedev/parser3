/** @file
	Parser: @b random parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: random.C,v 1.10 2001/04/28 08:43:48 paf Exp $
*/

#include "pa_config_includes.h"
#include "pa_common.h"
#include "pa_vint.h"
#include "pa_request.h"

// defines

#define RANDOM_CLASS_NAME "random"

// class

class MRandom : public Methoded {
public:
	MRandom(Pool& pool);
	bool used_directly() { return true; }
};

// methods

/**	^random.generate[]
	^random.generate(range)
*/
static void _generate(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& range=params->get_junction(0, "range must be expression");
    uint max=params->size()?(uint)r.process(range).as_double():0;
    if(max<=1)
		PTHROW(0, 0,
			&method_name,
			"bad range [0..%u]", max);
	
	r.write_no_lang(*new(pool) VInt(pool, rand()%max));
}

// constructor

MRandom::MRandom(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), RANDOM_CLASS_NAME));


	// setting seed
	srand(getpid()+time(NULL));  rand();
	

	// ^random.generate[]
	// ^random.generate(range)
	add_native_method("generate", Method::CT_STATIC, _generate, 1, 1);
}

// creator

Methoded *MRandom_create(Pool& pool) {
	return new(pool) MRandom(pool);
}
