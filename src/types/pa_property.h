/** @file
	Parser: Property class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_PROPERTY_H
#define PA_PROPERTY_H

#define IDENT_PA_PROPERTY_H "$Id: pa_property.h,v 1.8 2020/12/15 17:10:38 moko Exp $"

class Method;
class Value;

/** \b junction is getter and setter methods.

	it is used to cache our knowledge that some element is in fact property
	and to save us one hash lookup and one name construction (element--get_element/set_leement)
*/
class Property: public PA_Object {
public:
	Method* getter;
	Method* setter;
	Value *value;

	Property() : getter(0), setter(0), value(0){}
	Property(Property &prop) : getter(prop.getter), setter(prop.setter), value(prop.value){}
};

#endif
