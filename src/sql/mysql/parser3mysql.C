/** @file
	Parser: MySQL driver.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3mysql.C,v 1.2 2001/04/04 11:47:30 paf Exp $
*/

#include "pa_sql_driver.h"


/// MySQL server driver
class MySQL_Driver : public SQL_Driver {
public:

	MySQL_Driver() : SQL_Driver() {}

	/// get api version
	int api_version() { return SQL_API_VERSION; }
	/// connect @test move info inside
	bool connect(const char *url, void **info, const char **error) {
		/**error="mysql connect failed";
		return false;*/
		*info=0;
		return true;
	}
	bool disconnect(void *info, const char **error) {
		*error="mysql disconnect failed";
		return false;
	}
	bool commit(void *info, const char **error) {
		*error="mysql commit failed";
		return false;
	}
	bool rollback(void *info, const char **error) {
		*error="mysql rollback failed";
		return false;
	}
};

extern "C" SQL_Driver *create() {
	return new MySQL_Driver();
}