/** @file
	Parser: aliased class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_valiased.h,v 1.14 2001/09/26 10:32:26 parser Exp $
*/

#ifndef PA_VALIASED_H
#define PA_VALIASED_H

#include "pa_globals.h"
#include "pa_value.h"

class Temp_alias;

#define CLASS_NAME "CLASS"
#define BASE_NAME "BASE"

/**
	the object or class which effective class can change along method call.

	consider this:
	@verbatim
		@CLASS
		article
		@display[]
		^display_head[]
		^display_body[]
		^display_tail[]

		@CLASS
		news_article
		@display_tail[]
		...some calls...

		@main[]
		$na[^news_article:load[]]
		^na.display[]
	@endverbatim
	here ^na.display[] call would go to @a article class
	then back to news_article:display_tail.

	basically object's effective class changed twice:
	-# from news_article to article in ^display[] call
	-# from article to news_article in ^display_tail[] call
	  
	@see Temp_alias
*/
class VAliased : public Value {
	friend Temp_alias;
public: // creation

	VAliased(Pool& apool, VStateless_class& aclass_alias) : Value(apool), 
		fclass_alias(&aclass_alias) {
	}

	/// VAliased: this
	VAliased *get_aliased() { return this; }

	/// VAliased: $CLASS
	Value *get_element(const String& aname);

protected: // VAliased

/**
	used in get_element for getting $CLASS.

	to disable those elements classes can override 
	this method with @c {return true;}
*/
	virtual bool hide_class() { return false; }

private: // alias handling

	// VAliased replacement mechanism is 'private'zed from direct usage
	// Temp_alias object enforces paired set/restore
	VStateless_class *set_alias(VStateless_class *aclass_alias) {
		VStateless_class *result=fclass_alias;
		fclass_alias=aclass_alias;
		return result;
	}
	void restore_alias(VStateless_class *aclass_alias) {
		fclass_alias=aclass_alias;
	}

protected:

	VStateless_class *fclass_alias;
};

///	Auto-object used for temporarily changing object's effective class
class Temp_alias {
	VAliased& vobject;
	VStateless_class *saved_alias;
public:
	Temp_alias(VAliased& avobject, VStateless_class& alias) : 
		vobject(avobject),
		saved_alias(avobject.set_alias(&alias)) {
	}
	~Temp_alias() { 
		vobject.restore_alias(saved_alias); 
	}
};

#endif
