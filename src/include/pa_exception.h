/** @file
	Parser: exception decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

static const char * const IDENT_EXCEPTION_H="$Date: 2008/09/04 09:34:11 $";

const char* const PARSER_RUNTIME = "parser.runtime";
const char* const IMAGE_FORMAT = "image.format";
const char* const PCRE_EXCEPTION_TYPE = "pcre.execute";
const char* const DATE_RANGE_EXCEPTION_TYPE = "date.range";

const char* const NAME_MUST_BE_STRING = "name must be string";
const char* const FILE_NAME_MUST_BE_STRING = "file name must be string";
const char* const NAMESPACEURI_MUST_BE_STRING = "namespaceURI must be string";
const char* const DATA_MUST_BE_STRING = "data must be string";
const char* const LOCALNAME_MUST_BE_STRING = "localName must be string";
const char* const VALUE_MUST_BE_STRING = "value must be string";
const char* const PARAMETER_MUST_BE_STRING = "parameter must be string";
const char* const COLUMN_NAME_MUST_BE_STRING = "column name must be string";

const char* const FILE_NAME_MUST_NOT_BE_CODE = "file name must not be code";
const char* const COORDINATES_MUST_NOT_BE_CODE = "coordinates must not be code";
const char* const FIRST_ARG_MUST_NOT_BE_CODE = "first argument must not be code";
const char* const PARAM_MUST_NOT_BE_CODE = "param must not be code";
const char* const MODE_MUST_NOT_BE_CODE = "mode must not be code";

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
	operator bool() { return ftype || fproblem_source || fcomment; }
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
