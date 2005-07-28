/**	@file
	Parser: @b property class decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VPROPERTY_H
#define PA_VPROPERTY_H

static const char * const IDENT_VPROPERTY_H="$Date: 2005/07/28 11:23:02 $";

// include

#include "pa_value.h"
#include "pa_vbool.h"
#include "pa_property.h"

/// property two methods
class VProperty: public Value {
public: // VProperty

	override const char* type() const { return "property"; }

	/// VProperty: 0
	override VStateless_class *get_class() { bark("is '%s', it has no class"); return 0; }

	/// VProperty: false
	override bool is_defined() const { bark("is '%s', it has no 'defined'"); return false; }

	/// VProperty: false
	override bool as_bool() const { return is_defined(); }

	/// VProperty: false
	override Value& as_expr_result(bool) { return *new VBool(as_bool()); }

	/// VProperty: property
	override Property* get_property() { return &fproperty; }

public: // usage

	Property& get() { return fproperty; }

private:

	Property fproperty;
};


#endif
