/**	@file
	Parser: write context class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_wcontext.h,v 1.22 2001/11/19 12:17:07 paf Exp $
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
	virtual void write(const String& astring, uchar lang);

	/// writes Value; raises an error if already
	virtual void write(Value& avalue);

	/**
		if value is VString writes fstring,
		else writes Value; raises an error if already
	*/
	virtual void write(Value& avalue, uchar lang);

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
		fvalue(avalue) {
		flags.constructing=
			flags.entered_class=
			flags.entered_object=0;
	}

	void set_constructing(bool aconstructing) { flags.constructing=aconstructing?1:0; }
	bool get_constructing() { return flags.constructing!=0; }

	void set_somebody_entered_some_class() { flags.entered_class=1; }
	bool get_somebody_entered_some_class() { return flags.entered_class!=0; }

	void set_somebody_entered_some_object(bool aentered_object) {   
		flags.entered_object=aentered_object?1:0; }
	bool get_somebody_entered_some_object() { return flags.entered_object!=0; }

protected:
	String& fstring;
	Value *fvalue;
private:
	struct {
		int constructing:1;
		int entered_object:1;
		int entered_class:1;
	} flags;
};

#endif
