/** @file
	Parser Oracle driver.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	2001.07.30 using Oracle 8.1.6 [@test tested with Oracle 7.x.x]
*/
static const char *RCSId="$Id: parser3oracle.C,v 1.12 2001/08/24 07:14:36 parser Exp $"; 

#include "config_includes.h"

#include "pa_sql_driver.h"

#include <oci.h>

#define MAX_COLS 500
#define MAX_IN_LOBS 5
#define MAX_LOB_NAME_LENGTH 100
#define MAX_OUT_STRING_LENGTH 4000

#define EMPTY_CLOB_FUNC_CALL "empty_clob()"

#include "ltdl.h"

#define MAX_STRING 0x400
#define MAX_NUMBER 20

#if _MSC_VER
#	define snprintf _snprintf
#	define strcasecmp _stricmp
#	define strncasecmp _strnicmp
#	define STOP _asm int 3
#endif

#ifndef max
inline int max(int a, int b) { return a>b?a:b; }
inline int min(int a, int b){ return a<b?a:b; }
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

struct OracleSQL_connection_struct {
	jmp_buf mark; char error[MAX_STRING];
	OCIEnv *envhp;
	OCIServer *srvhp;
	OCIError *errhp;
	OCISvcCtx *svchp;
	OCISession *usrhp;
};

struct OracleSQL_query_lobs {
	struct return_rows {
		struct return_row {
			OCILobLocator *locator; ub4 len;
			int ind;		
			ub2 rcode;
		} *row;
		int count;
	};
	struct cbf_context_struct {
		SQL_Driver_services *services;
		OracleSQL_connection_struct *cs;
		return_rows *rows;
	};
	struct Item {
		const char *name_ptr; size_t name_size;
		char *data_ptr; size_t data_size;
		OCILobLocator *locator;
		OCIBind *bind;
		return_rows rows;
	} items[MAX_IN_LOBS];
	int count;
};


// forwards
void check(
		   SQL_Driver_services& services, OracleSQL_connection_struct &cs, 
		   const char *step, sword status);
static sb4 cbf_no_data(
					   dvoid *ctxp, 
					   OCIBind *bindp, 
					   ub4 iter, ub4 index, 
					   dvoid **bufpp, 
					   ub4 *alenpp, 
					   ub1 *piecep, 
					   dvoid **indpp);
static sb4 cbf_get_data(dvoid *ctxp, 
						OCIBind *bindp, 
						ub4 iter, ub4 index, 
						dvoid **bufpp, 
						ub4 **alenp, 
						ub1 *piecep, 
						dvoid **indpp, 
						ub2 **rcodepp);
void tolower(char *out, const char *in, size_t size);

/**
	OracleSQL server driver
*/
class OracleSQL_Driver : public SQL_Driver {
public:

	OracleSQL_Driver() : SQL_Driver() {
	}

	/// get api version
	int api_version() { return SQL_DRIVER_API_VERSION; }
	/// initialize driver by loading sql dynamic link library
	const char *initialize(const char *dlopen_file_spec) {
		const char *error=dlopen_file_spec?
			dlink(dlopen_file_spec):"client library column is empty";
		if(!error) {
			OCIInitialize((ub4)OCI_THREADED, (dvoid *)0, 
				(dvoid * (*)(void *, unsigned int))0, 
				(dvoid * (*)(void*, void*, unsigned int))0,  
				(void (*)(void*, void*))0 );
		}

		return error;
	}

	/**	connect
		@param used_only_in_connect_url
			format: @b user:pass@service
	*/
	void connect(
		char *used_only_in_connect_url, 
		SQL_Driver_services& services, 
		void **connection ///< output: OracleSQL_connection_struct *
		) {
		// connections are cross-request, do not use services._alloc [linked with request]
		OracleSQL_connection_struct &cs=
			*(OracleSQL_connection_struct  *)::calloc(sizeof(OracleSQL_connection_struct), 1);

		char *user=used_only_in_connect_url;
		char *service=lsplit(user, '@');
		char *pwd=lsplit(user, ':');

		if(!(user && pwd && service))
			services._throw("mailformed connect part, must be 'user:pass@service'");

		if(setjmp(cs.mark))
			services._throw(cs.error);

		// Allocate and initialize OCIError handle, attempt #1
		/*
			grabbed from sample 
			/server.804/a58234/oci_func.htm#446192
			but doc 
			/server.804/a58234/oci_func.htm#446100
			doesnt have this param listed as allowed
			8.1.6 client library barks as OCI_INVALID_HANDLE
			and debugging revealed that OCI_HTYPE_ENV param value is invalid
			later in doc 
			/server.804/a58234/oci_func.htm#446192
			on OCIEnvInit thay say
			"No changes are done to an already initialized handle"
			think, this is some sort of backward compatibility wonder.
			leaving as it is, and without check()
		*/
		OCIHandleAlloc((dvoid *)NULL, (dvoid **) &cs.envhp, (ub4)OCI_HTYPE_ENV, 0, 0);
		// Initialize an environment handle, attempt #2
		check(services, cs, "EnvInit", OCIEnvInit(
			&cs.envhp, (ub4)OCI_DEFAULT, 0, 0));		
		// Allocate and initialize OCIError handle
		check(services, cs, "HandleAlloc errhp", OCIHandleAlloc( 
			(dvoid *)cs.envhp, (dvoid **) &cs.errhp, (ub4)OCI_HTYPE_ERROR, 0, 0));
		// Allocate and initialize OCIServer handle
		check(services, cs, "HandleAlloc srvhp", OCIHandleAlloc( 
			(dvoid *)cs.envhp, (dvoid **) &cs.srvhp, (ub4)OCI_HTYPE_SERVER, 0, 0));		
		// Attach to a 'service'; initialize server context handle  
		check(services, cs, "ServerAttach", OCIServerAttach( 
			cs.srvhp, cs.errhp, (text *)service, (sb4)strlen(service), (ub4)OCI_DEFAULT));
		// Allocate and initialize OCISvcCtx handle
		check(services, cs, "HandleAlloc svchp", OCIHandleAlloc( 
			(dvoid *)cs.envhp, (dvoid **) &cs.svchp, (ub4)OCI_HTYPE_SVCCTX, 0, 0));		
		// set attribute server context in the service context
		check(services, cs, "AttrSet server-service", OCIAttrSet( 
			(dvoid *)cs.svchp, (ub4)OCI_HTYPE_SVCCTX, 
			(dvoid *)cs.srvhp, (ub4)0, 
			(ub4)OCI_ATTR_SERVER, (OCIError *)cs.errhp));		
		// allocate a user context handle
		check(services, cs, "HandleAlloc usrhp", OCIHandleAlloc(
			(dvoid *)cs.envhp, (dvoid **)&cs.usrhp, (ub4)OCI_HTYPE_SESSION, 0, 0));
		// set 'user' name
		check(services, cs, "AttrSet user-session", OCIAttrSet(
			(dvoid *)cs.usrhp, (ub4)OCI_HTYPE_SESSION, 
			(dvoid *)user, (ub4)strlen(user), 
			OCI_ATTR_USERNAME, cs.errhp));		
		// set 'pwd' password
		check(services, cs, "AttrSet pwd-session", OCIAttrSet(
			(dvoid *)cs.usrhp, (ub4)OCI_HTYPE_SESSION, 
			(dvoid *)pwd, (ub4)strlen(pwd), 
			OCI_ATTR_PASSWORD, cs.errhp));
		// Authenticate a user  
		check(services, cs, "SessionBegin", OCISessionBegin(
			cs.svchp, cs.errhp, cs.usrhp, 
			OCI_CRED_RDBMS, OCI_DEFAULT));
		// remember connection in session
		check(services, cs, "AttrSet service-session", OCIAttrSet(
			(dvoid *)cs.svchp, (ub4)OCI_HTYPE_SVCCTX, 
			(dvoid *)cs.usrhp, (ub4)0, 
			OCI_ATTR_SESSION, cs.errhp));

		// return created connection
		*(OracleSQL_connection_struct **)connection=&cs;
	}
	void disconnect(void *connection) {
	    OracleSQL_connection_struct &cs=*(OracleSQL_connection_struct *)connection;
		// Terminate a user session
		OCISessionEnd(
			cs.svchp, cs.errhp, cs.usrhp, (ub4)OCI_DEFAULT);
		// Detach from a server; uninitialize server context handle
		OCIServerDetach(
			cs.srvhp, cs.errhp, (ub4)OCI_DEFAULT);
		// Free a previously allocated handles
		OCIHandleFree(
			(dvoid *)cs.srvhp, (ub4)OCI_HTYPE_SERVER);
		OCIHandleFree(
			(dvoid *)cs.svchp, (ub4)OCI_HTYPE_SVCCTX);
		OCIHandleFree(
			(dvoid *)cs.errhp, (ub4)OCI_HTYPE_ERROR);
		OCIHandleFree(
			(dvoid *)cs.envhp, (ub4)OCI_HTYPE_ENV);

		// connections are cross-request, do not use services._alloc [linked with request]
		::free(&cs);
	}
	void commit(SQL_Driver_services& services, void *connection) {
	    OracleSQL_connection_struct &cs=*(OracleSQL_connection_struct *)connection;
		if(setjmp(cs.mark))
			services._throw(cs.error);

		check(services, cs, "commit", OCITransCommit(cs.svchp, cs.errhp, 0));
	}
	void rollback(SQL_Driver_services& services, void *connection) {
	    OracleSQL_connection_struct &cs=*(OracleSQL_connection_struct *)connection;
		if(setjmp(cs.mark))
			services._throw(cs.error);

		check(services, cs, "rollback", OCITransRollback(cs.svchp, cs.errhp, 0));
	}

	bool ping(SQL_Driver_services&, void *connection) {
		// maybe OCIServerVersion?
		return true;
	}

	unsigned int quote(
		SQL_Driver_services&, void *, 
		char *to, const char *from, unsigned int length) {
		/*
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
		
		OracleSQL_connection_struct &cs=*(OracleSQL_connection_struct *)connection;
		OracleSQL_query_lobs lobs={{0}, 0};
		OCIStmt *stmthp=0;

		bool failed=false;
		if(setjmp(cs.mark)) {
			failed=true;
			goto cleanup;
		} else {
			const char *statement=preprocess_statement(services, cs, 
				astatement, lobs);

			check(services, cs, "HandleAlloc STMT", OCIHandleAlloc( 
				(dvoid *)cs.envhp, (dvoid **) &stmthp, (ub4)OCI_HTYPE_STMT, 0, 0));
			check(services, cs, "syntax", 
				OCIStmtPrepare(stmthp, cs.errhp, (unsigned char *)statement, 
				(ub4)strlen((char *)statement), 
				(ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT));
			{
				for(int i=0; i<lobs.count; i++) {
					check(services, cs, "alloc output var desc", OCIDescriptorAlloc(
						(dvoid *)cs.envhp, (dvoid **)&lobs.items[i].locator, (ub4)OCI_DTYPE_LOB, 0, 0));

					check(services, cs, "bind output", OCIBindByPos(stmthp, 
						&lobs.items[i].bind, cs.errhp, 
						(ub4)1+i, 
						(dvoid *)&lobs.items[i].locator, 
						(sword)sizeof (lobs.items[i].locator), SQLT_CLOB, (dvoid *)0, 
						(ub2 *)0, (ub2 *)0, (ub4)0, (ub4 *)0, OCI_DATA_AT_EXEC));

					lobs.items[i].rows.count=0;
					OracleSQL_query_lobs::cbf_context_struct cbf_context={
						&services, &cs, &lobs.items[i].rows};
					check(services, cs, "bind dynamic", OCIBindDynamic(
						lobs.items[i].bind, cs.errhp, 
						(dvoid *) &cbf_context, cbf_no_data, 
						(dvoid *) &cbf_context, cbf_get_data));
				}
			}

			execute_prepared(services, cs, 
				statement, stmthp, lobs, 
				offset, limit, handlers);
		}
cleanup: // no check call after this point!
		{
			for(int i=0; i<lobs.count; i++) {
				/* free var locator */
				if(OCILobLocator *locator=lobs.items[i].locator)
					OCIDescriptorFree((dvoid *)locator, (ub4)OCI_DTYPE_LOB);

				/* free rows descriptors */
				OracleSQL_query_lobs::return_rows &rows=lobs.items[i].rows;
				for(int r=0; r<rows.count; r++)
					OCIDescriptorFree((dvoid *)rows.row[r].locator, (ub4)OCI_DTYPE_LOB);
			}
		}
		if(stmthp)
			OCIHandleFree((dvoid *)stmthp, (ub4)OCI_HTYPE_STMT);

		if(failed)
			services._throw(cs.error);
	}

private: // private funcs

	const char *preprocess_statement(SQL_Driver_services& services, OracleSQL_connection_struct &cs, 
		const char *astatement, OracleSQL_query_lobs &lobs) {
		size_t statement_size=strlen(astatement);

		char *result=(char *)services.malloc(statement_size
			+MAX_STRING // in case of short 'strings'
			+11/* returning */+6/* into */+(MAX_LOB_NAME_LENGTH+2/*:, */)*2/*ret into*/*MAX_IN_LOBS
			+1);
		const char *o=astatement;

		// /**xxx**/'literal' -> EMPTY_CLOB_FUNC_CALL
		char *n=result;
		while(*o) {
			if(
				o[0]=='/' &&
				o[1]=='*' && 
				o[2]=='*') { // name start
				o+=3;
				const char *name_begin=o;
				while(*o)
					if(
						o[0]=='*' &&
						o[1]=='*' &&
						o[2]=='/' &&
						o[3]=='\'') { // name end
						const char *name_end=o;
						o+=4;
						OracleSQL_query_lobs::Item &item=lobs.items[lobs.count++];
						item.name_ptr=name_begin; item.name_size=name_end-name_begin;
						item.data_ptr=(char *)services.malloc(statement_size/*max*/); item.data_size=0;

						const char *start=o;
						bool escaped=false;
						while(*o && !(o[0]=='\'' && o[1]!='\'' && !escaped)) {
							escaped=*o=='\\' || (o[0]=='\'' && o[1]=='\'');
							if(escaped) {
								// write pending, skip "\" or "'"
								if(size_t size=o-start) {
									memcpy(item.data_ptr+item.data_size, start, size);
									item.data_size+=size;
								}
								start=++o;
							} else
								o++;
						}
						if(size_t size=o-start) {
							memcpy(item.data_ptr+item.data_size, start, size);
							item.data_size+=size;
						}
						if(*o)
							o++; // skip "'"

						n+=sprintf(n, EMPTY_CLOB_FUNC_CALL);
						break;
					} else
						o++; // /**skip**/'xxx'
			} else
				*n++=*o++;
		}
		*n=0;

		if(lobs.count) {
			int i;
			n+=sprintf(n, " returning ");
			for(i=0; i<lobs.count; i++) {
				if(i)
					*n++=',';
				n+=sprintf(n, "%.*s", lobs.items[i].name_size, lobs.items[i].name_ptr);
				/*memcpy(n, lobs.items[i].name_ptr, lobs.items[i].name_size);
				n+=lobs.items[i].name_size;*/
			}
			n+=sprintf(n, " into ");
			for(i=0; i<lobs.count; i++) {
				if(i)
					*n++='x';
				n+=sprintf(n, ":%.*s", lobs.items[i].name_size, lobs.items[i].name_ptr);
				/**n++=':';
				memcpy(n, lobs.items[i].name_ptr, lobs.items[i].name_size);
				n+=lobs.items[i].name_size;*/
			}
		}

		return result;
	}

	void execute_prepared(
		SQL_Driver_services& services, OracleSQL_connection_struct &cs, 
		const char *statement, OCIStmt *stmthp, OracleSQL_query_lobs &lobs, 
		unsigned long offset, unsigned long limit, 
		SQL_Driver_query_event_handlers& handlers) {

		ub2 stmt_type=-1; // UNKNOWN // OCI_STMT_SELECT;//
	/*
		//gpfs on sun. paf 000818
		//Zanyway, this is needed before. 
		check(services, cs, "get stmt type", OCIAttrGet(
			(dvoid *)stmthp, (ub4)OCI_HTYPE_STMT, (ub1 *)&stmt_type, 
			(ub4 *)0, OCI_ATTR_STMT_TYPE, cs.errhp));
	*/
		if(strncasecmp(statement, "select", 6)==0) 
			stmt_type=OCI_STMT_SELECT;
		else if(strncasecmp(statement, "insert", 6)==0)
			stmt_type=OCI_STMT_INSERT;
		else if(strncasecmp(statement, "update", 6)==0)
			stmt_type=OCI_STMT_UPDATE;

		sword status=OCIStmtExecute(cs.svchp, stmthp, cs.errhp, 
			(ub4)stmt_type==OCI_STMT_SELECT?0:1, (ub4)0, 
			(OCISnapshot *)NULL, 
			(OCISnapshot *)NULL, (ub4)OCI_DEFAULT);

		if(status!=OCI_NO_DATA)
			check(services, cs, "execute", status);

		{
			for(int i=0; i<lobs.count; i++) 
				if(ub4 bytes_to_write=lobs.items[i].data_size) {
					OracleSQL_query_lobs::return_rows *rows=&lobs.items[i].rows;
					for(int r=0; r<rows->count; r++) {
						OCILobLocator *locator=rows->row[r].locator;
						check(services, cs, "lobwrite", OCILobWrite (
							cs.svchp, cs.errhp, 
							locator, &bytes_to_write, 1, 
							(dvoid *)lobs.items[i].data_ptr, (ub4)bytes_to_write, OCI_ONE_PIECE, 
							(dvoid *)0, 0, (ub2)0, 
							(ub1) SQLCS_IMPLICIT));
					}
				}
		}
		
		switch(stmt_type) {
		case OCI_STMT_SELECT:
			fetch_table(services, cs,
				stmthp, offset, limit, 
				handlers);
			break;
		default:
		/*
		case OCI_STMT_INSERT:
		case OCI_STMT_UPDATE:
		*/
			break;
		}
	}

	void fetch_table(SQL_Driver_services& services, OracleSQL_connection_struct &cs, 
		OCIStmt *stmthp, unsigned long offset, unsigned long limit, 
		SQL_Driver_query_event_handlers& handlers) {

		OCIParam          *mypard;
		ub2                    dtype;
		text                  *col_name;

		struct {
			ub2 type;
			char *str;
			OCILobLocator *var;
			OCIDefine *def;
			sb2 indicator;
		} cols[MAX_COLS]={0};
		int column_count=0;

		bool failed=false;
		jmp_buf saved_mark; memcpy(saved_mark, cs.mark, sizeof(jmp_buf));
		if(setjmp(cs.mark)) {
			failed=true;
			goto cleanup;
		} else {
			// idea of preincrementing is that at error time all handles would free up
			while(++column_count<=MAX_COLS) {
				/* get next descriptor, if there is one */
				if(OCIParamGet(stmthp, OCI_HTYPE_STMT, cs.errhp, (void **)&mypard, 
					(ub4) column_count)!=OCI_SUCCESS)
					break;
				
				/* Retrieve the data type attribute */
				check(services, cs, "get type", OCIAttrGet(
					(dvoid*) mypard, (ub4)OCI_DTYPE_PARAM, 
					(dvoid*) &dtype, (ub4 *)0, (ub4)OCI_ATTR_DATA_TYPE, 
					(OCIError *)cs.errhp));
				
				/* Retrieve the column name attribute */
				ub4 col_name_len;
				check(services, cs, "get name", OCIAttrGet(
					(dvoid*) mypard, (ub4)OCI_DTYPE_PARAM, 
					(dvoid**) &col_name, (ub4 *) &col_name_len, (ub4)OCI_ATTR_NAME, 
					(OCIError *)cs.errhp));
				
				{
					size_t size=(size_t)col_name_len;
					char *ptr=(char *)services.malloc(size);
					tolower(ptr, (char *)col_name, size);
					handlers.add_column(ptr, size);
				}
				
				ub2 coerce_type=dtype;
				sb4 size=0;
				void *ptr;
				
				switch(dtype) {
				case SQLT_CLOB: 
					{
						check(services, cs, "alloc output var desc", OCIDescriptorAlloc(
							(dvoid *)cs.envhp, (dvoid **)(ptr=&cols[column_count-1].var), 
							(ub4)OCI_DTYPE_LOB, 
							0, (dvoid **)0));
						
						size=0;
						break;
					}
				default:
					coerce_type=SQLT_STR;
					ptr=cols[column_count-1].str=(char *)services.malloc(MAX_OUT_STRING_LENGTH+1);
					size=MAX_OUT_STRING_LENGTH;
					break;
				}
				
				cols[column_count-1].type=coerce_type;
				
				check(services, cs, "DefineByPos", OCIDefineByPos(
					stmthp, &cols[column_count-1].def, cs.errhp, 
					column_count, (ub1 *) ptr, size, 
					coerce_type, (dvoid *) &cols[column_count-1].indicator, 
					(ub2 *)0, (ub2 *)0, OCI_DEFAULT));
			}
			
			handlers.before_rows();
			
			for(unsigned long row=0; !limit||row<limit+offset; row++) {
				sword status=OCIStmtFetch(stmthp, cs.errhp, (ub4)1,  (ub4)OCI_FETCH_NEXT, 
					(ub4)OCI_DEFAULT);
				if(status!=OCI_SUCCESS) {
					if(status!=OCI_NO_DATA)
						check(services, cs, "fetch", status);
					break;
				}
				if(row>=offset) {
					handlers.add_row();
					for(int i=0; i<column_count; i++) {
						size_t size;
						void *ptr;
						if(cols[i].indicator) { // NULL?
							size=0;
							ptr=0;
						} else // not NULL
							switch(cols[i].type) {
							case SQLT_CLOB: 
								{
									ub4   amtp=4096000000UL;
									ub4   offset=1;
									ub4   loblen=0;
									OCILobLocator *var=(OCILobLocator *)cols[i].var;
									OCILobGetLength(cs.svchp, cs.errhp, var, &loblen);
									if(loblen) {
										size=(size_t)loblen;
										ptr=services.malloc(size);
										check(services, cs, "lobread", OCILobRead(cs.svchp, cs.errhp, 
											var, &amtp, offset, (dvoid *) ptr, 
											loblen, (dvoid *)0, 
											0, 
											(ub2)0, (ub1) SQLCS_IMPLICIT));
									}
									break;
								}
							default:
								if(const char *str=cols[i].str) {
									size=strlen(str);
									ptr=services.malloc(size);
									memcpy(ptr, str, size);
								} else {
									size=0;
									ptr=0;
								}
								break;
						}
						handlers.add_row_cell(ptr, size);
					}
				}
			}
		}

cleanup: // no check call after this point!
		for(int i=0; i<column_count; i++) {
			switch(cols[i].type) {
			case SQLT_CLOB:
				/* free var locator */
				OCIDescriptorFree((dvoid *) cols[i].var, (ub4)OCI_DTYPE_LOB);
				break;
			default:
				break;
			}
		}

		if(failed) // need rethrow?
			longjmp(saved_mark, 1);
	}

private: // conn client library funcs
	
	friend void check(
		SQL_Driver_services& services, OracleSQL_connection_struct &cs, 
		const char *step, sword status);
	friend sb4 cbf_get_data(dvoid *ctxp, 
		OCIBind *bindp, 
		ub4 iter, ub4 index, 
		dvoid **bufpp, 
		ub4 **alenp, 
		ub1 *piecep, 
		dvoid **indpp, 
		ub2 **rcodepp);


#define OCI_DECL(name, params) \
	typedef sword (*t_OCI##name)params; t_OCI##name OCI##name

	OCI_DECL(Initialize, (ub4 mode, dvoid *ctxp, 
		dvoid * (*malocfp)(dvoid *ctxp, size_t size),
		dvoid * (*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize),
		void (*mfreefp)(dvoid *ctxp, dvoid *memptr) ));

	OCI_DECL(EnvInit, (OCIEnv **envp, ub4 mode, 
		size_t xtramem_sz, dvoid **usrmempp)); 
		
	OCI_DECL(AttrGet, (CONST dvoid *trgthndlp, ub4 trghndltyp, 
		dvoid *attributep, ub4 *sizep, ub4 attrtype, 
		OCIError *errhp));
	
	OCI_DECL(AttrSet, (dvoid *trgthndlp, ub4 trghndltyp, dvoid *attributep,
								ub4 size, ub4 attrtype, OCIError *errhp));

	OCI_DECL(BindByPos, (OCIStmt *stmtp, OCIBind **bindp, OCIError *errhp,
		ub4 position, dvoid *valuep, sb4 value_sz,
		ub2 dty, dvoid *indp, ub2 *alenp, ub2 *rcodep,
		ub4 maxarr_len, ub4 *curelep, ub4 mode));

	OCI_DECL(BindDynamic, (OCIBind *bindp, OCIError *errhp, dvoid *ictxp,
		OCICallbackInBind icbfp, dvoid *octxp,
		OCICallbackOutBind ocbfp));
	
	OCI_DECL(DefineByPos, (OCIStmt *stmtp, OCIDefine **defnp, OCIError *errhp,
		ub4 position, dvoid *valuep, sb4 value_sz, ub2 dty,
		dvoid *indp, ub2 *rlenp, ub2 *rcodep, ub4 mode));
	
	OCI_DECL(DescriptorAlloc, (CONST dvoid *parenth, dvoid **descpp, 
		CONST ub4 type, CONST size_t xtramem_sz, 
		dvoid **usrmempp));
	
	OCI_DECL(DescriptorFree, (dvoid *descp, CONST ub4 type));

	
	OCI_DECL(ErrorGet, (dvoid *hndlp, ub4 recordno, OraText *sqlstate,
                       sb4 *errcodep, OraText *bufp, ub4 bufsiz, ub4 type));

	OCI_DECL(HandleAlloc, (CONST dvoid *parenth, dvoid **hndlpp, CONST ub4 type, 
                       CONST size_t xtramem_sz, dvoid **usrmempp));

	OCI_DECL(HandleFree, (dvoid *hndlp, CONST ub4 type));
					   
	OCI_DECL(LobGetLength, (OCISvcCtx *svchp, OCIError *errhp, 
                          OCILobLocator *locp,
                          ub4 *lenp));

	OCI_DECL(LobRead, (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                     ub4 *amtp, ub4 offset, dvoid *bufp, ub4 bufl, 
                     dvoid *ctxp, sb4 (*cbfp)(dvoid *ctxp, 
                                              CONST dvoid *bufp, 
                                              ub4 len, 
                                              ub1 piece),
                     ub2 csid, ub1 csfrm));

	OCI_DECL(LobWrite, (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                      ub4 *amtp, ub4 offset, dvoid *bufp, ub4 buflen, 
                      ub1 piece, dvoid *ctxp, 
                      sb4 (*cbfp)(dvoid *ctxp, 
                                  dvoid *bufp, 
                                  ub4 *len, 
                                  ub1 *piece),
                      ub2 csid, ub1 csfrm));

	OCI_DECL(ParamGet, (CONST dvoid *hndlp, ub4 htype, OCIError *errhp, 
                     dvoid **parmdpp, ub4 pos));

	OCI_DECL(ServerAttach, (OCIServer *srvhp, OCIError *errhp,
                          CONST OraText *dblink, sb4 dblink_len, ub4 mode));

	OCI_DECL(ServerDetach, (OCIServer *srvhp, OCIError *errhp, ub4 mode));

	OCI_DECL(SessionBegin, (OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp,
                          ub4 credt, ub4 mode));

	OCI_DECL(SessionEnd, (OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp, 
                         ub4 mode));

	OCI_DECL(StmtExecute, (OCISvcCtx *svchp, OCIStmt *stmtp, OCIError *errhp, 
                         ub4 iters, ub4 rowoff, CONST OCISnapshot *snap_in, 
                         OCISnapshot *snap_out, ub4 mode));

	OCI_DECL(StmtFetch, (OCIStmt *stmtp, OCIError *errhp, ub4 nrows, 
                        ub2 orientation, ub4 mode));

	OCI_DECL(StmtPrepare, (OCIStmt *stmtp, OCIError *errhp, CONST OraText *stmt,
                          ub4 stmt_len, ub4 language, ub4 mode));

	OCI_DECL(TransCommit, (OCISvcCtx *svchp, OCIError *errhp, ub4 flags));

	OCI_DECL(TransRollback, (OCISvcCtx *svchp, OCIError *errhp, ub4 flags));

private: // conn client library funcs linking

	const char *dlink(const char *dlopen_file_spec) {
        lt_dlhandle handle=lt_dlopen(dlopen_file_spec);
        if(!handle)
			return "can not open the dynamic link module";

		#define DSLINK(name, action) \
			name=(t_##name)lt_dlsym(handle, #name); \
				if(!name) \
					action;

		#define OLINK(name) DSLINK(OCI##name, return "function OCI" #name " was not found")
		
		OLINK(Initialize);
		OLINK(EnvInit);
		OLINK(AttrGet);		OLINK(AttrSet);
		OLINK(BindByPos);		OLINK(BindDynamic);
		OLINK(DefineByPos);
		OLINK(DescriptorAlloc);		OLINK(DescriptorFree);
		OLINK(ErrorGet);
		OLINK(HandleAlloc);		OLINK(HandleFree);
		OLINK(LobGetLength);
		OLINK(LobRead);		OLINK(LobWrite);
		OLINK(ParamGet);
		OLINK(ServerAttach);		OLINK(ServerDetach);
		OLINK(SessionBegin);		OLINK(SessionEnd);
		OLINK(StmtExecute);		OLINK(StmtFetch);		OLINK(StmtPrepare);
		OLINK(TransCommit);		OLINK(TransRollback);

		return 0;
	}

} *OracleSQL_driver;

void check(
	SQL_Driver_services& services, OracleSQL_connection_struct &cs, 
	const char *step, sword status) {

	const char *msg;
	char reason[MAX_STRING/2];

	switch (status) {
	case OCI_SUCCESS:
		return; // hurrah
	case OCI_ERROR:
		{
		sb4 errcode;
		if(OracleSQL_driver->OCIErrorGet((dvoid *)cs.errhp, (ub4)1, (text *)NULL, &errcode, 
			(text *)reason, (ub4)sizeof(reason), OCI_HTYPE_ERROR)==OCI_SUCCESS)
			msg=reason;
		else
			msg="[can not get error description]";
		break;
		}
	case OCI_SUCCESS_WITH_INFO:
		msg="SUCCESS_WITH_INFO"; break;
	case OCI_NEED_DATA:
		msg="NEED_DATA"; break;
	case OCI_NO_DATA:
		msg="NODATA"; break;
	case OCI_INVALID_HANDLE:
		msg="INVALID_HANDLE"; break;
	case OCI_STILL_EXECUTING:
		msg="STILL_EXECUTE"; break;
	case OCI_CONTINUE:
		msg="CONTINUE"; break;
	default:
		msg="unknown"; break;
	}

	snprintf(cs.error, sizeof(cs.error), "%s (%s, %d)", 
		msg, step, (int)status);
	longjmp(cs.mark, 1);
}


/* ----------------------------------------------------------------- */
/* Intbind callback that does not do any data input.                 */
/* ----------------------------------------------------------------- */
static sb4 cbf_no_data(
				dvoid *ctxp, 
				OCIBind *bindp, 
				ub4 iter, ub4 index, 
				dvoid **bufpp, 
				ub4 *alenpp, 
				ub1 *piecep, 
				dvoid **indpp) {
	*bufpp=(dvoid *)0;
	*alenpp=0;
	static sb2 null_ind=-1;
	*indpp=(dvoid *) &null_ind;
	*piecep=OCI_ONE_PIECE;
	
	return OCI_CONTINUE;
}

/* ----------------------------------------------------------------- */
/* Outbind callback for returning data.                              */
/* ----------------------------------------------------------------- */
static sb4 cbf_get_data(dvoid *ctxp, 
				 OCIBind *bindp, 
				 ub4 iter, ub4 index, 
				 dvoid **bufpp, 
				 ub4 **alenp, 
				 ub1 *piecep, 
				 dvoid **indpp, 
				 ub2 **rcodepp) {
	OracleSQL_query_lobs::cbf_context_struct &context=
		*(OracleSQL_query_lobs::cbf_context_struct *)ctxp;

	if(index==0) {
		static ub4  rows;
		check(*context.services, *context.cs, "AttrGet cbf_get_data ROWS_RETURNED", 
			OracleSQL_driver->OCIAttrGet(
				(CONST dvoid *) bindp, OCI_HTYPE_BIND, (dvoid *)&rows, 
				(ub4 *)sizeof(ub2), OCI_ATTR_ROWS_RETURNED, context.cs->errhp))	;
		context.rows->count=(ub2)rows;
		context.rows->row=(OracleSQL_query_lobs::return_rows::return_row *)
			context.services->malloc(sizeof(OracleSQL_query_lobs::return_rows::return_row)*rows);
	}

	OracleSQL_query_lobs::return_rows::return_row &var=context.rows->row[index];

	check(*context.services, *context.cs, "alloc output var desc dynamic", OracleSQL_driver->OCIDescriptorAlloc(
		(dvoid *) context.cs->envhp, (dvoid **)&var.locator, 
		(ub4)OCI_DTYPE_LOB, 
		0, (dvoid **)0));

	*bufpp=var.locator;
	*alenp=&var.len;
	*indpp=(dvoid *) &var.ind;
	*piecep=OCI_ONE_PIECE;
	*rcodepp=&var.rcode;
	
	return OCI_CONTINUE;
}

void tolower(char *out, const char *in, size_t size) {
	while(size--)
		*out++=tolower(*in++);
}

extern "C" SQL_Driver *create() {
	//_asm int 3;
	return OracleSQL_driver=new OracleSQL_Driver();
}
