/** @file
	Parser: sql connection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_sql_connection.h,v 1.1 2001/04/05 08:09:21 paf Exp $
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

		set_services(&services);
		fdriver.connect(url_cstr, &info);
	}

	void close() {
		set_services(0); // deassociate from request
		SQL_driver_manager->close_connection(furl, *this);
	}

	void disconnect() { fdriver.disconnect(info); }
	void commit() { fdriver.commit(info); }
	void rollback() { fdriver.rollback(info); }

private:

	SQL_Driver& fdriver;
	void *info;
	const String& furl;
};

#endif
