/** @file
	Parser: @b date parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: date.C,v 1.1 2001/07/07 16:38:01 parser Exp $
*/
static const char *RCSId="$Id: date.C,v 1.1 2001/07/07 16:38:01 parser Exp $"; 

#include "classes.h"
#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vdate.h"

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
		time=(time_t)(params->as_double(0, r)*SECS_PER_DAY);
	else if(params->size()>=3) { // ^set(y;m;d[;h[;m[;s]]])
		tm tmIn={0};
		tmIn.tm_isdst=-1;
		int year=params->as_int(0, r);
		if(year<70) // 0..69 -> 100..169 [2000..2069]
			year+=100;
		tmIn.tm_year=year;
		tmIn.tm_mon=params->as_int(1, r)-1;
		tmIn.tm_mday=params->as_int(2, r);
		if(params->size()>3) tmIn.tm_hour=params->as_int(3, r);
		if(params->size()>4) tmIn.tm_min=params->as_int(4, r);
		if(params->size()>5) tmIn.tm_sec=params->as_int(5, r);
		time=mktime(&tmIn);
		if(time<0)
			PTHROW(0, 0,
				&method_name,
				"invalid datetime");
	} else
		PTHROW(0, 0,
			&method_name,
			"invalid params count, must be 1 or >=3");
	vdate->set_time(time);
}

static void _string(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.self);
	int size=4+1+2+1+2 +1+ 2+1+2+1+2 +1;
	char *buf=(char *)pool.malloc(size);
	time_t time=vdate->get_time();
	size=strftime(buf, size, "%Y-%m-%d %H:%M:%S", gmtime(&time));
	
	Value& result=*new(pool) VString(*new(pool) String(pool, buf, size));
	r.write_assign_lang(result);
}


static int isLeap(int year) {
    return !(
             (year % 4) || ((year % 400) && !(year % 100))
            );
}

static int getMonthDays(int year, int month) {
    int monthDays[]={
        31,
        isLeap(year) ? 29 : 28,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31
    };
    return monthDays[month];
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
		PTHROW(0, 0,
			&what,
			"must be year|month|day");
	
	*offset=params->as_int(1, r);
	if(!(*offset==1 || *offset==-1))
		PTHROW(0, 0,
			&method_name,
			"offset must be +/- 1");

	time_t tIn=vdate->get_time();
    tm *tmIn=localtime(&tIn);
	tmIn->tm_year+=oyear;
	time_t t=mktime(tmIn);
	if(t<0)
		PTHROW(0, 0,
			&method_name,
			"invalid datetime");
	t+=omonth*getMonthDays(tmIn->tm_year, (tmIn->tm_mon+(omonth<0?-1:0)+12)%12)*SECS_PER_DAY;
    t+=oday*SECS_PER_DAY;
	vdate->set_time(t);
}

// constructor

MDate::MDate(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DATE_CLASS_NAME));


	// ^now[]
	add_native_method("now", Method::CT_DYNAMIC, _now, 0, 0);

	// ^set(float days)
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 6);

	// ^string[]
	add_native_method("string", Method::CT_DYNAMIC, _string, 0, 0);

	// ^roll(year|month|day;+/- 1)
	add_native_method("roll", Method::CT_DYNAMIC, _roll, 2, 2);

}
// global variable

Methoded *date_class;

// creator

Methoded *MDate_create(Pool& pool) {
	return date_class=new(pool) MDate(pool);
}
