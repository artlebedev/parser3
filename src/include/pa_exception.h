/** @file
	Parser: exception decls.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

static const char* IDENT_EXCEPTION_H="$Date: 2003/01/21 15:51:10 $";

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
	const char *type(bool can_be_empty=false) const { 
		if(can_be_empty)
			return ftype; 
		else
			return ftype?ftype:"<no type>";
	}
	/// extracts exception problem_source
	const String *problem_source() const { 
		return fproblem_source && fproblem_source->size()?fproblem_source:0; 
	}
	/// extracts exception comment
	const char *comment(bool can_be_empty=false) const { 
		const char *result=fcomment && *fcomment?fcomment:0;
		if(can_be_empty)
			return result; 
		else
			return result?result:"<no comment>";
	}

private:

	const char *ftype;
	const String *fproblem_source;
	bool owns_comment; char *fcomment;

};

#endif
