/** @file
	Parser: @b bool class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vbool.h,v 1.10 2001/05/11 17:45:10 parser Exp $
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

#include "pa_common.h"
#include "pa_value.h"

#define MAX_BOOL_AS_STRING 20

/// VBool
class VBool : public Value {
public: // Value

	const char *type() const { return "bool"; }
	/// VBool: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }
	/// VBool: fbool
	bool as_bool() { return fbool; }
	/// VBool: fbool
	double as_double() { return fbool; }
	/// VBool: fbool
	int as_int() { return fbool; }

public: // usage

	VBool(Pool& apool, bool abool) : Value(apool), 
		fbool(abool) {
	}

private:

	bool fbool;

};

#endif
