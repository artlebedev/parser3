/** @file
	Parser: @b date parser class.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_DATE_C="$Date: 2003/04/09 11:18:33 $";

#include "classes.h"
#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vdate.h"
#include "pa_vtable.h"

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

static void _now(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.get_self());

	time_t t=time(0);
	if(params->size()==1) // ^now(offset)
		t+=(time_t)round(params->as_double(0, "offset must be double", r)*SECS_PER_DAY);
	
	vdate->set_time(t);
}

static int NN_year_to_NNNN(int year) {
	if(year<70) // 0..69 -> 100..169 [2000..2069]
		year+=100;
	if(year>=1900)
		year-=1900;
	return year;
}

// 2002-04-25 18:14:00
// 18:14:00
// 2002:04:25 [+maybe time]
time_t cstr_to_time_t(char *cstr, const String *report_error_origin) { // used in image.C
	char *cur=cstr;
	int date_delim=isdigit(cur[0])&&isdigit(cur[1])&&isdigit(cur[2])&&isdigit(cur[3])&&cur[4]==':'?':'
		:'-';
	const char *year=lsplit(&cur, date_delim);
	const char *month=lsplit(&cur, date_delim);
	const char *mday=lsplit(&cur, ' ');
	if(!month)
		cur=cstr;
	const char *hour=lsplit(&cur, ':');
	const char *min=lsplit(&cur, ':');
	const char *sec=cur;

	tm tmIn={0};
	tmIn.tm_isdst=-1;
	if(!month)
		if(min) {
			year=mday=0; // HH:MM
			time_t t=time(0);
			tm *tmNow=localtime(&t);
			tmIn.tm_year=tmNow->tm_year;
			tmIn.tm_mon=tmNow->tm_mon;
			tmIn.tm_mday=tmNow->tm_mday;
			goto date_part_set;
		} else
			hour=min=sec=0; // not YYYY- & not HH: = just YYYY					
	tmIn.tm_year=NN_year_to_NNNN(atoi(year));
	tmIn.tm_mon=month?atoi(month)-1:0;
	tmIn.tm_mday=mday?atoi(mday):1;
date_part_set:
	tmIn.tm_hour=hour?atoi(hour):0;
	tmIn.tm_min=min?atoi(min):0;
	tmIn.tm_sec=sec?atoi(sec):0;
	time_t result=mktime(&tmIn);
	if(result<0)
		if(report_error_origin)
			throw Exception(0,
				report_error_origin,
				"invalid datetime");

	return result;
}

static void _create(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.get_self());

	time_t t;
	if(params->size()==1) { 
		// ^create[2002-04-25 18:14:00]
		// ^create[18:14:00]
		if(const String *sdate=params->get(0).get_string())
			t=cstr_to_time_t(sdate->cstr(), sdate);
		else { // ^create(float days)
			t=(time_t)round(params->as_double(0, "float days must be double", r)*SECS_PER_DAY);
			if(t<0 || !localtime(&t))
				throw Exception(0,
					&method_name,
					"invalid datetime");
		}
	} else if(params->size()>=2) { // ^create(y;m;d[;h[;m[;s]]])
		tm tmIn={0};
		tmIn.tm_isdst=-1;
		tmIn.tm_year=NN_year_to_NNNN(params->as_int(0, "year must be int", r));
		tmIn.tm_mon=params->as_int(1, "month must be int", r)-1;
		tmIn.tm_mday=params->size()>2?params->as_int(2, "mday must be int", r):1;
		if(params->size()>3) tmIn.tm_hour=params->as_int(3, "hour must be int", r);
		if(params->size()>4) tmIn.tm_min=params->as_int(4, "minutes must be int", r);
		if(params->size()>5) tmIn.tm_sec=params->as_int(5, "seconds must be int", r);
		t=mktime(&tmIn);
		if(t<0)
			throw Exception(0,
				&method_name,
				"invalid datetime");
	} else
		throw Exception("parser.runtime",
			&method_name,
			"invalid params count, must be 1 or >=2");
	vdate->set_time(t);
}

static void _sql_string(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.get_self());
	int size=1+ 4+1+2+1+2 +1+ 2+1+2+1+2 +1 +1;
	char *buf=(char *)pool.malloc(size);
	time_t time=vdate->get_time();
	size=strftime(buf, size, "%Y-%m-%d %H:%M:%S", localtime(&time));
	
	String& string=*new(pool) String(pool);
	string.APPEND_CLEAN(buf, size, 
		method_name.origin().file, 
		method_name.origin().line);
	r.write_assign_lang(string);
}


static void _roll(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDate *vdate=static_cast<VDate *>(r.get_self());

	const String& what=params->as_string(0, "'what' must be string");
	int oyear=0;
	int omonth=0;
	int oday=0;
	int *offset;
	if(what=="year") offset=&oyear;
	else if(what=="month") offset=&omonth;
	else if(what=="day") offset=&oday;
	else
		throw Exception("parser.runtime",
			&what,
			"must be year|month|day");
	
	*offset=params->as_int(1, "offset must be int", r);

	time_t self_time=vdate->get_time();
	tm tmIn=*localtime(&self_time);
	tm tmSaved=tmIn;
	int adjust_day=0;
	time_t t_changed_date;
	while(true) {
		tmIn.tm_year+=oyear;
		tmIn.tm_mon+=omonth;
		tmIn.tm_mday+=oday+adjust_day;
		tmIn.tm_hour=24/2; 
		tmIn.tm_min=0;
		tmIn.tm_sec=0;
		int saved_mon=(tmIn.tm_mon+12*100)%12; // crossing year boundary backwards
		t_changed_date=mktime/*normalizetime*/(&tmIn);
		if(t_changed_date<0)
			throw Exception(0,
				&method_name,
				"bad resulting time (rolled out of valid date range)");
		if(oday==0 && tmIn.tm_mon!=saved_mon) {
			if(adjust_day <= -3/*31->28 max, so never, but...*/)
				throw Exception(0,
					&method_name,
					"bad resulting time (day hole still with %d day adjustment)", adjust_day );
			
			tmIn=tmSaved; // restoring
			--adjust_day; //retrying with prev day
		} else
			break;			
	}

	tm *tmOut=localtime(&t_changed_date);
	if(!tmOut)
		throw Exception(0,
			&method_name,
			"bad resulting time (seconds from epoch=%d)", t_changed_date);
    
	tmOut->tm_hour=tmSaved.tm_hour;
	tmOut->tm_min=tmSaved.tm_min;
	tmOut->tm_sec=tmSaved.tm_sec;
	tmOut->tm_isdst=-1; 
	{
		time_t t_changed_time=mktime/*normalizetime*/(tmOut);
		/*autofix: in msk timezone last sunday of april hour hole: [2am->3am)
		if(
			tmOut->tm_hour!=tmSaved.tm_hour
			||tmOut->tm_min!=tmSaved.tm_min)
			throw Exception(0,
				&method_name,
				"bad resulting time (hour hole)");
		*/

		if(t_changed_time<0)
			throw Exception(0,
				&method_name,
				"bad resulting time (after reconstruction)");
		
		vdate->set_time(t_changed_time);
	}
}

static Table *fill_month_days(Request& r, 
							  const String& method_name, MethodParams *params, bool rus){
	Pool& pool=r.pool();
	Table *result=new(pool) Table(pool, *date_calendar_table_template);

	int year=params->as_int(1, "year must be int", r);
	int month=max(1, min(params->as_int(2, "month must be int", r), 12)) -1;
	
	tm tmIn={0, 0, 0, 1, month, year-1900};
	time_t t=mktime(&tmIn);
	if(t<0)
		throw Exception(0, 
			&method_name, 
			"invalid date");
	tm *tmOut=localtime(&t);
	
	int weekDay1=tmOut->tm_wday;
	if(rus) 
		weekDay1=weekDay1?weekDay1-1:6; //sunday last
	int monthDays=getMonthDays(year, month);
	
	for(int _day=1-weekDay1; _day<=monthDays;) {
		Array& row=*new(pool) Array(pool, 7);
		// calculating year week no [1..54]
		char *weekno_buf;
		size_t weekno_size;
		int weekyear;
		// 0..6 week days-cells fill with month days
		for(int wday=0; wday<7; wday++, _day++) {
			String *cell=new(pool) String(pool);
			if(_day>=1 && _day<=monthDays) {
				char *buf=(char *)pool.malloc(2+1); 
				cell->APPEND_CLEAN(buf, sprintf(buf, "%02d", _day), 
					method_name.origin().file, method_name.origin().line);
            }
			row+=cell;            

			if(wday==(rus?3:4)/*thursday*/) {
				tm tms={0,0,0,  _day, month, year-1900};
				/*normalize*/mktime(&tms);

				weekyear=tms.tm_year+1900;

				const int weekno_buf_size=2+1/*for stupid snprintfs*/ +1;

				// http://www.merlyn.demon.co.uk/weekinfo.htm
				const int FirstThurs[] = {7,5,4,3,2,7,6,5,4,2,1,7,6,4,3,2,1,6,5,4,3,1,7,6,5,3,2,1};
				int n=1 + (tms.tm_yday-(FirstThurs[weekyear % 28]-3))/7;
				weekno_buf=(char *)pool.malloc(weekno_buf_size);
				weekno_size=snprintf(weekno_buf, weekno_buf_size, "%02d", n);
			}
		
        }
		// appending year week no
		{
        	String *cell=new(pool) String(pool);
			cell->APPEND_CLEAN(weekno_buf, weekno_size, 
				method_name.origin().file, method_name.origin().line);
			row+=cell;
		}
		// appending year week year
		{
        	String *cell=new(pool) String(pool);
			char *buf=(char *)pool.malloc(4+1); 
			cell->APPEND_CLEAN(buf, sprintf(buf, "%02d", weekyear), 
				method_name.origin().file, method_name.origin().line);
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
	columns+=new(pool) String(pool, "year");
	columns+=new(pool) String(pool, "month");
	columns+=new(pool) String(pool, "day");
	columns+=new(pool) String(pool, "weekday");
	Table *result=new(pool) Table(pool, &method_name, &columns);

    int year=params->as_int(1, "year must be int", r);
    int month=max(1, min(params->as_int(2, "month must be int", r), 12)) -1;
    int day=params->as_int(3, "day must be int", r);
    
    tm tmIn={0, 0, 18, day, month, year-1900};
    time_t t=mktime(&tmIn);
	if(t<0)
		throw Exception(0, 
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
		throw Exception("parser.runtime", 
			&what, 
			"must be rus|eng");

	Table *table;
	if(params->size()==1+2) 
		table=fill_month_days(r, method_name, params, rus);
	else // 1+3
		table=fill_week_days(r, method_name, params, rus);

	VTable& result=*new(pool) VTable(pool, table);
	r.write_no_lang(result);
}

// constructor

MDate::MDate(Pool& apool) : Methoded(apool, "date") {
	// ^now[]
	add_native_method("now", Method::CT_DYNAMIC, _now, 0, 1);

	// ^create(float days)
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 6);
	// old name for compatibility with <= v1.17 2002/2/18 12:13:42 paf
	add_native_method("set", Method::CT_DYNAMIC, _create, 1, 6);

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
