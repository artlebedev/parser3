/** @file
	Parser: sql connection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_connection.h,v 1.5 2001/04/17 19:31:11 paf Exp $
*/

#ifndef PA_SQL_CONNECTION_H
#define PA_SQL_CONNECTION_H

#include "pa_pool.h"
#include "pa_sql_driver.h"
#include "pa_sql_driver_manager.h"

/// SQL connection. handy wrapper around low level SQL_Driver
class SQL_Connection : public Pooled {

public:

	void set_services(SQL_Driver_services *services) {
		fdriver.set_services(services);
	}

	SQL_Connection(Pool& pool,
		const String& aurl, 
		SQL_Driver& adriver, SQL_Driver_services& services, 
		char *url_cstr) : Pooled(pool),
		furl(aurl),
		fdriver(adriver) {

		set_services(&services); // associate with services[request]
		fdriver.connect(url_cstr, &connection);
	}

	void close() {
		set_services(0); // deassociate from services[request]
		SQL_driver_manager->close_connection(furl, *this);
	}

	void disconnect() { fdriver.disconnect(connection); }
	void commit() { fdriver.commit(connection); }
	void rollback() { fdriver.rollback(connection); }
	bool ping() { return fdriver.ping(connection); }
	uint quote(char *to, const char *from, unsigned int length) {
		return fdriver.quote(connection, to, from, length);
	}

	void query(
		const char *statement, unsigned long offset, unsigned long limit,
		unsigned int *column_count, SQL_Driver::Cell **columns,
		unsigned long *row_count, SQL_Driver::Cell ***rows) { 
		fdriver.query(connection, 
			statement, offset, limit, 
			column_count, columns,
			row_count, rows);
	}


private:

	SQL_Driver& fdriver;
	void *connection;
	const String& furl;
};

#endif
