/** @file
	Parser: sql db decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_table.h,v 1.1 2001/10/25 13:18:19 paf Exp $
*/

#ifndef PA_DB_TABLE_H
#define PA_DB_TABLE_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_db_connection.h"
#include "pa_globals.h"

#ifdef HAVE_DB_H
#	include <db.h>
#endif

// defines

#define PA_DB_ACCESS_METHOD DB_BTREE

// forwards

class Auto_transaction;
class DB_Cursor;

// class

/// DB table. handy wrapper around low level <db.h> calls
class DB_Table : public Pooled {
	friend Auto_transaction;
	friend DB_Cursor;
public:

	DB_Table(Pool& pool, const String& afile_spec, DB_Connection& aconnection);

	void set_services(Pool *aservices_pool) {
		time_used=time(0); // they started to use at this time
		fservices_pool=aservices_pool;
	}
	bool expired(time_t older_dies) {
		return time_used<older_dies;
	}

	void close() {
		fconnection.close_table(ffile_spec, *this);
	}

	bool connected() { return db!=0; }
	void connect();
	void disconnect();	
	bool ping() { return errors==0; }

	void put(const String& key, const String& data, time_t time_to_die);
	String *get(const String& key);
	void remove(const String& key);

private:

	DB_Connection& fconnection;
	DB_ENV& dbenv;
	const String& ffile_spec; const char *file_spec_cstr;
	Pool *fservices_pool;
	DB *db;
	int errors;
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
		DB_TXN *parent=ftid;
		check("txn_begin", &ffile_spec, ::txn_begin(dbenv.tx_info, parent, &ftid));

		return parent;
	}
	
private:

	void check(const char *operation, const String *source, int error);
	void *malloc(size_t size) { return fservices_pool->malloc(size); }
	void *calloc(size_t size) { return fservices_pool->calloc(size); }
	/// pass empty dbt, would fill it from string
	void key_string_to_dbt(const String& key_string, DBT& key_result);
	/// @returns new string
	String& key_dbt_to_string(const DBT& key_dbt);
	/// pass empty dbt, would fill it from string
	void data_string_to_dbt(const String& data_string,  time_t time_to_die, 
		DBT& data_result);
	/// @returns new string if it not expired
	String *data_dbt_to_string(const DBT& data_dbt);

};

///	Auto-object used for temporary changing DB_Table::tid.
class Auto_transaction {
	DB_Table& ftable;
	bool marked_to_rollback;
	DB_TXN *saved_tid;
public:
	Auto_transaction(DB_Table& atable) : 
		ftable(atable), marked_to_rollback(false),
		saved_tid(atable.transaction_begin_save()) {
	}
	~Auto_transaction() { 
		if(marked_to_rollback)
			ftable.rollback_restore(saved_tid);
		else
			ftable.commit_restore(saved_tid);
	}
	void mark_to_rollback() {
		marked_to_rollback=true;
	}
};

/// DB cursor. handy wrapper around low level <db.h> calls
class DB_Cursor {
	friend DB_Table;
public:
	DB_Cursor(DB_Table& atable, const String *asource);
	~DB_Cursor();
	/// pass empty strings to key&data, would fill them
	bool get(String *& key, String *& data, u_int32_t flags);
	void remove(u_int32_t flags);
private:
	const String *fsource;
	DB_Table& ftable;
	DBC *cursor;
private:
	void check(const char *operation, const String *source, int error) {
		ftable.check(operation, source, error);
	}
	/// @returns new string
	String& key_dbt_to_string(DBT& key_dbt) {
		return ftable.key_dbt_to_string(key_dbt);
	}
	/// @returns new string if it not expired
	String *data_dbt_to_string(const DBT& data_dbt) {	
		return ftable.data_dbt_to_string(data_dbt);
	}
};

#endif
