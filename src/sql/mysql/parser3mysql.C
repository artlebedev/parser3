/** @file
	Parser: MySQL driver.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3mysql.C,v 1.1 2001/04/04 10:54:33 paf Exp $
*/

#include "pa_sql_driver.h"

class MySQL_Driver : public SQL_Driver {
public:

	MySQL_Driver() : SQL_driver() {}

	/// get api version
	int api_version() { return SQL_API_VERSION; }
	/// connect
	virtual void *connect(const char *url) { 
		return 0;
	}
	/// disconnect
	virtual void disconnect(void *info) {
		;
	}
};

extern "C" SQL_Driver *create() {
	return new MySQL_Driver();
}