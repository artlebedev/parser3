/** @file
	Parser: sql driver interface.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver.h,v 1.10 2001/05/17 13:23:28 parser Exp $


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

	SQL_Driver() {}
	/// get api version
	virtual int api_version() =0;
	/// initialize driver by loading sql dynamic link library
	virtual const char *initialize(const char *dlopen_file_spec) =0;
	/**	connect to sql database using 
		@param used_only_to_connect_url 
			format is driver specific
			WARNING: must be used only to connect, for buffer doesn't live long enough

		@returns true+'connection' on success. 'error' on failure
	*/
	virtual void connect(char *used_only_in_connect_url_cstr, 
		SQL_Driver_services& services, void **connection) =0;
	virtual void disconnect(
		SQL_Driver_services& services, void *connection) =0;
	virtual void commit(
		SQL_Driver_services& services, void *connection) =0;
	virtual void rollback(
		SQL_Driver_services& services, void *connection) =0;
	/// @returns true to indicate that connection still alive 
	virtual bool ping(
		SQL_Driver_services& services, void *connection) =0;
	/// encodes the string in 'from' to an escaped SQL string
	virtual unsigned int quote(
		SQL_Driver_services& services, void *connection,
		char *to, const char *from, unsigned int length) =0;
	virtual void query(
		SQL_Driver_services& services, void *connection,
		const char *statement, unsigned long offset, unsigned long limit,
		unsigned int *column_count, Cell **columns,
		unsigned long *row_count, Cell ***rows) =0;
	/// log error message
	//static void log(Pool& pool, const char *fmt, ...);

};

typedef SQL_Driver *(*SQL_Driver_create_func)();

#endif
