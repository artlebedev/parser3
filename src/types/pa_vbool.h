/** @file
	Parser: @b bool class decls.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

static const char * const IDENT_VBOOL_H="$Date: 2003/11/20 16:34:28 $";

// include

#include "pa_common.h"
#include "pa_value.h"

#define MAX_BOOL_AS_STRING 20

/// VBool
class VBool: public Value {
public: // Value

	override const char* type() const { return "bool"; }
	/// VBool: 0
	override VStateless_class *get_class() { return 0; }
	/// VBool: clone
	override Value& as_expr_result(bool) { return *new VBool(fbool); }
	/// VBool: fbool
	override bool as_bool() const { return fbool; }
	/// VBool: fbool
	override double as_double() const { return fbool; }
	/// VBool: fbool
	override int as_int() const { return fbool; }

public: // usage

	VBool(bool abool): fbool(abool) {}

private:

	bool fbool;

};

#endif
