/** @file
	Parser: exception decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_exception.h,v 1.29 2002/01/23 10:38:17 paf Exp $
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

#include "pa_types.h"
#include "pa_string.h"

class Pool;

// defines

/// Just simple longjump mechanizm dressed in class clothes. use Temp_exception
class Exception {
public:

	Exception();
	Exception(
		const String *atype, const String *acode,
		const String *aproblem_source, 
		const char *comment_fmt, ...);
	Exception(const Exception& src);
	Exception& operator =(const Exception& src);
	~Exception();

#ifdef XML
	Exception(
		const String *atype, const String *acode,
		const String *aproblem_source, 
		GdomeException& exc);
#endif

	/// extracts exception type
	const String *type() const { return ftype; }
	/// extracts exception code
	const String *code() const { return fcode; }
	/// extracts exception problem_source
	const String *problem_source() const { return fproblem_source; }
	/// extracts exception comment
	const char *comment() const { return fcomment; }

private:

	const String *ftype, *fcode, *fproblem_source;
	bool owns_comment;
	char *fcomment;

};

#endif
