/** @file
	Parser: @b date parser class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDATE_H
#define PA_VDATE_H

#define IDENT_PA_VDATE_H "$Id: pa_vdate.h,v 1.66 2020/12/15 17:10:39 moko Exp $"

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vdouble.h"

// defines

#define VDATE_TYPE "date"

//#define PA_DATE64

#ifdef PA_DATE64
#define pa_time_t time_t
#else
#define pa_time_t double
#endif

#define SECS_PER_DAY 86400L

// externs

extern Methoded* date_class;

/// value of type 'date'.
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

	/// VDate: method,field
	override Value* get_element(const String& aname);

	/// VDate: field
	override const VJunction* put_element(const String& aname, Value* avalue);

public: // usage

	VDate(pa_time_t adate) : ftz_cstr(0) {
		set_time(adate);
	}

	VDate(tm &tmIn) : ftz_cstr(0) {
		set_tm(tmIn);
	}

	void set_time(pa_time_t atime);
	void set_tm(tm &tmIn);
	void validate();

	void set_tz(const char* atz);
	static void set_default_tz(const char* atz);

	pa_time_t get_time() const { return ftime; }
	tm get_tm() const { return ftm; }
	const char *get_tz() const { return ftz_cstr; }

	enum sql_string_type {sql_string_datetime, sql_string_date, sql_string_time};
	enum iso_string_type {iso_string_default = 0, iso_string_ms = 1, iso_string_no_colon = 2, iso_string_no_z = 4};

	const String* get_sql_string(sql_string_type format = sql_string_datetime);
	const String* get_iso_string(iso_string_type format = iso_string_default);
	const String* get_gmt_string();

	struct yw {
		int year;
		int week;
	}; 
	
	static yw CalcWeek(tm tms);
	static int getMonthDays(int year, int month);

private:
	pa_time_t ftime;
	tm ftm;
	const char* ftz_cstr;

};

#endif
