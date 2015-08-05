/** @file
	Parser: @b date parser class decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDATE_H
#define PA_VDATE_H

#define IDENT_PA_VDATE_H "$Id: pa_vdate.h,v 1.56 2015/08/05 22:07:17 moko Exp $"

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vdouble.h"

// defines

#define VDATE_TYPE "date"

// externs

extern Methoded* date_class;

/// value of type 'date'. implemented with @c time_t
class VDate: public VStateless_object {
public: // Value

	override const char* type() const { return VDATE_TYPE; }
	override VStateless_class *get_class() { return date_class; }
	
	/// VDate: json-string
	override const String* get_json_string(Json_options& options);

	/// VDate: ftime -> float days
	override Value& as_expr_result() { return *new VDouble(as_double()); }

	/// VDate: true
	override bool is_evaluated_expr() const { return true; }

	/// VDate: ftime -> float days
	override double as_double() const { return ((double)ftime)/ SECS_PER_DAY; }

	/// VDate: 0 or !0
	override bool as_bool() const { return ftime!=0; }

	tm& get_localtime();

	enum sql_string_type {sql_string_datetime, sql_string_date, sql_string_time};

	const String* get_sql_string(sql_string_type format = sql_string_datetime);
	const String* get_gmt_string();

	/// VDate: method,field
	override Value* get_element(const String& aname);

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
		if(atime==-1)
			throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "invalid datetime");
		ftime=atime;
	}

	void set_time(tm tmIn) {
		time_t t=mktime(&tmIn);
		if(t==-1) {
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
