/** @file
	Parser PgSQL driver.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	2001.07.30 using PgSQL 7.1.2
*/
static const char *RCSId="$Id: parser3pgsql.C,v 1.3 2001/07/31 07:58:47 parser Exp $"; 

#include "config_includes.h"

#include "pa_sql_driver.h"

#include <libpq-fe.h>
#include "ltdl.h"

#define MAX_STRING 0x400
#define MAX_NUMBER 20

#if _MSC_VER
#	define snprintf _snprintf
#	define strcasecmp _stricmp
#endif

static char *lsplit(char *string, char delim) {
    if(string) {
		char *v=strchr(string, delim);
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return 0;
}

/**
	PgSQL server driver
*/
class PgSQL_Driver : public SQL_Driver {
public:

	PgSQL_Driver() : SQL_Driver() {
	}

	/// get api version
	int api_version() { return SQL_DRIVER_API_VERSION; }
	/// initialize driver by loading sql dynamic link library
	const char *initialize(const char *dlopen_file_spec) {
		return dlopen_file_spec?
			dlink(dlopen_file_spec):"client library column is empty";
	}
	/**	connect
		@param used_only_in_connect_url
			format: @b user:pass@host[:port]|[local]/database
			3.23.22b
			Currently the only option for @b character_set_name is cp1251_koi8.
			WARNING: must be used only to connect, for buffer doesn't live long
	*/
	void connect(
		char *used_only_in_connect_url, 
		SQL_Driver_services& services, 
		void **connection ///< output: PGconn *
		) {
		char *user=used_only_in_connect_url;
		char *host=lsplit(user, '@');
		char *db=lsplit(host, '/');
		char *pwd=lsplit(user, ':');
		char *port=lsplit(host, ':');

//		_asm  int 3; 
		PGconn *pgsql=PQsetdbLogin(
			strcasecmp(host, "local")==0?NULL/* local Unix domain socket */:host, port, 
			NULL, NULL, db, user, pwd);
		if(!pgsql)
			services._throw("PQsetdbLogin failed");
		if(PQstatus(pgsql)!=CONNECTION_OK)  
			services._throw(PQerrorMessage(pgsql));

		*(PGconn **)connection=pgsql;
	}
	void disconnect(SQL_Driver_services&, void *connection) {
	    PQfinish((PGconn *)connection);
	}
	void commit(SQL_Driver_services&, void *) {}
	void rollback(SQL_Driver_services&, void *) {}

	bool ping(SQL_Driver_services&, void *connection) {
		return PQstatus((PGconn *)connection)==CONNECTION_OK;
	}

	unsigned int quote(
		SQL_Driver_services&, void *connection,
		char *to, const char *from, unsigned int length) {
		/*
			3.23.22b
			You must allocate the to buffer to be at least length*2+1 bytes long. 
			(In the worse case, each character may need to be encoded as using two bytes, 
			and you need room for the terminating null byte.)

			it's already UNTAINT_TIMES_BIGGER
		*/
		unsigned int result=length;
		while(length--) {
			switch(*from) {
			case '\'': // "'" -> "''"
				*to++='\'';
				break;
			case '\\': // "\" -> "\\"
				*to++='\'';
				break;
			}
			*to++=*from++;
		}
		return result;
	}
	void query(
		SQL_Driver_services& services, void *connection, 
		const char *astatement, unsigned long offset, unsigned long limit,
		SQL_Driver_query_event_handlers& handlers) {

		PGconn *pgsql=(PGconn *)connection;

		const char *statement;
		if(offset || limit) {
			size_t statement_size=strlen(astatement);
			char *statement_limited=(char *)services.malloc(
				statement_size+MAX_NUMBER*2+15/* limit # offset #*/+1);
			char *cur=statement_limited;
			memcpy(cur, astatement, statement_size); cur+=statement_size;
			if(limit)
				cur+=snprintf(cur, 7+MAX_NUMBER, " limit %u", limit);
			if(offset)
				cur+=snprintf(cur, 8+MAX_NUMBER, " offset %u", offset);
			statement=statement_limited;
		} else
			statement=astatement;

		PGresult *res=PQexec(pgsql, statement);
		if(!res) 
			services._throw(PQerrorMessage(pgsql));

		switch(PQresultStatus(res)) {
		case PGRES_EMPTY_QUERY: 
			PQclear(res);
			services._throw("no query");
			break;
		case PGRES_COMMAND_OK:
			// empty result: insert|delete|update|...
			return;
		case PGRES_TUPLES_OK: 
			break;	
		default:
			PQclear(res);
			services._throw("unknown PQexec error"); 
			break;
		}
		
		int column_count=PQnfields(res);
		if(!column_count) {
			PQclear(res);
			services._throw("result contains no columns");
		}

		for(int i=0; i<column_count; i++){
			char *name=PQfname(res, i);
			size_t size=strlen(name);
			void *ptr=services.malloc(size);
			memcpy(ptr, name, size);
			handlers.add_column(ptr, size);
		}

		handlers.before_rows();
		
		if(unsigned long row_count=(unsigned long)PQntuples(res))
			for(unsigned long r=0; r<row_count; r++) {
				handlers.add_row();
				for(int i=0; i<column_count; i++){
					size_t size=(size_t)PQgetlength(res, r, i);
					void *ptr;
					if(size) {
						ptr=services.malloc(size);
						memcpy(ptr, PQgetvalue(res, r, i), size);
					} else
						ptr=0;
					handlers.add_row_cell(ptr, size);
				}
			}

		PQclear(res);
	}

private: // pgsql client library funcs

	typedef PGconn* (*t_PQsetdbLogin)(
		const char *pghost,
		const char *pgport,
		const char *pgoptions,
		const char *pgtty,
		const char *dbName,
		const char *login,
		const char *pwd); t_PQsetdbLogin PQsetdbLogin;
	typedef void (*t_PQfinish)(PGconn *conn);  t_PQfinish PQfinish;
	typedef char *(*t_PQerrorMessage)(const PGconn* conn); t_PQerrorMessage PQerrorMessage;
	typedef ConnStatusType (*t_PQstatus)(const PGconn *conn); t_PQstatus PQstatus;
	typedef PGresult *(*t_PQexec)(PGconn *conn,
	                 const char *query); t_PQexec PQexec;
	typedef ExecStatusType (*t_PQresultStatus)(const PGresult *res); t_PQresultStatus PQresultStatus;
	typedef int (*t_PQgetlength)(const PGresult *res,
					int tup_num,
					int field_num); t_PQgetlength PQgetlength;
	typedef char* (*t_PQgetvalue)(const PGresult *res,
					 int tup_num,
					 int field_num); t_PQgetvalue PQgetvalue;
	typedef int (*t_PQntuples)(const PGresult *res); t_PQntuples PQntuples;
	typedef char *(*t_PQfname)(const PGresult *res,
						int field_index); t_PQfname PQfname;
	typedef int (*t_PQnfields)(const PGresult *res); t_PQnfields PQnfields;
	typedef void (*t_PQclear)(PGresult *res); t_PQclear PQclear;


private: // pgsql client library funcs linking

	const char *dlink(const char *dlopen_file_spec) {
        lt_dlhandle handle=lt_dlopen(dlopen_file_spec);
        if(!handle)
			return "can not open the dynamic link module";

		#define DSLINK(name, action) \
			name=(t_##name)lt_dlsym(handle, #name); \
				if(!name) \
					action;

		#define DLINK(name) DSLINK(name, return "function " #name " was not found")
		
		DLINK(PQsetdbLogin);
		DLINK(PQerrorMessage);
		DLINK(PQstatus);
		DLINK(PQfinish);
		DLINK(PQgetvalue);
		DLINK(PQgetlength);
		DLINK(PQntuples);
		DLINK(PQfname);
		DLINK(PQnfields);
		DLINK(PQclear);
		DLINK(PQresultStatus);
		DLINK(PQexec);

		return 0;
	}

};

extern "C" SQL_Driver *create() {
	return new PgSQL_Driver();
}