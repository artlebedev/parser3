/** @file
	Parser: @b Methoded class decl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#ifndef CLASSES_H
#define CLASSES_H

static const char* IDENT_CLASSES_H="$Date: 2003/01/21 15:51:06 $";

#include "pa_vstateless_class.h"
#include "pa_array.h"

/**	Pure virtual base for configurable Methoded descendants
	@see Methoded_array
*/
class Methoded : public VStateless_class {
public: // Value

	/*override*/ bool put_element(const String& /*name*/, Value * /*value*/, bool /*replace*/) { 
		return false; // fewer useless Exceptions
	}

public: // Methoded

	/** should Methoded_array::register_directly_used register this class in
		Request::classes() or not. 
		if not - global variable with Methoded descendant 
		is used in VStateless_class parameter
	*/
	virtual bool used_directly() =0;
	/// use this method to read parameters from root "auto.p"; access r.main_class
	virtual void configure_admin(Request& ) {}
	/// use this method to read parameters from 'MAIN' class; access r.main_class
	virtual void configure_user(Request& ) {}
	/// use it to construct static variables. check some static so that would be only ONCE!
	virtual void construct_statics() {}

public: // usage

	Methoded(Pool& apool, const char *aname, VStateless_class *abase=0) : 
		VStateless_class(apool, new(apool) String(apool, aname), abase) {
	}

	void register_directly_used(Request& r);

};

/// all Methoded registered here in autogenerated classes.C
class Methoded_array : public Array {
public:
	Methoded_array(Pool& pool);

public: // Methoded for_each-es
	/// @see Methoded::configure_admin
	void configure_admin(Request& r);
	/// @see Methoded::configure_user
	void configure_user(Request& r);
	/// @see Methoded::register_directly_used
	void register_directly_used(Request& r);
};

extern Methoded_array *methoded_array;
void init_methoded_array(Pool& pool);

#endif
