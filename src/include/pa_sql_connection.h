/** @file
	Parser: sql fconnection decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SQL_CONNECTION_H
#define PA_SQL_CONNECTION_H

static const char * const IDENT_SQL_CONNECTION_H="$Date: 2003/12/10 14:17:45 $";


#include "pa_sql_driver.h"
#include "pa_sql_driver_manager.h"

// defines

/// @see SQL_Driver_services_impl::_throw
#ifdef PA_WITH_SJLJ_EXCEPTIONS
	#define SQL_CONNECTION_SERVICED_FUNC_GUARDED(actions) actions
#else
	#define SQL_CONNECTION_SERVICED_FUNC_GUARDED(actions) \
		if(!setjmp(fservices.mark)) { \
			actions; \
		} else \
			fservices.propagate_exception();
#endif

/// SQL_Driver_services Pooled implementation
class SQL_Driver_services_impl: public SQL_Driver_services {
	const String* furl;
	Exception fexception;
public:
	SQL_Driver_services_impl(): furl(0) {}
	void set_url(const String& aurl) { furl=&aurl;}
	const String& url_without_login() const;

	override void *malloc(size_t size) { return pa_malloc(size); }
	override void *malloc_atomic(size_t size) { return pa_malloc_atomic(size); }
	override void *realloc(void *ptr, size_t size) { return pa_realloc(ptr, size); }

	/**
		normally we can't 'throw' from dynamic library, so
		the idea is to #1 jump to C++ some function to main body, where
		every function stack frame has exception unwind information
		and from there... #2 propagate_exception()

        but when parser configured --with-sjlj-exceptions
		one can simply 'throw' from dynamic library.
		[sad story: one can not longjump/throw due to some bug in gcc as of 3.2.1 version]
	*/
	override void _throw(const SQL_Error& aexception) { 
		// converting SQL_exception to parser Exception
		// hiding passwords and addresses from accidental show [imagine user forgot @exception]
#ifdef PA_WITH_SJLJ_EXCEPTIONS
		throw
#else
		fexception=
#endif
		Exception(aexception.type(), 
				&url_without_login(),
				aexception.comment()); 

#ifndef PA_WITH_SJLJ_EXCEPTIONS
		longjmp(mark, 1);
#endif
	}
	virtual void propagate_exception() {
#ifndef PA_WITH_SJLJ_EXCEPTIONS
		throw fexception;
#endif
	}
};

/// SQL connection. handy wrapper around low level SQL_Driver
class SQL_Connection: public PA_Object {
	const String&  furl;
	SQL_Driver& fdriver;
	SQL_Driver_services_impl fservices;
	void *fconnection;
	time_t time_used;
	bool marked_to_rollback;

public:

	SQL_Connection(const String& aurl, SQL_Driver& adriver):
		furl(aurl),
		fdriver(adriver),
		fconnection(0),
		time_used(0), 
		marked_to_rollback(false) {
	}

	SQL_Driver_services_impl& services() { return fservices; }
	
	const String& get_url() { return furl; }

	void set_url() {
		fservices.set_url(furl);
	}
	void use() {
		time_used=time(0); // they started to use at this time
	}
	bool expired(time_t older_dies) {
		return /*!freferences && */time_used<older_dies;
	}
	time_t get_time_used() { return time_used; }

	bool connected() { return fconnection!=0; }
	void connect(char *used_only_in_connect_url_cstr) { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.connect(used_only_in_connect_url_cstr, fservices, &fconnection)
		);
	}
	void disconnect() { 
		fdriver.disconnect(fconnection); fconnection=0; 
	}
	bool ping() { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			return fdriver.ping(fservices, fconnection)
		);
	}
	const char* quote(const char* str, unsigned int length) {
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			return fdriver.quote(fservices, fconnection, str, length)
		);
//		return 0; // never reached
	}

	void query(
		const char* statement, unsigned long offset, unsigned long limit,
		SQL_Driver_query_event_handlers& handlers, 
		const String& source) {
		try {
			SQL_CONNECTION_SERVICED_FUNC_GUARDED(
				fdriver.query(fservices, fconnection, 
					statement, offset, limit, 
					handlers)
			);	
		} catch(const Exception& e) { // query problem
			if(strcmp(e.type(), "sql.connect")==0) { // if it is _throw exception, 
				// give more specific source [were url]
				throw Exception("sql.execute",
					&source, 
					"%s", e.comment());
			} else
				rethrow;
		}
	}

	void commit() { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.commit(fservices, fconnection) 
		);
	}
	void rollback() { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.rollback(fservices, fconnection)
		);
	}

	/// return to cache
	void close() {
		if(marked_to_rollback) {
			rollback();
			marked_to_rollback=false;
		} else
			commit();

		SQL_driver_manager.close_connection(furl, this);
	}

};

#endif
