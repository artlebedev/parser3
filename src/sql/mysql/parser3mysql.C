/** @file
	Parser: MySQL driver.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3mysql.C,v 1.6 2001/04/05 08:09:26 paf Exp $
*/

#include <stdlib.h>

#include "pa_sql_driver.h"
#include "mysql.h"
#include "pa_common.h"

char *lsplit(char *string, char delim) {
    if(string) {
		char *v=strchr(string, delim);
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return 0;
}

char *lsplit(char **string_ref, char delim) {
    char *result=*string_ref;
	char *next=lsplit(*string_ref, delim);
    *string_ref=next;
    return result;
}

/**
	MySQL server driver
	
	@todo 
		figure out about memory for errors:
		- static=add multithread locks
		- dynamic=who should free it up?
*/
class MySQL_Driver : public SQL_Driver {
public:

	MySQL_Driver() : SQL_Driver() {
	}

	/// get api version
	int api_version() { return SQL_DRIVER_API_VERSION; }
	/// connect
	void connect(
		char *url, ///< @b user:pass@host[:port]/database
		void **connection ///< output: MYSQL *
		) {
		char *user=url;
		char *host=lsplit(user, '@');
		char *db=lsplit(host, '/');
		char *pwd=lsplit(user, ':');
		char *error_pos=0;
		char *port_cstr=lsplit(host, ':');
		int port=port_cstr?strtol(port_cstr, &error_pos, 0):0;

	    MYSQL *mysql=mysql_init(NULL);
		if(!mysql_real_connect(mysql, 
			host, user, pwd, db, port?port:MYSQL_PORT, NULL, 0))
			fservices->_throw(mysql_error(mysql));

		*(MYSQL **)connection=mysql;
	}
	void disconnect(void *connection) {
	    mysql_close((MYSQL *)connection);
	}
	void commit(void *connection) {}
	void rollback(void *connection) {}
};

extern "C" SQL_Driver *create() {
	return new MySQL_Driver();
}