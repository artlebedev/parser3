/** @file
	Parser: @b date parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdate.h,v 1.1 2001/07/07 16:38:02 parser Exp $
*/

#ifndef PA_VDATE_H
#define PA_VDATE_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vint.h"

#define SECS_PER_DAY (60*60*24)

extern Methoded *date_class;

/// value of type 'date'. implemented with @c time_t
class VDate : public VStateless_object {
public: // Value

	const char *type() const { return "date"; }
	/// VDate: ftime -> float days
	Value *as_expr_result(bool return_string_as_is=false) {
		return NEW VDouble(pool(), as_double());
	}

	/// VDate: ftime -> float days
	double as_double() { return ((double)ftime)/ SECS_PER_DAY; }
	/// VDate: 0 or !0
	bool as_bool() { return ftime!=0; }


	/// VDate: CLASS,BASE,method,field
	Value *get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_object::get_element(aname))
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
		else return 0;
		return NEW VInt(pool(), result);
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VDate(Pool& apool, time_t adate) : VStateless_object(apool, *date_class), 
		ftime(adate) {
	}

	time_t get_time() { return ftime; }
	void set_time(time_t atime) { ftime=atime; }

private:

	time_t ftime;

};

#endif
