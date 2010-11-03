/** @file
	Parser: Value, Method, Junction .

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

static const char * const IDENT_VALUE_H="$Date: 2010/11/03 22:08:44 $";

#include "pa_common.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_property.h"

// forwards

class VStateless_class;
class WContext;
class Request;
class Request_charsets;
class Junction;
class VJunction;
class Method;
class Value;
class MethodParams;
class VObject;
class VMethodFrame;
class VFile;
class Table;
typedef Array<Value*> ArrayValue;

struct Json_options {
	Request* r;
	HashStringValue* methods;
	Value* params;
	bool skip_unknown;
	const char* indent;
	enum Date { D_SQL, D_GMT, D_TIMESTAMP } date;
	enum Table { T_ARRAY, T_OBJECT, T_COMPACT } table;
	enum File { F_BODYLESS, F_BASE64, F_TEXT } file;

	Json_options(Request* arequest): 
		r(arequest),
		methods(NULL),
		params(NULL),
		skip_unknown(false),
		indent(NULL),
		date(D_SQL),
		table(T_OBJECT),
		file(F_BODYLESS)
	{}

	bool set_date_format(const String &value){
		if(value == "gmt-string") date = D_GMT;
		else if (value == "sql-string") date = D_SQL;
		else if (value == "unix-timestamp") date = D_TIMESTAMP;
		else return false;
		return true;
	}

	bool set_table_format(const String &value){
		if(value == "array") table = T_ARRAY;
		else if (value == "object") table = T_OBJECT;
		else if (value == "compact") table = T_COMPACT;
		else return false;
		return true;
	}

	bool set_file_format(const String &value){
		if(value == "base64") file = F_BASE64;
		else if (value == "text") file = F_TEXT;
		else return false;
		return true;
	}
};

///	grandfather of all @a values in @b Parser
class Value: public PA_Object {
public: // Value

	/// value type, used for error reporting and 'is' expression operator
	virtual const char* type() const =0;

	/**
		all except VObject/VClass: this if @atype eq type()
		VObject/VClass: can locate parent class by it's type
	*/
	virtual Value* as(const char* atype) {
		return atype && strcmp(type(), atype)==0?this:0;
	}
	/// type checking helper, uses Value::as
	bool is(const char* atype) { return as(atype)!=0; }
	
	/// is this value defined?
	virtual bool is_defined() const { return true; }
	
	/// is this value string?
	virtual bool is_string() const { return false; }

	/// is this value void?
	virtual bool is_void() const { return false; }

	/// is this value bool?
	virtual bool is_bool() const { return false; }

	/// is this value number?
	virtual bool is_evaluated_expr() const { return false; }

	/// what's the meaning of this value in context of expression?
	virtual Value& as_expr_result(bool /*return_string_as_is*/=false) {
		return *bark("is '%s', can not be used in expression"); 
	}
	
	

	/** extract HashStringValue if any
		WARNING: FOR LOCAL USE ONLY, THIS POINTER IS NOT TO PASS TO ANYBODY!
	*/
	virtual HashStringValue* get_hash() { return 0; }
	
	/// extract const String
	virtual const String* get_string() { return 0; }

	/// extract json-string
	virtual const String* get_json_string(Json_options* options=0);

	virtual HashStringValue* get_fields() { return 0; }
	
	/// extract double
	virtual double as_double() const { bark("is '%s', it does not have numerical (double) value"); return 0; }
	
	/// extract integer
	virtual int as_int () const { bark("is '%s', it does not have numerical (int) value"); return 0; }

	/// extract bool
	virtual bool as_bool() const { bark("is '%s', it does not have logical value"); return 0; }
	
	/// extract file
	virtual VFile* as_vfile(String::Language lang, const Request_charsets* charsets=0);
	
	/// extract Junction
	virtual Junction* get_junction();
	
	/// @return Value element; can return Junction for methods; Code-Junction for code; Getter-Junction for property
	virtual Value* get_element(const String& /*aname*/);

	/// indicator value meaning that put_element overwritten something
	#define PUT_ELEMENT_REPLACED_ELEMENT reinterpret_cast<const VJunction*>(1)

	/// store Value element under @a name
	/// @returns putter method junction, or it can just report[PUT_ELEMENT_REPLACED_ELEMENT] 
	/// that it replaced something in base fields 
	virtual const VJunction* put_element(const String& aname, Value* /*avalue*/, bool /*areplace*/) { 
		// to prevent modification of system classes,
		// created at system startup, and not having exception
		// handler installed, we neet to bark using request.pool
		bark("element can not be stored to %s", &aname); 
		return 0;
	}

	/// VObject default getter & setter support
	virtual void enable_default_getter(){ }
	virtual void enable_default_setter(){ }
	virtual void disable_default_getter(){ }
	virtual void disable_default_setter(){ }
	virtual bool is_enabled_default_getter(){ return true; }
	
	/// extract VStateless_class
	virtual VStateless_class* get_class()=0;

	/// extract base class, if any
	virtual VStateless_class* base() { return 0; }

	/// extract VTable
	virtual Table* get_table() { return 0; }

public: // usage

	/// @return sure String. if it doesn't have string value barks
	const String& as_string() {
		const String* result=get_string(); 
		if(!result)
			bark("is '%s', it has no string representation");

		return *result;
	}

	/// throws exception specifying bark-reason and name() type() of problematic value
	Value* bark(const char *reason, const String *problem_source=0) const {
		//if(this) // removing warnings on unreachable code
			throw Exception(PARSER_RUNTIME,
				problem_source, 
				reason, type());

		//return 0;
	}

};
	

/**
	$content-type[text/html] -> 
		content-type: text/html
	$content-type[$.value[text/html] $.charset[windows-1251]] -> 
		content-type: text/html; charset=windows-1251
*/
const String& attributed_meaning_to_string(Value& meaning, 
					   String::Language lang, 
					   bool forced=false,
					   bool allow_bool=false);

// defines

///@{common field names
#define CHARSET_NAME "charset"
#define VALUE_NAME "value"
#define EXPIRES_NAME "expires"
#define CONTENT_TYPE_NAME "content-type"
#define NAME_NAME "name"
//@}

///@{common field names
extern const String value_name;
extern const String expires_name;
extern const String content_type_name;
extern const String name_name;
///@}

#endif
