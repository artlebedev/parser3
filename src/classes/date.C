/** @file
	Parser: @b date parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: date.C,v 1.16 2002/02/08 08:30:09 paf Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vdate.h"
#include "pa_vtable.h"

// defines

#define DATE_CLASS_NAME "date"

// class

class MDate : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VDate(pool, 0); }

public:
	MDate(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _now(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.self);
	vdate->set_time(time(0));
}

static void _set(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.self);

	time_t time;
	if(params->size()==1) // ^set(float days)
		time=(time_t)(params->as_double(0, "float days must be double", r)*SECS_PER_DAY);
	else if(params->size()>=3) { // ^set(y;m;d[;h[;m[;s]]])
		tm tmIn={0};
		tmIn.tm_isdst=-1;
		int year=params->as_int(0, "year must be int", r);
		if(year<70) // 0..69 -> 100..169 [2000..2069]
			year+=100;
		if(year>=1900)
			year-=1900;
		tmIn.tm_year=year;
		tmIn.tm_mon=params->as_int(1, "month must be int", r)-1;
		tmIn.tm_mday=params->as_int(2, "mday must be int", r);
		if(params->size()>3) tmIn.tm_hour=params->as_int(3, "hour must be int", r);
		if(params->size()>4) tmIn.tm_min=params->as_int(4, "minutes must be int", r);
		if(params->size()>5) tmIn.tm_sec=params->as_int(5, "seconds must be int", r);
		time=mktime(&tmIn);
		if(time<0)
			throw Exception(0, 0,
				&method_name,
				"invalid datetime");
	} else
		throw Exception(0, 0,
			&method_name,
			"invalid params count, must be 1 or >=3");
	vdate->set_time(time);
}

static void _sql_string(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.self);
	int size=1+ 4+1+2+1+2 +1+ 2+1+2+1+2 +1 +1;
	char *buf=(char *)pool.malloc(size);
	time_t time=vdate->get_time();
	size=strftime(buf, size, "%Y-%m-%d %H:%M:%S", localtime(&time));
	
	String& string=*new(pool) String(pool);
	string.APPEND_CLEAN(buf, size, 
		method_name.origin().file, 
		method_name.origin().line);
	Value& result=*new(pool) VString(string);
	r.write_assign_lang(result);
}


static void _roll(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.self);

	const String& what=params->as_string(0, "'what' must be string");
    int oyear=0;
    int omonth=0;
    int oday=0;
	int *offset;
	if(what=="year") offset=&oyear;
	else if(what=="month") offset=&omonth;
	else if(what=="day") offset=&oday;
	else
		throw Exception(0, 0,
			&what,
			"must be year|month|day");
	
	*offset=params->as_int(1, "offset must be int", r);
	if(!(*offset==1 || *offset==-1))
		throw Exception(0, 0,
			&method_name,
			"offset must be +/- 1");

	time_t self_time=vdate->get_time();
	tm tmIn=*localtime(&self_time);
	// we will preserve daytime from day-light-saving shifts across roll
	tm tmSaved=tmIn;
	tmIn.tm_hour=24/2; tmIn.tm_min=tmIn.tm_sec=0;

    tmIn.tm_year+=oyear;
    time_t t=mktime(&tmIn);
    t+=omonth*getMonthDays(tmIn.tm_year, (tmIn.tm_mon+(omonth<0?-1:0)+12)%12)*SECS_PER_DAY;
    t+=oday*SECS_PER_DAY;
    
    tm *tmOut=localtime(&t);
	if(!tmOut)
		throw Exception(0, 0,
			&method_name,
			"bad resulting time (seconds from epoch=%ld)", t);
    
    tmOut->tm_hour=tmSaved.tm_hour;
    tmOut->tm_min=tmSaved.tm_min;
    tmOut->tm_sec=tmSaved.tm_sec;
	{
		time_t t=mktime(tmOut);
		if(t<0)
			throw Exception(0, 0,
			&method_name,
			"bad resulting time (after reconstruction)");
		
		vdate->set_time(t);
	}
}

static Table *fill_month_days(Request& r, 
							  const String& method_name, MethodParams *params, bool rus){
	Pool& pool=r.pool();
	Table *result=new(pool) Table(pool, &method_name, 0/*&columns*/);

    int year=params->as_int(1, "year must be int", r);
    int month=max(1, min(params->as_int(2, "month must be int", r), 12)) -1;

    tm tmIn={0, 0, 0, 1, month, year-1900};
    time_t t=mktime(&tmIn);
	if(t<0)
		throw Exception(0, 0, 
			&method_name, 
			"invalid date");
    tm *tmOut=localtime(&t);

    int weekDay1=tmOut->tm_wday;
	if(rus) 
		weekDay1=weekDay1?weekDay1-1:6; //sunday last
    int monthDays=getMonthDays(year, month);
    
    for(int _day=1-weekDay1; _day<=monthDays;) {
		Array& row=*new(pool) Array(pool, 7);
    	for(int wday=0; wday<7; wday++, _day++) {
        	String *cell=new(pool) String(pool);
			if(_day>=1 && _day<=monthDays) {
				char *buf=(char *)pool.malloc(2+1); 
				cell->APPEND_CLEAN(buf, sprintf(buf, "%02d", _day), 
					method_name.origin().file, method_name.origin().line);
            }
			row+=cell;            
        }
    	*result+=&row;
    }
    
    return result;
}

static Table *fill_week_days(Request& r, 
							 const String& method_name, MethodParams *params, bool rus){
	Pool& pool=r.pool();
	Array& columns=*new(pool) Array(pool, 4);
	Table *result=new(pool) Table(pool, &method_name, &columns);

    int year=params->as_int(1, "year must be int", r);
    int month=max(1, min(params->as_int(2, "month must be int", r), 12)) -1;
    int day=params->as_int(3, "day must be int", r);
    
    tm tmIn={0, 0, 18, day, month, year-1900};
    time_t t=mktime(&tmIn);
	if(t<0)
		throw Exception(0, 0, 
			&method_name, 
			"invalid date");
    tm *tmOut=localtime(&t);
    
    int baseWeekDay=tmOut->tm_wday;
	if(rus) 
		baseWeekDay=baseWeekDay?baseWeekDay-1:6; //sunday last

    t-=baseWeekDay*SECS_PER_DAY;

    for(int curWeekDay=0; curWeekDay<7; curWeekDay++, t+=SECS_PER_DAY) {
        tm *tmOut=localtime(&t);
		Array& row=*new(pool) Array(pool, 4);
#define WDFILL(size, value) { \
		char *buf=(char *)pool.malloc(size+1); \
		String *cell=new(pool) String(pool); \
		cell->APPEND_CLEAN(buf, sprintf(buf, "%0"#size"d", value), \
			method_name.origin().file, \
			method_name.origin().line); \
		row+=cell; \
		}
		WDFILL(4, 1900+tmOut->tm_year);
		WDFILL(2, 1+tmOut->tm_mon);
		WDFILL(2, tmOut->tm_mday);
		WDFILL(2, tmOut->tm_wday);
        *result+=&row;
    }
    
    return result;
}

static void _calendar(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	const String& what=params->as_string(0, "format must be strig");
	bool rus=false;
	if(what=="rus")
		rus=true;
	else if(what=="eng")
		rus=false;
	else
		throw Exception(0, 0, 
			&what, 
			"must be rus|eng");

	Table *table;
	if(params->size()==1+2) 
		table=fill_month_days(r, method_name, params, rus);
	else // 1+3
		table=fill_week_days(r, method_name, params, rus);

	VTable& result=*new(pool) VTable(pool, table);
	result.set_name(method_name);
	r.write_no_lang(result);
}

// constructor

MDate::MDate(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DATE_CLASS_NAME));


	// ^now[]
	add_native_method("now", Method::CT_DYNAMIC, _now, 0, 0);

	// ^set(float days)
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 6);

	// ^sql-string[]
	add_native_method("sql-string", Method::CT_DYNAMIC, _sql_string, 0, 0);

	// ^roll(year|month|day;+/- 1)
	add_native_method("roll", Method::CT_DYNAMIC, _roll, 2, 2);

	// ^date:calendar[month|montheng;year;month]  = table
	// ^date:calendar[week|weekeng;year;month;day] = table
	add_native_method("calendar", Method::CT_STATIC, _calendar, 3, 4);

}
// global variable

Methoded *date_class;

// creator

Methoded *MDate_create(Pool& pool) {
	return date_class=new(pool) MDate(pool);
}
