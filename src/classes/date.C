/** @file
	Parser: @b date parser class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_DATE_C="$Date: 2008/07/04 11:17:32 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vdate.h"
#include "pa_vtable.h"

// class

class MDate: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&, HashStringValue&) { return new VDate(0); }

public:
	MDate();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(date, new MDate, 0);

// helpers

class Date_calendar_table_template_columns: public ArrayString {
public:
	Date_calendar_table_template_columns(): ArrayString(6+2) {
		for(int i=0; i<=6; i++) {
			char *cname=new(PointerFreeGC) char[1/*strlen("6")*/+1/*terminating 0*/];
			*this+=new String(cname, sprintf(cname, "%d", i)); // .i column name
		}
		*this+=new String("week");
		*this+=new String("year");
	}
};


Table date_calendar_table_template(new Date_calendar_table_template_columns);

// methods

static void _now(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	time_t t=time(0);
	if(params.count()==1) // ^now(offset)
		t+=(time_t)round(params.as_double(0, "offset must be double", r)*SECS_PER_DAY);
	
	vdate.set_time(t);
}

/// shrinked range: 1970/1/1 to 2038/1/1
static int to_year(int iyear) {
	if(iyear<1970 || iyear>2038)
		throw Exception(0,
			0,
			"year '%d' is out of valid range", iyear);
	return iyear;
}

static int to_month(int imonth) {
	return max(1, min(imonth, 12)) -1;
}

static int to_tm_year(int iyear) {
	return to_year(iyear)-1900;
}


// 2002-04-25 18:14:00
// 18:14:00
// 2002:04:25 [+maybe time]
/*not static, used in image.C*/ tm cstr_to_time_t(char *cstr) {
	if( !cstr || !*cstr )
		throw Exception(0,
			0,
			"empty string is not valid datetime");

	char *cur=cstr;
	char date_delim=isdigit((unsigned char)cur[0])&&isdigit((unsigned char)cur[1])&&isdigit((unsigned char)cur[2])&&isdigit((unsigned char)cur[3])&&cur[4]==':'?':'
		:'-';
	const char* year=lsplit(&cur, date_delim);
	const char* month=lsplit(&cur, date_delim);
	const char* mday=lsplit(&cur, ' ');
	if(!month)
		cur=cstr;
	const char* hour=lsplit(&cur, ':');
	const char* min=lsplit(&cur, ':');
	const char* sec=lsplit(&cur, '.');
	const char* msec=cur;

	tm tmIn;  memset(&tmIn, 0, sizeof(tmIn));
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
			hour=min=sec=msec=0; // not YYYY- & not HH: = just YYYY					
	tmIn.tm_year=to_tm_year(pa_atoi(year));
	tmIn.tm_mon=month?pa_atoi(month)-1:0;
	tmIn.tm_mday=mday?pa_atoi(mday):1;
date_part_set:
	tmIn.tm_hour=hour?pa_atoi(hour):0;
	tmIn.tm_min=min?pa_atoi(min):0;
	tmIn.tm_sec=sec?pa_atoi(sec):0;	
	//tmIn.tm_[msec<<no such, waits reimplementation of the class]=f(msec);
	return tmIn;
}

static void _create(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	if(params.count()==1){
		if(const String* sdate=params[0].get_string()){ // ^create[2002-04-25 18:14:00] ^create[18:14:00]
			vdate.set_time(cstr_to_time_t(sdate->cstrm()));
		} else { // ^create(float days) or ^create[date object]
			time_t t=(time_t)round(params.as_double(0, "float days must be double", r)*SECS_PER_DAY);
			if(t<0 || !localtime(&t))
				throw Exception(0,
					0,
					"invalid datetime");
			vdate.set_time(t);
		}
	} else { // ^create(y;m;d[;h[;m[;s]]])
		assert(params.count()<=6);
		tm tmIn; memset(&tmIn, 0, sizeof(tmIn));
		tmIn.tm_isdst=-1;
		tmIn.tm_year=to_tm_year(params.as_int(0, "year must be int", r));
		tmIn.tm_mon=params.as_int(1, "month must be int", r)-1;
		tmIn.tm_mday=params.count()>2?params.as_int(2, "mday must be int", r):1;
		int savedHour=0;
		if(params.count()>3) savedHour=tmIn.tm_hour=params.as_int(3, "hour must be int", r);
		if(params.count()>4) tmIn.tm_min=params.as_int(4, "minutes must be int", r);
		if(params.count()>5) tmIn.tm_sec=params.as_int(5, "seconds must be int", r);
		vdate.set_time(tmIn);
	};
}

static void _sql_string(Request& r, MethodParams&) {
	VDate& vdate=GET_SELF(r, VDate);
	int size=1+ 4+1+2+1+2 +1+ 2+1+2+1+2 +1 +1;
	char *buf=new(PointerFreeGC) char[size];
	size=strftime(buf, size, "%Y-%m-%d %H:%M:%S", &vdate.get_localtime());
	
	r.write_assign_lang(String(buf, size));
}

static void _gmt_string(Request& r, MethodParams&) {
	VDate& vdate=GET_SELF(r, VDate);

	time_t when=vdate.get_time();
	
	r.write_assign_lang(String(date_gmt_string(gmtime(&when))));
}

static void _roll(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	const String& what=params.as_string(0, "'what' must be string");
	int oyear=0;
	int omonth=0;
	int oday=0;
	int *offset;
	if(what=="year") offset=&oyear;
	else if(what=="month") offset=&omonth;
	else if(what=="day") offset=&oday;
	else if(what=="TZ") {
		const String& argument_tz=params.as_string(1, "'TZ' must be string");
		vdate.set_tz(&argument_tz);
		return;
	} else
		throw Exception(PARSER_RUNTIME,
			&what,
			"must be year|month|day|TZ");
	
	*offset=params.as_int(1, "offset must be int", r);

	time_t self_time=vdate.get_time();
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
				0,
				"bad resulting time (rolled out of valid date range)");
		if(oday==0 && tmIn.tm_mon!=saved_mon/*but it changed*/) {
			if(adjust_day <= -3/*31->28 max, so never, but...*/)
				throw Exception(0,
					0,
					"bad resulting time (day hole still with %d day adjustment)", adjust_day );
			
			tmIn=tmSaved; // restoring
			--adjust_day; //retrying with prev day
		} else
			break;			
	}

	tm *tmOut=localtime(&t_changed_date);
	if(!tmOut)
		throw Exception(0,
			0,
			"bad resulting time (seconds from epoch=%d)", t_changed_date);
    
	tmOut->tm_hour=tmSaved.tm_hour;
	tmOut->tm_min=tmSaved.tm_min;
	tmOut->tm_sec=tmSaved.tm_sec;
	tmOut->tm_isdst=-1; 
	{
		time_t t_changed_time=mktime/*normalizetime*/(tmOut);
		/*autofix: in msk timezone last sunday of march hour hole: [2am->3am)
		if(
			tmOut->tm_hour!=tmSaved.tm_hour
			||tmOut->tm_min!=tmSaved.tm_min)
			throw Exception(0,
				0,
				"bad resulting time (hour hole)");
		*/

		if(t_changed_time<0)
			throw Exception(0,
				0,
				"bad resulting time (after reconstruction)");
		
		vdate.set_time(t_changed_time);
	}
}

static Table& fill_month_days(Request& r, MethodParams& params, bool rus){
	Table::Action_options table_options;
	Table& result=*new Table(date_calendar_table_template, table_options);
	
	int year=to_year(params.as_int(1, "year must be int", r));
	int month=to_month(params.as_int(2, "month must be int", r));
	
	tm tmIn;  
	memset(&tmIn, 0, sizeof(tmIn)); 
	tmIn.tm_mday=1;
	tmIn.tm_mon=month; 
	tmIn.tm_year=year-1900;

	time_t t=mktime(&tmIn);
	if(t<0)
		throw Exception(0, 
			0, 
			"invalid date");
	tm *tmOut=localtime(&t);
	
	int weekDay1=tmOut->tm_wday;
	if(rus) 
		weekDay1=weekDay1?weekDay1-1:6; //sunday last
	int monthDays=getMonthDays(year, month);
	
	for(int _day=1-weekDay1; _day<=monthDays;) {
		Table::element_type row(new ArrayString(7));
		// calculating year week no [1..54]
		char *weekno_buf=0; // surely would be assigned to, but to calm down compiler
		size_t weekno_size=0; // same
		int weekyear=0; // same
		// 0..6 week days-cells fill with month days
		for(int wday=0; wday<7; wday++, _day++) {
			String* cell=new String;
			if(_day>=1 && _day<=monthDays) {
				char *buf=new(PointerFreeGC) char[2+1]; 
				cell->append_know_length(buf, sprintf(buf, "%02d", _day), String::L_CLEAN);
			}
			*row+=cell;            

			if(wday==(rus?3:4)/*thursday*/) {
				tm tms;
				memset(&tms, 0, sizeof(tmIn)); 
				tms.tm_mday=_day;
				tms.tm_mon=month; 
				tms.tm_year=year-1900;
				
				/*normalize*/mktime(&tms);
				weekyear=tms.tm_year+1900;

				const int weekno_buf_size=2+1/*for stupid snprintfs*/ +1;

				weekno_buf=new(PointerFreeGC) char[weekno_buf_size];
				VDate::yw week = VDate::CalcWeek(tms);
				weekno_size=snprintf(weekno_buf, weekno_buf_size, "%02d", week.week);
			}
		
        }
		// appending year week no
		*row+=new String(weekno_buf, weekno_size);
		// appending year week year
		{
			char* buf=new(PointerFreeGC) char[4+1]; 
			*row+=new String(buf, sprintf(buf, "%02d", weekyear));
		}
		result+=row;
	}
	
	return result;
}

static Table& fill_week_days(Request& r, MethodParams& params, bool rus){
	Table::columns_type columns(new ArrayString(4));
	*columns+=new String("year");
	*columns+=new String("month");
	*columns+=new String("day");
	*columns+=new String("weekday");
	Table& result=*new Table(columns);

	int year=to_year(params.as_int(1, "year must be int", r));
	int month=to_month(params.as_int(2, "month must be int", r));
	int day=params.as_int(3, "day must be int", r);
	
	tm tmIn;
	memset(&tmIn, 0, sizeof(tmIn)); 
	tmIn.tm_hour=18;
	tmIn.tm_mday=day;
	tmIn.tm_mon=month; 
	tmIn.tm_year=year-1900;
		
	time_t t=mktime(&tmIn);
	if(t<0)
		throw Exception(0, 
			0, 
			"invalid date");
	tm *tmOut=localtime(&t);
    
	int baseWeekDay=tmOut->tm_wday;
	if(rus) 
		baseWeekDay=baseWeekDay?baseWeekDay-1:6; //sunday last
	
	t-=baseWeekDay*SECS_PER_DAY;
		
	for(int curWeekDay=0; curWeekDay<7; curWeekDay++, t+=SECS_PER_DAY) {
		tm *tmOut=localtime(&t);
		Table::element_type row(new ArrayString(4));
#define WDFILL(size, value) { \
			char *buf=new(PointerFreeGC) char[size+1]; \
			*row+=new String(buf, sprintf(buf, "%0"#size"d", value)); \
		}
		WDFILL(4, 1900+tmOut->tm_year);
		WDFILL(2, 1+tmOut->tm_mon);
		WDFILL(2, tmOut->tm_mday);
		WDFILL(2, tmOut->tm_wday);
		result+=row;
	}
	
	return result;
}

static void _calendar(Request& r, MethodParams& params) {
	const String& what=params.as_string(0, "format must be strig");
	bool rus=false;
	if(what=="rus")
		rus=true;
	else if(what=="eng")
		rus=false;
	else
		throw Exception(PARSER_RUNTIME, 
			&what, 
			"must be rus|eng");

	Table* table;
	if(params.count()==1+2) 
		table=&fill_month_days(r, params, rus);
	else // 1+3
		table=&fill_week_days(r, params, rus);

	r.write_no_lang(*new VTable(table));
}

static void _unix_timestamp(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	if(params.count()==0) { 
		// ^date.unix-timestamp[]
		r.write_no_lang(*new VInt((int)vdate.get_time()));
	} else {
		if(vdate.get_time())
			throw Exception(0,
				0,
				"date object already constructed");

		// ^unix-timestamp(time_t)
		time_t t=(time_t)params.as_int(0, "Unix timestamp must be integer", r);

		vdate.set_time(t);
	}
}

static void _last_day(Request& r, MethodParams& params) {
	int year;
	int month;
	if(&r.get_self() == date_class) {
		if(params.count() != 2)
			throw Exception(PARSER_RUNTIME,
				0,
				"year and month must be defined");

		// ^date:lastday(year;month)
		year=to_year(params.as_int(0, "year must be int", r));
		month=to_month(params.as_int(1, "month must be int", r));
	} else {
		// ^date.lastday[]
		tm &tmIn=GET_SELF(r, VDate).get_localtime();
		year=tmIn.tm_year+1900;
		month=tmIn.tm_mon;
	}
	r.write_no_lang(*new VInt(getMonthDays(year, month)));
}


// constructor

MDate::MDate(): Methoded("date") {
	// ^date::now[]
	add_native_method("now", Method::CT_DYNAMIC, _now, 0, 1);

	// ^date::create(float days)
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 6);
	// old name for compatibility with <= v1.17 2002/2/18 12:13:42 paf
	add_native_method("set", Method::CT_DYNAMIC, _create, 1, 6);

	// ^date.sql-string[]
	add_native_method("sql-string", Method::CT_DYNAMIC, _sql_string, 0, 0);

	// ^date.gmt-string[]
	add_native_method("gmt-string", Method::CT_DYNAMIC, _gmt_string, 0, 0);

	// ^date:lastday(year;month)
	// ^date.lastday[]
	add_native_method("last-day", Method::CT_ANY, _last_day, 0, 2);

	// ^date.roll(year|month|day;+/- 1)
	add_native_method("roll", Method::CT_DYNAMIC, _roll, 2, 2);

	// ^date:calendar[month|montheng;year;month]  = table
	// ^date:calendar[week|weekeng;year;month;day] = table
	add_native_method("calendar", Method::CT_STATIC, _calendar, 3, 4);


	// ^date.unix-timestamp[]
	// ^date::unix-timestamp[]
	add_native_method("unix-timestamp", Method::CT_DYNAMIC, _unix_timestamp, 0, 1);
}
