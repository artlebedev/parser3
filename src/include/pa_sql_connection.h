/** @file
	Parser: sql fconnection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_connection.h,v 1.14 2001/10/22 16:44:42 parser Exp $
*/

#ifndef PA_SQL_CONNECTION_H
#define PA_SQL_CONNECTION_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_sql_driver.h"
#include "pa_sql_driver_manager.h"

/// SQL connection. handy wrapper around low level SQL_Driver
class SQL_Connection : public Pooled {

public:

	SQL_Connection(Pool& pool, const String& aurl, SQL_Driver& adriver) : Pooled(pool),
		furl(aurl),
		fdriver(adriver),
		fconnection(0),
		time_stamp(0) {
	}
	
	const String& url() { return furl; }

	void set_services(SQL_Driver_services *aservices) {
		time_stamp=time(0); // they started to use at this time
		fservices=aservices;
	}
	bool expired(time_t older_dies) {
		return time_stamp<older_dies;
	}

	void close() {
		SQL_driver_manager->close_connection(furl, *this);
	}

	bool connected() { return fconnection!=0; }
	void connect(char *used_only_in_connect_url_cstr) { 
		fdriver.connect(used_only_in_connect_url_cstr, *fservices, &fconnection);
	}
	void disconnect() { fdriver.disconnect(fconnection); fconnection=0; }
	void commit() { fdriver.commit(*fservices, fconnection); }
	void rollback() { fdriver.rollback(*fservices, fconnection); }
	bool ping() { return fdriver.ping(*fservices, fconnection); }
	uint quote(char *to, const char *from, unsigned int length) {
		return fdriver.quote(*fservices, fconnection, to, from, length);
	}

	void query(
		const char *statement, unsigned long offset, unsigned long limit,
		SQL_Driver_query_event_handlers& handlers) { 
		fdriver.query(*fservices, fconnection, 
			statement, offset, limit, 
			handlers);
	}


private:

	const String& furl;
	SQL_Driver& fdriver;
	SQL_Driver_services *fservices;
	void *fconnection;
	time_t time_stamp;
};

#endif
