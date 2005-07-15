/** @file
	Parser: Property class decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_PROPERTY_H
#define PA_PROPERTY_H

static const char * const IDENT_PROPERTY_H="$Date: 2005/07/15 06:16:41 $";

class Method;

/** \b junction is getter and setter methods.

	it is used to cache our knowledge that some element is in fact property
	and to save us one hash lookup and one name construction (element--get_element/set_leement)
*/
class Property: public PA_Object {
public:
	Method* getter;
	Method* setter;
};

#endif
