/** @file
	Parser: sql driver interface.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver.h,v 1.8 2001/04/26 14:55:31 paf Exp $


	driver dynamic library must look like this:
	@code
		class X_SQL_Driver : public SQL_Driver {
		public:

			X_SQL_Driver() : SQL_driver() {}

			int api_version() { return SQL_DRIVER_API_VERSION; }
			
			//...
		};

		extern "C" SQL_Driver *create() {
			return new X_SQL_Driver();
		}	
	@endcode
*/

#ifndef PA_SQL_DRIVER_H
#define PA_SQL_DRIVER_H

#include <sys/types.h>

/// service functions for SQL driver to use
class SQL_Driver_services {
public:
	/// allocates some bytes on pool
	virtual void *malloc(size_t size) =0;
	/// allocates some bytes clearing them with zeros
	virtual void *calloc(size_t size) =0;
	/// throw exception
	virtual void _throw(const char *comment) =0;
};

#define SQL_DRIVER_API_VERSION 0x0301

/// SQL driver API
class SQL_Driver {
public:

	/// row cell & column title  storage
	struct Cell {
		void *ptr;
		size_t size;
	};

public:

	/// assignes services to driver. you can not use driver until this
	void set_services(SQL_Driver_services *aservices) { services=aservices; }

	SQL_Driver() :
		services(0) {
	}
	/// get api version
	virtual int api_version() =0;
	/// initialize driver by loading sql dynamic link library
	virtual const char *initialize(const char *dlopen_file_spec) =0;
	/// connect. @returns true+'connection' on success. 'error' on failure
	virtual void connect(char *url, void **connection) =0;
	virtual void disconnect(void *connection) =0;
	virtual void commit(void *connection) =0;
	virtual void rollback(void *connection) =0;
	/// @returns true to indicate that connection still alive 
	virtual bool ping(void *connection) =0;
	/// encodes the string in 'from' to an escaped SQL string
	virtual unsigned int quote(void *connection,
		char *to, const char *from, unsigned int length) =0;
	virtual void query(void *connection,
		const char *statement, unsigned long offset, unsigned long limit,
		unsigned int *column_count, Cell **columns,
		unsigned long *row_count, Cell ***rows) =0;
	/// log error message
	//static void log(Pool& pool, const char *fmt, ...);

protected:

	SQL_Driver_services *services;
};

typedef SQL_Driver *(*SQL_Driver_create_func)();

#endif
