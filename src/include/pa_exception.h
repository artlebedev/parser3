/** @file
	Parser: exception decls.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

static const char * const IDENT_EXCEPTION_H="$Date: 2004/02/11 15:33:13 $";

// includes

#include "pa_memory.h"

// forwards

class String;

// defines

class Exception {
public:

	Exception();
	Exception(
		const char* atype,
		const String* aproblem_source, 
		const char* comment_fmt, ...);
	Exception(const Exception& src);
	Exception& operator =(const Exception& src);

	/// extracts exception type
	const char* type(bool can_be_empty=false) const { 
		if(can_be_empty)
			return ftype; 
		else
			return ftype?ftype:"<no type>";
	}
	/// extracts exception problem_source
	const String* problem_source() const;
	/// extracts exception comment
	const char* comment(bool can_be_empty=false) const { 
		const char* result=fcomment && *fcomment?fcomment:0;
		if(can_be_empty)
			return result; 
		else
			return result?result:"<no comment>";
	}

protected:

	const char* ftype;
	const String* fproblem_source;
	char* fcomment;

};

#endif
