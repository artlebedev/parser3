/** @file
	Parser: pa_int support functions

	Copyright (c) 2001-2026 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_int.h"
#include "pa_string.h"
#include "pa_exception.h"

volatile const char * IDENT_PA_STRING_C="$Id: pa_int.C,v 1.1 2026/01/06 13:07:58 moko Exp $" IDENT_PA_INT_H;

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
		const uint min_abs = (uint)(-( (uint)INT_MIN + 1 )) + 1;
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
		const pa_uwint min_abs = (pa_uwint)(-( (pa_wint)PA_WINT_MIN + 1 )) + 1;
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

