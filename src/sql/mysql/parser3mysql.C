/** @file
	Parser: MySQL driver.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3mysql.C,v 1.3 2001/04/04 12:13:20 paf Exp $
*/

#include "pa_sql_driver.h"


/// MySQL server driver
class MySQL_Driver : public SQL_Driver {
public:

	MySQL_Driver() : SQL_Driver(),
		info(0) {
	}

	/// get api version
	int api_version() { return SQL_API_VERSION; }
	/// connect @test move info inside
	const char *connect(const char *url) {
		/*return "mysql connect failed";*/
		return 0;
	}
	const char *disconnect() {
		return "mysql disconnect failed";
	}
	const char *commit() {
		return 0;//"mysql commit failed";
	}
	const char *rollback() {
		return "mysql rollback failed";
	}
	
private:

	void *info;
};

extern "C" SQL_Driver *create() {
	return new MySQL_Driver();
}