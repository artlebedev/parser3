/** @file
	Parser: MySQL driver.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: parser3mysql.C,v 1.4 2001/04/04 12:45:47 paf Exp $
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

/// MySQL server driver
class MySQL_Driver : public SQL_Driver {
public:

	MySQL_Driver() : SQL_Driver() {
	}

	/// get api version
	int api_version() { return SQL_API_VERSION; }
	/// connect
	const char *connect(char *url, void **info) {
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
			return "mysql connect failed";

		*(MYSQL **)info=mysql;
		return 0;
	}
	const char *disconnect(void *info) {
	    mysql_close((MYSQL *)info);
		return 0;
	}
	const char *commit(void *info) {
		return 0;//"mysql commit failed";
	}
	const char *rollback(void *info) {
		return 0;//"mysql rollback failed";
	}
	;
};

extern "C" SQL_Driver *create() {
	return new MySQL_Driver();
}