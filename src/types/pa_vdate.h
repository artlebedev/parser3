/** @file
	Parser: @b date parser class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDATE_H
#define PA_VDATE_H

static const char * const IDENT_VDATE_H="$Date: 2007/04/17 08:42:43 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vint.h"

// defines

#define VDATE_TYPE "date"

// externs

extern Methoded* date_class;

inline void set_tz(const char* tz, char* buf, size_t buf_size) {
#ifndef WIN32
	if(tz && *tz){
#endif
		snprintf(buf, buf_size, "TZ=%s", tz);
		putenv(buf);
#ifndef WIN32
	} else
#ifdef HAVE_UNSETENV
        unsetenv("TZ");
#else
		putenv("TZ");
#endif
#endif
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

	/// @TODO 'static' approach is NOT thread safe!
	tm& get_localtime()
	{
		char saved_tz[MAX_STRING];
		static char saved_tz_pair[MAX_STRING]; //TODO: this is NOT thread safe!
		static char temp_tz_pair[MAX_STRING];
		if(ftz_cstr) {
			if(const char* ltz=getenv("TZ")) {
				strncpy(saved_tz, ltz, sizeof(saved_tz)-1);
				saved_tz[sizeof(saved_tz)-1]=0;
			} else 
				saved_tz[0]=0;
			
			::set_tz(ftz_cstr, temp_tz_pair, sizeof(temp_tz_pair));
		}
		tm *result=::localtime(&ftime);
		if(ftz_cstr) {
			::set_tz(saved_tz, saved_tz_pair, sizeof(saved_tz_pair));
		}			
		if(!result)
			throw Exception(0,
				0,
				"invalid datetime (after changing TZ)");

		return *result;
	}


	/// VDate: method,field
	override Value* get_element(const String& aname, Value& aself, bool looking_up) {
		// $method
		if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
			return result;

		// $TZ
		if(aname=="TZ") 
			return ftz? new VString(*ftz): new VString();

		// $year month day  hour minute second  weekday
		tm& tms=get_localtime();

		int result;
		if(aname=="year") result=1900+tms.tm_year;
		else if(aname=="month") result=1+tms.tm_mon;
		else if(aname=="day") result=tms.tm_mday;
		else if(aname=="hour") result=tms.tm_hour;
		else if(aname=="minute") result=tms.tm_min;
		else if(aname=="second") result=tms.tm_sec;
		else if(aname=="weekday") result=tms.tm_wday;
		else if(aname=="yearday") result=tms.tm_yday;
		else if(aname=="daylightsaving") result=tms.tm_isdst;
		else if(aname=="week") {
			yw week = CalcWeek(tms);
			result=week.week;
		}
		else if(aname=="weekyear") {
			yw week = CalcWeek(tms);
			result=1900+week.year;
		} else { return bark("%s field not found", &aname); }
		return new VInt(result);
	}

public: // usage

	VDate(time_t adate) : 
		ftz(0),
		ftz_cstr(0) {
		set_time(adate);
	}

	VDate(tm tmIn) : 
		ftime(0),
		ftz(0),
		ftz_cstr(0) {
		set_time(tmIn);
	}

	time_t get_time() const { return ftime; }
	void set_time(time_t atime) { 
		if(atime<0)
			throw Exception(0,
				0,
				"invalid datetime");
		ftime=atime; 
	}
	void set_time(tm tmIn) { 
		time_t t=mktime(&tmIn);
		if(t<0) {
			// on some platforms mktime does not fix spring daylightsaving time hole
			// in russia -- last sunday of march, 2am->3am hole
			// trying to recover:
			tmIn.tm_hour--;
			t=mktime(&tmIn);
		}
		set_time(t);
	}
	void set_tz(const String* atz) { 
		if((ftz=atz))
			ftz_cstr=ftz->cstr();
	}

	struct yw {
		int year;
		int week;
	}; 
	
	static yw CalcWeek(tm& tms) {
		yw week = {tms.tm_year, 0};

		// http://www.merlyn.demon.co.uk/weekinfo.htm
		static const unsigned int FirstThurs[] = {7,5,4,3,2,7,6,5,4,2,1,7,6,4,3,2,1,6,5,4,3,1,7,6,5,3,2,1};
		int diff = tms.tm_yday-(FirstThurs[(tms.tm_year+1900) % 28]-4);
		if (diff < 0){
			tms.tm_mday = diff;
			mktime(&tms); // normalize
			week = CalcWeek(tms);
		} else {
			week.week = 1 + diff/7;
			if ( week.week > 52 && ISOWeekCount(week.year) < week.week ){
				week.year++;
				week.week = 1;
			}
	}
		return week;
	}

	static int ISOWeekCount (int year) {
		static const unsigned int YearWeeks[] = {
			52,52,52,52,53, 52,52,52,52,52,
			53,52,52,52,52, 52,53,52,52,52,
			52,53,52,52,52, 52,52,53
		};
		return YearWeeks[(year+1900) % 28];
	}

private:
	time_t ftime;
	const String* ftz;
	const char* ftz_cstr;

};

#endif
