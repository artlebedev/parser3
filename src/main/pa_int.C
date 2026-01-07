/** @file
	Parser: pa_int support functions

	Copyright (c) 2001-2026 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_int.h"
#include "pa_string.h"
#include "pa_exception.h"

volatile const char * IDENT_PA_INT_C="$Id: pa_int.C,v 1.7 2026/01/07 14:48:54 moko Exp $" IDENT_PA_INT_H;

#ifdef PA_WIDE_INT
int check4int(pa_wint avalue){
	if( (avalue > INT_MAX) || (avalue < INT_MIN))
		throw Exception("number.format", 0, "%s is out of regular int range", pa_itoa(avalue));
	return (int)avalue;
}
#endif

char* pa_itoa(int i){ // custom to support INT_MIN
	const int base=10;
	char buf[MAX_NUMBER + 1];
	char* pos=buf + MAX_NUMBER;
	*pos='\0';

	bool negative=i < 0;
	unsigned int n= i < 0 ? 0u-(unsigned int)i : (unsigned int)i;

	do {
		*(--pos)=(char)(n % base) + '0';
		n/=base;
	} while (n > 0);

	if (negative) {
		*(--pos) = '-';
	}
	return pa_strdup(pos, buf + MAX_NUMBER - pos);
}

// pa_atoui is based on Manuel Novoa III _strto_l for uClibc

template<typename T> inline T pa_ato_any(const char *str, int base, const String* problem_source,const T max){
	T result = 0;
	const char *pos = str;

	while (isspace(*pos)) /* skip leading whitespace */
		++pos;

	if (base == 16 && *pos == '0') { /* handle option prefix */
		++pos;
		if (*pos == 'x' || *pos == 'X') {
			++pos;
		}
	}

	if (base == 0) { /* dynamic base */
		base = 10; /* default is 10 */
		if (*pos == '0') {
			++pos;
			if (*pos == 'x' || *pos == 'X'){
				++pos;
				base=16;
			}
		}
	}

	if (base < 2 || base > 16) /* illegal base */
		throw Exception(PARSER_RUNTIME, 0, "base to must be an integer from 2 to 16");
	if (*pos == '-')
		throw Exception("number.format", problem_source, problem_source ? "out of range (negative)" : "'%s' is out if range (negative)", str);

	T cutoff = max / base;
	int cutoff_digit = (int)(max - cutoff * base);

	while(true) {
		int digit;
		
		if ((*pos >= '0') && (*pos <= '9')) {
			digit = (*pos - '0');
		} else if (*pos >= 'a') {
			digit = (*pos - 'a' + 10);
		} else if (*pos >= 'A') {
			digit = (*pos - 'A' + 10);
		} else break;

		if (digit >= base) {
			break;
		}
		
		++pos;
		
		/* adjust number, with overflow check */
		if ((result > cutoff) || ((result == cutoff) && (digit > cutoff_digit))) {
			throw Exception("number.format", problem_source, problem_source ? "out of range (int)" : "'%s' is out of range (int)", str);
		} else {
			result  = result * base + digit;
		}
	}

	while(char c=*pos++)
		if(!isspace(c))
			throw Exception("number.format", problem_source, problem_source ? "invalid number (int)" : "'%s' is an invalid number (int)", str);

	return result;
}

unsigned int pa_atoui(const char *str, int base, const String* problem_source){
	if(!str)
		return 0;

	return pa_ato_any<unsigned int>(str, base, problem_source, UINT_MAX);
}

uint64_t pa_atoul(const char *str, int base, const String* problem_source){
	if(!str)
		return 0;

	return pa_ato_any<uint64_t>(str, base, problem_source, ULLONG_MAX);
}

int pa_atoi(const char* str, int base, const String* problem_source) {
	if(!str)
		return 0;

	while(isspace(*str))
		str++;

	if(!*str)
		return 0;

	const char *str_copy=str;
	bool negative=false;
	if(str[0]=='-') {
		negative=true;
		str++;
		if(!*str || isspace(*str))
			throw Exception("number.format", problem_source, problem_source ? "invalid number (int)" : "'%s' is an invalid number (int)", str_copy);
	} else if(str[0]=='+') {
		str++;
		if(!*str || isspace(*str))
			throw Exception("number.format", problem_source, problem_source ? "invalid number (int)" : "'%s' is an invalid number (int)", str_copy);
	}

	if(negative){
		const uint min_abs = (uint)0 - (uint)INT_MIN;
		uint result=pa_ato_any<uint>(str, base, problem_source, min_abs);
		if(result==min_abs) return INT_MIN;
		return -(int)result;
	} else {
		return (int)pa_ato_any<uint>(str, base, problem_source, INT_MAX);
	}
}

pa_wint pa_atowi(const char* str, int base, const String* problem_source) {
	if(!str)
		return 0;

	while(isspace(*str))
		str++;

	if(!*str)
		return 0;

	const char *str_copy=str;
	bool negative=false;
	if(str[0]=='-') {
		negative=true;
		str++;
		if(!*str || isspace(*str))
			throw Exception("number.format", problem_source, problem_source ? "invalid number (int)" : "'%s' is an invalid number (int)", str_copy);
	} else if(str[0]=='+') {
		str++;
		if(!*str || isspace(*str))
			throw Exception("number.format", problem_source, problem_source ? "invalid number (int)" : "'%s' is an invalid number (int)", str_copy);
	}

	if(negative){
		const pa_uwint min_abs = (pa_uwint)0 - (pa_uwint)PA_WINT_MIN;
		pa_uwint result=pa_ato_any<pa_uwint>(str, base, problem_source, min_abs);
		if(result==min_abs) return PA_WINT_MIN;
		return -(pa_wint)result;
	} else {
		return (pa_wint)pa_ato_any<pa_uwint>(str, base, problem_source, PA_WINT_MAX);
	}
}

double pa_atod(const char* str, const String* problem_source /* never null */) {
	if(!str)
		return 0;

	while(isspace(*str))
		str++;

	if(!*str)
		return 0;

	bool negative=false;
	if(str[0]=='-') {
		negative=true;
		str++;
		if(!*str || isspace(*str))
			throw Exception("number.format", problem_source, "invalid number (double)");
	} else if(str[0]=='+') {
		str++;
		if(!*str || isspace(*str))
			throw Exception("number.format", problem_source, "invalid number (double)");
	}

	double result;
	if(str[0]=='0') {
		if(str[1]=='x' || str[1]=='X') {
			// 0xABC
			result=(double)pa_atoul(str, 0, problem_source);
			return negative ? -result : result;
		} else {
			 // skip leading 0000, to disable octal interpretation
			do str++; while(*str=='0');
		}
	}

	char *error_pos;
	result=strtod(str, &error_pos);

	while(const char c=*error_pos++)
		if(!isspace(c))
			throw Exception("number.format", problem_source, "invalid number (double)");

	return negative ? -result : result;
}


// format: %[flags][width][.precision]type          https://msdn.microsoft.com/ru-ru/library/56e442dc(en-us,VS.80).aspx
//		flags: '-', '+', ' ', '#', '0'      https://msdn.microsoft.com/ru-ru/library/8aky45ct(en-us,VS.80).aspx
//		width, precision: non negative decimal number

#define MAX_FORMAT_LEN 10 // %+0XX.XXg\0

enum FormatType {
	FormatInvalid,
	FormatInt,
	FormatUInt,
	FormatDouble
};

FormatType format_type(const char* fmt){
	enum FormatState {
		Percent,
		Flags,
		Width,
		Precision,
		Done
	} state=Percent;

	FormatType result=FormatInvalid;

	const char* pos=fmt;
	while(char c=*(pos++)){
		switch(state){
			case Percent:
				if(c=='%'){
					state=Flags;
				} else {
					return FormatInvalid; // 1st char must be '%' only
				}
				break;
			case Flags:
				if(strchr("-+ #0", c)!=0){
					break;
				}
				// go to the next step
			case Width:
				if(c=='.'){
					state=Precision;
					break;
				}
				// go to the next step
			case Precision:
				if(c>='0' && c<='9'){
					if(state == Flags) state=Width; // no more flags
					break;
				} else if(c=='d' || c=='i'){
					result=FormatInt;
				} else if(strchr("feEgG", c)!=0){
					result=FormatDouble;
				} else if(strchr("uoxX", c)!=0){
					result=FormatUInt;
				} else {
					return FormatInvalid; // invalid char
				}
				state=Done;
				break;
			case Done:
				return FormatInvalid; // no chars allowed after 'type'
		}
	}
	return pos-fmt > MAX_FORMAT_LEN ? FormatInvalid : result;
}

#ifdef PA_WIDE_INT

#if defined(_MSC_VER) && (_MSC_VER < 1900)
// old VS: %I64d
#define PA_INTMOD "I64"
#else
// C99: %lld
#define PA_INTMOD "ll"
#endif

static const char* wide_int_fmt(const char *fmt, char *dst){
	const size_t len = strlen(fmt);
	const size_t head = len - 1;
	memcpy(dst, fmt, head);
	memcpy(dst + head, PA_INTMOD, strlen(PA_INTMOD));
	dst[head + strlen(PA_INTMOD)] = fmt[head];
	dst[head + strlen(PA_INTMOD) + 1] = '\0';
	return dst;
}
#endif


const char* format_double(double value, const char* fmt) {
	char local_buf[MAX_NUMBER];
	int size=-1;

	if(fmt && strlen(fmt)){
		switch(format_type(fmt)){
			case FormatDouble:
				size=snprintf(local_buf, sizeof(local_buf), fmt, value);
				break;
			case FormatUInt:
				if(value >= 0){ // on Apple M1 (uint)<negative value> is 0
#ifdef PA_WIDE_INT
					char fmt_buf[MAX_FORMAT_LEN+4];
					size=snprintf(local_buf, sizeof(local_buf), wide_int_fmt(fmt, fmt_buf), (unsigned long long)clip2wint(value)); // WUINT_MAX == WINT_MAX
#else
					size=snprintf(local_buf, sizeof(local_buf), fmt, clip2uint(value));
#endif
				} else {
					// for unsigned formats, wrap negatives to uint as unsigned wint would exceed the 53-bit range
					size=snprintf(local_buf, sizeof(local_buf), fmt, clip2int(value));
				}
				break;
			case FormatInt:{
#ifdef PA_WIDE_INT
				char fmt_buf[MAX_FORMAT_LEN+4];
				size=snprintf(local_buf, sizeof(local_buf), wide_int_fmt(fmt, fmt_buf), (long long)clip2wint(value));
#else
				size=snprintf(local_buf, sizeof(local_buf), fmt, clip2int(value));
#endif
				break;
				}
			case FormatInvalid:
				throw Exception(PARSER_RUNTIME, 0, "Incorrect format string '%s' was specified.", fmt);
		}
	} else
		return pa_itoa(clip2wint(value));

	if(size < 0 || size >= MAX_NUMBER-1){ // on win32 we manually reduce max size while printing
		throw Exception(PARSER_RUNTIME, 0, "Error occurred white executing snprintf with format string '%s'.", fmt);
	}

	return pa_strdup(local_buf, (size_t)size);
}

