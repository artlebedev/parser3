/** @file
	Parser: exception class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_sapi.h"
#include "pa_globals.h"

volatile const char * IDENT_PA_EXCEPTION_C="$Id: pa_exception.C,v 1.57 2020/12/15 17:10:36 moko Exp $" IDENT_PA_EXCEPTION_H;

// methods

Exception::Exception(): ftype(0), fproblem_source(0), fcomment(0) {}

Exception::Exception(const Exception& src):
	ftype(src.ftype),
	fproblem_source(src.fproblem_source),
	fcomment(src.fcomment) {
}

Exception& Exception::operator =(const Exception& src) {
	ftype=src.ftype;
	fproblem_source=src.fproblem_source;
	fcomment=src.fcomment;
	return *this;
}

Exception::Exception(const char* atype, const String* aproblem_source, const char* comment_fmt, ...) {
	ftype=atype;
	fproblem_source=aproblem_source ? new String(*aproblem_source) : 0;

	if(comment_fmt) {
		char comment[MAX_STRING];
		va_list args;
		va_start(args, comment_fmt);
		fcomment=pa_strdup(comment, vsnprintf(comment, MAX_STRING, comment_fmt, args));
		va_end(args);
	} else
		fcomment=0;
}

void Exception::add_comment(const char* acomment) {
	if(acomment)
		fcomment=pa_strcat(fcomment, acomment);
}

const String* Exception::problem_source() const {
	return fproblem_source && !fproblem_source->is_empty() ? fproblem_source : 0;
}
