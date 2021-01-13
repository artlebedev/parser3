/** @file
	Parser: @b math parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)

	portions from gen_uuid.c,
	Copyright (C) 1996, 1997, 1998, 1999 Theodore Ts'o.
*/

#include "pa_vmethod_frame.h"
#include "pa_common.h"
#include "pa_base64.h"
#include "pa_vint.h"
#include "pa_vmath.h"
#include "pa_vfile.h"
#include "pa_request.h"
#include "pa_md5.h"
#include "pa_sha2.h"
#include "pa_random.h"

#ifdef HAVE_CRYPT
extern "C" char *crypt(const char* , const char* );
#endif

volatile const char * IDENT_MATH_C="$Id: math.C,v 1.97 2021/01/13 21:28:15 moko Exp $";

// defines

#define MAX_SALT 8

// class

class MMath: public Methoded {
public: // Methoded
	bool used_directly() { return false; }

public:
	MMath();
};

// global variables

DECLARE_CLASS_VAR(math, new MMath);

// methods

static void _random(Request& r, MethodParams& params) {
	double top=params.as_double(0, "range must be expression", r);
	if(top<1 || top>INT32_MAX)
		throw Exception(PARSER_RUNTIME, 0, "top(%.15g) must be [1..%u]", top, INT32_MAX);
	r.write(*new VInt(_random(uint(top))));
}


typedef double(*math1_func_ptr)(double);
static double frac(double param) { return param-trunc(param); }
static double degrees(double param) { return param /PI *180; }
static double radians(double param) { return param /180 *PI; }

static void math1(Request& r, MethodParams& params, math1_func_ptr func) {
	double param=params.as_double(0, "parameter must be expression", r);
	double result=func(param);
	r.write(*new VDouble(result));
}

#define MATH1(name) \
	static void _##name(Request& r, MethodParams& params) {\
		math1(r, params, &name);\
	}

#define MATH1P(name_parser, name_c) \
	static void _##name_parser(Request& r, MethodParams& params) {\
		math1(r, params, &name_c);\
	}

MATH1(round)
MATH1(floor)
MATH1P(ceiling, ceil)
MATH1(trunc)
MATH1(frac)
MATH1P(abs, fabs)
MATH1(sign)
MATH1(exp)
MATH1(log)
MATH1(log10)
MATH1(sin)
MATH1(asin)
MATH1(cos)
MATH1(acos)
MATH1(tan)
MATH1(atan)
MATH1(degrees)
MATH1(radians)
MATH1(sqrt)


typedef double (*math2_func_ptr)(double, double);
static void math2(Request& r, MethodParams& params, math2_func_ptr func) {
	double a=params.as_double(0, "parameter must be expression", r);
	double b=params.as_double(1, "parameter must be expression", r);
	double result=func(a, b);
	r.write(*new VDouble(result));
}

#define MATH2(name) \
	static void _##name(Request& r, MethodParams& params) {\
		math2(r, params, &name);\
	}

MATH2(pow)

inline bool is_salt_body_char(unsigned char c) {
	return pa_isalnum(c) || c == '.' || c=='/';
}

static size_t calc_prefix_size(const char* salt) {
	if(strlen(salt)) {
		if(!is_salt_body_char((unsigned char)salt[0])) { // $...  {...
			const char* cur=salt+1; // skip
			while(is_salt_body_char((unsigned char)*cur++)) // ...$  ...}
				;
			return cur-salt;
		} else
			return 0;
	} else
		return 0;
}
static void _crypt(Request& r, MethodParams& params) {
	const char* password=params.as_string(0, "password must be string").cstr();
	const char* maybe_bodyless_salt=params.as_string(1, "salt must be string").cstr();

	size_t prefix_size=calc_prefix_size(maybe_bodyless_salt);
	const char* normal_salt;
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
		char *sample_buf=new(PointerFreeGC) char[sample_size];
		pa_MD5Encode((const unsigned char *)password,
				(const unsigned char *)normal_salt, sample_buf, sample_size);
		String sample(sample_buf);
		r.write(sample);
	} else {
#ifdef HAVE_CRYPT
		const char* static_sample_buf=crypt(password, normal_salt);
		if(!static_sample_buf  // nothing generated
			|| !static_sample_buf[0] // generated nothing
			|| strncmp(static_sample_buf, normal_salt, prefix_size)!=0) // salt prefix not preserved
			throw Exception(PARSER_RUNTIME,
				0,
				"crypt on this platform does not support '%.*s' salt prefix", prefix_size, normal_salt);
		
		r.write(String(pa_strdup(static_sample_buf)));
#else
		throw Exception(PARSER_RUNTIME,
			0,
			"salt must start with '" PA_MD5PW_ID "'");
#endif
	}
}

static void _md5(Request& r, MethodParams& params) {
	const char *string=params.as_string(0, PARAMETER_MUST_BE_STRING).cstr_to_string_body_untaint(String::L_AS_IS, r.connection(false), &r.charsets).cstr();

	PA_MD5_CTX context;
	unsigned char digest[16];
	pa_MD5Init(&context);
	pa_MD5Update(&context, (const unsigned char*)string, strlen(string));
	pa_MD5Final(digest, &context);

	r.write(*new String(hex_string(digest, sizeof(digest), false)));
}


//SHA-1:

struct SHA1Context {
	unsigned Message_Digest[5], Length_Low, Length_High;
	unsigned int Message_Block[64];
	int Message_Block_Index, Computed, Corrupted;
};

#define SHA1CircularShift(bits,word) ((((word) << (bits)) & 0xFFFFFFFF)|((word) >> (32-(bits))))
void SHA1ProcessMessageBlock(SHA1Context *);
void SHA1PadMessage(SHA1Context *);
void SHA1Reset(SHA1Context *context) {
	context->Length_Low = context->Length_High = context->Message_Block_Index = 0;
	context->Message_Digest[0]	= 0x67452301;
	context->Message_Digest[1]	= 0xEFCDAB89;
	context->Message_Digest[2]	= 0x98BADCFE;
	context->Message_Digest[3]	= 0x10325476;
	context->Message_Digest[4]	= 0xC3D2E1F0;
	context->Computed = context->Corrupted  = 0;
}

int SHA1Result(SHA1Context *context) {
	if (context->Corrupted)
		return 0;
	if (!context->Computed) {
		SHA1PadMessage(context);
		context->Computed = 1;
	}
	return 1;
}

void SHA1Input(SHA1Context *context, const unsigned char *message_array, unsigned length) {
	if (!length)
		return;
	if (context->Computed || context->Corrupted) {
		context->Corrupted = 1;
		return;
	}

	while(length-- && !context->Corrupted) {
		context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);
		context->Length_Low += 8;
		context->Length_Low &= 0xFFFFFFFF;
		if (!context->Length_Low && !(context->Length_High=((1+context->Length_High)&0xFFFFFFFF)))
			context->Corrupted = 1; // too long message
		if (context->Message_Block_Index == 64)
			SHA1ProcessMessageBlock(context);
		message_array++;
	}
}

void SHA1ProcessMessageBlock(SHA1Context *context) {
	const unsigned K[] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };
	int t;
	unsigned    temp, W[80], buf[5];
	unsigned &A=buf[0], &B=buf[1], &C=buf[2], &D=buf[3], &E=buf[4];

	for(t = 0; t < 16; t++)
		W[t] = (((unsigned) context->Message_Block[t * 4]) << 24) | (((unsigned) context->Message_Block[t * 4 + 1]) << 16) | (((unsigned) context->Message_Block[t * 4 + 2]) << 8) | ((unsigned) context->Message_Block[t * 4 + 3]);

	for(t = 16; t < 80; t++)
		W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);

	memcpy (buf, context->Message_Digest, sizeof(buf));
	for(t = 0; t < 20; t++) {
		temp =  (SHA1CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0]) & 0xFFFFFFFF;
		E = D; D = C;
		C = SHA1CircularShift(30,B);
		B = A; A = temp;
	}

	for(t = 20; t < 40; t++) {
		temp = (SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1]) & 0xFFFFFFFF;
		E = D; D = C;
		C = SHA1CircularShift(30,B);
		B = A; A = temp;
	}

	for(t = 40; t < 60; t++) {
		temp = (SHA1CircularShift(5,A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2]) & 0xFFFFFFFF;
		E = D; D = C;
		C = SHA1CircularShift(30,B);
		B = A; A = temp;
	}

	for(t = 60; t < 80; t++) {
		temp = (SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3]) & 0xFFFFFFFF;
		E = D; D = C;
		C = SHA1CircularShift(30,B);
		B = A; A = temp;
	}

	for (t = 0; t < 5; t++)
		context->Message_Digest[t] = (context->Message_Digest[t] + buf[t]) & 0xFFFFFFFF;

	context->Message_Block_Index = 0;
}

void SHA1PadMessage(SHA1Context *context) {
	context->Message_Block[context->Message_Block_Index++] = 0x80;
	if (context->Message_Block_Index > 56) {
		//was 55, one shift
		while(context->Message_Block_Index < 64)
			context->Message_Block[context->Message_Block_Index++] = 0;
		SHA1ProcessMessageBlock(context);
		while(context->Message_Block_Index < 56)
			context->Message_Block[context->Message_Block_Index++] = 0;
	} else
		while(context->Message_Block_Index < 56)
			context->Message_Block[context->Message_Block_Index++] = 0;
	context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
	context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
	context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
	context->Message_Block[59] = (context->Length_High) & 0xFF;
	context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
	context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
	context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
	context->Message_Block[63] = (context->Length_Low) & 0xFF;
	SHA1ProcessMessageBlock(context);
}

#ifdef PA_BIG_ENDIAN
#define SWAP(n) (n)
#else
#define SWAP(n) (((n) << 24) | (((n) & 0xff00) << 8) | (((n) >> 8) & 0xff00) | ((n) >> 24))
#endif

void SHA1ReadDigest(void *buf, SHA1Context *c)
{
	if(!SHA1Result(c))
		throw Exception (PARSER_RUNTIME, 0, "Can not compute SHA1");

	((uint32_t *)buf)[0] = SWAP(c->Message_Digest[0]);
	((uint32_t *)buf)[1] = SWAP(c->Message_Digest[1]);
	((uint32_t *)buf)[2] = SWAP(c->Message_Digest[2]);
	((uint32_t *)buf)[3] = SWAP(c->Message_Digest[3]);
	((uint32_t *)buf)[4] = SWAP(c->Message_Digest[4]);
}

static void _sha1(Request& r, MethodParams& params) {
	const char *string = params.as_string(0, PARAMETER_MUST_BE_STRING).cstr_to_string_body_untaint(String::L_AS_IS, r.connection(false), &r.charsets).cstr();

	SHA1Context c;
	unsigned char digest[20];
	SHA1Reset (&c);
	SHA1Input (&c, (const unsigned char*)string, strlen(string));
	SHA1ReadDigest(digest, &c);

	r.write(*new String(hex_string(digest, sizeof(digest), false)));
}

String::C getData(Value& vdata, Request& r){
	if(const String* sdata=vdata.get_string()){
		String::Body body=sdata->cstr_to_string_body_untaint(String::L_AS_IS, r.connection(false), &r.charsets); // explode content, honor tainting changes
		return String::C(body.cstr(), body.length());
	} else {
		VFile *file=vdata.as_vfile(String::L_AS_IS);
		return String::C(file->value_ptr(),file->value_size());
	}
}

void memxor(char *dest, const char *src, size_t n){
	for (;n>0;n--) *dest++ ^= *src++;
}

#define IPAD 0x36
#define OPAD 0x5c

#define HMAC(key,init,update,final,blocklen,digestlen){				\
	unsigned char tempdigest[digestlen], keydigest[digestlen];		\
	size_t keylen=strlen(key);						\
	/* Reduce the key's size, so that it becomes <= blocklen bytes. */	\
	if (keylen > blocklen){							\
		init(&c);							\
		update(&c,(const unsigned char*)hmac, keylen);			\
		final(keydigest, &c);						\
		key = (char *)keydigest;					\
		keylen = digestlen;						\
	}									\
	/* Compute TEMP from KEY and STRING. */					\
	char block[blocklen];							\
	memset (block, IPAD, blocklen);						\
	memxor (block, key, keylen);						\
	init(&c);								\
	update(&c, (const unsigned char*)block, blocklen);			\
	update(&c, (const unsigned char*)data.str, data.length);		\
	final(tempdigest, &c);							\
	/* Compute result from KEY and TEMP. */					\
	memset (block, OPAD, blocklen);						\
	memxor (block, key, keylen);						\
	init(&c);								\
	update(&c, (const unsigned char*)block, blocklen);			\
	update(&c, (const unsigned char*)tempdigest, digestlen);		\
}

static void _digest(Request& r, MethodParams& params) {
	const String &smethod = params.as_string(0, PARAMETER_MUST_BE_STRING);

	String::C data=getData(params.as_no_junction(1, "parameter must be string or file"), r);

	enum Method { M_MD5, M_SHA1, M_SHA256, M_SHA512 } method;

	if (smethod == "md5") method = M_MD5;
	else if (smethod == "sha1" ) method = M_SHA1;
	else if (smethod == "sha256" ) method = M_SHA256;
	else if (smethod == "sha512" ) method = M_SHA512;
	else throw Exception(PARSER_RUNTIME, &smethod, "must be 'md5' or 'sha1' or 'sha256' or 'sha512'");

	const char *hmac=0;
	enum Format { F_HEX, F_BASE64 } format = F_HEX;

	if(params.count() == 3)
		if(HashStringValue* options=params.as_hash(2)) {
			int valid_options=0;
			if(Value* value=options->get("hmac")) {
				hmac=value->as_string().cstr();
				valid_options++;
			}
			if(Value* value=options->get("format")) {
				const String& sformat=value->as_string();
				if (sformat == "hex") format = F_HEX;
				else if (sformat == "base64" ) format = F_BASE64;
				else throw Exception(PARSER_RUNTIME, &sformat, "must be 'hex' or 'base64'");
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	String::C digest;

	if(method == M_MD5){
		PA_MD5_CTX c;
		if(hmac){
			HMAC(hmac, pa_MD5Init, pa_MD5Update, pa_MD5Final, 64, 16);
		} else {
			pa_MD5Init(&c);
			pa_MD5Update(&c, (const unsigned char*)data.str, data.length);
		}
		char *str=(char *)pa_malloc(16);
		pa_MD5Final((unsigned char *)str, &c);
		digest = String::C(str, 16);
	}

	if(method == M_SHA1){
		SHA1Context c;
		if(hmac){
			HMAC(hmac, SHA1Reset, SHA1Input, SHA1ReadDigest, 64, 20);
		} else {
			SHA1Reset(&c);
			SHA1Input(&c, (const unsigned char*)data.str, data.length);
		}
		char *str=(char *)pa_malloc(20);
		SHA1ReadDigest(str, &c);
		digest = String::C(str, 20);
	}

	if(method == M_SHA256){
		SHA256_CTX c;
		if(hmac){
			HMAC(hmac, pa_SHA256_Init, pa_SHA256_Update, pa_SHA256_Final, 64, SHA256_DIGEST_LENGTH);
		} else {
			pa_SHA256_Init(&c);
			pa_SHA256_Update(&c, (const unsigned char*)data.str, data.length);
		}
		char *str=(char *)pa_malloc(SHA256_DIGEST_LENGTH);
		pa_SHA256_Final((unsigned char *)str, &c);
		digest = String::C(str, SHA256_DIGEST_LENGTH);
	}

	if(method == M_SHA512){
		SHA512_CTX c;
		if(hmac){
			HMAC(hmac, pa_SHA512_Init, pa_SHA512_Update, pa_SHA512_Final, 128, SHA512_DIGEST_LENGTH);
		} else {
			pa_SHA512_Init(&c);
			pa_SHA512_Update(&c, (const unsigned char*)data.str, data.length);
		}
		char *str=(char *)pa_malloc(SHA512_DIGEST_LENGTH);
		pa_SHA512_Final((unsigned char *)str, &c);
		digest = String::C(str, SHA512_DIGEST_LENGTH);
	}

	if(format == F_HEX){
		r.write(*new String(hex_string((unsigned char *)digest.str, digest.length, false)));
	}
	if(format == F_BASE64){
		r.write(*new String(pa_base64_encode(digest.str, digest.length, Base64Options(false /*no wrap*/))));
	}
}

static void _uuid(Request& r, MethodParams& params) {
	bool lower=false;
	bool solid=false;

	if (params.count() == 1)
		if (HashStringValue* options = params.as_hash(0)) {
			int valid_options = 0;
			if (Value* vlower = options->get("lower")) {
				lower = r.process(*vlower).as_bool();
				valid_options++;
			}
			if (Value* vsolid = options->get("solid")) {
				solid = r.process(*vsolid).as_bool();
				valid_options++;
			}
			if (valid_options != options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	
	r.write(*new String(get_uuid_cstr(lower, solid)));
}

static void _uid64(Request& r, MethodParams& params) {
	bool lower = false;

	if (params.count() == 1)
		if (HashStringValue* options = params.as_hash(0)) {
			int valid_options = 0;
			if (Value* vlower = options->get("lower")) {
				lower = r.process(*vlower).as_bool();
				valid_options++;
			}
			if (valid_options != options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	unsigned char id[64/8];
	random(&id, sizeof(id));

	r.write(*new String(hex_string(id, sizeof(id), !lower)));
}

static void _crc32(Request& r, MethodParams& params) {
	const char *string=params.as_string(0, PARAMETER_MUST_BE_STRING).cstr();
	r.write(*new VInt(pa_crc32(string, strlen(string))));
}

static const char* abc_hex = "0123456789ABCDEF";

static unsigned char hex_lookup[256] = {
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
	 0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char abc_lookup[256] = {};
static unsigned char abc_256_lookup[256] = {};

inline unsigned char *init_abc_256() {
	if(!abc_256_lookup[255])
		for(int i=0; i<256; i++) abc_256_lookup[i] = (unsigned char)i;
	return abc_256_lookup;
}

static void _convert(Request& r, MethodParams& params) {
	String::C data=getData(params.as_no_junction(0, "parameter must be string or file"), r);

	bool abc_mode = true;
	unsigned char *lookup;
	const char *abc_from;
	int base_from;

	if(params[1].is_string()) {
		abc_from = params[1].get_string()->cstr();
		base_from = strlen(abc_from);
		if(base_from < 2)
			throw Exception(PARSER_RUNTIME, 0, "alphabet 'from' must contain at least 2 characters");
		lookup = abc_lookup;
		memset(abc_lookup,0,sizeof(abc_lookup));
		for(int i=0; i<base_from; i++) abc_lookup[(unsigned char)abc_from[i]] = (unsigned char)i;
	} else {
		base_from=params.as_int(1, "base 'from' must be integer or string", r);
		if(base_from < 2 || base_from > 16 && base_from != 256)
			throw Exception(PARSER_RUNTIME, 0, "base 'from' must be an integer from 2 to 16 or 256");
		if (base_from == 256) {
			abc_from = "";
			lookup = init_abc_256();
		} else {
			abc_mode = false;
			abc_from = abc_hex;
			lookup = hex_lookup;
		}
	}

	const char *abc_to;
	int base_to;

	if(params[2].is_string()) {
		abc_to=params[2].get_string()->cstr();
		base_to=strlen(abc_to);
		if(base_to < 2)
			throw Exception(PARSER_RUNTIME, 0, "alphabet 'to' must contain at least 2 characters");
	} else {
		base_to=params.as_int(2, "base 'to' must be integer or string", r);
		if(base_to < 2 || base_to > 16 && base_to != 256)
			throw Exception(PARSER_RUNTIME, 0, "base 'to' must be an integer from 2 to 16 or 256");
		if (base_to == 256) {
			abc_to = (char *)init_abc_256();
		} else {
			abc_to = abc_hex;
		}
	}

	VFile* result_file = 0;

	if(params.count() == 4)
		if(HashStringValue* options=params.as_hash(3)) {
			int valid_options=0;
			if(Value* value=options->get("format")) {
				const String& sformat=value->as_string();
				if (sformat == "file" ) result_file = new VFile;
				else if (sformat != "string") throw Exception(PARSER_RUNTIME, &sformat, "must be 'string' or 'file'");
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	bool negative=false;
	bool sign=false;

	// converting digits to their numeric values

	unsigned char *src=(unsigned char *)pa_strdup(data.str, data.length);
	const unsigned char *src_end = src + data.length;

	unsigned char *c;

	if(abc_mode){

		for(c=src;c<src_end;c++) {
			unsigned char digit=lookup[*c];
			if(!digit && *c != abc_from[0])
				throw Exception("number.format", 0, "'%c' is invalid digit", *c);
			*c=digit;
		}

	} else {
		// numbers mode, allow whitespace and sign

		while(isspace(*src))
			src++;

		if(src[0]=='-') {
			negative=true;
			sign=true;
			src++;
		} else if(src[0]=='+') {
			sign=true;
			src++;
		}

		for(c=src;c<src_end;c++) {
			unsigned char digit=lookup[*c];
			if(!digit && *c != abc_from[0] || digit>=base_from) {
				for(unsigned char *s=c;s<src_end;s++)
					if(!isspace(*s))
						throw Exception("number.format", 0, "'%c' is invalid digit", *s);
				src_end=c;
				break;
			}
			*c=digit;
		}

	}

	if(src==src_end) {
		if(sign)
			throw Exception("number.format", 0,  "'%c' is invalid number", negative ? '-' : '+');
		if(result_file)
			r.write(*result_file);
		return;
	}

	// core, using log since log2 is not present in FreeBSD < 8.X

	Array<char> remainders((size_t)round(data.length * log((double)base_from) / log((double)base_to)) + 1);

	do {
		int carry = 0;
		unsigned char *dst = src;
		for (c=src; c<src_end; c++) {
			carry = carry * base_from + *c;
			if (carry >= base_to) {
				*(dst++) = (unsigned char)(carry / base_to);
				carry %= base_to;
			} else if (dst > src) {
				*(dst++) = 0;
			}
		}
		src_end = dst;
		remainders += abc_to[carry];
	} while (src_end > src);

	// result processing

	size_t result_length = negative + remainders.count();
	char *result_str = (char *)pa_malloc_atomic(result_length+1);
	if(negative)
		result_str[0] = '-';
	for(size_t i=0; i<remainders.count(); i++)
		result_str[result_length - 1 - i] = remainders[i];
	result_str[result_length]='\0';

	if(result_file) {
		result_file->set(true /*tainted*/, 0 /*binary*/, result_str, result_length, 0, 0, &r);
		r.write(*result_file);
	} else {
		if(memchr(result_str, 0, result_length))
			throw Exception(PARSER_RUNTIME, 0, "Invalid \\x00 character found while converting to string. Convert to file instead.");

		fix_line_breaks(result_str, result_length);

		if(result_length)
			r.write(*new String(result_str, String::L_TAINTED));
	}
}

// constructor

MMath::MMath(): Methoded("math") {
	// ^FUNC(expr)	
#define ADDX(name, X) \
	add_native_method(#name, Method::CT_STATIC, _##name, X, X)
#define ADD0(name) ADDX(name, 0)
#define ADD1(name) ADDX(name, 1)
#define ADD2(name) ADDX(name, 2)

	ADD1(round);	ADD1(floor);	ADD1(ceiling);
	ADD1(trunc);	ADD1(frac);
	ADD1(abs);	ADD1(sign);
	ADD1(exp);
	ADD1(log);	ADD1(log10);
	ADD1(sin);	ADD1(asin);
	ADD1(cos);	ADD1(acos);
	ADD1(tan);	ADD1(atan);
	ADD1(degrees);	ADD1(radians);
	ADD1(sqrt);
	ADD1(random);

	// ^math:pow(x;y)
	ADD2(pow);

	// ^math:crypt[password;salt]
	ADD2(crypt);

	// ^math:md5[string]
	ADD1(md5);

	// ^math:sha1[string]
	ADD1(sha1);
	
	// ^math:digest[method;string|file;options]
	add_native_method("digest", Method::CT_STATIC, _digest, 2, 3);
	
	// ^math:crc32[string]
	ADD1(crc32);

	// ^math:uuid[]
	// ^math:uuid[options hash]
	add_native_method("uuid", Method::CT_STATIC, _uuid, 0, 1);

	// ^math:uid64[]
	// ^math:uid64[options hash]
	add_native_method("uid64", Method::CT_STATIC, _uid64, 0, 1);

	// ^math:convert[number|file](base-from)|[abc_from](base-to)|[abc_to][options]
	add_native_method("convert", Method::CT_STATIC, _convert, 3, 4);
}
