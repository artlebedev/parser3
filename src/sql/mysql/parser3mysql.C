/** @file
	Parser: MySQL driver.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: parser3mysql.C,v 1.10 2001/04/05 16:30:43 paf Exp $
*/

#include <stdlib.h>
#include <string.h>

#include "pa_sql_driver.h"
#include "mysql.h"

#define MAX_STRING 0x400

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

/**
	MySQL server driver
	
	@todo figure out about memory for errors:
		static=add multithread locks;
		dynamic=who should free it up?
*/
class MySQL_Driver : public SQL_Driver {
public:

	MySQL_Driver() : SQL_Driver() {
	}

	/// get api version
	int api_version() { return SQL_DRIVER_API_VERSION; }
	/// connect
	void connect(
		char *url, /**< @b user:pass@host[:port]/database/charset 
				   3.23.22b
				   Currently the only option for character_set_name is cp1251_koi8 */
		void **connection ///< output: MYSQL *
		) {
		char *user=url;
		char *host=lsplit(user, '@');
		char *db=lsplit(host, '/');
		char *pwd=lsplit(user, ':');
		char *error_pos=0;
		char *port_cstr=lsplit(host, ':');
		int port=port_cstr?strtol(port_cstr, &error_pos, 0):0;
		char *charset=lsplit(db, '/');

	    MYSQL *mysql=mysql_init(NULL);
		if(!mysql_real_connect(mysql, 
			host, user, pwd, db, port?port:MYSQL_PORT, NULL, 0))
			fservices->_throw(mysql_error(mysql));

		if(charset) { 
			// set charset
			char statement[MAX_STRING]="set CHARACTER SET "; // cp1251_koi8
			strncat(statement, charset, MAX_STRING);
			
			if(mysql_query(mysql, statement)) 
				fservices->_throw(mysql_error(mysql));
			mysql_store_result(mysql); // throw out the result [don't need but must call]
		}

		*(MYSQL **)connection=mysql;
	}
	void disconnect(void *connection) {
	    mysql_close((MYSQL *)connection);
	}
	void commit(void *connection) {}
	void rollback(void *connection) {}

	bool ping(void *connection) {
		return false;
		return mysql_ping((MYSQL *)connection)==0;
	}

	unsigned int quote(void *connection,
		char *to, const char *from, unsigned int length) {
		/*
			3.23.22b
			You must allocate the to buffer to be at least length*2+1 bytes long. 
			(In the worse case, each character may need to be encoded as using two bytes, 
			and you need room for the terminating null byte.)

			it's already UNTAINT_TIMES_BIGGER
		*/
		return mysql_escape_string(to, from, length);
	}
	void query(void *connection, 
		const char *statement, unsigned long offset, unsigned long limit,
		unsigned int *column_count, Cell **columns, 
		unsigned long *row_count, Cell ***rows) {

		MYSQL *mysql=(MYSQL *)connection;
		MYSQL_RES *res=NULL;

		if(mysql_query(mysql, statement)) 
			fservices->_throw(mysql_error(mysql));
		if(!(res=mysql_store_result(mysql)) && mysql_field_count(mysql)) 
			fservices->_throw(mysql_error(mysql));
		if(!res) {
			// empty result
			*row_count=0;
			*column_count=0;
			return;
		}
		
		*column_count=mysql_num_fields(res);
		*columns=(Cell *)fservices->malloc(sizeof(Cell)*(*column_count));

		*row_count=(unsigned long)mysql_num_rows(res);
		*rows=(Cell **)fservices->malloc(sizeof(Cell *)*(*row_count));
		
		for(unsigned int i=0; i<(*column_count); i++){
			MYSQL_FIELD *field=mysql_fetch_field(res);
			size_t size=strlen(field->name);
			(*columns)[i].size=size;
			(*columns)[i].ptr=fservices->malloc(size);
			memcpy((*columns)[i].ptr, field->name, size);
		}
		
		for(unsigned long r=0; r<(*row_count); r++) 
			if(MYSQL_ROW mysql_row=mysql_fetch_row(res)) { // never false..
				unsigned long *lengths=mysql_fetch_lengths(res);
				Cell *row=(Cell *)malloc(sizeof(Cell)*(*column_count));
				(*rows)[r]=row;
				for(unsigned int i=0; i<(*column_count); i++){
					size_t size=(size_t)lengths[i];
					row[i].size=size;
					row[i].ptr=fservices->malloc(size);
					memcpy(row[i].ptr, mysql_row[i], size);
				}
		}
		
		mysql_free_result(res);
	}
};

extern "C" SQL_Driver *create() {
	return new MySQL_Driver();
}