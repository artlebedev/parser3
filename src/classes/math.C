/** @file
	Parser: @b math parser class.

	Copyright(c) 2001, 2003 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)

	portions from gen_uuid.c,
	Copyright (C) 1996, 1997, 1998, 1999 Theodore Ts'o.
*/

static const char* IDENT_MATH_C="$Date: 2003/07/24 08:19:20 $";

#include "pa_common.h"
#include "pa_vint.h"
#include "pa_vmath.h"
#include "pa_request.h"
#include "pa_md5.h"
#include "pa_threads.h"

#ifdef WIN32
#	define _WIN32_WINNT 0x400
#	include <windows.h>
#	include <wincrypt.h>
#endif

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

// defines

#define PI 3.1415926535
#define MAX_SALT 8

// class

class MMath : public Methoded {
public:
	MMath(Pool& pool);

public: // Methoded
	bool used_directly() { return false; }
};

#ifdef WIN32
class Random_provider {
	HCRYPTPROV fhProv;
	
	void acquire() {
		SYNCHRONIZED;

		if(fhProv)
			return;

		if(!CryptAcquireContext(&fhProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
			throw Exception(0,
				0,
				"CryptAcquireContext failed");
	}
	void release() {
		if(fhProv)
			CryptReleaseContext(fhProv, 0);
	}
	
public:
	Random_provider(): fhProv(0) {}
	~Random_provider() { release(); }
	void generate(void *buffer, size_t size) {
		acquire();

		if(!CryptGenRandom(fhProv, size, (BYTE*)buffer))
			throw Exception(0,
				0,
				"CryptGenRandom failed");
	}
}
	random_provider;

#else

/// from gen_uuid.c
static int get_random_fd(void)
{
        struct timeval  tv;
        static int      fd = -2;
        int             i;

        if (fd == -2) {
                gettimeofday(&tv, 0);
                fd = open("/dev/urandom", O_RDONLY);
                if (fd == -1)
                        fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
                srand((getpid() << 16) ^ getuid() ^ tv.tv_sec ^ tv.tv_usec);
        }
        /* Crank the random number generator a few times */
        gettimeofday(&tv, 0);
        for (i = (tv.tv_sec ^ tv.tv_usec) & 0x1F; i > 0; i--)
                rand();
        return fd;
}


/*
 * Generate a series of random bytes.  Use /dev/urandom if possible,
 * and if not, use srandom/random.
 */
static void get_random_bytes(void *buf, int nbytes)
{
        int i, fd = get_random_fd();
        int lose_counter = 0;
        char *cp = (char *) buf;

        if (fd >= 0) {
                while (nbytes > 0) {
                        i = read(fd, cp, nbytes);
                        if (i <= 0) {
                                if (lose_counter++ > 16)
                                        break;
                                continue;
                        }
                        nbytes -= i;
                        cp += i;
                        lose_counter = 0;
                }
        }

        /* XXX put something better here if no /dev/random! */
        for (i = 0; i < nbytes; i++)
                *cp++ = rand() & 0xFF;
        return;
}


#endif


// helpers

static void random(void *buffer, size_t size) {
#ifdef WIN32
	random_provider.generate(buffer, size);
#else
	get_random_bytes(buffer, size);
#endif
}

// methods
static inline int _random(uint top) {
	uint raw;
	random(&raw, sizeof(raw));
	return int(double(raw) / 0xFFFFFFFFu * top );
}
static void _random(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& range=params->as_junction(0, "range must be expression");
	double top=r.process_to_value(range).as_double();
	if(top<=0)
		throw Exception("parser.runtime",
			&method_name,
			"top must be above 0(%g)", top);
	
	r.write_no_lang(*new(pool) VInt(pool, _random(uint(top))));
}


typedef double(*math1_func_ptr)(double);
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

inline bool is_salt_body_char(int c) {
	return isalnum(c) || c == '.' || c=='/';
}
static size_t calc_prefix_size(const char *salt) {
	if(size_t salt_size=strlen(salt)) {
		if(!is_salt_body_char(salt[0])) { // $...  {...
			const char *cur=salt+1; // skip
			while(is_salt_body_char(*cur++)) // ...$  ...}
				;
			return cur-salt;
		} else
			return 0;
	} else
		return 0;
}
static void _crypt(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const char *password=params->as_string(0, "password must be string").cstr();
	const char *maybe_bodyless_salt=params->as_string(1, "salt must be string").cstr();

	size_t prefix_size=calc_prefix_size(maybe_bodyless_salt);
	const char *normal_salt;
	char normalize_buf[MAX_STRING];
	if(prefix_size==strlen(maybe_bodyless_salt)) { // bodyless?
		strncpy(normalize_buf, maybe_bodyless_salt, MAX_STRING-MAX_SALT-1);
		char *cur=normalize_buf+strlen(normalize_buf);
		// sould add up MAX_SALT random chars
		static unsigned char itoa64[] =         /* 0 ... 63 => ASCII - 64 */
		"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		for(int i=0; i<MAX_SALT; i++)
			*cur++=itoa64[_random(64)];
		*cur=0;
		normal_salt=normalize_buf;
	} else
		normal_salt=maybe_bodyless_salt;

    /* FreeBSD style MD5 string 
     */
    if(strncmp(normal_salt, PA_MD5PW_ID, PA_MD5PW_IDLEN) == 0) {
		const size_t sample_size=120;
		char *sample_buf=(char *)pool.malloc(sample_size);
		PA_MD5Encode((const unsigned char *)password,
			(const unsigned char *)normal_salt,
			sample_buf, sample_size);
		r.write_pass_lang(*new(pool) String(pool, sample_buf));
    } else {
#ifdef HAVE_CRYPT
		const char *sample_buf=crypt(password, normal_salt);
		if(!sample_buf  // nothing generated
			|| !sample_buf[0] // generated nothing
			|| strncmp(sample_buf, normal_salt, prefix_size)!=0) // salt prefix not preserved
			throw Exception("parser.runtime",
				&method_name,
				"on this platform does not support '%.*s' salt prefix", prefix_size, normal_salt);
		
		r.write_pass_lang(*new(pool) String(pool, sample_buf));
#else
		throw Exception("parser.runtime",
			&method_name,
			"salt must start with '" PA_MD5PW_ID "'");
#endif
	}
}

static const char* hex_string(Pool& pool, unsigned char* bytes, size_t size, bool upcase) {
	char *bytes_hex=(char *)pool.malloc(size*2/*byte->hh*/+1/*for zero-teminator*/);
	unsigned char *src=bytes;
	unsigned char *end=bytes+size;
	char *dest=bytes_hex;

	static const char *hex=upcase?"0123456789ABCDEF":"0123456789abcdef";

	const char *format=upcase?"%02X":"%02x";
	for(; src<end; src++) {
                *dest++=hex[*src/0x10];
                *dest++=hex[*src%0x10];
	}
	*dest=0;

	return bytes_hex;
}

static void _md5(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const char *string=params->as_string(0, "parameter must be string").cstr();


	PA_MD5_CTX context;
	unsigned char digest[16];
	PA_MD5Init(&context);
	PA_MD5Update(&context, (const unsigned char*)string, strlen(string));
	PA_MD5Final(digest, &context);

	r.write_pass_lang(*new(pool) String(pool, 
		hex_string(pool, digest, sizeof(digest), false)));
}

/// to hell with extra bytes on 64bit platforms
struct uuid {
        unsigned int   time_low;
        unsigned short   time_mid;
        unsigned short   time_hi_and_version;
        unsigned short   clock_seq;
        unsigned char    node[6];
};
static void _uuid(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	// random
	struct uuid uuid;
	random(&uuid, sizeof(uuid));

	// http://www.opengroup.org/onlinepubs/9629399/apdxa.htm#tagtcjh_35
	// ~
	// version = DCE Security version, with embedded POSIX UIDs.  
	// variant = DCE
	//
	// DCE=Distributed Computing Environment
	// http://www.opengroup.org/dce/
	//
	// they say this influences comparison&such,
	// but could not figure out how, hence structure layout specified strictly
	// anyhow, uuidgen on Win32 yield those values
	// 
	// xxxxxxxx-xxxx-4xxx-{8,9,A,B}xxx-xxxxxxxxxxxx
	uuid.clock_seq = (uuid.clock_seq & 0x3FFF) | 0x8000;
        uuid.time_hi_and_version = (uuid.time_hi_and_version & 0x0FFF) | 0x4000;
 
	// format 
	const int uuid_cstr_bufsize=36+1/*for zero-teminator*/+1/*buggy snprintfs*/;
	char *uuid_cstr=(char *)pool.malloc(uuid_cstr_bufsize);
        snprintf(uuid_cstr, uuid_cstr_bufsize,
                "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
                uuid.clock_seq >> 8, uuid.clock_seq & 0xFF,
                uuid.node[0], uuid.node[1], uuid.node[2],
                uuid.node[3], uuid.node[4], uuid.node[5]);

	r.write_pass_lang(*new(pool) String(pool, uuid_cstr));
}

static void _uid64(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	unsigned char id[64/8];
	random(&id, sizeof(id));

	r.write_pass_lang(*new(pool) String(pool, 
		hex_string(pool, id, sizeof(id), true)));
}

// constructor

MMath::MMath(Pool& apool) : Methoded(apool, "math") {
	// ^FUNC(expr)	
#define ADDX(name, X) \
	add_native_method(#name, Method::CT_STATIC, _##name, X, X)
#define ADD0(name) ADDX(name, 0)
#define ADD1(name) ADDX(name, 1)
#define ADD2(name) ADDX(name, 2)

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

	// ^pow(x;y)
	ADD2(pow);

	// ^crypt[password;salt]
	ADD2(crypt);

	// ^md5[string]
	ADD1(md5);

	// ^uuid[]
	ADD0(uuid);

	// ^uid64[]
	ADD0(uid64);
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
