/** @file
	Parser: sql driver interface.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)


	driver dynamic library must look like this:
	@code
		class X_SQL_Driver: public SQL_Driver {
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

#define IDENT_PA_SQL_DRIVER_H "$Id: pa_sql_driver.h,v 1.52 2020/12/15 17:10:37 moko Exp $"

#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>
#include <limits.h>

/*
    1..8 not logged
	9 introducing placeholders
	10 limit fixed (default: SQL_NO_LIMIT [ULONG_MAX]), path to document_root added
*/
#define SQL_DRIVER_API_VERSION 10
//#define SQL_DRIVER_API_VERSION 9
#define SQL_DRIVER_CREATE create /* used in driver implementation */
#define SQL_DRIVER_CREATE_NAME "create" /* could not figure out how to # it :( */

#define SQL_NO_LIMIT ULONG_MAX
//#define SQL_NO_LIMIT 0

/// fields are freed elsewhere
class SQL_Error {
	bool fdefined;
	const char* ftype;
	const char* fcomment;
public:
	SQL_Error():
		fdefined(false) {}
	SQL_Error(
		const char* atype,
		const char* acomment):
		fdefined(true),
		ftype(atype),
		fcomment(acomment) {}
	SQL_Error(const char* acomment):
		fdefined(true),
		ftype(0),
		fcomment(acomment) {}
	SQL_Error& operator =(const SQL_Error& src) {
		fdefined=src.fdefined;
		ftype=src.ftype;
		fcomment=src.fcomment;
		return *this;
	}

	bool defined() const { return fdefined; }
	const char* type() const { return ftype; }
	const char* comment() const { return fcomment; }
};

/// service functions for SQL driver to use
class SQL_Driver_services {
public:
	/// allocates some bytes
	virtual void *malloc(size_t size) =0;
	/// allocates some bytes, user promises: no pointers inside
	virtual void *malloc_atomic(size_t size) =0;
	/// reallocates bytes 
	virtual void *realloc(void *ptr, size_t size) =0;
	/// $request:charset
	virtual const char* request_charset() =0;
	/// $request:document-root
	virtual const char* request_document_root() =0;
	/// transcoder. 
	/// WARNING: can store pointers to charset names to speedup name-to-instance resolving
	/// so do NOT pass pointers to local vars and change those vars after that
	virtual void transcode(const char* src, size_t src_length,
		const char*& dst, size_t& dst_length,
		const char* charset_from_name,
		const char* charset_to_name
		) =0;
	/// prepare throw exception
	virtual void _throw(const SQL_Error& e) =0;
	/// throw C++ exception from prepared
	virtual void propagate_exception() =0;
	/// helper func
	void _throw(const char* comment) { _throw(SQL_Error("sql.connect", comment)); }
public:
	/// regretrully public, because can't make stack frames: "nowhere to return to"
	jmp_buf mark;
};

/** events, occuring when SQL_Driver::query()-ing. 

		when OK must return false.
		must NOT throw exceptions, must store them to error & return true.
*/
class SQL_Driver_query_event_handlers {
public:
	virtual bool add_column(SQL_Error& error, const char* str, size_t length) =0;
	virtual bool before_rows(SQL_Error& error) =0;
	virtual bool add_row(SQL_Error& error) =0;
	virtual bool add_row_cell(SQL_Error& error, const char* str, size_t length) =0;
};

/// SQL driver API
class SQL_Driver {
public:

	/// @todo can be optimized to contain type information, 
	/// to pass IN and OUT int/double NOT in string format
	struct Placeholder {
		const char* name;
		const char* value;
		bool is_null;
		bool were_updated;
	};

	/// get api version
	virtual int api_version() =0;
	/// initialize driver by loading sql dynamic link library
	virtual const char* initialize(char *dlopen_file_spec) =0;
	/**	connect to sql database using 
		@param url_cstr
			format is driver specific

		@returns true+'connection' on success. 'error' on failure
	*/
	virtual void connect(char *url_cstr, 
		SQL_Driver_services& services, void **connection) =0;
	virtual void disconnect(void *connection) =0;
	virtual void commit(void *connection) =0;
	virtual void rollback(void *connection) =0;
	/// @returns true to indicate that connection still alive 
	virtual bool ping(void *connection) =0;
	/// encodes the string in 'from' to an escaped SQL string
	virtual const char* quote(void *connection,
		const char* str, unsigned int length) =0;
	virtual void query(void *connection,
		const char* statement, 
		size_t placeholders_count, Placeholder* placeholders,
		unsigned long offset, unsigned long limit,
		SQL_Driver_query_event_handlers& handlers) =0;
};

typedef SQL_Driver *(*SQL_Driver_create_func)();

#endif
