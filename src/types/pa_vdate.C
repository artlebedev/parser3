/** @file
	Parser: @b date parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_vstateless_object.h"
#include "pa_vdate.h"
#include "pa_vint.h"
#include "pa_vstring.h"

volatile const char * IDENT_PA_PA_VDATE_C="$Id: pa_vdate.C,v 1.24 2020/12/15 17:10:39 moko Exp $" IDENT_PA_VDATE_H;

#define ZERO_DATE (-62169984000ll-SECS_PER_DAY) // '0000-00-00 00:00:00' - 1 day
#define MAX_DATE (253402300799ll+SECS_PER_DAY) // '9999-12-31 23:59:59' + 1 day

static const int DAYS_IN_MONTH[12] =
{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const int DAYS_BEFORE_MONTH[12] =
{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define IS_LEAP(y) (((y) % 4) == 0 && (((y) % 100) != 0 || (((y)+1900) % 400) == 0))

int VDate::getMonthDays(int year, int month) {
	return (month == 1 /* january -- 0 */ && IS_LEAP(year)) ? 29 : DAYS_IN_MONTH[month];
}

void pa_gmtime(pa_time_t lcltime, struct tm *res);
pa_time_t pa_mktime(struct tm *tim_p);

static int gmt_offset() {
#if defined(HAVE_TIMEZONE)
	tzset();
#if _MSC_VER >= 1900
	long timezone = 0;
	_get_timezone(&timezone);
#endif
	return -timezone;
#else
	time_t t=time(0);
	tm *tm=localtime(&t);
#if defined(HAVE_TM_GMTOFF)
	return tm->tm_gmtoff;
#elif defined(HAVE_TM_TZADJ)
	return -tm->tm_tzadj;
#else
#error neither HAVE_TIMEZONE nor HAVE_TM_GMTOFF nor HAVE_TM_TZADJ defined
#endif
#endif
}

static void pa_set_tz(const char* ntz) {
	if(ntz && *ntz){
		static char temp_tz_pair[MAX_STRING];
		snprintf(temp_tz_pair, sizeof(temp_tz_pair), "TZ=%s", ntz);
		putenv(temp_tz_pair);
	} else {
#ifdef HAVE_UNSETENV
		unsetenv("TZ");
#else
		putenv("TZ=");
#endif
	}
	tzset(); // required in Windows
}

///	Auto-object used for temporarily substituting/removing timezone variable
class Temp_tz {
	const char* ntz;
	char saved_tz[MAX_STRING];
public:
	static const char *default_tz;
public:
	Temp_tz(const char *atz) : ntz(atz) {
		if(!ntz)
			ntz=default_tz;
		if(!ntz)
			return;
		if(const char* ctz=getenv("TZ")){
			strncpy(saved_tz, ctz, sizeof(saved_tz)-1);
		} else
			saved_tz[0]=0;
		pa_set_tz(ntz);
	}
	~Temp_tz() {
		if(ntz)
			pa_set_tz(saved_tz);
	}
};

const char *Temp_tz::default_tz=0;

static void pa_localtime(const char *tz, pa_time_t atime, struct tm &tmIn) {
	Temp_tz temp_tz(tz);
#ifdef PA_DATE64
	tmIn=*localtime(&atime);
#else
	if(atime >= 0 && atime <= INT_MAX){
		time_t itime=(time_t)atime;
		tmIn=*localtime(&itime);
	} else {
		pa_gmtime(atime+gmt_offset(), &tmIn);
	}
#endif
}

static pa_time_t pa_mktime(const char *tz, struct tm &tmIn) {
	Temp_tz temp_tz(tz);
#ifdef PA_DATE64
	return mktime(&tmIn);
#else
	time_t result=mktime(&tmIn);
	if(result != -1)
		return (pa_time_t)result;
	return pa_mktime(&tmIn)-gmt_offset();
#endif
}

const String* VDate::get_sql_string(sql_string_type aformat) {
	switch(aformat){
		case sql_string_datetime:{
			static const char *format="%.4d-%.2d-%.2d %.2d:%.2d:%.2d";
			static int size=4+1+2+1+2 +1+ 2+1+2+1+2 +1/*zero-teminator*/+1/*for faulty snprintfs*/;
			char *buf=new(PointerFreeGC) char[size];
			snprintf(buf, size, format, ftm.tm_year+1900, ftm.tm_mon+1, ftm.tm_mday, ftm.tm_hour, ftm.tm_min, ftm.tm_sec);
			return new String(buf);
		}
		case sql_string_date:{
			static const char *format="%.4d-%.2d-%.2d";
			static int size=4+1+2+1+2 +1/*zero-teminator*/+1/*for faulty snprintfs*/;
			char *buf=new(PointerFreeGC) char[size];
			snprintf(buf, size, format, ftm.tm_year+1900, ftm.tm_mon+1, ftm.tm_mday);
			return new String(buf);
		}
		case sql_string_time:{
			static const char *format="%.2d:%.2d:%.2d";
			static int size=2+1+2+1+2 +1/*zero-teminator*/+1/*for faulty snprintfs*/;
			char *buf=new(PointerFreeGC) char[size];
			snprintf(buf, size, format, ftm.tm_hour, ftm.tm_min, ftm.tm_sec);
			return new String(buf);
		}
	}
	return &String::Empty;
}


const String* VDate::get_gmt_string() {
	struct tm gtm;
#ifdef PA_DATE64
	gtm=*gmtime(&ftime);
#else
	pa_gmtime(ftime, &gtm);
#endif
	/// http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.3
	static const char month_names[12][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	static const char days[7][4]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	static const char *format="%s, %.2d %s %.4d %.2d:%.2d:%.2d GMT";
	static int size=3+1+1+2+1+3+1+4+1+2+1+2+1+2+4 +1/*zero-teminator*/+1/*for faulty snprintfs*/;
	char *buf=new(PointerFreeGC) char[size];
	snprintf(buf, size, format, days[gtm.tm_wday], gtm.tm_mday, month_names[gtm.tm_mon], gtm.tm_year+1900, gtm.tm_hour, gtm.tm_min, gtm.tm_sec);
	return new String(buf);
}

const String* VDate::get_iso_string(iso_string_type format) {
	Temp_tz temp_tz(ftz_cstr);
	int offset=gmt_offset();
	/// http://www.w3.org/TR/NOTE-datetime
	if(offset || (format & iso_string_no_z)){
		char sign=offset<0 ? '-':'+';
		offset=abs(offset);
		static const char *sformats[]={
			"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d%c%.2d:%.2d",
			"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.000%c%.2d:%.2d",
			"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d%c%.2d%.2d",
			"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.000%c%.2d%.2d",
		};
		static int size=4+1+2+1+2 +1 +2+1+2+1+2 +4 +1 +2+1+2 +1/*zero-teminator*/+1/*for faulty snprintfs*/;
		const char *sformat=sformats[format & (iso_string_ms | iso_string_no_colon)];
		char *buf=new(PointerFreeGC) char[size];
		snprintf(buf, size, sformat, ftm.tm_year+1900, ftm.tm_mon+1, ftm.tm_mday, ftm.tm_hour, ftm.tm_min, ftm.tm_sec,
			sign, offset/3600, (offset/60)%60);
		return new String(buf);
	} else {
		static const char *sformats[]={
			"%.4d-%.2d-%.2dT%.2d:%.2d:%.2dZ",
			"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.000Z"
		};
		static int size=4+1+2+1+2 +1 +2+1+2+1+2 +4 +1 +1/*zero-teminator*/+1/*for faulty snprintfs*/;
		const char *sformat=sformats[format & (iso_string_ms)];
		char *buf=new(PointerFreeGC) char[size];
		snprintf(buf, size, sformat, ftm.tm_year+1900, ftm.tm_mon+1, ftm.tm_mday, ftm.tm_hour, ftm.tm_min, ftm.tm_sec);
		return new String(buf);
	}
}

Value* VDate::get_element(const String& aname) {
	// $method
	if(Value* result=VStateless_object::get_element(aname))
		return result;

	// $TZ
	if(aname=="TZ")
		return ftz_cstr ? new VString(*new String(ftz_cstr)): new VString();

	int result;
	if(aname=="year") result=1900+ftm.tm_year;
	else if(aname=="month") result=1+ftm.tm_mon;
	else if(aname=="day") result=ftm.tm_mday;
	else if(aname=="hour") result=ftm.tm_hour;
	else if(aname=="minute") result=ftm.tm_min;
	else if(aname=="second") result=ftm.tm_sec;
	else if(aname=="weekday") result=ftm.tm_wday;
	else if(aname=="yearday") result=ftm.tm_yday;
	else if(aname=="daylightsaving") result=ftm.tm_isdst;
	else if(aname=="week") {
		yw week = CalcWeek(ftm);
		result=week.week;
	}
	else if(aname=="weekyear") {
		yw week = CalcWeek(ftm);
		result=1900+week.year;
	} else { return bark("%s field not found", &aname); }
	return new VInt(result);
}

extern int to_year(int iyear);

const VJunction* VDate::put_element(const String& aname, Value* avalue) {
	tm tmIn=get_tm();

	if(aname=="year") tmIn.tm_year=to_year(avalue->as_int());
	else if(aname=="month") tmIn.tm_mon=avalue->as_int()-1;
	else if(aname=="day") tmIn.tm_mday=avalue->as_int();
	else if(aname=="hour") tmIn.tm_hour=avalue->as_int();
	else if(aname=="minute") tmIn.tm_min=avalue->as_int();
	else if(aname=="second") tmIn.tm_sec=avalue->as_int();
	else bark("%s field not found", &aname);

	set_tm(tmIn);

	return 0;
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
		case Json_options::D_ISO:
			result->append_quoted(get_iso_string());
			break;
		case Json_options::D_TIMESTAMP:
			*result << format((int)ftime, 0);
			break;
	}
	return result;
}

void VDate::validate() {
	if((ftm.tm_year==-1901) && (ftm.tm_mon==10) && (ftm.tm_mday==30)){
		ftm.tm_year=-1900;
		ftm.tm_mon=-1;
		ftm.tm_mday=0;
	}
	if((ftm.tm_year+1900)<0 || (ftm.tm_year+1900)>9999){
		throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "year '%d' is out of range 0..9999", ftm.tm_year+1900);
	}
}

void VDate::set_time(pa_time_t atime) {
	if(atime==-1)
		throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "invalid datetime");
	if(atime<ZERO_DATE || atime>MAX_DATE)
		throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "unix time %.15g is out of range 0..9999 year", (double)atime);
	ftime=atime;
	pa_localtime(ftz_cstr, ftime, ftm);
	validate();
}

void VDate::set_tm(tm &tmIn) {
	pa_time_t atime=pa_mktime(ftz_cstr, tmIn);
	if(atime==-1)
		throw Exception(DATE_RANGE_EXCEPTION_TYPE, 0, "invalid datetime '%04d-%02d-%02d'", tmIn.tm_year+1900, tmIn.tm_mon+1, tmIn.tm_mday);
	ftime=atime;
	ftm=tmIn;
	validate();
}

void VDate::set_tz(const char* atz) {
	ftz_cstr=atz && atz[0] ? atz : 0; // ftm should be updated afterwards
}

void VDate::set_default_tz(const char* atz) {
	Temp_tz::default_tz=atz && atz[0] ? atz : 0;
}

static int ISOWeekCount (int year) {
	static const unsigned int YearWeeks[] = {
		52,52,52,52,53, 52,52,52,52,52,
		53,52,52,52,52, 52,53,52,52,52,
		52,53,52,52,52, 52,52,53
	};
	return YearWeeks[(year+1900) % 28];
}

VDate::yw VDate::CalcWeek(tm tms) {
	yw week = {tms.tm_year, 0};

	// http://www.merlyn.demon.co.uk/weekinfo.htm
	static const unsigned int FirstThurs[] = {7,5,4,3,2,7,6,5,4,2,1,7,6,4,3,2,1,6,5,4,3,1,7,6,5,3,2,1};
	int diff = tms.tm_yday-(FirstThurs[(tms.tm_year+1900) % 28]-4);
	if (diff < 0){
		tms.tm_mday = diff;
		pa_mktime(0, tms); // normalize
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

#ifndef PA_DATE64

/*
 * gmtime_r.c
 * Original Author: Adapted from tzcode maintained by Arthur David Olson.
 * Modifications:
 * - Changed to mktm_r and added __tzcalc_limits - 04/10/02, Jeff Johnston
 * - Fixed bug in mday computations - 08/12/04, Alex Mogilnikov <alx@intellectronika.ru>
 * - Fixed bug in __tzcalc_limits - 08/12/04, Alex Mogilnikov <alx@intellectronika.ru>
 * - Move code from _mktm_r() to gmtime_r() - 05/09/14, Freddie Chopin <freddie_chopin@op.pl>
 * - Fixed bug in calculations for dates after year 2069 or before year 1901. Ideas for
 *   solution taken from musl's __secs_to_tm() - 07/12/2014, Freddie Chopin
 *   <freddie_chopin@op.pl>
 * - Use faster algorithm from civil_from_days() by Howard Hinnant - 12/06/2014,
 * Freddie Chopin <freddie_chopin@op.pl>
 *
 * Converts the calendar time pointed to by tim_p into a broken-down time
 * expressed as local time. Returns a pointer to a structure containing the
 * broken-down time.
 */

/* Move epoch from 01.01.1970 to 01.03.0000 (yes, Year 0) - this is the first
 * day of a 400-year long "era", right after additional day of leap year.
 * This adjustment is required only for date calculation, so instead of
 * modifying time_t value (which would require 64-bit operations to work
 * correctly) it's enough to adjust the calculated number of days since epoch.
 */

#define SECS_PER_HOUR 3600
#define SECS_PER_MIN 60
#define DAYS_PER_WEEK 7
#define YEAR_BASE 1900

#define EPOCH_ADJUSTMENT_DAYS	719468L
/* year to which the adjustment was made */
#define ADJUSTED_EPOCH_YEAR	0
/* 1st March of year 0 is Wednesday */
#define ADJUSTED_EPOCH_WDAY	3
/* there are 97 leap years in 400-year periods. ((400 - 97) * 365 + 97 * 366) */
#define DAYS_PER_ERA		146097L
/* there are 24 leap years in 100-year periods. ((100 - 24) * 365 + 24 * 366) */
#define DAYS_PER_CENTURY	36524L
/* there is one leap year every 4 years */
#define DAYS_PER_4_YEARS	(3 * 365 + 366)
/* number of days in a non-leap year */
#define DAYS_PER_YEAR		365
/* number of days in January */
#define DAYS_IN_JANUARY		31
/* number of days in non-leap February */
#define DAYS_IN_FEBRUARY	28
/* number of years per era */
#define YEARS_PER_ERA		400

void pa_gmtime(pa_time_t lcltime, struct tm *res) {
  long days, rem;
  int era, weekday, year;
  unsigned erayear, yearday, month, day;
  unsigned long eraday;

  days = (long)(lcltime / SECS_PER_DAY);
  rem = (long)(lcltime - (pa_time_t)days * SECS_PER_DAY);
  days += EPOCH_ADJUSTMENT_DAYS;
  if (rem < 0)
    {
      rem += SECS_PER_DAY;
      --days;
    }

  /* compute hour, min, and sec */
  res->tm_hour = (int) (rem / SECS_PER_HOUR);
  rem %= SECS_PER_HOUR;
  res->tm_min = (int) (rem / SECS_PER_MIN);
  res->tm_sec = (int) (rem % SECS_PER_MIN);

  /* compute day of week */
  if ((weekday = ((ADJUSTED_EPOCH_WDAY + days) % DAYS_PER_WEEK)) < 0)
    weekday += DAYS_PER_WEEK;
  res->tm_wday = weekday;

  /* compute year, month, day & day of year */
  /* for description of this algorithm see
   * http://howardhinnant.github.io/date_algorithms.html#civil_from_days */
  era = (days >= 0 ? days : days - (DAYS_PER_ERA - 1)) / DAYS_PER_ERA;
  eraday = days - era * DAYS_PER_ERA;	/* [0, 146096] */
  erayear = (eraday - eraday / (DAYS_PER_4_YEARS - 1) + eraday / DAYS_PER_CENTURY -
      eraday / (DAYS_PER_ERA - 1)) / 365;	/* [0, 399] */
  yearday = eraday - (DAYS_PER_YEAR * erayear + erayear / 4 - erayear / 100);	/* [0, 365] */
  month = (5 * yearday + 2) / 153;	/* [0, 11] */
  day = yearday - (153 * month + 2) / 5 + 1;	/* [1, 31] */
  month += month < 10 ? 2 : -10;
  year = ADJUSTED_EPOCH_YEAR + erayear + era * YEARS_PER_ERA + (month <= 1);

  res->tm_yday = yearday >= DAYS_PER_YEAR - DAYS_IN_JANUARY - DAYS_IN_FEBRUARY ?
      yearday - (DAYS_PER_YEAR - DAYS_IN_JANUARY - DAYS_IN_FEBRUARY) :
      yearday + DAYS_IN_JANUARY + DAYS_IN_FEBRUARY + IS_LEAP(erayear);
  res->tm_year = year - YEAR_BASE;
  res->tm_mon = month;
  res->tm_mday = day;

  res->tm_isdst = 0;
}


/*
 * mktime.c
 * Original Author:	G. Haley
 *
 * Converts the broken-down time, expressed as local time, in the structure
 * pointed to by tim_p into a calendar time value. The original values of the
 * tm_wday and tm_yday fields of the structure are ignored, and the original
 * values of the other fields have no restrictions. On successful completion
 * the fields of the structure are set to represent the specified calendar
 * time. Returns the specified calendar time. If the calendar time can not be
 * represented, returns the value (time_t) -1.
 */

#define _DAYS_IN_MONTH(x) ((x == 1) ? days_in_feb : DAYS_IN_MONTH[x])
#define _DAYS_IN_YEAR(year) (IS_LEAP(year) ? 366 : 365)

static void validate_structure(struct tm *tim_p) {
  div_t res;
  int days_in_feb = 28;

  /* calculate time & date to account for out of range values */
  if (tim_p->tm_sec < 0 || tim_p->tm_sec > 59)
    {
      res = div (tim_p->tm_sec, 60);
      tim_p->tm_min += res.quot;
      if ((tim_p->tm_sec = res.rem) < 0)
	{
	  tim_p->tm_sec += 60;
	  --tim_p->tm_min;
	}
    }

  if (tim_p->tm_min < 0 || tim_p->tm_min > 59)
    {
      res = div (tim_p->tm_min, 60);
      tim_p->tm_hour += res.quot;
      if ((tim_p->tm_min = res.rem) < 0)
	{
	  tim_p->tm_min += 60;
	  --tim_p->tm_hour;
        }
    }

  if (tim_p->tm_hour < 0 || tim_p->tm_hour > 23)
    {
      res = div (tim_p->tm_hour, 24);
      tim_p->tm_mday += res.quot;
      if ((tim_p->tm_hour = res.rem) < 0)
	{
	  tim_p->tm_hour += 24;
	  --tim_p->tm_mday;
        }
    }

  if (tim_p->tm_mon < 0 || tim_p->tm_mon > 11)
    {
      res = div (tim_p->tm_mon, 12);
      tim_p->tm_year += res.quot;
      if ((tim_p->tm_mon = res.rem) < 0)
        {
	  tim_p->tm_mon += 12;
	  --tim_p->tm_year;
        }
    }

  if (_DAYS_IN_YEAR (tim_p->tm_year) == 366)
    days_in_feb = 29;

  if (tim_p->tm_mday <= 0)
    {
      while (tim_p->tm_mday <= 0)
	{
	  if (--tim_p->tm_mon == -1)
	    {
	      tim_p->tm_year--;
	      tim_p->tm_mon = 11;
	      days_in_feb =
		((_DAYS_IN_YEAR (tim_p->tm_year) == 366) ?
		 29 : 28);
	    }
	  tim_p->tm_mday += _DAYS_IN_MONTH (tim_p->tm_mon);
	}
    }
  else
    {
      while (tim_p->tm_mday > _DAYS_IN_MONTH (tim_p->tm_mon))
	{
	  tim_p->tm_mday -= _DAYS_IN_MONTH (tim_p->tm_mon);
	  if (++tim_p->tm_mon == 12)
	    {
	      tim_p->tm_year++;
	      tim_p->tm_mon = 0;
	      days_in_feb =
		((_DAYS_IN_YEAR (tim_p->tm_year) == 366) ?
		 29 : 28);
	    }
	}
    }
}

pa_time_t pa_mktime(struct tm *tim_p) {
  pa_time_t tim = 0;
  long days = 0;
  int year;

  /* validate structure */
  validate_structure (tim_p);

  /* compute hours, minutes, seconds */
  tim += tim_p->tm_sec + (tim_p->tm_min * SECS_PER_MIN) +
    (tim_p->tm_hour * SECS_PER_HOUR);

  /* compute days in year */
  days += tim_p->tm_mday - 1;
  days += DAYS_BEFORE_MONTH[tim_p->tm_mon];
  if (tim_p->tm_mon > 1 && _DAYS_IN_YEAR (tim_p->tm_year) == 366)
    days++;

  /* compute day of the year */
  tim_p->tm_yday = days;

  if (tim_p->tm_year > 10000 || tim_p->tm_year < -10000)
      return (time_t) -1;

  /* compute days in other years */
  if ((year = tim_p->tm_year) > 70)
    {
      for (year = 70; year < tim_p->tm_year; year++)
	days += _DAYS_IN_YEAR (year);
    }
  else if (year < 70)
    {
      for (year = 69; year > tim_p->tm_year; year--)
	days -= _DAYS_IN_YEAR (year);
      days -= _DAYS_IN_YEAR (year);
    }

  /* compute total seconds */
  tim += (pa_time_t)days * SECS_PER_DAY;

  /* compute day of the week */
  if ((tim_p->tm_wday = (days + 4) % 7) < 0)
    tim_p->tm_wday += 7;

  return tim;
}

#endif
