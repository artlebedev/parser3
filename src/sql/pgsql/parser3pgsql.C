/** @file
	Parser PgSQL driver.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	2001.07.30 using PgSQL 7.1.2
*/
static const char *RCSId="$Id: parser3pgsql.C,v 1.4 2001/07/31 12:44:36 parser Exp $"; 

#include "config_includes.h"

#include "pa_sql_driver.h"

#include <libpq-fe.h>
#include <libpq-fs.h>

// #include <catalog/pg_type.h>
#define OIDOID			26
#define LO_BUFSIZE		  8192


#include "ltdl.h"

#define MAX_STRING 0x400
#define MAX_NUMBER 20

#if _MSC_VER
#	define snprintf _snprintf
#	define strcasecmp _stricmp
#endif

#ifndef max
inline int max(int a,int b) { return a>b?a:b; }
inline int min(int a,int b){ return a<b?a:b; }
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
		PGconn *conn=PQsetdbLogin(
			strcasecmp(host, "local")==0?NULL/* local Unix domain socket */:host, port, 
			NULL, NULL, db, user, pwd);
		if(!conn)
			services._throw("PQsetdbLogin failed");
		if(PQstatus(conn)!=CONNECTION_OK)  
			services._throw(PQerrorMessage(conn));

		*(PGconn **)connection=conn;
		begin_transaction(services, conn);
	}
	void disconnect(SQL_Driver_services&, void *connection) {
//		_asm int 3;
	    PQfinish((PGconn *)connection);
	}
	void commit(SQL_Driver_services& services, void *connection) {
//		_asm int 3;
		PGconn *conn=(PGconn *)connection;
		if(PGresult *res=PQexec(conn, "COMMIT"))
			PQclear(res);
		else
			services._throw(PQerrorMessage(conn));
		begin_transaction(services, conn);
	}
	void rollback(SQL_Driver_services& services, void *connection) {
//		_asm int 3;
		PGconn *conn=(PGconn *)connection;
		if(PGresult *res=PQexec(conn, "ROLLBACK"))
			PQclear(res);
		else
			services._throw(PQerrorMessage(conn));
		begin_transaction(services, conn);
	}

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

		PGconn *conn=(PGconn *)connection;

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

		PGresult *res=PQexec(conn, statement);
		if(!res) 
			services._throw(PQerrorMessage(conn));

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
					const char *cell=PQgetvalue(res, r, i);
					size_t size;
					void *ptr;
					if(PQftype(res, i)==OIDOID) {
						// ObjectID column, read object bytes
//						_asm int 3;

						#define PQclear_n_throw { \
								PQclear(res); \
								services._throw(PQerrorMessage(conn)); \
							}
						
						char *error_pos=0;
						Oid oid=cell?atoi(cell):0;
						int fd=lo_open(conn, oid, INV_READ);
						if(fd>=0) {
							// seek to end
							if(lo_lseek(conn, fd, 0, SEEK_END)<0)
								PQclear_n_throw;
							// get size
							int size_tell=lo_tell(conn, fd);
							if(size_tell<0)
								PQclear_n_throw;
							// seek to begin
							if(lo_lseek(conn, fd, 0, SEEK_SET)<0)
								PQclear_n_throw;
							size=(size_t)size_tell;
							if(size) {
								// read 
								ptr=services.malloc(size);
								char *buf=(char *)ptr;
								int countdown=size_tell;
								int size_read;
								while(countdown && (size_read=lo_read(conn, fd, buf, min(LO_BUFSIZE, countdown)))>0) {
									buf+=size_read;
									countdown-=size_read;									
								}
								if(countdown) {
									PQclear(res);
									services._throw("lo_read can not read all of object bytes");
								}
							} else
								ptr=0;
							if(lo_close(conn, fd)<0)
								PQclear_n_throw;
						} else
							PQclear_n_throw;
					} else {
						// normal column, read it as ASCII string
						size=(size_t)PQgetlength(res, r, i);
						if(size) {
							ptr=services.malloc(size);
							memcpy(ptr, cell, size);
						} else
							ptr=0;
					}
					handlers.add_row_cell(ptr, size);
				}
			}

		PQclear(res);
	}

private: // private funcs

	void begin_transaction(SQL_Driver_services& services, PGconn *conn) {
		if(PGresult *res=PQexec(conn, "BEGIN"))
			PQclear(res);
		else
			services._throw(PQerrorMessage(conn));
	}

private: // conn client library funcs

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

	typedef Oid	(*t_PQftype)(const PGresult *res, int field_num); t_PQftype PQftype;

	typedef int	(*t_lo_open)(PGconn *conn, Oid lobjId, int mode); t_lo_open lo_open;
	typedef int	(*t_lo_close)(PGconn *conn, int fd); t_lo_close lo_close;
	typedef int	(*t_lo_read)(PGconn *conn, int fd, char *buf, size_t len); t_lo_read lo_read;
	typedef int	(*t_lo_write)(PGconn *conn, int fd, char *buf, size_t len); t_lo_write lo_write;
	typedef int	(*t_lo_lseek)(PGconn *conn, int fd, int offset, int whence); t_lo_lseek lo_lseek;
	typedef Oid	(*t_lo_creat)(PGconn *conn, int mode); t_lo_creat lo_creat;
	typedef int	(*t_lo_tell)(PGconn *conn, int fd); t_lo_tell lo_tell;
	typedef int	(*t_lo_unlink)(PGconn *conn, Oid lobjId); t_lo_unlink lo_unlink;
	typedef Oid	(*t_lo_import)(PGconn *conn, const char *filename); t_lo_import lo_import;
	typedef int	(*t_lo_export)(PGconn *conn, Oid lobjId, const char *filename); t_lo_export lo_export;

private: // conn client library funcs linking

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
		DLINK(PQftype);
		DLINK(lo_open);		DLINK(lo_close);
		DLINK(lo_read);		DLINK(lo_write);
		DLINK(lo_lseek);		DLINK(lo_creat);
		DLINK(lo_tell);		DLINK(lo_unlink);
		DLINK(lo_import);		DLINK(lo_export);

		return 0;
	}

};

extern "C" SQL_Driver *create() {
	return new PgSQL_Driver();
}