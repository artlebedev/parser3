/** @file
	Parser: @b date parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDATE_H
#define PA_VDATE_H

static const char* IDENT_VDATE_H="$Date: 2002/08/13 13:02:41 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vint.h"

// defines

#define VDATE_TYPE "date"

// externs

extern Methoded *date_class;

/// value of type 'date'. implemented with @c time_t
class VDate : public VStateless_object {
public: // Value

	const char *type() const { return VDATE_TYPE; }
	VStateless_class *get_class() { return date_class; }
	
	/// VDate: ftime -> float days
	Value *as_expr_result(bool return_string_as_is=false) {
		return NEW VDouble(pool(), as_double());
	}

	/// VDate: ftime -> float days
	double as_double() const { return ((double)ftime)/ SECS_PER_DAY; }
	/// VDate: 0 or !0
	bool as_bool() const { return ftime!=0; }


	/// VDate: method,field
	Value *get_element(const String& aname, Value *aself) {
		// $method
		if(Value *result=VStateless_object::get_element(aname, aself))
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
		else return 0;
		return NEW VInt(pool(), result);
	}

public: // usage

	VDate(Pool& apool, time_t adate) : VStateless_object(apool), 
		ftime(adate) {
	}

	time_t get_time() { return ftime; }
	void set_time(time_t atime) { ftime=atime; }

private:

	time_t ftime;

};

#endif
