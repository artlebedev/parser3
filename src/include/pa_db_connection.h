/** @file
	Parser: sql db decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.h,v 1.1 2001/10/22 16:44:42 parser Exp $
*/

#ifndef PA_DB_CONNECTION_H
#define PA_DB_CONNECTION_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_db_manager.h"
#include "pa_globals.h"

#ifdef HAVE_DB_H
#	include <db.h>
#endif

// defines

#define PA_DB_ACCESS_METHOD DB_BTREE

// class

/// DB connection. handy wrapper around low level DB_Driver
class DB_Connection : public Pooled {

public:

	DB_Connection(Pool& pool, const String& afile_spec, DB_ENV& adbenv);
	
	//const String& url() { return ffile_spec; }

	void set_services(Pool *aservices_pool) {
		time_used=time(0); // they started to use at this time
		fservices_pool=aservices_pool;
	}
	bool expired(time_t older_dies) {
		return time_used<older_dies;
	}

	void close() {
		DB_manager->close_connection(ffile_spec, *this);
	}

	bool connected() { return db!=0; }
	void connect();

	/// @todo this one of reasons of not having ^try for now
	void DB_Connection::disconnect() { 
		check("close", &ffile_spec, db->close(db, 0/*flags*/));  db=0; 
	}
	
	void commit() { 
		if(tid) {
			check("txn_commit", &ffile_spec, txn_commit(tid)); tid=0;
		}
	}
	
	void rollback() {
		if(tid) {
			check("txn_abort", &ffile_spec, txn_abort(tid)); tid=0;
		}
	}
	
	void txn_begin() {
		check("txn_begin", &ffile_spec, ::txn_begin(fdbenv.tx_info, 0/*parent*/, &tid));
	}
	
	bool ping() { return !needs_recovery; }

	void put(const String& key, const String& data);
	String *get(const String& key);

private:

	DB_ENV& fdbenv;
	const String& ffile_spec;
	Pool *fservices_pool;
	DB *db;
	bool needs_recovery;
	DB_TXN *tid;
	time_t time_used;

private:

	void check(const char *operation, const String *source, int error);
	void *malloc(size_t size) { return fservices_pool->malloc(size); }
	void *calloc(size_t size) { return fservices_pool->calloc(size); }
	
};

#endif
