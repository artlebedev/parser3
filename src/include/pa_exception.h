/** @file
	Parser: exception decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

#define IDENT_PA_EXCEPTION_H "$Id: pa_exception.h,v 1.68 2020/12/15 17:10:31 moko Exp $"

const char* const PARSER_RUNTIME = "parser.runtime";
const char* const IMAGE_FORMAT = "image.format";
const char* const PCRE_EXCEPTION_TYPE = "pcre.execute";
const char* const DATE_RANGE_EXCEPTION_TYPE = "date.range";
const char* const BASE64_FORMAT = "base64.format";

const char* const NAME_MUST_BE_STRING = "name must be string";
const char* const FILE_NAME_MUST_BE_STRING = "file name must be string";
const char* const VALUE_MUST_BE_STRING = "value must be string";
const char* const PARAMETER_MUST_BE_STRING = "parameter must be string";
const char* const COLUMN_NAME_MUST_BE_STRING = "column name must be string";

const char* const FILE_NAME_MUST_BE_SPECIFIED = "file name must be specified";
const char* const FILE_NAME_MUST_NOT_BE_CODE = "file name must not be code";
const char* const FIRST_ARG_MUST_NOT_BE_CODE = "first argument must not be code";
const char* const PARAM_MUST_NOT_BE_CODE = "param must not be code";
const char* const PARAM_MUST_BE_HASH = "param must be hash";
const char* const MODE_MUST_NOT_BE_CODE = "mode must not be code";
const char* const OPTIONS_MUST_NOT_BE_CODE = "options must not be code";

const char* const CALLED_WITH_INVALID_OPTION = "called with invalid option";

// includes

#include "pa_memory.h"

// forwards

class String;

// defines

class Exception {
public:

	Exception();
	Exception(const char* atype, const String* aproblem_source, const char* comment_fmt, ...);
	Exception(const Exception& src);

	operator bool() { return ftype || fproblem_source || fcomment; }
	Exception& operator =(const Exception& src);

	/// extracts exception type
	const char* type(bool can_be_empty=false) const {
		return can_be_empty || ftype ? ftype : "<no type>";
	}

	/// extracts exception problem_source
	const String* problem_source() const;

	/// extracts exception comment
	const char* comment(bool can_be_empty=false) const {
		const char* result=fcomment && *fcomment ? fcomment : 0;
		return can_be_empty || result ? result : "<no comment>";
	}

	// modifies exception to fine-tune error reporting
	void set_source(const String* aproblem_source) {
		if(!problem_source())
			fproblem_source=aproblem_source;
	}

	void add_comment(const char* acomment);

#define ALTER_EXCEPTION_SOURCE(code, source)	\
	try {					\
		code;				\
	} catch (Exception& e) {		\
		e.set_source(source);		\
		rethrow;			\
	}

#define ALTER_EXCEPTION_COMMENT(code, comment)	\
	try {					\
		code;				\
	} catch (Exception& e) {		\
		e.add_comment(comment);		\
		rethrow;			\
	}

protected:

	const char* ftype;
	const String* fproblem_source;
	const char* fcomment;

};

#endif
