/**	@file
	Parser: write context class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

static const char* IDENT_WCONTEXT_H="$Date: 2002/08/29 12:22:48 $";

#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vhash.h"

class Request;

class StringOrValue {
public:
	StringOrValue() : fstring(0), fvalue(0) {}
	/// anticipating either String or Value [must not be 0&0]
	StringOrValue(const String *astring, Value *avalue) : fstring(astring), fvalue(avalue) {}
	void set_string(const String& astring) { fstring=&astring; }
	void set_value(Value& avalue) { fvalue=&avalue; }
	const String *get_string() { return fstring; }
	Value *get_value() { return fvalue; }
	Value& as_value() const {
		return *(fvalue?fvalue:new(fstring->pool()) VString(*fstring));
	}
	const String& as_string() const {
		return fstring?*fstring:fvalue->as_string();
	}
private:
	const String *fstring;
	Value *fvalue;
};

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

public: // WContext

	/// appends a fstring to result
	virtual void write(const String& astring, uchar lang) {
		fstring.append(astring, lang);
	}
	/// writes Value; raises an error if already, providing origin
	virtual void write(Value& avalue, const String* origin=0);

	/**
		if value is VString writes fstring,
		else writes Value; raises an error if already, providing origin
	*/
	void write(Value& avalue, uchar lang, const String* origin=0) {
		if(const String *fstring=avalue.get_string())
			write(*fstring, lang);
		else
			write(avalue, origin);
	}

	/**
		retrives the resulting value
		that can be String if value==0 or the Value object
		wmethod_frame first checks for $result and if there is one, returns it instead
	*/
	virtual StringOrValue result() {
		return fvalue?StringOrValue(0, fvalue):StringOrValue(&fstring, 0);
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

	void set_in_expression(bool ain_expression) { flags.in_expression=ain_expression?1:0; }
	bool get_in_expression() { return flags.in_expression!=0; }

	void set_somebody_entered_some_class() { flags.entered_class=1; }
	bool get_somebody_entered_some_class() { return flags.entered_class!=0; }

protected:
	String& fstring;
	Value *fvalue;
private:
	struct {
		int constructing:1;
		int in_expression:1;
		int entered_object:1;
		int entered_class:1;
	} flags;

};

#endif
