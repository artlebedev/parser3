/** @file
	Parser: @b date parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDATE_H
#define PA_VDATE_H

static const char* IDENT_VDATE_H="$Id: pa_vdate.h,v 1.11 2002/08/01 11:26:54 paf Exp $";

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


	/// VDate: CLASS,method,field
	Value *get_element(const String& aname) {
		// $CLASS,$method
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

	VDate(Pool& apool, time_t adate) : VStateless_object(apool), 
		ftime(adate) {
	}

	time_t get_time() { return ftime; }
	void set_time(time_t atime) { ftime=atime; }

private:

	time_t ftime;

};

#endif
