/** @file
	Parser: @b date parser class decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDATE_H
#define PA_VDATE_H

static const char* IDENT_VDATE_H="$Date: 2003/07/24 11:31:25 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vint.h"

// defines

#define VDATE_TYPE "date"

// externs

extern Methoded* date_class;

/// value of type 'date'. implemented with @c time_t
class VDate: public VStateless_object {
public: // Value

	override const char* type() const { return VDATE_TYPE; }
	override VStateless_class *get_class() { return date_class; }
	
	/// VDate: ftime -> float days
	override Value& as_expr_result(bool return_string_as_is=false) { return *new VDouble(as_double()); }

	/// VDate: ftime -> float days
	override double as_double() const { return ((double)ftime)/ SECS_PER_DAY; }
	/// VDate: 0 or !0
	override bool as_bool() const { return ftime!=0; }


	/// VDate: method,field
	override Value* get_element(const String& aname, Value& aself, bool looking_up) {
		// $method
		if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
			return result;

		// $year month day  hour minute second  weekday
		tm *tmOut=localtime(&ftime);
		int result;
		if(aname=="year") result=1900+tmOut->tm_year;
		else if(aname=="month") result=1+tmOut->tm_mon;
		else if(aname=="day") result=tmOut->tm_mday;
		else if(aname=="hour") result=tmOut->tm_hour;
		else if(aname=="minute") result=tmOut->tm_min;
		else if(aname=="second") result=tmOut->tm_sec;
		else if(aname=="weekday") result=tmOut->tm_wday;
		else if(aname=="yearday") result=tmOut->tm_yday;
		else if(aname=="daylightsaving") result=tmOut->tm_isdst;
		else { bark("%s field not found", 0, &aname); return 0; }
		return new VInt(result);
	}

public: // usage

	VDate(time_t atime): ftime(atime) {}

	time_t get_time() const { return ftime; }
	void set_time(time_t atime) { ftime=atime; }

private:

	time_t ftime;

};

#endif
