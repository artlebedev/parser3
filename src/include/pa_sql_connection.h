/** @file
	Parser: sql fconnection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_connection.h,v 1.18 2001/11/05 10:21:26 paf Exp $
*/

#ifndef PA_SQL_CONNECTION_H
#define PA_SQL_CONNECTION_H

#include "pa_config_includes.h"
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

public:

	SQL_Connection(Pool& pool, const String& aurl, SQL_Driver& adriver) : Pooled(pool),
		furl(aurl),
		fdriver(adriver),
		fconnection(0),
		time_stamp(0) {
	}
	
	const String& get_url() { return furl; }

	void set_services(SQL_Driver_services *aservices) {
		time_stamp=time(0); // they started to use at this time
		fservices=aservices;
	}
	bool expired(time_t older_dies) {
		return time_stamp<older_dies;
	}
	time_t get_time_stamp() { return time_stamp; }

	void close() {
		SQL_driver_manager->close_connection(furl, *this);
	}

	bool connected() { return fconnection!=0; }
	void connect(char *used_only_in_connect_url_cstr) { 
		SQL_CONNECTION_SERVICED_FUNC_GUARDED(
			fdriver.connect(used_only_in_connect_url_cstr, *fservices, &fconnection)
		);
	}
	void disconnect() { 
		fdriver.disconnect(fconnection); fconnection=0; 
	}
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


private:

	const String& furl;
	SQL_Driver& fdriver;
	SQL_Driver_services *fservices;
	void *fconnection;
	time_t time_stamp;
};

#endif
