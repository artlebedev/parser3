/** @file
	Parser: @b math parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: math.C,v 1.3 2001/07/07 16:38:01 parser Exp $"; 

#include "pa_config_includes.h"
#include "pa_common.h"
#include "pa_vint.h"
#include "pa_vmath.h"
#include "pa_request.h"

// defines

#define PI 3.1415926535
#define MATH_CLASS_NAME "math"

// class

class MMath : public Methoded {
public:
	MMath(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _random(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& range=params->as_junction(0, "range must be expression");
    uint max=(uint)r.process(range).as_double();
    if(max<=1)
		PTHROW(0, 0,
			&method_name,
			"bad range [0..%u]", max);
	
	r.write_no_lang(*new(pool) VInt(pool, rand()%max));
}


typedef double (*math1_func_ptr)(double);
static double round(double param) { return floor(param+0.5); }
static double sign(double param) { return param > 0 ? 1 : ( param < 0 ? -1 : 0 ); }
static double trunc(double param) { return param > 0? floor(param) : ceil(param); }
static double frac(double param) { return param-trunc(param); }
static double degrees(double param) { return param /PI *180; }
static double radians(double param) { return param /180 *PI; }

static void math1(Request& r, 
				  const String& method_name, MethodParams *params,
				  math1_func_ptr func) {
	Pool& pool=r.pool();
	Value& param=params->as_junction(0, "parameter must be expression");

	Value& result=*new(pool) VDouble(pool, (*func)(r.process(param).as_double()));
	result.set_name(method_name);
	r.write_no_lang(result);
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

	Value& result=*new(pool) VDouble(pool, (*func)(
		r.process(a).as_double(),
		r.process(b).as_double()));
	result.set_name(method_name);
	r.write_no_lang(result);
}

#define MATH2(name) \
	static void _##name(Request& r, const String& method_name, MethodParams *params) {\
		math2(r, method_name, params, &name);\
	}
MATH2(pow);

// constructor

MMath::MMath(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), MATH_CLASS_NAME));


	// setting seed
	srand(getpid()+time(NULL));  rand();
	
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
