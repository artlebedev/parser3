/** @file
	Parser: sql fconnection decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_sql_connection.h,v 1.24 2002/03/25 09:40:01 paf Exp $
*/

#ifndef PA_SQL_CONNECTION_H
#define PA_SQL_CONNECTION_H

#include "pa_pool.h"
#include "pa_sql_driver.h"
#include "pa_sql_driver_manager.h"

// defines

/// @see SQL_Driver_services_impl::_throw
#define SQL_CONNECTION_SERVICED_FUNC_GUARDED(actions) \
	if(!fservices || !setjmp(fservices->mark)) { \
		actions; \
	} else \
		fservices->propagate_exception();

/// SQL connection. handy wrapper around low level SQL_Driver
class SQL_Connection : public Pooled {

	friend class SQL_Connection_ptr;

public:

	SQL_Connection(Pool& pool, const String& aurl, SQL_Driver& adriver) : Pooled(pool),
		furl(aurl),
		fdriver(adriver),
		fconnection(0),
		time_used(0), used(0),
		marked_to_rollback(false) {
	}
	
	const String& get_url() { return furl; }

	void set_services(SQL_Driver_services *aservices) {
		fservices=aservices;
	}
	bool expired(time_t older_dies) {
		return !used && time_used<older_dies;
	}
	time_t get_time_used() { return time_used; }

	bool connected() { return fconnection!=0; }
	void connect(char *used_only_in_connect_url_cstr) { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.connect(used_only_in_connect_url_cstr, *fservices, &fconnection)
		);
	}
	void disconnect() { 
		fdriver.disconnect(fconnection); fconnection=0; 
	}
	bool ping() { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			return fdriver.ping(*fservices, fconnection)
		);
		return 0; // never reached
	}
	uint quote(char *to, const char *from, unsigned int length) {
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			return fdriver.quote(*fservices, fconnection, to, from, length)
		);
		return 0; // never reached
	}

	void query(
		const char *statement, unsigned long offset, unsigned long limit,
		SQL_Driver_query_event_handlers& handlers) { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.query(*fservices, fconnection, 
				statement, offset, limit, 
				handlers)
		);
	}

	void mark_to_rollback() {
		marked_to_rollback=true;
	}

private: // closing process

	void commit() { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.commit(*fservices, fconnection) 
		);
	}
	void rollback() { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.rollback(*fservices, fconnection)
		);
	}

	/// return to cache
	void close() {
		if(marked_to_rollback) {
			rollback();
			marked_to_rollback=false;
		} else
			commit();

		SQL_driver_manager->close_connection(furl, *this);
	}

private: // connection usage methods

	void use() {
		time_used=time(0); // they started to use at this time
		used++;
	}
	void unuse() {
		used--;
		if(!used)
			close();
	}

private: // connection usage data

	int used;

private:

	const String& furl;
	SQL_Driver& fdriver;
	SQL_Driver_services *fservices;
	void *fconnection;
	time_t time_used;
	bool marked_to_rollback;
};

/// Auto-object used to track SQL_Connection usage
class SQL_Connection_ptr {
	SQL_Connection *fconnection;
public:
	explicit SQL_Connection_ptr(SQL_Connection *aconnection) : fconnection(aconnection) {
		fconnection->use();
	}
	~SQL_Connection_ptr() {
		fconnection->unuse();
	}
	SQL_Connection* operator->() {
		return fconnection;
	}
	SQL_Connection* get() const {
		return fconnection; 
	}

	// copying
	SQL_Connection_ptr(const SQL_Connection_ptr& src) : fconnection(src.fconnection) {
		fconnection->use();
	}
	SQL_Connection_ptr& operator =(const SQL_Connection_ptr& src) {
		// may do without this=src check
		fconnection->unuse();
		fconnection=src.fconnection;
		fconnection->use();

		return *this;
	}
};

#endif
