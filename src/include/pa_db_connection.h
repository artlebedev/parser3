/** @file
	Parser: sql driver connection decl.
	global sql driver connection, must be thread-safe

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_db_connection.h,v 1.11 2001/10/26 16:27:17 paf Exp $
*/

#ifndef PA_DB_CONNECTION_H
#define PA_DB_CONNECTION_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_db_table.h"

#ifdef HAVE_DB_H
#	include <db.h>
#endif

// defines

// forwards

class DB_Table;
class DB_Connection_ptr;

/// sql driver connection
class DB_Connection : public Pooled {
	friend DB_Table;
	friend DB_Connection_ptr;
public:

	DB_Connection(Pool& apool, const String& db_home);
	~DB_Connection();

	bool expired(time_t older_dies) {
		return !used && time_used<older_dies;
	}

	/**
		connect to specified file_name, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	DB_Table_ptr get_table_ptr(const String& file_name, const String *source);
	void clear_dbfile(const String& file_name);

private: // connection usage methods

	void use() {
		time_used=time(0); // they started to use at this time
		used++;
	}
	void unuse() {
		used--;
	}

private: // connection usage data

	int used;

private: // table cache

	DB_Table *get_table_from_cache(const String& file_name);
	void put_table_to_cache(const String& file_name, DB_Table& table);
	void maybe_expire_table_cache();
private:
	time_t prev_expiration_pass_time;

private: // for DB_Table

	/// caches table
	void close_table(const String& file_name, DB_Table& table);

private:

	time_t time_used;

	const String& fdb_home;
	DB_ENV dbenv;
	Hash table_cache;

private:

	void check(const char *operation, const String *source, int error);
};

/// Auto-object used to track DB_Connection usage
class DB_Connection_ptr {
	DB_Connection *fconnection;
public:
	DB_Connection_ptr(DB_Connection *aconnection) : fconnection(aconnection) {
		fconnection->use();
	}
	~DB_Connection_ptr() {
		fconnection->unuse();
	}
	DB_Connection* operator->() {
		return fconnection;
	}

	// copying
	DB_Connection_ptr(const DB_Connection_ptr& src) : fconnection(src.fconnection) {
		fconnection->use();
	}
	DB_Connection_ptr& operator =(const DB_Connection_ptr& src) {
		// may do without this=src check
		fconnection->unuse();
		fconnection=src.fconnection;
		fconnection->use();

		return *this;
	}
};

#endif
