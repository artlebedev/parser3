/** @file
	Parser: sql driver interface.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver.h,v 1.1 2001/04/05 08:09:25 paf Exp $


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

#include "pa_pool.h"

/// service functions for SQL driver to use
class Services_for_SQL_driver {
public:
	/// allocates some bytes on pool
	virtual void *malloc(size_t size) =0;
	/// allocates some bytes clearing them with zeros
	virtual void *calloc(size_t size) =0;
	/// throw exception
	virtual void _throw(const char *comment) =0;
};

#define SQL_DRIVER_API_VERSION 0x0300

/// SQL driver API
class SQL_Driver {
public:

	/// assignes services to driver. you can not use driver until this
	void set_services(Services_for_SQL_driver *aservices) { fservices=aservices; }

	SQL_Driver() :
		fservices(0) {
	}
	/// get api version
	virtual int api_version() =0;
	/// connect. @returns true+'connection' on success. 'error' on failure
	virtual void connect(char *url, void **connection) =0;
	virtual void disconnect(void *connection) =0;
	virtual void commit(void *connection) =0;
	virtual void rollback(void *connection) =0;
	/// log error message
	//static void log(Pool& pool, const char *fmt, ...);

protected:

	Services_for_SQL_driver *fservices;
};

typedef SQL_Driver *(*SQL_Driver_create_func)();

#endif
