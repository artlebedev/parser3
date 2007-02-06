/** @file
	Parser: @b bool class decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

static const char * const IDENT_VBOOL_H="$Date: 2007/02/06 17:01:12 $";

// include

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VBOOL_TYPE "bool"
#define MAX_BOOL_AS_STRING 20

extern Methoded* bool_class;

// VBool
class VBool: public VStateless_object {
public: // Value

	override const char* type() const { return VBOOL_TYPE; }
	/// VBool: 0
	override VStateless_class *get_class() { return bool_class; }

	/// VBool: true
	override bool is_evaluated_expr() const { return true; }
	/// VBool: clone
	override Value& as_expr_result(bool) { return *new VBool(fbool); }

	/// VBool: fbool
	virtual bool is_defined() const { return true; }

	/// VBool: fbool
	override double as_double() const { return as_int(); }
	/// VBool: fbool
	override int as_int() const { return fbool ? 1 : 0; }
	/// VBool: fbool
	override bool as_bool() const { return fbool; }

	override bool is_bool() const { return true; }

public: // usage

	VBool(bool abool): fbool(abool) {}
	bool get_bool() { return fbool; }

private:

	bool fbool;

};

#endif
