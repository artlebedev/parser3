/**	@file
	Parser: write context class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_wcontext.h,v 1.19 2001/10/29 16:07:36 paf Exp $
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vhash.h"

class Request;

/** Write context
	they do different write()s here, later picking up the result
	@see Request::wcontext
*/
class WContext : public Value {
	friend class Request;

public: // Value

	const char *type() const { return "wcontext"; }
	/// WContext: accumulated fstring
	const String *get_string() { return &fstring; };

	/// WContext: none yet | transparent
	VStateless_class *get_class() { return fvalue?fvalue->get_class():0; }
	/// WContext: transparent
	VAliased *get_aliased() { return fvalue?fvalue->get_aliased():0; }

public: // WContext

	/// appends a fstring to result
	virtual void write(const String& astring, String::Untaint_lang lang);

	/// writes Value; raises an error if already
	virtual void write(Value& avalue);

	/**
		if value is VString writes fstring,
		else writes Value; raises an error if already
	*/
	virtual void write(Value& avalue, String::Untaint_lang lang);

	/**
		retrives the resulting value
		that can be VString if value==0 or the Value object
		wmethod_frame first checks for $result and if there is one, returns it instead
	*/
	virtual Value *result() {
		return fvalue?fvalue:NEW VString(fstring);
	}

public: // usage

	WContext(Pool& apool, Value *avalue) : Value(apool), 
		fstring(*new(apool) String(apool)),
		fvalue(avalue),
		fconstructing(false), fentered_object(false), fentered_class(false) {
	}

	void set_constructing(bool aconstructing) { fconstructing=aconstructing; }
	bool get_constructing() { return fconstructing; }

	void set_somebody_entered_some_class() { fentered_class=true; }
	bool get_somebody_entered_some_class() { return fentered_class; }

	void set_somebody_entered_some_object(bool aentered_object) {   
		fentered_object=aentered_object; }
	bool get_somebody_entered_some_object() { return fentered_object; }

protected:
	String& fstring;
	Value *fvalue;
private:
	struct {
		bool fconstructing:1;
		bool fentered_object:1;
		bool fentered_class:1;
	};
};

#endif
