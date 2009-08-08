/**	@file
	Parser: @b write_wrapper write context

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_WWRAPPER_H
#define PA_WWRAPPER_H

static const char * const IDENT_WWRAPPER_H="$Date: 2009/08/08 13:30:22 $";

#define OPTIMIZE_SINGLE_STRING_WRITE

#include "pa_wcontext.h"
#include "pa_exception.h"

/// specialized write context, adds to WContext VHash autocreation ability
class WWrapper: public WContext {
public: // Value

	override const char* type() const { return "wwrapper"; }
	/// WWrapper: transparent
	override Value* get_element(const String& aname) { 
		return as_value().get_element(aname); 
	}
	/// WWrapper: transparent
	override const VJunction* put_element(const String& aname, Value* avalue, bool areplace) { 
		if(!fvalue) {
			fvalue=new VHash;
			// not constructing anymore [if were constructing]
			// so to allow method calls after real constructor-method call
			// sample:
			//	$hash[
			//		$.key1[$i]
			//		^i.inc[]  ^rem{allow such calls}
			//		$.key2[$1]
		}

		return fvalue->put_element(aname, avalue, areplace); 
	}

public: // usage

	WWrapper(WContext *aparent) : 
		WContext(aparent) {
	}
	
private:
	// raises an exception on 0 value
	Value& as_value() const {
		if(!fvalue)
			throw Exception(0,
				0,
				"accessing wrapper without value");

		return *fvalue;
	}
};

#ifdef OPTIMIZE_SINGLE_STRING_WRITE
class WObjectPoolWrapper: public WWrapper {
public:

	enum WState {
		WS_NONE,
		WS_KEEP_VALUE,
		WS_TRANSPARENT
	};

	WObjectPoolWrapper(WContext *aparent) : 
		WWrapper(aparent), fstate(WS_NONE) {
	}

	override const VJunction* put_element(const String& aname, Value* avalue, bool areplace) { 
		if(fstate == WS_KEEP_VALUE)
			fvalue=0; // VHash will be created, thus no need to flush fvalue
		fstate=WS_TRANSPARENT;
		return WWrapper::put_element(aname, avalue, areplace); 
	}

	override void write(const String& astring, String::Language alang) {
		if(fstate == WS_KEEP_VALUE)
			flush();
		fstate=WS_TRANSPARENT;
		WWrapper::write(astring, alang);
	}

	override void write(Value& avalue) {
		if(fstate == WS_KEEP_VALUE)
			flush();
		fstate=WS_TRANSPARENT;
		WWrapper::write(avalue);
	}

	override void write(Value& avalue, String::Language alang) {
		switch(fstate){
			case WS_NONE:{
				// alang is allways L_PASS_APPENDED, but just in case we check it
				// only VString can be cached, no get_string() call as VInt/etc will be affected
				if(avalue.is_string() && alang == String::L_PASS_APPENDED){
					fvalue=&avalue;
					fstate=WS_KEEP_VALUE;
					return;
				}
				break;
			}
			case WS_KEEP_VALUE:{
				flush();
				break;
			}
		}
		fstate=WS_TRANSPARENT;
		// we copy WWrapper::write here to prevent virtual call to our class
		if(const String* fstring=avalue.get_string())
			WWrapper::write(*fstring, alang);
		else
			WWrapper::write(avalue);
	}

	//override StringOrValue result() - not required as as_value() will be allways called
private:

	WState fstate;

	inline void flush(){
		WWrapper::write(*fvalue->get_string(), String::L_PASS_APPENDED);
		fvalue=0;
	}
};
#endif

#endif
