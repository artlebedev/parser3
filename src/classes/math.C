/** @file
	Parser: @b math parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: math.C,v 1.19 2002/06/21 12:42:19 paf Exp $
*/

#include "pa_common.h"
#include "pa_vint.h"
#include "pa_vmath.h"
#include "pa_request.h"
#include "pa_md5.h"

#ifdef WIN32
// for threadID
#	include <windows.h>
#endif

// defines

#define PI 3.1415926535

// class

class MMath : public Methoded {
public:
	MMath(Pool& pool);
	void configure_admin(Request& r);

public: // Methoded
	bool used_directly() { return false; }
};

// methods
static unsigned int randomizer=0;
static void _random(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& range=params->as_junction(0, "range must be expression");
    double top=r.process_to_value(range).as_double();
    if(top<=1)
		throw Exception("parser.runtime",
			&method_name,
			"top must be above 1 (%g)", top);
	
	int result=(int)( ((double)((randomizer=rand())% RAND_MAX)) / RAND_MAX * uint(top) );
	r.write_no_lang(*new(pool) VInt(pool, result));
}


typedef double (*math1_func_ptr)(double);
static double frac(double param) { return param-trunc(param); }
static double degrees(double param) { return param /PI *180; }
static double radians(double param) { return param /180 *PI; }

static void math1(Request& r, 
				  const String& method_name, MethodParams *params,
				  math1_func_ptr func) {
	Pool& pool=r.pool();
	Value& param=params->as_junction(0, "parameter must be expression");

	double result=(*func)(r.process_to_value(param).as_double());
	r.write_no_lang(*new(pool) VDouble(pool, result));
}

#define MATH1(name) \
	static void _##name(Request& r, const String& method_name, MethodParams *params) {\
		math1(r, method_name, params, &name);\
	}
#define MATH1P(name_parser, name_c) \
	static void _##name_parser(Request& r, const String& method_name, MethodParams *params) {\
		math1(r, method_name, params, &name_c);\
	}
MATH1(round);	MATH1(floor);	MATH1P(ceiling, ceil);
MATH1(trunc);	MATH1(frac);
MATH1P(abs, fabs);	MATH1(sign);
MATH1(exp);	MATH1(log);	
MATH1(sin);	MATH1(asin);	
MATH1(cos);	MATH1(acos);	
MATH1(tan);	MATH1(atan);
MATH1(degrees);	MATH1(radians);
MATH1(sqrt);


typedef double (*math2_func_ptr)(double, double);
static void math2(Request& r, 
				  const String& method_name, MethodParams *params,
				  math2_func_ptr func) {
	Pool& pool=r.pool();
	Value& a=params->as_junction(0, "parameter must be expression");
	Value& b=params->as_junction(1, "parameter must be expression");

	double result=(*func)(
		r.process_to_value(a).as_double(),
		r.process_to_value(b).as_double());
	r.write_no_lang(*new(pool) VDouble(pool, result));
}

#define MATH2(name) \
	static void _##name(Request& r, const String& method_name, MethodParams *params) {\
		math2(r, method_name, params, &name);\
	}
MATH2(pow);

static void _crypt(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const char *password=params->as_string(0, "password must be string").cstr();
	const char *salt=params->as_string(1, "salt must be string").cstr();

    /* FreeBSD style MD5 string 
     */
    if (strncmp(salt, PA_MD5PW_ID, PA_MD5PW_IDLEN) == 0) {
		const size_t sample_size=120;
		char *sample_buf=(char *)pool.malloc(sample_size);
		pa_MD5Encode((const unsigned char *)password,
				 (const unsigned char *)salt, sample_buf, sample_size);
		r.write_pass_lang(*new(pool) String(pool, sample_buf));
    } else
		throw Exception("parser.runtime",
			&method_name,
			"salt must start with '" PA_MD5PW_ID "'");
}

// constructor

MMath::MMath(Pool& apool) : Methoded(apool, "math") {
	// ^FUNC(expr)	
#define ADD1(name) \
	add_native_method(#name, Method::CT_STATIC, _##name, 1, 1)

	ADD1(round);	ADD1(floor);	ADD1(ceiling);
	ADD1(trunc);	ADD1(frac);
	ADD1(abs);	ADD1(sign);
	ADD1(exp);	ADD1(log);	
	ADD1(sin);	ADD1(asin);	
	ADD1(cos);	ADD1(acos);	
	ADD1(tan);	ADD1(atan);
	ADD1(degrees);	ADD1(radians);
	ADD1(sqrt);
	ADD1(random);

#define ADD2(name) \
	add_native_method(#name, Method::CT_STATIC, _##name, 2, 2)

	// ^pow(x;y)
	ADD2(pow);

	// ^crypt[password;salt]
	ADD2(crypt);
}

// in MSVC each thread has it's own pseudo-random sequence
// in win32 apache each thread can handle multiple requests
// so to get proper randoms we remember random generated in one thread
void MMath::configure_admin(Request&) {
	// setting seed
	srand(
		randomizer
#ifdef WIN32
		^ GetCurrentThreadId()
#else
		^ getpid()
#endif
		^ (unsigned int)time(NULL)
	);
	if(!randomizer)
		randomizer=rand();
}

// global variables

Methoded *math_base_class;
Hash *math_consts;

// creator

Methoded *MMath_create(Pool& pool) {
	math_consts=new(pool) Hash(pool);
	math_consts->put(
		*new(pool) String(pool, "PI"), 
		new(pool) VDouble(pool, PI));

	return math_base_class=new(pool) MMath(pool);
}
