/** @file
	Parser: @b Methoded class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: classes.h,v 1.5 2001/04/28 15:22:38 paf Exp $
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

	virtual bool used_directly() =0;
	virtual void configure_admin(Request& r) {}
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
