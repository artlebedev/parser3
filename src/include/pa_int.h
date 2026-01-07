/** @file
	Parser: pa_int and support functions decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_INT_H
#define PA_INT_H

#define IDENT_PA_INT_H "$Id: pa_int.h,v 1.4 2026/01/07 13:40:02 moko Exp $"

// includes

#include "pa_memory.h"
#include "pa_types.h"

#define PA_WIDE_INT
#ifdef PA_WIDE_INT
// int53, -(2^53-1) .. +(2^53-1) safe convertion from double
#define PA_WINT_MIN -9007199254740991LL
#define PA_WINT_MAX 9007199254740991LL
typedef int64_t pa_wint;
typedef uint64_t pa_uwint;
int check4int(pa_wint avalue);
#else
#define PA_WINT_MIN INT_MIN
#define PA_WINT_MAX INT_MAX
typedef int pa_wint;
typedef uint pa_uwint;
static int check4int(pa_wint avalue){ return avalue; }
#endif

inline int clip2int(double value) {
	return value <= INT_MIN ? INT_MIN : ( value >= INT_MAX ? INT_MAX : (int)value );
}

inline pa_wint clip2wint(double value) {
	return value <= PA_WINT_MIN ? PA_WINT_MIN : ( value >= PA_WINT_MAX ? PA_WINT_MAX : (pa_wint)value );
}

inline uint clip2uint(double value) {
	return value <= 0 ? 0 : ( value >= UINT_MAX ? UINT_MAX : (uint)value );
}

/// Commonly used, templated to work with any integer type

template<typename T> char* pa_itoa(T n){
	const T base=10;
	char buf[MAX_NUMBER + 1];
	char* pos=buf + MAX_NUMBER;
	*pos='\0';

	bool negative=n < 0;
	if (n < 0){
		n=-n;
	}

	do {
		*(--pos)=(char)(n % base) + '0';
		n/=base;
	} while (n > 0);

	if (negative) {
		*(--pos) = '-';
	}
	return pa_strdup(pos, buf + MAX_NUMBER - pos);
}

char* pa_itoa(int); // custom to support INT_MIN

template<typename T> char* pa_uitoa(T n, T base=10){
	char buf[MAX_NUMBER + 1];
	char* pos=buf + MAX_NUMBER;
	*pos='\0';

	do {
		*(--pos)=(char)(n % base) + '0';
		n/=base;
	} while (n > 0);

	return pa_strdup(pos, buf + MAX_NUMBER - pos);
}

// forwards
class String;

// sign and whitespace are allowed
double pa_atod(const char* str, const String* problem_source);
int pa_atoi(const char* str, int base=10, const String* problem_source=0);
pa_wint pa_atowi(const char* str, int base=10, const String* problem_source=0);
// no sign or whitespace
unsigned int pa_atoui(const char *str, int base=10, const String* problem_source=0);
uint64_t pa_atoul(const char *str, int base=10, const String* problem_source=0);

const char* format_double(double value, const char *fmt);

#endif
