/** @file
	Parser: sql db decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_table.h,v 1.4 2001/10/27 10:14:45 paf Exp $
*/

#ifndef PA_DB_TABLE_H
#define PA_DB_TABLE_H

#include "pa_config_includes.h"
#include "pa_globals.h"
#include "pa_pool.h"

#ifdef HAVE_DB_H
#	include <db.h>
#endif

// defines

#define PA_DB_ACCESS_METHOD DB_BTREE

// forwards

class DB_Connection;

class DB_Table_ptr;
class DB_Transaction;
class DB_Cursor;

// class

/// DB table. handy wrapper around low level <db.h> calls
class DB_Table : public Pooled {
	friend DB_Table_ptr;
	friend DB_Transaction;
	friend DB_Cursor;
public:

	DB_Table(Pool& apool, const String& afile_name, DB_Connection& aconnection);
	~DB_Table();

	const String& file_name() { return ffile_name; }

	bool expired(time_t older_dies) {
		return !used && time_used<older_dies;
	}

	void put(DB_Transaction *t, const String& key, const String& data, time_t time_to_die);
	String *get(DB_Transaction *t, Pool& pool, const String& key);
	void remove(DB_Transaction *t, const String& key);

private: // table usage methods

	void use();
	void unuse();

private: // table usage data

	int used;

private:

	DB_Connection& fconnection;
	DB_ENV& dbenv;
	const String& ffile_name; const char *file_name_cstr;
	Pool *fservices_pool;
	DB *db;
	time_t time_used;

private:

	void check(const char *operation, const String *source, int error);
	void *malloc(size_t size) { return fservices_pool->malloc(size); }
	void *calloc(size_t size) { return fservices_pool->calloc(size); }
	/// pass empty dbt, would fill it from string
	void key_string_to_dbt(const String& key_string, DBT& key_result);
	/// @returns new string
	String& key_dbt_to_string(Pool& pool, const DBT& key_dbt);
	/// pass empty dbt, would fill it from string
	void data_string_to_dbt(const String& data_string,  time_t time_to_die, 
		DBT& data_result);
	/// @returns new string if it not expired
	String *data_dbt_to_string(Pool& pool, const DBT& data_dbt);

};

/// Auto-object used to track DB_Table usage
class DB_Table_ptr {
	DB_Table *ftable;
public:
	explicit DB_Table_ptr(DB_Table *atable) : ftable(atable) {
		ftable->use();
	}
	~DB_Table_ptr() {
		ftable->unuse();
	}
	DB_Table* operator->() {
		return ftable;
	}
	DB_Table& operator*() {
		return *ftable;
	}

	// copying
	DB_Table_ptr(const DB_Table_ptr& src) : ftable(src.ftable) {
		ftable->use();
	}
	DB_Table_ptr& operator =(const DB_Table_ptr& src) {
		// may do without this=src check
		ftable->unuse();
		ftable=src.ftable;
		ftable->use();

		return *this;
	}
};

///	Auto-object used for temporary changing DB_Table::tid.
class DB_Transaction {
public:

	DB_Transaction(Pool& apool, DB_Table& atable, DB_Transaction *& aparent_ref);
	~DB_Transaction();
	DB_TXN *id() { return fid; }
	void mark_to_rollback();

	void put(const String& key, const String& data, time_t time_to_die) {
		ftable.put(this, key, data, time_to_die);
	}
	String *get(const String& key) {
		return ftable.get(this, fpool, key);
	}
	void remove(const String& key) {
		ftable.remove(this, key);
	}

private:

	void check(const char *operation, const String *source, int error) {
		ftable.check(operation, source, error);
	}

private:

	Pool& fpool;
	DB_Table& ftable;
	DB_Transaction *parent;
	DB_Transaction *& fparent_ref;
	DB_TXN *fid;
	bool marked_to_rollback;

};

/// DB cursor. handy wrapper around low level <db.h> calls
class DB_Cursor {
public:
	DB_Cursor(DB_Table& atable, DB_Transaction *transaction, const String *asource);
	~DB_Cursor();
	/// pass empty strings to key&data, would fill them
	bool get(Pool& pool, String *& key, String *& data, u_int32_t flags);
	bool move(u_int32_t flags);
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
	String& key_dbt_to_string(Pool& pool, DBT& key_dbt) {
		return ftable.key_dbt_to_string(pool, key_dbt);
	}
	/// @returns new string if it not expired
	String *data_dbt_to_string(Pool& pool, const DBT& data_dbt) {	
		return ftable.data_dbt_to_string(pool, data_dbt);
	}
};

#endif
