/** @file
	Parser: sql driver interface.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)


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

static const char* IDENT_SQL_DRIVER_H="$Date: 2002/12/09 11:14:55 $";

#include <sys/types.h>

#define SQL_DRIVER_API_VERSION 0x0004
#define SQL_DRIVER_CREATE create /* used in driver implementation */
#define SQL_DRIVER_CREATE_NAME "create" /* could not figure out how to # it :( */

/// fields are freed elsewhere
class SQL_Exception {
	friend class SQL_Driver_services_impl;

	bool fdefined;
	const char *ftype;
	const void *fproblem_source;
	const char *fcomment;
public:
	SQL_Exception():
		fdefined(false) {}
	SQL_Exception(
		const char *atype,
		const void *aproblem_source, 
		const char *acomment):
		fdefined(true),
		ftype(atype),
		fproblem_source(aproblem_source),
		fcomment(acomment) {}
	SQL_Exception(const char *acomment):
		fdefined(true),
		ftype(0),
		fproblem_source(0),
		fcomment(acomment) {}
/*
	SQL_Exception(const SQL_Exception& src) : 
		fdefined(src.fdefined),
		ftype(src.ftype),
		fproblem_source(src.fproblem_source),
		fcomment(src.fcomment) {}
*/
	SQL_Exception& operator =(const SQL_Exception& src) {
		fdefined=src.fdefined;
		ftype=src.ftype;
		fproblem_source=src.fproblem_source;
		fcomment=src.fcomment;
		return *this;
	}

	bool defined() { return fdefined; }
	const char *type() { return ftype; }
	const void *problem_source() { return fproblem_source; }
	const char *comment() { return fcomment; }
};

/// service functions for SQL driver to use
class SQL_Driver_services {
public:
	/// allocates some bytes on pool
	virtual void *malloc(size_t size) =0;
	/// allocates some bytes clearing them with zeros
	virtual void *calloc(size_t size) =0;
	/// prepare throw exception
	virtual void _throw(const SQL_Exception& e) =0;
	/// throw C++ exception from prepared
	virtual void propagate_exception() =0;
	/// helper func
	void _throw(const char *comment) { _throw(SQL_Exception(0, 0, comment)); }
public:
	/// regretrully public, because can't make stack frames: "nowhere to return to"
	jmp_buf mark;
};

/// events, occuring when SQL_Driver::query()-ing
class SQL_Driver_query_event_handlers {
public:
	virtual void add_column(void *ptr, size_t size) =0;
	virtual void before_rows() =0;
	virtual void add_row() =0;
	virtual void add_row_cell(void *ptr, size_t size) =0;
};

/// SQL driver API
class SQL_Driver {
public:

	/// get api version
	virtual int api_version() =0;
	/// initialize driver by loading sql dynamic link library
	virtual const char *initialize(char *dlopen_file_spec) =0;
	/**	connect to sql database using 
		@param used_only_to_connect_url 
			format is driver specific
			WARNING: must be used only to connect, for buffer doesn't live long enough

		@returns true+'connection' on success. 'error' on failure
	*/
	virtual void connect(char *used_only_in_connect_url_cstr, 
		SQL_Driver_services& services, void **connection) =0;
	virtual void disconnect(void *connection) =0;
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
		SQL_Driver_query_event_handlers& handlers) =0;
};

typedef SQL_Driver *(*SQL_Driver_create_func)();

#endif
