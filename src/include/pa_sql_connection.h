/** @file
	Parser: sql fconnection decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SQL_CONNECTION_H
#define PA_SQL_CONNECTION_H

#define IDENT_PA_SQL_CONNECTION_H "$Id: pa_sql_connection.h,v 1.47 2020/12/15 17:10:32 moko Exp $"


#include "pa_sql_driver.h"
#include "pa_sql_driver_manager.h"

// defines

/// @see SQL_Driver_services_impl::_throw
#ifdef PA_WITH_SJLJ_EXCEPTIONS
	#define SQL_CONNECTION_SERVICED_FUNC_GUARDED(actions) \
		use(); \
		actions
#else
	#define SQL_CONNECTION_SERVICED_FUNC_GUARDED(actions) \
		use(); \
		if(!setjmp(fservices.mark)) { \
			actions; \
		} else \
			fservices.propagate_exception();
#endif

/// SQL_Driver_services Pooled implementation
class SQL_Driver_services_impl: public SQL_Driver_services {
	const String* furl;
	Exception fexception;
	const char* frequest_charset;
	const char* fdocument_root;
public:
	SQL_Driver_services_impl(const char* arequest_charset, const char* adocument_root): furl(0), frequest_charset(arequest_charset), fdocument_root(adocument_root) {}
	void set_url(const String& aurl) { furl=&aurl;}
	const String& url_without_login() const;

	override void* malloc(size_t size) { return pa_malloc(size); }
	override void* malloc_atomic(size_t size) { return pa_malloc_atomic(size); }
	override void* realloc(void *ptr, size_t size) { return pa_realloc(ptr, size); }

	override const char* request_charset() { return frequest_charset; }
	override const char* request_document_root() { return fdocument_root; }
	
	override void transcode(const char* src, size_t src_length,
		const char*& dst, size_t& dst_length,
		const char* charset_from_name,
		const char* charset_to_name
		);

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
		Exception(aexception.type() ? aexception.type() : "sql.connect", &url_without_login(), aexception.comment());

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

public:

	SQL_Connection(const String& aurl, SQL_Driver& adriver, const char* arequest_charset, const char* adocument_root):
		furl(aurl),
		fdriver(adriver),
		fservices(arequest_charset, adocument_root),
		fconnection(0),
		time_used(0) {
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
		return time_used<older_dies;
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
			return fdriver.ping(fconnection)
		);
		return false; // never reached, warning war
	}
	const char* quote(const char* str, unsigned int length) {
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			return fdriver.quote(fconnection, str, length)
		);
		return NULL; // never reached, warning war
	}

	void query(
		const char* statement, 
		size_t placeholders_count, SQL_Driver::Placeholder* placeholders,
		unsigned long offset, unsigned long limit,
		SQL_Driver_query_event_handlers& handlers, 
		const String& source) {
		try {
			SQL_CONNECTION_SERVICED_FUNC_GUARDED(
				fdriver.query(fconnection, statement, placeholders_count, placeholders, offset, limit, handlers)
			);
		} catch(const Exception& e) { // query problem
			if(strcmp(e.type(), "sql.connect")==0) { // if it is _throw exception, 
				// show query instead of connect string
				throw Exception("sql.execute", &source, "%s", e.comment());
			} else
				rethrow;
		}
	}

	void commit() { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.commit(fconnection) 
		);
	}
	void rollback() { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.rollback(fconnection)
		);
	}

	/// return to cache
	void close() {
		SQL_driver_manager->close_connection(furl, this);
	}

};

#endif
