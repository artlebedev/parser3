/** @file
	Parser: @b date parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vdate.h"
#include "pa_vtable.h"

volatile const char * IDENT_DATE_C="$Id: date.C,v 1.113 2021/01/11 16:29:37 moko Exp $" IDENT_PA_VDATE_H;

// class

class MDate: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VDate(0); }

public:
	MDate();
};

// global variable

DECLARE_CLASS_VAR(date, new MDate);

// helpers

class Date_calendar_table_template_columns: public ArrayString {
public:
	Date_calendar_table_template_columns(): ArrayString(6+2) {
		for(int i=0; i<=6; i++)
			*this+=new String(i, "%d"); // .i column name

		*this+=new String("week");
		*this+=new String("year");
	}
};


Table date_calendar_table_template(new Date_calendar_table_template_columns);

// methods

static void _now(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	pa_time_t t=(pa_time_t)time(0);
	if(params.count()==1) // ^now(offset)
		t+=(pa_time_t)round(params.as_double(0, "offset must be double", r)*SECS_PER_DAY);
	
	vdate.set_time(t);
}

static void _today(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	time_t t=time(0);

	tm today=*localtime(&t);

	if (params.count() == 1) // ^today(offset)
		today.tm_mday += params.as_int(0, "offset must be int", r);

	today.tm_hour=0;
	today.tm_min=0;
	today.tm_sec=0;

	vdate.set_tm(today);
}

int to_year(int iyear) {
	if(iyear<0 || iyear>9999)
		throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "year '%d' is out of range 0..9999", iyear);
	return iyear-1900;
}

static int to_month(int imonth) {
	return max(1, min(imonth, 12)) -1;
}


static const char *skip_number_throw(char *string, char c, const char *valid){
	if(!valid[0])
		throw Exception("date.format", 0, "invalid character '%c' after number in '%s'", c, string);
	if(!strcmp(valid, "+-Z"))
		throw Exception("date.format", 0, "invalid timezone character '%c' after number in '%s'", c, string);
	throw Exception("date.format", 0, "number delimiter '%c'%s expected, but found '%c' in date '%s'",
		valid[0], valid[strlen(valid)-1] == 'Z' ? " or timezone":"", c, string);
}

static char *skip_number(char* string, const char *valid_delim, char *delim) {
	if(string) {
		char *str=string;
		// skipping whitespace
		while(isspace(str[0])) str++;
		// skipping +-
		if(str[0]=='-' || str[0]=='+') str++;
		// at least one digit should be present
		if(!str[0])
			throw Exception("date.format", 0, "number expected in date '%s'", string);
		if(!isdigit(str[0]))
			throw Exception("date.format", 0, "'%c' must be number in date '%s'", str[0], string);
		str++;
		// skipping digits
		while(isdigit(str[0])) str++;
		// skipping trailing whitespace
		if(!strchr(valid_delim, ' '))
			while(isspace(str[0])) str++;
		// delimiter check
		if(char c=str[0]){
			if(!strchr(valid_delim, c))
				skip_number_throw(string, c, valid_delim);
			if(delim)
				*delim=c;
			str[0]=0;
			return str+1;
		}
	}
	if(delim)
		*delim=0;
	return 0;
}

static char *skip_number(char** string_ref, const char *valid_delim, char *delim=0) {
	char *result=*string_ref;
	*string_ref=skip_number(*string_ref, valid_delim, delim);
	return result;
}

static char *skip_writespace(char* str) {
	if(str){
		while(isspace(str[0])) str++;
		return str[0] ? str : 0;
	}
	return 0;
}

static char *numeric_tz(char prefix, char* tz) {
	// preparing POSIX TZ format
	char *buf=new(PointerFreeGC) char[4+5+1/*zero-teminator*/];
	strcpy(buf, prefix=='+' ? "SUB-":"SUB+");
	char *cur=buf+4;

	// hours
	if(!isdigit(*(cur++)=*(tz++)))
		return 0;
	if(isdigit(tz[0]))
		*(cur++)=*(tz++);

	if(tz[0] == ':'){
		// HH:mm format
		*(cur++)=*(tz++);
		if(!isdigit(*(cur++)=*(tz++)))
			return 0;
		if(isdigit(tz[0]))
			*(cur++)=*(tz++);
	} else if(isdigit(tz[0])){
		// HHmm format
		*(cur++)=':';
		if(!isdigit(*(cur++)=*(tz++)) || !isdigit(*(cur++)=*(tz++)))
			return 0;
	}
	// nothing more
	if(skip_writespace(tz))
		return 0;
	*cur=0;
	return buf;
}

// SQL 2002-04-25 18:14:00
// ISO 2002-04-25T18:14:00.45+01:00
// TIME 18:14:00
// ':' DELIMITED 2002:04:25 [+maybe time]
// not static, used in image.C
tm cstr_to_time_t(char *cstr, const char **tzOut) {
	if( !cstr || !*cstr )
		throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "empty string is not valid datetime");
	if(tzOut)
		*tzOut=0;

	tm tmIn;
	memset(&tmIn, 0, sizeof(tmIn));
	tmIn.tm_isdst=-1;

	char delim;
	char *cur=cstr;

	const char *year, *month, *mday;
	const char *hour, *min, *sec, *msec PA_ATTR_UNUSED;

	year=skip_number(&cur, "-:", &delim);
	if(delim != ':' || delim == ':' && strlen(year) >=4 ){
		// year present
		month=skip_number(&cur, delim == ':' ? ":" : "-");
		mday=skip_number(&cur, tzOut ? " \tT":" \t", &delim);
		if(delim != 'T'){
			// SQL date format
			cur=skip_writespace(cur);
			hour=skip_number(&cur, ":");
			min=skip_number(&cur, ":");
			sec=skip_number(&cur, ".");
			msec=skip_number(&cur, "");
		} else {
			// ISO date format
			hour=skip_number(&cur, ":");
			min=skip_number(&cur, ":+-Z", &delim);
			sec=delim==':' ? skip_number(&cur, ".+-Z", &delim) : 0;
			msec=delim=='.' ? skip_number(&cur, "+-Z", &delim) : 0;
			// timezone specification check
			const char *tz = delim == 'Z' ? (skip_writespace(cur) ? 0 : "UTC") : (cur ? numeric_tz(delim, cur) : 0);
			if(!tz){
				if(!delim)
					throw Exception("date.format", 0, "empty timezone");
				throw Exception("date.format", 0, "invalid timezone '%c%s'", delim, cur ? cur : "");
			}
			*tzOut=tz;
		}

		tmIn.tm_year=to_year(pa_atoi(year));
		tmIn.tm_mon=month?pa_atoi(month)-1:0;
		tmIn.tm_mday=mday?pa_atoi(mday):1;
	} else {
		// time only
		hour=year;
		min=skip_number(&cur, ":");
		sec=skip_number(&cur, ".");
		msec=skip_number(&cur, "");

		time_t t=time(0);
		tm *tmNow=localtime(&t);
		tmIn.tm_year=tmNow->tm_year;
		tmIn.tm_mon=tmNow->tm_mon;
		tmIn.tm_mday=tmNow->tm_mday;
	}

	tmIn.tm_hour=pa_atoi(hour);
	tmIn.tm_min=pa_atoi(min);
	tmIn.tm_sec=pa_atoi(sec);
	//tmIn.tm_[msec<<no such, waits reimplementation of the class]=f(msec);

	return tmIn;
}

static void _create(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	if(params.count()==1){
		if(params[0].is_string()){ // ^create[2002-04-25 18:14:00] ^create[18:14:00]
			const char *tz;
			tm tmIn=cstr_to_time_t(params[0].get_string()->cstrm(), &tz);
			if(tz)
				vdate.set_tz(tz);
			vdate.set_tm(tmIn);
		} else { // ^create(float days) or ^create[date object]
			if(Value* adate=params[0].as(VDATE_TYPE))
				vdate.set_tz(static_cast<VDate*>(adate)->get_tz());
			vdate.set_time(round(params.as_double(0, "float days must be double", r)*SECS_PER_DAY));
		}
	} else { // ^create(y;m;d[;h[;m[;s[;TZ]]]])
		tm tmIn; memset(&tmIn, 0, sizeof(tmIn));
		tmIn.tm_isdst=-1;
		tmIn.tm_year=to_year(params.as_int(0, "year must be int", r));
		tmIn.tm_mon=params.as_int(1, "month must be int", r)-1;
		tmIn.tm_mday=params.count()>2?params.as_int(2, "mday must be int", r):1;
		if(params.count()>3) tmIn.tm_hour=params.as_int(3, "hour must be int", r);
		if(params.count()>4) tmIn.tm_min=params.as_int(4, "minutes must be int", r);
		if(params.count()>5) tmIn.tm_sec=params.as_int(5, "seconds must be int", r);
		if(params.count()>6) vdate.set_tz(params.as_string(6, "TZ must be string").cstr());
		vdate.set_tm(tmIn);
	};
}

static void _sql_string(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	VDate::sql_string_type format = VDate::sql_string_datetime;
	if(params.count() > 0) {
		const String& what=params.as_string(0, "'type' must be string");
		if(what.is_empty() || what == "datetime")
			format = VDate::sql_string_datetime;
		else if(what == "date")
			format=VDate::sql_string_date;
		else if(what == "time")
			format=VDate::sql_string_time;
		else
			throw Exception(PARSER_RUNTIME, &what, "'type' must be 'date', 'time' or 'datetime'");
	}

	r.write(*vdate.get_sql_string(format));
}

static void _gmt_string(Request& r, MethodParams&) {
	VDate& vdate=GET_SELF(r, VDate);

	r.write(*vdate.get_gmt_string());
}

static void _iso_string(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	VDate::iso_string_type format=VDate::iso_string_default;

	if(params.count()>0)
		if(HashStringValue* options=params.as_hash(0)){
			int valid_options=0;
			if(Value* vshow_ms=options->get("ms")){
				if(r.process(*vshow_ms).as_bool())
					format=VDate::iso_string_type(format|VDate::iso_string_ms);
				valid_options++;
			}
			if(Value* vshow_colon=options->get("colon")){
				if(!r.process(*vshow_colon).as_bool())
					format=VDate::iso_string_type(format|VDate::iso_string_no_colon);
				valid_options++;
			}
			if(Value* vshow_z=options->get("z")){
				if(!r.process(*vshow_z).as_bool())
					format=VDate::iso_string_type(format|VDate::iso_string_no_z);
				valid_options++;
			}
			if(valid_options != options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	r.write(*vdate.get_iso_string(format));
}

static void _roll(Request& r, MethodParams& params) {
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
		if(&r.get_self() == date_class){
			VDate::set_default_tz(argument_tz.cstr());
		} else {
			VDate& vdate=GET_SELF(r, VDate);
			vdate.set_tz(argument_tz.cstr());
			vdate.set_time(vdate.get_time());
		}
		return;
	} else
		throw Exception(PARSER_RUNTIME, &what, "must be year|month|day|TZ");

	if(&r.get_self() == date_class)
		throw Exception(PARSER_RUNTIME, &what, "must be TZ to be called statically");

	VDate& vdate=GET_SELF(r, VDate);
	
	*offset=params.as_int(1, "offset must be int", r);

	tm tmIn=vdate.get_tm();
	tm tmSaved=tmIn;
	int adjust_day=0;
	while(true) {
		tmIn.tm_year+=oyear;
		tmIn.tm_mon+=omonth;
		tmIn.tm_mday+=oday+adjust_day;
		tmIn.tm_hour=24/2;
		tmIn.tm_min=0;
		tmIn.tm_sec=0;
		int saved_day=tmIn.tm_mday;
		vdate.set_tm(tmIn); /* normalize */

		if(oday==0 && tmIn.tm_mday!=saved_day /* but it changed */ ) {
			if(adjust_day <= -3 /* 31->28 max, so never, but... */ )
				throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "bad resulting time (day hole still with %d day adjustment)", adjust_day );
			
			tmIn=tmSaved; // restoring
			--adjust_day; //retrying with prev day
		} else
			break;
	}

	tmIn.tm_hour=tmSaved.tm_hour;
	tmIn.tm_min=tmSaved.tm_min;
	tmIn.tm_sec=tmSaved.tm_sec;
	tmIn.tm_isdst=-1;

	vdate.set_tm(tmIn);
}

static Table& fill_month_days(Request& r, MethodParams& params, bool rus){
	Table::Action_options table_options;
	Table& result=*new Table(date_calendar_table_template, table_options);
	
	tm tmIn;
	memset(&tmIn, 0, sizeof(tmIn));
	tmIn.tm_year=to_year(params.as_int(1, "year must be int", r));
	tmIn.tm_mon=to_month(params.as_int(2, "month must be int", r));
	tmIn.tm_mday=1;

	VDate t(tmIn); /* normalize */
	int weekDay1=tmIn.tm_wday;

	if(rus)
		weekDay1=weekDay1?weekDay1-1:6; //sunday last
	int monthDays=VDate::getMonthDays(tmIn.tm_year, tmIn.tm_mon);
	
	for(int _day=1-weekDay1; _day<=monthDays;) {
		Table::element_type row(new ArrayString(7));
		// calculating year week no [1..54]
		int weekyear=0; // surely would be assigned to, but to calm down compiler
		int weekno=0; // same
		// 0..6 week days-cells fill with month days
		for(int wday=0; wday<7; wday++, _day++) {
			*row+=(_day>=1 && _day<=monthDays)?new String(_day, "%02d"):new String();

			if(wday==(rus?3:4)/*thursday*/) {
				tm tms;
				memset(&tms, 0, sizeof(tms));
				tms.tm_mday=_day;
				tms.tm_mon=tmIn.tm_mon;
				tms.tm_year=tmIn.tm_year;
				
				VDate ts(tms); /*normalize*/
				weekyear=tms.tm_year+1900;
				weekno=VDate::CalcWeek(tms).week;
			}
		}
		// appending week no
		*row+=new String(weekno, "%02d");

		// appending week year
		*row+=new String(weekyear, "%04d");
		
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

	tm tmIn;
	memset(&tmIn, 0, sizeof(tmIn));
	tmIn.tm_year=to_year(params.as_int(1, "year must be int", r));
	tmIn.tm_mon=to_month(params.as_int(2, "month must be int", r));
	tmIn.tm_mday=params.as_int(3, "day must be int", r);
	tmIn.tm_hour=18;

	VDate t(tmIn); /* normalize */
	int baseWeekDay=tmIn.tm_wday;

	if(rus) 
		baseWeekDay=baseWeekDay?baseWeekDay-1:6; //sunday last
	
	t.set_time(t.get_time()-baseWeekDay*SECS_PER_DAY);
		
	for(int curWeekDay=0; curWeekDay<7; curWeekDay++, t.set_time(t.get_time()+SECS_PER_DAY)) {
		Table::element_type row(new ArrayString(4));

		tm tmOut=t.get_tm();
		*row+=new String(1900+tmOut.tm_year, "%04d");
		*row+=new String(1+tmOut.tm_mon, "%02d");
		*row+=new String(tmOut.tm_mday, "%02d");
		*row+=new String(tmOut.tm_wday, "%02d");

		result+=row;
	}
	
	return result;
}

static void _calendar(Request& r, MethodParams& params) {
	const String& what=params.as_string(0, "format must be string");
	bool rus=false;
	if(what=="rus")
		rus=true;
	else if(what=="eng")
		rus=false;
	else
		throw Exception(PARSER_RUNTIME, &what, "must be rus|eng");

	Table* table;
	if(params.count()==1+2) 
		table=&fill_month_days(r, params, rus);
	else // 1+3
		table=&fill_week_days(r, params, rus);

	r.write(*new VTable(table));
}

static void _unix_timestamp(Request& r, MethodParams& params) {
	VDate& vdate=GET_SELF(r, VDate);

	if(params.count()==0) { 
		// ^date.unix-timestamp[]
		r.write(*new VDouble((double)vdate.get_time()));
	} else {
		if(vdate.get_time())
			throw Exception(PARSER_RUNTIME, 0, "date object already constructed");
		// ^unix-timestamp(time_t)
		vdate.set_time(params.as_double(0, "Unix timestamp must be number", r));
	}
}

static void _last_day(Request& r, MethodParams& params) {
	tm tmIn;
	if(&r.get_self() == date_class) {
		if(params.count() != 2)
			throw Exception(PARSER_RUNTIME, 0, "year and month must be defined");
		// ^date:lastday(year;month)
		tmIn.tm_year=to_year(params.as_int(0, "year must be int", r));
		tmIn.tm_mon=to_month(params.as_int(1, "month must be int", r));
	} else {
		if(params.count() != 0)
			throw Exception(PARSER_RUNTIME, 0, "year and month must not be defined");
		// ^date.lastday[]
		tmIn=GET_SELF(r, VDate).get_tm();
	}
	r.write(*new VInt(VDate::getMonthDays(tmIn.tm_year, tmIn.tm_mon)));
}

// constructor

MDate::MDate(): Methoded("date") {
	// ^date::now[]
	// ^date::now(offset float days)
	add_native_method("now", Method::CT_DYNAMIC, _now, 0, 1);

	// ^date::today[]
	// ^date::today(offset int days)
	add_native_method("today", Method::CT_DYNAMIC, _today, 0, 1);

	// ^date::create(float days)
	// ^date::create[date]
	// ^date::create(year;month;day[;hour[;minute[;sec[;TZ]]]])
	// ^date::create[yyyy-mm-dd[ hh:mm:ss]]
	// ^date::create[hh:mm:ss]
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 7);
	// old name for compatibility with <= v1.17 2002/2/18 12:13:42 paf
	add_native_method("set", Method::CT_DYNAMIC, _create, 1, 7);

	// ^date.sql-string[]
	add_native_method("sql-string", Method::CT_DYNAMIC, _sql_string, 0, 1);

	// ^date.gmt-string[]
	add_native_method("gmt-string", Method::CT_DYNAMIC, _gmt_string, 0, 0);

	// ^date.iso-string[$.colon(true) $.z(true) $.ms(false)]
	add_native_method("iso-string", Method::CT_DYNAMIC, _iso_string, 0, 1);

	// ^date:lastday(year;month)
	// ^date.lastday[]
	add_native_method("last-day", Method::CT_ANY, _last_day, 0, 2);

	// ^date.roll[year|month|day](+/- 1)
	add_native_method("roll", Method::CT_ANY, _roll, 2, 2);

	// ^date:calendar[rus|eng](year;month)  = table
	// ^date:calendar[rus|eng](year;month;day) = table
	add_native_method("calendar", Method::CT_STATIC, _calendar, 3, 4);

	// ^date.unix-timestamp[]
	// ^date::unix-timestamp(timestamp)
	add_native_method("unix-timestamp", Method::CT_DYNAMIC, _unix_timestamp, 0, 1);
}
