/** @file
	Parser: @b date parser class decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDATE_H
#define PA_VDATE_H

static const char * const IDENT_VDATE_H="$Date: 2003/11/20 16:34:28 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vint.h"

// defines

#define VDATE_TYPE "date"

// externs

extern Methoded* date_class;

inline void set_tz(const char* tz, char* buf, size_t buf_size) {
	snprintf(buf, buf_size, "TZ=%s", tz?tz:"");
	putenv(buf);
	tzset();
}

/// value of type 'date'. implemented with @c time_t
class VDate: public VStateless_object {
public: // Value

	override const char* type() const { return VDATE_TYPE; }
	override VStateless_class *get_class() { return date_class; }
	
	/// VDate: ftime -> float days
	override Value& as_expr_result(bool /*return_string_as_is=false*/) { return *new VDouble(as_double()); }

	/// VDate: ftime -> float days
	override double as_double() const { return ((double)ftime)/ SECS_PER_DAY; }
	/// VDate: 0 or !0
	override bool as_bool() const { return ftime!=0; }


	/// VDate: method,field
	override Value* get_element(const String& aname, Value& aself, bool looking_up) {
		// $method
		if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
			return result;

		// $TZ
		if(aname=="TZ") 
			return new VString(*ftz);

		// $year month day  hour minute second  weekday

		const char* saved_tz=0;
		static char saved_tz_pair[MAX_STRING];
		static char temp_tz_pair[MAX_STRING];
		if(ftz_cstr) {
			saved_tz=getenv("TZ");
			::set_tz(ftz_cstr, temp_tz_pair, sizeof(temp_tz_pair));
		}
		tm *tmOut=localtime(&ftime);
		if(saved_tz)
			::set_tz(saved_tz, saved_tz_pair, sizeof(saved_tz_pair));

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
		else { return bark("%s field not found", &aname); }
		return new VInt(result);
	}

public: // usage

	VDate(time_t adate) : 
		ftime(adate),
		ftz(0),
		ftz_cstr(0) {
	}

	time_t get_time() const { return ftime; }
	void set_time(time_t atime) { ftime=atime; }
	void set_tz(const String* atz) { 
		if(ftz=atz)
			ftz_cstr=ftz->cstr();
	}

private:

	time_t ftime;
	const String* ftz;
	const char* ftz_cstr;

};

#endif
