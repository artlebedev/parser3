/** @file
	Parser: sql db decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.h,v 1.2 2001/10/23 12:41:05 parser Exp $
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

// forwards

class Auto_transaction;

// class

/// DB connection. handy wrapper around low level DB_Driver
class DB_Connection : public Pooled {
	friend Auto_transaction;
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
	void disconnect();	
	bool ping() { return !needs_recovery; }

	void put(const String& key, const String& data);
	String *get(const String& key);
	void _delete(const String& key);

private:

	DB_ENV& fdbenv;
	const String& ffile_spec;
	Pool *fservices_pool;
	DB *db;
	bool needs_recovery;
	DB_TXN *ftid;
	time_t time_used;

private: // transaction

	/// commits current transaction, restores previous transaction handle
	void commit_restore(DB_TXN *atid) { 
		if(ftid)
			check("txn_commit", &ffile_spec, txn_commit(ftid)); 

		ftid=atid;
	}
	
	/// rolls current transaction back, restores previous transaction handle
	void rollback_restore(DB_TXN *atid) {
		if(ftid)
			check("txn_abort", &ffile_spec, txn_abort(ftid));

		ftid=atid;
	}
	
	/// stars new current trunsaction @returns previous transaction handle
	DB_TXN *transaction_begin_save() {
		DB_TXN *result=ftid;
		check("txn_begin", &ffile_spec, ::txn_begin(fdbenv.tx_info, 0/*parent*/, &ftid));

		return result;
	}
	
private:


	void check(const char *operation, const String *source, int error);
	void *malloc(size_t size) { return fservices_pool->malloc(size); }
	void *calloc(size_t size) { return fservices_pool->calloc(size); }

};

///	Auto-object used for temporary changing DB_Connection::tid.
class Auto_transaction {
	DB_Connection& fconnection;
	bool marked_to_rollback;
	DB_TXN *saved_tid;
public:
	Auto_transaction(DB_Connection& aconnection) : 
		fconnection(aconnection), marked_to_rollback(false),
		saved_tid(aconnection.transaction_begin_save()) {
	}
	~Auto_transaction() { 
		if(marked_to_rollback)
			fconnection.rollback_restore(saved_tid);
		else
			fconnection.commit_restore(saved_tid);
	}
	void mark_to_rollback() {
		marked_to_rollback=true;
	}
};

#endif
