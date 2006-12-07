/** @file
	Parser: @b bool class decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

static const char * const IDENT_VBOOL_H="$Date: 2006/12/07 18:28:33 $";

// include

#include "pa_common.h"
#include "pa_value.h"

// defines

#define MAX_BOOL_AS_STRING 20
#define VBOOL_TYPE "bool"

/// VBool
class VBool: public Value {
public: // Value

	override const char* type() const { return VBOOL_TYPE; }
	/// VBool: 0
	override VStateless_class *get_class() { return 0; }
	/// VBool: true
	override bool is_evaluated_expr() const { return true; }
	/// VBool: clone
	override Value& as_expr_result(bool) { return *new VBool(fbool); }
	/// VBool: fbool
	virtual bool is_defined() const { return as_bool(); }
	/// VBool: fbool
	override bool as_bool() const { return fbool; }
	/// VBool: fbool
	override double as_double() const { return fbool; }
	/// VBool: fbool
	override int as_int() const { return fbool; }
	override bool is_bool() const { return true; }

public: // usage

	VBool(bool abool): fbool(abool) {}

private:

	bool fbool;

};

#endif
