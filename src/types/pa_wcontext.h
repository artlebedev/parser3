/**	@file
	Parser: write context class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_wcontext.h,v 1.13 2001/04/28 08:44:17 paf Exp $
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
	friend Request;
public: // Value

	/// all: for error reporting after fail(), etc
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
		// not constructing anymore [if were constructing]
		// so to allow method calls after real constructor-method call
		// sample:
		//	$complex[
		//		$class:constructor[$i]
		//		^i.inc[]  ^rem{allow such calls}
		//		$field[$1]
		fconstructing=false;
		return fvalue?fvalue:NEW VString(fstring);
	}

public: // usage

	WContext(Pool& apool, Value *avalue, bool aconstructing) : Value(apool), 
		fstring(*new(apool) String(apool)),
		fconstructing(aconstructing), fvalue(avalue),
		fsomebody_entered_some_class(false){
	}

	bool constructing() { return fconstructing; }
	void constructing(bool aconstructing) { fconstructing=aconstructing; }
	void set_somebody_entered_some_class() { fsomebody_entered_some_class=true; }
	bool somebody_entered_some_class() { return fsomebody_entered_some_class; }

protected:
	String& fstring;
	bool fconstructing;  Value *fvalue;
private:
	bool fsomebody_entered_some_class;
};

#endif
