/** @file
	Parser: @b date parser class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_vstateless_object.h"
#include "pa_vdate.h"
#include "pa_vint.h"
#include "pa_vstring.h"

volatile const char * IDENT_PA_PA_VDATE_C="$Id: pa_vdate.C,v 1.2 2015/08/15 22:51:17 moko Exp $" IDENT_PA_VDATE_H;

static void set_tz(const char* tz, char* buf, size_t buf_size) {
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

const String* VDate::get_sql_string(sql_string_type format) {
	static const char* formats[]={
		"%Y-%m-%d %H:%M:%S",
		"%Y-%m-%d",
		"%H:%M:%S"
	};
	static int sizes[]={
		4+1+2+1+2 +1+ 2+1+2+1+2 +1/*terminator*/,
		4+1+2+1+2 +1/*terminator*/,
		2+1+2+1+2 +1/*terminator*/
	};
	size_t size=sizes[format];
	char *buf=new(PointerFreeGC) char[size];
	strftime(buf, size, formats[format], &get_localtime());

	return new String(buf);
}

const String* VDate::get_gmt_string(){
	return new String(date_gmt_string(gmtime(&ftime)));
}

/// @TODO 'static' approach is NOT thread safe!
tm&  VDate::get_localtime()
{
	char saved_tz[MAX_STRING];
	static char saved_tz_pair[MAX_STRING]; // @TODO: this is NOT thread safe!
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
		throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "invalid datetime (after changing TZ)");

	return *result;
}

override Value* VDate::get_element(const String& aname) {
	// $method
	if(Value* result=VStateless_object::get_element(aname))
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

const String* VDate::get_json_string(Json_options& options) {
	String* result=new String();
	switch(options.date){
		case Json_options::D_SQL:
			result->append_quoted(get_sql_string());
			break;
		case Json_options::D_GMT:
			result->append_quoted(get_gmt_string());
			break;
		case Json_options::D_TIMESTAMP:
			*result << format((int)ftime, 0);
			break;
	}
	return result;
}

void VDate::set_time(time_t atime) {
	if(atime==-1)
		throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "invalid datetime");
	ftime=atime;
}

void VDate::set_time(tm tmIn) {
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

static int ISOWeekCount (int year) {
	static const unsigned int YearWeeks[] = {
		52,52,52,52,53, 52,52,52,52,52,
		53,52,52,52,52, 52,53,52,52,52,
		52,53,52,52,52, 52,52,53
	};
	return YearWeeks[(year+1900) % 28];
}

VDate::yw VDate::CalcWeek(tm& tms) {
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
