/** @file
	Parser ODBC driver.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)
*/
static const char *RCSId="$Id: parser3odbc.C,v 1.3 2001/08/24 07:04:25 parser Exp $"; 

#ifndef _MSC_VER
#	error compile ISAPI module with MSVC [no urge for now to make it autoconf-ed (PAF)]
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "pa_sql_driver.h"

#include <AFXDB.H>

#define MAX_STRING 0x400

#define snprintf _snprintf
#ifndef strncasecmp
#	define strncasecmp _strnicmp
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
	ODBC server driver
*/
class ODBC_Driver : public SQL_Driver {
public:

	ODBC_Driver() : SQL_Driver() {
	}

	/// get api version
	int api_version() { return SQL_DRIVER_API_VERSION; }
	const char *initialize(const char *dlopen_file_spec) { return 0; }
	/**	connect
		@param used_only_in_connect_url
			format: @b DSN=dsn;UID=user;PWD=password (ODBC connect string)
			WARNING: must be used only to connect, for buffer doesn't live long
	*/
	void connect(
		char *used_only_in_connect_url, 
		SQL_Driver_services& services, 
		void **connection ///< output: CDatabase *
		) {
	//	_asm int 3;
		CDatabase *db;
		TRY {
			db=new CDatabase();
			db->OpenEx(used_only_in_connect_url, CDatabase::noOdbcDialog);
			db->BeginTrans();
		} 
		CATCH_ALL (e) {
			_throw(services, e);
			db=0; // calm, compiler
		}
		END_CATCH_ALL

		*(CDatabase **)connection=db;
	}
	void disconnect(void *connection) {
		CDatabase *db=static_cast<CDatabase *>(connection);
		TRY
			delete db;
		CATCH_ALL (e) {
			// nothing
		}
		END_CATCH_ALL
	}
	void commit(SQL_Driver_services& services, void *connection) {
		CDatabase *db=static_cast<CDatabase *>(connection);
		TRY
			db->CommitTrans();
			db->BeginTrans();
		CATCH_ALL (e) {
			_throw(services, e);
		}
		END_CATCH_ALL
	}
	void rollback(SQL_Driver_services& services, void *connection) {
		CDatabase *db=static_cast<CDatabase *>(connection);
		TRY
			db->Rollback();
			db->BeginTrans();
		CATCH_ALL (e) {
			_throw(services, e);
		}
		END_CATCH_ALL
	}

	bool ping(SQL_Driver_services&, void *connection) {
		return true;
	}

	unsigned int quote(
		SQL_Driver_services&, void *connection,
		char *to, const char *from, unsigned int length) {
		/*
			You must allocate the to buffer to be at least length*2+1 bytes long. 
			(In the worse case, each character may need to be encoded as using two bytes, 
			and you need room for the terminating null byte.)

			it's already UNTAINT_TIMES_BIGGER
		*/
		// ' -> ''
		unsigned int result=length;
		while(length--) {
			if(*from=='\'')
				*to++='\'';
			*to++=*from++;
		}
		return result;
	}
	void query(
		SQL_Driver_services& services, void *connection, 
		const char *statement, unsigned long offset, unsigned long limit,
		SQL_Driver_query_event_handlers& handlers) {

		CDatabase *db=static_cast<CDatabase *>(connection);

		while(isspace(*statement)) 
			statement++;
		
		TRY {
			if(strncasecmp(statement, "select", 6)==0) {
				CRecordset rs(db); 
				rs.Open(
					CRecordset::forwardOnly, 
					statement,
					CRecordset::executeDirect   
					);

				int column_count=rs.GetODBCFieldCount();
				if(!column_count)
					services._throw("result contains no columns");

				for(int i=0; i<column_count; i++){
					CString string;
					CODBCFieldInfo fieldinfo;
					rs.GetODBCFieldInfo(i, fieldinfo);
					size_t size=fieldinfo.m_strName.GetLength();
					void *ptr=0;
					if(size) {
						ptr=services.malloc(size);
						memcpy(ptr, (char *)LPCTSTR(fieldinfo.m_strName), size);
					}
					handlers.add_column(ptr, size);
				}

				handlers.before_rows();

				unsigned long row=0;
				while(!rs.IsEOF() && (!limit||(row<offset+limit))) {
					if(row>=offset) {
						handlers.add_row();
						for(int i=0; i<column_count; i++) 						{
							CString string;
							rs.GetFieldValue(i, string);
							size_t size=string.GetLength();
							void *ptr=0;
							if(size) {
								ptr=services.malloc(size);
								memcpy(ptr, (char *)LPCTSTR(string), size);
							}
							handlers.add_row_cell(ptr, size);
						}
					}
					rs.MoveNext();  row++;
				}
				
				rs.Close();
			} else {
				db->ExecuteSQL(statement);
			}
		} 
		CATCH_ALL (e) {
			_throw(services, e);
		}
		END_CATCH_ALL
	}

	void _throw(SQL_Driver_services& services, CException *e) {
		char szCause[MAX_STRING]; szCause[0]=0;
		e->GetErrorMessage(szCause, MAX_STRING);
		char msg[MAX_STRING];
		snprintf(msg, MAX_STRING, "%s: %s",
			e->GetRuntimeClass()->m_lpszClassName,
			*szCause?szCause:"unknown");
		services._throw(msg);
	}

};

extern "C" SQL_Driver *create() {
	return new ODBC_Driver();
}