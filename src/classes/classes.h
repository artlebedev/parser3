/** @file
	Parser: @b Methoded class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: classes.h,v 1.6 2001/05/03 15:05:15 paf Exp $
*/

#ifndef CLASSES_H
#define CLASSES_H

#include "pa_vstateless_class.h"
#include "pa_array.h"

/**	Pure virtual base for configurable Methoded descendants
*/
class Methoded : public VStateless_class {
public: // Value

	/// all: for error reporting after fail(), etc
	const char *type() const { return "m_base"; }

public: // Methoded

	/** should Methoded_array::register_directly_used register this class in
		Request::classes() or not. 
		if not - global variable with Methoded descendant 
		is used in VStateless_class parameter
	*/
	virtual bool used_directly() =0;
	/// use this method to read parameters from root "auto.p"; access r.main_class
	virtual void configure_admin(Request& r) {}
	/// use this method to read parameters from 'MAIN' class; access r.main_class
	virtual void configure_user(Request& r) {}

public: // usage

	Methoded(Pool& pool) : VStateless_class(pool) {
	}

	void register_directly_used(Request& r);

};


class Methoded_array : public Array {
public:
	Methoded_array(Pool& pool);

public: // Methoded for_each-es
	void configure_admin(Request& r);
	void configure_user(Request& r);
	void register_directly_used(Request& r);
};

extern Methoded_array *methoded_array;
void init_methoded_array(Pool& pool);

#endif
