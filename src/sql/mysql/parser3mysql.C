/** @file
	Parser MySQL driver.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)
*/
static const char *RCSId="$Id: parser3mysql.C,v 1.26 2001/07/23 13:59:52 parser Exp $"; 

#include "config_includes.h"

#include "pa_sql_driver.h"

#define NO_CLIENT_LONG_LONG
#include "mysql.h"
#include "ltdl.h"

#define MAX_STRING 0x400
#define MAX_NUMBER 20

#if _MSC_VER
#	define snprintf _snprintf
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
	MySQL server driver
*/
class MySQL_Driver : public SQL_Driver {
public:

	MySQL_Driver() : SQL_Driver() {
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
			format: @b user:pass@host[:port]|[/unix/socket]/database/charset 
			3.23.22b
			Currently the only option for @b character_set_name is cp1251_koi8.
			WARNING: must be used only to connect, for buffer doesn't live long
	*/
	void connect(
		char *used_only_in_connect_url, 
		SQL_Driver_services& services, 
		void **connection ///< output: MYSQL *
		) {
		char *user=used_only_in_connect_url;
		char *s=lsplit(user, '@');
		char *host=0;
		char *unix_socket=0;
		if(s && s[0]=='[') { // unix socket
			unix_socket=1+s;
			s=lsplit(unix_socket, ']');
		} else { // IP
			host=s;
		}
		char *db=lsplit(s, '/');
		char *pwd=lsplit(user, ':');
		char *error_pos=0;
		char *port_cstr=lsplit(host, ':');
		int port=port_cstr?strtol(port_cstr, &error_pos, 0):0;
		char *charset=lsplit(db, '/');

	    MYSQL *mysql=mysql_init(NULL);
		if(!mysql_real_connect(mysql, 
			host, user, pwd, db, port?port:MYSQL_PORT, unix_socket, 0))
			services._throw(mysql_error(mysql));

		if(charset) { 
			// set charset
			char statement[MAX_STRING]="set CHARACTER SET "; // cp1251_koi8
			strncat(statement, charset, MAX_STRING);
			
			if(mysql_query(mysql, statement)) 
				services._throw(mysql_error(mysql));
			(*mysql_store_result)(mysql); // throw out the result [don't need but must call]
		}

		*(MYSQL **)connection=mysql;
	}
	void disconnect(SQL_Driver_services&, void *connection) {
	    mysql_close((MYSQL *)connection);
	}
	void commit(SQL_Driver_services&, void *) {}
	void rollback(SQL_Driver_services&, void *) {}

	bool ping(SQL_Driver_services&, void *connection) {
		return mysql_ping((MYSQL *)connection)==0;
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
		return (*mysql_escape_string)(to, from, length);
	}
	void query(
		SQL_Driver_services& services, void *connection, 
		const char *astatement, unsigned long offset, unsigned long limit,
		SQL_Driver_query_event_handlers& handlers) {

		MYSQL *mysql=(MYSQL *)connection;
		MYSQL_RES *res=NULL;

		const char *statement;
		if(offset || limit) {
			size_t statement_size=strlen(astatement);
			char *statement_limited=(char *)services.malloc(
				statement_size+MAX_NUMBER*2+8/* limit #,#*/+1);
			char *cur=statement_limited;
			memcpy(cur, astatement, statement_size); cur+=statement_size;
			cur+=sprintf(cur, " limit ");
			if(offset)
				cur+=snprintf(cur, MAX_NUMBER+1, "%u,", offset);
			if(limit)
				cur+=snprintf(cur, MAX_NUMBER, "%u", limit);
			statement=statement_limited;
		} else
			statement=astatement;

		if(mysql_query(mysql, statement)) 
			services._throw(mysql_error(mysql));
		if(!(res=mysql_store_result(mysql)) && mysql_field_count(mysql)) 
			services._throw(mysql_error(mysql));
		if(!res) // empty result: insert|delete|update|...
			return;
		
		int column_count=mysql_num_fields(res);
		if(!column_count) // old client
			column_count=mysql_field_count(mysql);

		if(!column_count)
			services._throw("result contains no columns");

		for(int i=0; i<column_count; i++){
			MYSQL_FIELD *field=mysql_fetch_field(res);
			size_t size=strlen(field->name);
			void *ptr=services.malloc(size);
			memcpy(ptr, field->name, size);
			handlers.add_column(ptr, size);
		}

		handlers.before_rows();
		
		if(unsigned long row_count=(unsigned long)mysql_num_rows(res))
			for(unsigned long r=0; r<row_count; r++) 
				if(MYSQL_ROW mysql_row=mysql_fetch_row(res)) { // never false..
					handlers.add_row();
					unsigned long *lengths=mysql_fetch_lengths(res);
					for(int i=0; i<column_count; i++){
						size_t size=(size_t)lengths[i];
						void *ptr;
						if(size) {
							ptr=services.malloc(size);
							memcpy(ptr, mysql_row[i], size);
						} else
							ptr=0;
						handlers.add_row_cell(ptr, size);
					}
				}

		mysql_free_result(res);
	}

private: // mysql client library funcs

	typedef MYSQL* (STDCALL *t_mysql_init)(MYSQL *); 	t_mysql_init mysql_init;
	
	typedef MYSQL_RES* (STDCALL *t_mysql_store_result)(MYSQL *); t_mysql_store_result mysql_store_result;
	
	typedef int		(STDCALL *t_mysql_query)(MYSQL *, const char *q); t_mysql_query mysql_query;
	
	typedef char * (STDCALL *t_mysql_error)(MYSQL *); t_mysql_error mysql_error;
	static char* STDCALL subst_mysql_error(MYSQL *mysql) { return (mysql)->net.last_error; }

	typedef MYSQL*		(STDCALL *t_mysql_real_connect)(MYSQL *, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned int clientflag); t_mysql_real_connect mysql_real_connect;

	typedef void		(STDCALL *t_mysql_close)(MYSQL *); t_mysql_close mysql_close;
	
	typedef int		(STDCALL *t_mysql_ping)(MYSQL *); t_mysql_ping mysql_ping;
	
	typedef unsigned long	(STDCALL *t_mysql_escape_string)(char *to,const char *from,
					    unsigned long from_length); t_mysql_escape_string mysql_escape_string;
	
	typedef void		(STDCALL *t_mysql_free_result)(MYSQL_RES *result); t_mysql_free_result mysql_free_result;
	
	typedef unsigned long* (STDCALL *t_mysql_fetch_lengths)(MYSQL_RES *result); t_mysql_fetch_lengths mysql_fetch_lengths;
	
	typedef MYSQL_ROW	(STDCALL *t_mysql_fetch_row)(MYSQL_RES *result); t_mysql_fetch_row mysql_fetch_row;
	
	typedef MYSQL_FIELD*	(STDCALL *t_mysql_fetch_field)(MYSQL_RES *result); t_mysql_fetch_field mysql_fetch_field;
	
	typedef my_ulonglong (STDCALL *t_mysql_num_rows)(MYSQL_RES *); t_mysql_num_rows mysql_num_rows;
	static my_ulonglong STDCALL subst_mysql_num_rows(MYSQL_RES *res) { return res->row_count; }
	
	typedef unsigned int (STDCALL *t_mysql_num_fields)(MYSQL_RES *); t_mysql_num_fields mysql_num_fields;
	static unsigned int STDCALL subst_mysql_num_fields(MYSQL_RES *res) { return res->field_count; }

	typedef unsigned int (STDCALL *t_mysql_field_count)(MYSQL *); t_mysql_field_count mysql_field_count;
	static unsigned int STDCALL subst_mysql_field_count(MYSQL *mysql) { return mysql->field_count; }

private: // mysql client library funcs linking

	const char *dlink(const char *dlopen_file_spec) {
        lt_dlhandle handle=lt_dlopen(dlopen_file_spec);
        if (!handle)
			return "can not open the dynamic link module";

		#define DSLINK(name, action) \
			name=(t_##name)lt_dlsym(handle, #name); \
				if(!name) \
					action;

		#define DLINK(name) DSLINK(name, return "function " #name " was not found")
		#define SLINK(name) DSLINK(name, name=subst_##name)
		
		DLINK(mysql_init);
		DLINK(mysql_store_result);
		DLINK(mysql_query);
		SLINK(mysql_error);
		DLINK(mysql_real_connect);
		DLINK(mysql_close);
		DLINK(mysql_ping);
		DLINK(mysql_escape_string);
		DLINK(mysql_free_result);
		DLINK(mysql_fetch_lengths);
		DLINK(mysql_fetch_row);
		DLINK(mysql_fetch_field);
		SLINK(mysql_num_rows);
		SLINK(mysql_num_fields);
		SLINK(mysql_field_count);
		return 0;
	}

};

extern "C" SQL_Driver *create() {
	return new MySQL_Driver();
}