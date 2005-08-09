/**	@file
	Parser: write context class decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

static const char * const IDENT_WCONTEXT_H="$Date: 2005/08/09 08:05:09 $";

#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"

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
	override const String* get_string() { return &fstring; };

	/// WContext: none yet | transparent
	override VStateless_class *get_class() { return fvalue?fvalue->get_class():0; }

public: // WContext

	/// appends a fstring to result
	virtual void write(const String& astring, String::Language alang) {
		were_string_writes=true;
		fstring.append(astring, alang);
	}
	/// writes Value; raises an error if already, providing origin
	virtual void write(Value& avalue);

	/**
		if value is VString writes fstring,
		else writes Value; raises an error if already, providing origin
	*/
	void write(Value& avalue, String::Language alang) {
		if(avalue.is_void())
			return; // ignoring $void write attempts

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
		return fvalue?StringOrValue(*fvalue):
			were_string_writes? StringOrValue(fstring):
				StringOrValue(empty_result);
	}

	void attach_junction(Junction* ajunction) {
		junctions+=ajunction;
	}

public: // usage

	WContext(Value* avalue, WContext *aparent):
		fstring(*new String),
		fvalue(avalue),
		fparent(aparent) {
		constructing=in_expression=entered_class=entered_object=were_string_writes=false;
	}
	virtual ~WContext() {
		detach_junctions();
	}

	void set_constructing(bool aconstructing) { constructing=aconstructing; }
	bool get_constructing() { return constructing; }

	void set_in_expression(bool ain_expression) { in_expression=ain_expression; }
	bool get_in_expression() { return in_expression; }

	void set_somebody_entered_some_class() { entered_class=true; }
	bool get_somebody_entered_some_class() { return entered_class; }

private:

	void detach_junctions(); 

private:

	static VVoid empty_result;

protected:
	String& fstring;
	Value* fvalue;

private: // status

	bool constructing;
	bool in_expression;
	bool entered_object;
	bool entered_class;
	bool were_string_writes;

private:

	WContext *fparent;
	Array<Junction*>  junctions;

};

#endif
