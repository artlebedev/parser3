/** @file
	Parser: @b Methoded class decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef CLASSES_H
#define CLASSES_H

#define IDENT_CLASSES_H "$Id: classes.h,v 1.42 2023/09/26 20:49:05 moko Exp $"

// include

#include "pa_vstateless_class.h"
#include "pa_array.h"

/**	Pure virtual base for configurable Methoded descendants
	@see Methoded_array
*/
class Methoded: public VStateless_class {
public: // Value
	
	override const char* type() const { return ftype; }

public: // Methoded

	/** should Methoded_array::register_directly_used register this class in
		Request::classes() or not. 
		if not - global variable with Methoded descendant 
		is used in VStateless_class parameter
	*/
	virtual bool used_directly() { return true; }
	/// use this method to read parameters from root "auto.p"; access r.main_class
	virtual void configure_admin(Request& ) {}
	/// use this method to read parameters from 'MAIN' class; access r.main_class
	virtual void configure_user(Request& ) {}

private:

	const char* ftype;

public: // usage

	Methoded(const char* atype): ftype(atype){}

	void register_directly_used(Request& r);

};

/// all Methoded registered here in autogenerated classes.C
class Methoded_array: public Array<Methoded*> {
public:
	Methoded_array();

public: // Methoded for_each-es
	/// @see Methoded::configure_admin
	void configure_admin(Request& r);
	/// @see Methoded::configure_user
	void configure_user(Request& r);
	/// @see Methoded::register_directly_used
	void register_directly_used(Request& r);
};

// globals

Methoded_array& methoded_array();

// defines

#define DECLARE_CLASS_VAR(name, self) \
	Methoded* name##_class=self

#endif
