/** @file
	Parser: sql driver connection decl.
	global sql driver connection, must be thread-safe

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_db_connection.h,v 1.21 2002/02/08 08:30:12 paf Exp $
*/

#ifndef PA_DB_CONNECTION_H
#define PA_DB_CONNECTION_H

#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_db_table.h"
#include "pa_value.h"

// defines

// forwards

class DB_Table;
class DB_Connection_ptr;

/// sql driver connection
class DB_Connection : public Pooled {
	friend class DB_Table;
	friend class DB_Connection_ptr;
public:

	DB_Connection(Pool& apool, const String& db_home);
	~DB_Connection();

	bool expired(time_t older_dies) {
		return !used && time_used<older_dies;
	}
	const String& db_home() { return fdb_home; }
	time_t get_time_used() { return time_used; }
	int get_users_count() { return used; }

	/**
		connect to specified file_name
	*/
	DB_Table_ptr get_table_ptr(const String& file_name, const String *source);

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

public: 

	Value& get_status(Pool& pool, const String *source);
};

/// Auto-object used to track DB_Connection usage
class DB_Connection_ptr {
	DB_Connection *fconnection;
public:
	explicit DB_Connection_ptr(DB_Connection *aconnection) : fconnection(aconnection) {
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
