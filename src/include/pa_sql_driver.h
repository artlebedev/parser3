/** @file
	Parser: sql driver interface.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_driver.h,v 1.1 2001/04/04 10:53:54 paf Exp $


	driver dynamic library must look like this:
	@code
		class X_SQL_Driver : public SQL_Driver {
		public:

			X_SQL_Driver() : SQL_driver() {}

			int version() { return SQL_API_VERSION; }
			
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

#define SQL_API_VERSION 0x0300

/// sQl-driver API
class SQL_Driver {
public:

	SQL_Driver() {}
	/// get api version
	virtual int api_version() =0;
	/// connect. @returns true+'info' on success. 'error' on failure
	virtual bool connect(const char *url, void **info, char **error) =0;
	/// disconnect
	virtual void disconnect(void *info) =0;
	/// log error message
	//static void log(Pool& pool, const char *fmt, ...);
};

typedef SQL_Driver *(*SQL_Driver_create_func)();

#endif
