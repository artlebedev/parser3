/** @file
	Parser: exception decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_exception.h,v 1.12 2001/05/17 10:22:24 parser Exp $
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

#include "pa_config_includes.h"
#include "pa_types.h"
#include "pa_string.h"

/// Just simple longjump mechanizm dressed in class clothes. use Temp_exception
class Exception {
public:

	/**
		address for long jump to jump to 
		regretfully public:
		can't make local unless sure of inlining
		for to-die stack frame ruins it all
	*/
	mutable jmp_buf mark;

	/// throws an exception[really goes to mark-ed position
	void _throw(
		const String *atype, const String *acode,
		const String *aproblem_source, 
		const char *comment_fmt, ...) const;

	/// extracts exception type
	const String *type() const { return ftype; }
	/// extracts exception code
	const String *code() const { return fcode; }
	/// extracts exception problem_source
	const String *problem_source() const { return fproblem_source; }
	/// extracts exception comment
	const char *comment() const { return fcomment[0]?fcomment:0; }

private:

	mutable const String *ftype, *fcode, *fproblem_source;
	mutable char fcomment[MAX_STRING];
};

#endif
