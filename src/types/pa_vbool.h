/** @file
	Parser: @b bool class decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

static const char* IDENT_VBOOL_H="$Id: pa_vbool.h,v 1.19 2002/08/01 11:26:54 paf Exp $";

#include "pa_common.h"
#include "pa_value.h"

#define MAX_BOOL_AS_STRING 20

/// VBool
class VBool : public Value {
public: // Value

	const char *type() const { return "bool"; }
	/// VBool: 0
	VStateless_class *get_class() { return 0; }
	/// VBool: clone
	Value *as_expr_result(bool) { return NEW VBool(pool(), fbool); }
	/// VBool: fbool
	bool as_bool() const { return fbool; }
	/// VBool: fbool
	double as_double() const { return fbool; }
	/// VBool: fbool
	int as_int() const { return fbool; }

public: // usage

	VBool(Pool& apool, bool abool) : Value(apool), 
		fbool(abool) {
	}

private:

	bool fbool;

};

#endif
