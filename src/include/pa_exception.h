/** @file
	Parser: exception decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_exception.h,v 1.15 2001/10/19 12:43:30 parser Exp $
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

#include "pa_config_includes.h"
#include "pa_types.h"
#include "pa_string.h"

#ifdef XML
#include <PlatformSupport/XSLException.hpp>
#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>
#include <util/XMLException.hpp>
#endif

class Pool;

/// Just simple longjump mechanizm dressed in class clothes. use Temp_exception
class Exception {
public:

	Exception(
		const String *atype, const String *acode,
		const String *aproblem_source, 
		const char *comment_fmt, ...);

#ifdef XML
	/// converts XSL exception to parser exception
	void _throw(Pool& pool, const String *source, const XSLException& e);
	/// converts SAX exception to parser exception
	void _throw(Pool& pool, const String *source, const SAXException& e);
	/// converts SAX parse exception to parser exception
	void _throw(Pool& pool, const String *source, const SAXParseException& e);
	/// converts XML exception to parser exception
	void _throw(Pool& pool, const String *source, const XMLException& e);
	/// converts Xalan DOM exception to parser exception
	void _throw(Pool& pool, const String *source, const XalanDOMException& e);
#endif

	/// extracts exception type
	const String *type() const { return ftype; }
	/// extracts exception code
	const String *code() const { return fcode; }
	/// extracts exception problem_source
	const String *problem_source() const { return fproblem_source; }
	/// extracts exception comment
	const char *comment() const { return fcomment[0]?fcomment:0; }

private:

	const String *ftype, *fcode, *fproblem_source;
	char fcomment[MAX_STRING];

};

#endif
