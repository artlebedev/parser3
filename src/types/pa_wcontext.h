/**	@file
	Parser: write context class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

#define IDENT_PA_WCONTEXT_H "$Id: pa_wcontext.h,v 1.71 2020/12/15 17:10:44 moko Exp $"

#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vhash.h"

class Request;

/** ValueRef
	convenient helper when delayed initialization required
*/

class ValueRef {
public:
	ValueRef() : fvalue(0) {}
	ValueRef(Value& avalue) : fvalue(&avalue) {}
	ValueRef(Value* avalue) : fvalue(avalue) {}
	operator Value& () { return *fvalue; }
	Value* operator &() { return fvalue; }
	const String &as_string() { return fvalue->as_string(); }
private:
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
		return fstring ? fstring : &empty;
	};

	/// WContext: none yet | transparent
	override VStateless_class *get_class() { return fvalue ? fvalue->get_class() : 0; }

public: // WContext

	/// appends a fstring to result
	virtual void write(const String& astring) {
		if(!fstring) fstring=new String;
		astring.append_to(*fstring);
	}
	/// writes Value; raises an error if already, providing origin
	virtual void write(Value& avalue);

	/// if value is string convertable writes fstring, else writes Value
	virtual void write_as_string(Value& avalue) {
		if(const String* string=avalue.get_string())
			write(*string);
		else
			write(avalue);
	}

	/**
		retrives the resulting value
		that can be String if value==0 or the Value object
		wmethod_frame first checks for $result and if there is one, returns it instead
	*/
	virtual ValueRef result() {
		static String empty;
		static VString vempty(empty);
		return fvalue ? fvalue : fstring ? new VString(*fstring) : &vempty;
	}

	void attach_junction(VJunction* ajunction) {
		junctions+=ajunction;
	}

public: // usage

	WContext(WContext *aparent):
		fparent(aparent),
		fstring(0),
		fvalue(0){}

	virtual ~WContext() {
		detach_junctions();
	}

private:

	void detach_junctions(); 

protected:
	WContext *fparent;
	String* fstring;
	Value* fvalue;

private:
	Array<VJunction*> junctions;

};

#endif
