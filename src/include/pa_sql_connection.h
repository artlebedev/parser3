/** @file
	Parser: sql connection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_connection.h,v 1.2 2001/04/05 11:01:55 paf Exp $
*/

#ifndef PA_SQL_CONNECTION_H
#define PA_SQL_CONNECTION_H

#include "pa_pool.h"
#include "pa_sql_driver.h"
#include "pa_exception.h"

/// sql connection
class SQL_Connection : public Pooled {

public:

	void set_services(Services_for_SQL_driver *services) {
		fdriver.set_services(services);
	}

	SQL_Connection(Pool& pool,
		const String& aurl, 
		SQL_Driver& adriver, Services_for_SQL_driver& services, 
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
	void query(
		const char *statement, 
		unsigned int *column_count, SQL_Driver::Cell **columns,
		unsigned long *row_count, SQL_Driver::Cell ***rows) { 
		fdriver.query(connection, 
			statement, 
			column_count, columns,
			row_count, rows);
	}


private:

	SQL_Driver& fdriver;
	void *connection;
	const String& furl;
};

#endif
