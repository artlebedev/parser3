/**	@file
	Parser: write context class decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

#define IDENT_PA_WCONTEXT_H "$Id: pa_wcontext.h,v 1.59 2013/10/11 20:45:43 moko Exp $"

#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vhash.h"

class Request;

class StringOrValue {
public:
	StringOrValue() : fstring(0), fvalue(0) {}
	/// anticipating either String or Value [must not be 0&0]
	StringOrValue(const String& astring) : fstring(&astring), fvalue(0) {}
	StringOrValue(Value& avalue) : fstring(0), fvalue(&avalue) {}

	void set_string(const String& astring) { fstring=&astring; }
	void set_value(Value& avalue) { fvalue=&avalue; }
	const String* get_string() { return fstring; }
	Value* get_value() { return fvalue; }
	Value& as_value() const {
		Value* result=fvalue?fvalue:new VString(*fstring);
		return *result;
	}
	const String& as_string() const {
		return fstring?*fstring:fvalue->as_string();
	}
private:
	const String* fstring;
	Value* fvalue;
};

/** Write context
	they do different write()s here, later picking up the result
	@see Request::wcontext
*/
class WContext: public Value {
	friend class Request;

public: // Value

	override const char* type() const { return "wcontext"; }
	/// WContext: accumulated fstring
	override const String* get_string() {
		static String empty;
		return fstring?fstring:&empty;
	};

	/// WContext: none yet | transparent
	override VStateless_class *get_class() { return fvalue?fvalue->get_class():0; }

public: // WContext

	/// appends a fstring to result
	virtual void write(const String& astring, String::Language alang) {
		if(!fstring) fstring=new String;
		astring.append_to(*fstring, alang);
	}
	/// writes Value; raises an error if already, providing origin
	virtual void write(Value& avalue);

	/**
		if value is VString writes fstring,
		else writes Value; raises an error if already, providing origin
	*/
	virtual void write(Value& avalue, String::Language alang) {
		if(const String* fstring=avalue.get_string())
			write(*fstring, alang);
		else
			write(avalue);
	}

	/**
		retrives the resulting value
		that can be String if value==0 or the Value object
		wmethod_frame first checks for $result and if there is one, returns it instead
	*/
	virtual StringOrValue result() {
		static String empty;
		return fvalue?StringOrValue(*fvalue):StringOrValue(fstring?*fstring:empty);
	}

	void attach_junction(VJunction* ajunction) {
		junctions+=ajunction;
	}

public: // usage

	WContext(WContext *aparent):
		fparent(aparent),
		fstring(0),
		fvalue(0),
		in_expression(false),
		entered_class(false){}

	virtual ~WContext() {
		detach_junctions();
	}

	void set_in_expression(bool ain_expression) { in_expression=ain_expression; }
	bool get_in_expression() { return in_expression; }

	void set_somebody_entered_some_class() { entered_class=true; }
	bool get_somebody_entered_some_class() { bool result=entered_class; entered_class=0; return result; }

private:

	void detach_junctions(); 

protected:
	WContext *fparent;
	String* fstring;
	Value* fvalue;

private: // status

	bool in_expression;
	bool entered_class;

private:
	Array<VJunction*> junctions;

};

#endif
