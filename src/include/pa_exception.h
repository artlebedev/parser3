/** @file
	Parser: exception decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_exception.h,v 1.33 2002/03/27 15:30:35 paf Exp $
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

#include "pa_types.h"
#include "pa_string.h"

class Pool;

// defines

class Exception {
public:

	Exception();
	Exception(
		const char *atype,
		const String *aproblem_source, 
		const char *comment_fmt, ...);
	Exception(const Exception& src);
	Exception& operator =(const Exception& src);
	~Exception();

#ifdef XML
	Exception(
		const String *aproblem_source, 
		GdomeException& exc);
#endif

	/// extracts exception type
	const char *type() const { return ftype; }
	/// extracts exception problem_source
	const String *problem_source() const { return fproblem_source; }
	/// extracts exception comment
	const char *comment() const { return fcomment; }

private:

	const char *ftype;
	const String *fproblem_source;
	bool owns_comment; char *fcomment;

};

#endif
