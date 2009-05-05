/** @file
	Parser: exception class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_EXCEPTION_C="$Date: 2009/05/05 10:07:11 $";

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_sapi.h"
#include "pa_globals.h"


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

Exception::Exception(const char* atype, 
			const String* aproblem_source, 
			const char* comment_fmt, ...) {
	ftype=atype;
	fproblem_source=aproblem_source;

	if(comment_fmt) {
		fcomment=new(PointerFreeGC) char[MAX_STRING];
		va_list args;
		va_start(args, comment_fmt);
		vsnprintf(fcomment, MAX_STRING, comment_fmt, args);
//		_asm int 3;
		va_end(args);
	} else
		fcomment=0;
}

const String* Exception::problem_source() const { 
	return fproblem_source && !fproblem_source->is_empty()?fproblem_source:0; 
}
