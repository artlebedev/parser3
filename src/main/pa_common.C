/** @file
	Parser: commonly functions.

	Copyright(c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_COMMON_C="$Date: 2003/06/20 09:56:01 $"; 

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_globals.h"
#include "pa_hash.h"
#include "pa_table.h"
#include "pa_vstring.h"
#include "pa_vdate.h"
#include "pa_vhash.h"
#include "pa_vtable.h"

#ifdef WIN32
#	include <windows.h>
#else
#	define closesocket close
#endif

// some maybe-undefined constants

#ifndef _O_TEXT
#	define _O_TEXT 0
#endif
#ifndef _O_BINARY
#	define _O_BINARY 0
#endif

#ifdef HAVE_FTRUNCATE
#	define PA_O_TRUNC 0
#else
#	ifdef _O_TRUNC
#		define PA_O_TRUNC _O_TRUNC
#	else
#		error you must have either ftruncate function or _O_TRUNC bit declared
#	endif
#endif

// locking constants

#ifdef HAVE_FLOCK

static int lock_shared_blocking(int fd) { return flock(fd, LOCK_SH); }
static int lock_exclusive_blocking(int fd) { return flock(fd, LOCK_EX); }
static int lock_exclusive_nonblocking(int fd) { return flock(fd, LOCK_EX || LOCK_NB); }
static int unlock(int fd) { return flock(fd, LOCK_UN); }

#else
#ifdef HAVE__LOCKING

#define FLOCK(operation) lseek(fd, 0, SEEK_SET);  return _locking(fd, operation, 1)
static int lock_shared_blocking(int fd) { FLOCK(_LK_LOCK); }
static int lock_exclusive_blocking(int fd) { FLOCK(_LK_LOCK); }
static int lock_exclusive_nonblocking(int fd) { FLOCK(_LK_NBLCK); }
static int unlock(int fd) { FLOCK(_LK_UNLCK); }

#else
#ifdef HAVE_FCNTL

#define FLOCK(cmd, arg) struct flock ls={arg, SEEK_SET};  return fcntl(fd, cmd, &ls)
static int lock_shared_blocking(int fd) { FLOCK(F_SETLKW, F_RDLCK); }
static int lock_exclusive_blocking(int fd) { FLOCK(F_SETLKW, F_WRLCK); }
static int lock_exclusive_nonblocking(int fd) { FLOCK(F_SETLK, F_RDLCK); }
static int unlock(int fd) { FLOCK(F_SETLK, F_UNLCK); }

#else
#ifdef HAVE_LOCKF

#define FLOCK(fd, operation) lseek(fd, 0, SEEK_SET);  return lockf(fd, operation, 1)
static int lock_shared_blocking(int fd) { FLOCK(F_LOCK); } // on intel solaris man doesn't have doc on shared blocking
static int lock_exclusive_blocking(int fd) { FLOCK(F_LOCK); }
static int lock_exclusive_nonblocking(int fd) { FLOCK(F_TLOCK); }
static int unlock(int fd) { FLOCK(F_TLOCK); }

#else

#error unable to find file locking func

#endif
#endif
#endif
#endif

#define DEFAULT_USER_AGENT "parser3"


void fix_line_breaks(char* buf, size_t& size) {
	if(size==0)
		return;

	//_asm int 3;
	const char* const eob=buf+size;
	char* dest=buf;
	// fix DOS: \r\n -> \n
	// fix Macintosh: \r -> \n
	char* bol=buf;
	while(char* eol=(char*)memchr(bol, '\r', eob -bol)) {
		size_t len=eol-bol;
		if(dest!=bol)
			memcpy(dest, bol, len); 
		dest+=len;
		*dest++='\n'; 

		if(&eol[1]<eob && eol[1]=='\n') { // \r, \n = DOS
			bol=eol+2;
			size--; 
		} else // \r, not \n = Macintosh
			bol=eol+1;
	}
	// last piece without \r, including terminating 0
	if(dest!=bol)
		memcpy(dest, bol, eob-bol); 
}

char* file_read_text(Pool& pool, const String& file_spec, 
					 bool fail_on_read_problem,
					 Hash *params, Hash** out_fields) {
	void *result;  size_t size;
	return file_read(pool, file_spec, result, size, true, params, out_fields, fail_on_read_problem)?(char *)result:0;
}

//http request stuff
/* ************************ http stuff *********************** */

static bool set_addr(struct sockaddr_in *addr, const char* host, const short port){
    memset(addr, 0, sizeof(*addr)); 
    addr->sin_family=AF_INET;
    addr->sin_port=htons(port); 
    if(host) {
		if(struct hostent *hostIP=gethostbyname(host)) 
			memcpy(&addr->sin_addr, hostIP->h_addr, hostIP->h_length); 
		else
			return false;
    } else 
		addr->sin_addr.s_addr=INADDR_ANY;
    return true;
}

static int http_read_response(String& response, int sock, bool fail_on_status_ne_200){
	const String* status_code=0;
	ssize_t EOLat=0;
	while(true) {
		char *buf=(char *)response.pool().malloc(MAX_STRING); 
		ssize_t size=recv(sock, buf, MAX_STRING, 0); 
		if(size<=0)
			break;
		response.APPEND_TAINTED(buf, size, "remote HTTP server response", 0); 
		if(!status_code && (EOLat=response.pos("\r\n", 2))>=0) { // checking status in first response
			const String& status_line=response.mid(0, (size_t)EOLat);
			Array astatus(response.pool()); 
			size_t pos_after_ref=0; status_line.split(astatus, &pos_after_ref, " ", 1); 
			status_code=astatus.get_string(1); 

			if(fail_on_status_ne_200 && *status_code!="200")
				throw Exception("http.status",
					status_code,
					"invalid HTTP response status");
		}
	}
	if(status_code)
		return status_code->as_int();
	else
		throw Exception("http.response",
			0,
			"bad response from host - no status found (size=%lu)", response.size()); 
}

/* ********************** request *************************** */

#if defined(SIGALRM) && defined(HAVE_SIGSETJMP) && defined(HAVE_SIGLONGJMP)
#	define PA_USE_ALARM
#endif

#ifdef PA_USE_ALARM
static sigjmp_buf timeout_env;
static void timeout_handler(int sig){
    siglongjmp(timeout_env, 1); 
}
#endif

static int http_request(String& response,
			const String *origin_string, 
			const char* host, int port, 
			const char* request, 
			int timeout,
			bool fail_on_status_ne_200) {
	if(!host)
		throw Exception("http.host", 
			origin_string, 
			"zero hostname");  //never

#ifdef PA_USE_ALARM
	signal(SIGALRM, timeout_handler); 
#endif
	int sock=-1;
#ifdef PA_USE_ALARM
	if(sigsetjmp(timeout_env, 1)) {
		// stupid gcc [2.95.4] generated bad code
		// which failed to handle sigsetjmp+throw: crashed inside of pre-throw code.
		// rewritten simplier [though duplicating closesocket code]
		if(sock>=0) 
			closesocket(sock); 
		throw Exception("http.timeout", 
			origin_string, 
			"timeout occured while retrieving document"); 
		return 0; // never
	} else {
		alarm(timeout); 
#endif
		try {
			int result;
			struct sockaddr_in dest;

	    		if(!set_addr(&dest, host, port))
				throw Exception("http.host", 
					origin_string, 
					"can not resolve hostname \"%s\"", host); 
			
			if((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP/*0*/))<0)
				throw Exception("http.connect", 
					origin_string, 
					"can not make socket: %s (%d)", strerror(errno), errno); 
			if(connect(sock, (struct sockaddr *)&dest, sizeof(dest)))
				throw Exception("http.connect", 
					origin_string, 
					"can not connect to host \"%s\": %s (%d)", host, strerror(errno), errno); 
			size_t request_size=strlen(request);
			if(send(sock, request, request_size, 0)!=(ssize_t)request_size)
				throw Exception("http.connect", 
					origin_string, 
					"error sending request: %s (%d)", strerror(errno), errno); 

			result=http_read_response(response, sock, fail_on_status_ne_200); 
			closesocket(sock); 
#ifdef PA_USE_ALARM
			alarm(0); 
#endif
			return result;
		} catch(...) {
#ifdef PA_USE_ALARM
			alarm(0); 
#endif
			if(sock>=0) 
				closesocket(sock); 
			/*re*/throw;
		}
#ifdef PA_USE_ALARM
	}
#endif
}

#ifndef DOXYGEN
struct Http_pass_header_info {
	String* request;
	bool user_agent_specified;
};
#endif
static void http_pass_header(const Hash::Key& key, Hash::Val *value, void *info)
{
	Http_pass_header_info& i=*static_cast<Http_pass_header_info *>(info);
	Pool& pool=i.request->pool();
    
    *(i.request)<<key<<": "
		<< attributed_meaning_to_string(*static_cast<Value *>(value), String::UL_HTTP_HEADER, false)
		<<"\n"; 
	
	if(key.change_case(pool, String::CC_UPPER)=="USER-AGENT")
		i.user_agent_specified=true;
}
/// @todo build .cookies field. use ^file.tables.SET-COOKIES.menu{ for now
static void file_read_http(Pool& pool, const String& file_spec, 
					void*& data, size_t& data_size, 
					Hash *options=0, Hash** out_fields=0) {
	char host[MAX_STRING]; 
	const char* uri; 
	int port;
	const char* method="GET"; 
	int timeout=2;
	bool fail_on_status_ne_200=true;
	Value *vheaders=0;

	String& connect_string=*new(pool) String(pool); // must not be local [exception may be reported outside]
	// not in ^sql{... UL_SQL ...} spirit, but closer to ^file::load one
	connect_string.append(file_spec, String::UL_URI); // tainted pieces -> URI pieces

	char* connect_string_cstr=connect_string.cstr(String::UL_UNSPECIFIED); 
	if(strncmp(connect_string_cstr, "http://", 7)!=0)
		throw Exception(0, 
			&connect_string, 
			"does not start with http://"); //never
	connect_string_cstr+=7;

	strncpy(host, connect_string_cstr, sizeof(host)-1);  host[sizeof(host)-1]=0;
	char* host_uri=lsplit(host, '/'); 
	uri=host_uri?connect_string_cstr+(host_uri-1-host):"/"; 
	char* port_cstr=lsplit(host, ':'); 
	char* error_pos=0;
	port=port_cstr?strtol(port_cstr, &error_pos, 0):80;

	if(options) {
		int valid_options=0;
		if(Value *vmethod=static_cast<Value *>(options->get(*http_method_name))) {
			valid_options++;
			method=vmethod->as_string().cstr(); 
		}
		if(Value *vtimeout=static_cast<Value *>(options->get(*http_timeout_name))) {
			valid_options++;
			timeout=vtimeout->as_int(); 
		}
		if(vheaders=static_cast<Value *>(options->get(*http_headers_name))) {
			valid_options++;
		}
		if(Value *vany_status=static_cast<Value *>(options->get(*http_any_status_name))) {
			valid_options++;
			fail_on_status_ne_200=!vany_status->as_bool(); 
		}

		if(valid_options!=options->size())
			throw Exception("parser.runtime",
				0,
				"invalid option passed");
	} 

	//making request
	String request(pool);
	request<< method <<" "<< uri <<" HTTP/1.0\nHost: "<< host<<"\n"; 
	bool user_agent_specified=false;
	if(vheaders && !vheaders->is_string()) { // allow empty
		if(Hash *headers=vheaders->get_hash(&connect_string)) {
			Http_pass_header_info info={&request};
			headers->for_each(http_pass_header, &info); 
			user_agent_specified=info.user_agent_specified;
		} else
			throw Exception("parser.runtime", 
				&connect_string,
				"headers param must be hash"); 
	};
	if(!user_agent_specified) // defaulting
		request << "user-agent: " DEFAULT_USER_AGENT "\n";
	request<<"\n"; 
	
	//sending request
	String response(pool); 
	int status_code=http_request(response,
		&connect_string, host, port, request.cstr(String::UL_UNSPECIFIED), 
		timeout, fail_on_status_ne_200); 
	
	//processing results	
	int pos=response.pos("\r\n\r\n", 4); 
	if(pos<1){
		throw Exception("http.response", 
			&connect_string,
			"bad response from host - no headers found"); 
	}
	String header_block=response.mid(0, pos); 
	String body=response.mid(pos+4, response.size()); 
	
	Array aheaders(pool); 
	Hash& headers=*new(pool) Hash(pool); 
	VHash* vtables=new(pool) VHash(pool);
	headers.put(*http_tables_name, vtables);
	Hash& tables=vtables->hash(0);

	size_t pos_after_ref=0;
	header_block.split(aheaders, &pos_after_ref, "\r\n", 2); 
	
	//processing headers
	for(int i=1;i<aheaders.size();i++) {
		if(const String *line=aheaders.get_string(i)) {
			pos=line->pos(": ", 2); 
			if(pos<1)
				throw Exception("http.response", 
					&connect_string,
					"bad response from host - bad header \"%s\"", line->cstr()); 
		
			const String& sname=line->mid(0, pos).change_case(pool, String::CC_UPPER);
			const String& string=line->mid(pos+2, line->size());

			// tables
			{
				Value *valready=(Value *)tables.get(sname);
				bool existed=valready!=0;
				Table *table;
				if(existed) {
					// second+ appearence
					table=valready->get_table();
				} else {
					// first appearence
					Array& columns=*new(pool) Array(pool, 1);
					columns+=new(pool) String(pool, "value");
					table=new(pool) Table(pool, 0, &columns);
				}
				// this string becomes next row
				Array& row=*new(pool) Array(pool, 1);
				row+=&string;
				*table+=&row;
				// not existed before? add it
				if(!existed)
					tables.put(sname, new(pool) VTable(pool, table));
			}
			headers.put(sname, new(pool) VString(string));
		} else
			throw Exception("http.response", 
				&connect_string, 
				"bad response from host - bad headers \"%s\"", header_block.cstr()); 
	}

	// output response
	data=body.cstr(); data_size=body.size();
	if(out_fields) {
		headers.put(*file_status_name, new(pool) VInt(pool, status_code)); 
		*out_fields=&headers;
	}
}

#ifndef DOXYGEN
struct File_read_action_info {
	void **data; size_t *data_size;
}; 
#endif
static void file_read_action(Pool& pool, 
							 struct stat& finfo, 
							 int f, 
							 const String& file_spec, const char* fname, bool as_text, 
							 void *context) {
	File_read_action_info& info=*static_cast<File_read_action_info *>(context); 
	if(size_t to_read_size=(size_t)finfo.st_size) { 
		*info.data=pool.malloc(to_read_size+(as_text?1:0), 3); 
		*info.data_size=(size_t)read(f, *info.data, to_read_size); 

		if(ssize_t(*info.data_size)<0 || *info.data_size>to_read_size)
			throw Exception(0, 
				&file_spec, 
				"read failed: actually read %lu bytes count not in [0..%lu] valid range", 
					*info.data_size, to_read_size); 
	} else { // empty file
		if(as_text) {
			*info.data=pool.malloc(1); 
			*(char*)(*info.data)=0;
		} else 
			*info.data=0;
		*info.data_size=0;
		return;
	}
}
bool file_read(Pool& pool, const String& file_spec, 
			   void*& data, size_t& data_size, 
			   bool as_text, Hash *params, Hash** out_fields, 
			   bool fail_on_read_problem) {
	bool result;
	if(file_spec.starts_with("http://", 7)) {
		// fail on read problem
		file_read_http(pool, file_spec, data, data_size, params, out_fields); 
		result=true;
	} else {
		File_read_action_info info={&data, &data_size}; 
		result=file_read_action_under_lock(pool, file_spec, 
			"read", file_read_action, &info, 
			as_text, fail_on_read_problem); 
	}

	if(result && as_text) {
		// UTF-8 signature: EF BB BF
		if(data_size>=3) {
			char *in=(char *)data;
			if((in[0] == '\xEF') && (in[1] == '\xBB') &&
				(in[2] == '\xBF')) {
				data=in+3; data_size-=3;// skip prefix
			}
		}

		fix_line_breaks((char *)(data), data_size); 
		// note: after fixing
		((char*&)(data))[data_size]=0;
	}

	return result;
}

#ifdef PA_SAFE_MODE
void check_safe_mode(struct stat finfo, const String& file_spec, const char* fname) {
	if(finfo.st_uid/*foreign?*/!=geteuid()
		&& finfo.st_gid/*foreign?*/!=getegid())
		throw Exception("parser.runtime", 
			&file_spec, 
			"parser is in safe mode: "
			"reading files of foreign group and user disabled "
			"[recompile parser with --disable-safe-mode configure option], "
			"actual filename '%s', "
			"fuid(%d)!=euid(%d) or fgid(%d)!=egid(%d)", 
				fname,
				finfo.st_uid, geteuid(),
				finfo.st_gid, getegid());
}
#endif

bool file_read_action_under_lock(Pool& pool, const String& file_spec, 
				const char* action_name, File_read_action action, void *context, 
				bool as_text, 
				bool fail_on_read_problem) {
	const char* fname=file_spec.cstr(String::UL_FILE_SPEC); 
	int f;

	// first open, next stat:
	// directory update of NTFS hard links performed on open.
	// ex: 
	//   a.html:^test[] and b.html hardlink to a.html
	//   user inserts ! before ^test in a.html
	//   directory entry of b.html in NTFS not updated at once, 
	//   they delay update till open, so we would receive "!^test[" string
	//   if would do stat, next open.
	// later: it seems, even this does not help sometimes
    if((f=open(fname, O_RDONLY|(as_text?_O_TEXT:_O_BINARY)))>=0) {
		try {
			if(lock_shared_blocking(f)!=0)
				throw Exception("file.lock", 
						&file_spec, 
						"shared lock failed: %s (%d), actual filename '%s'", 
							strerror(errno), errno, fname); 

			struct stat finfo;
			if(stat(fname, &finfo)!=0)
				throw Exception("file.missing", // hardly possible: we just opened it OK
					&file_spec, 
					"stat failed: %s (%d), actual filename '%s'", 
						strerror(errno), errno, fname); 

#ifdef PA_SAFE_MODE
			check_safe_mode(finfo, file_spec, fname);
#endif

			action(pool, finfo, f, file_spec, fname, as_text, context); 
		} catch(...) {
			unlock(f);close(f); 
			if(fail_on_read_problem)
				/*re*/throw;
			return false;			
		} 

		unlock(f);close(f); 
		return true;
    } else {
		if(fail_on_read_problem)
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
				&file_spec, 
				"%s failed: %s (%d), actual filename '%s'", 
					action_name, strerror(errno), errno, fname); 
		return false;
	}
}

static void create_dir_for_file(const String& file_spec) {
	size_t pos_after=1;
	int pos_before;
	while((pos_before=file_spec.pos("/", 1, pos_after))>=0) {
		mkdir(file_spec.mid(0, pos_before).cstr(String::UL_FILE_SPEC), 0775); 
		pos_after=pos_before+1;
	}
}

bool file_write_action_under_lock(
				const String& file_spec, 
				const char* action_name, File_write_action action, void *context, 
				bool as_text, 
				bool do_append, 
				bool do_block, 
				bool fail_on_lock_problem) {
	const char* fname=file_spec.cstr(String::UL_FILE_SPEC); 
	int f;
	if(access(fname, W_OK)!=0) // no
		create_dir_for_file(file_spec); 

	if((f=open(fname, 
		O_CREAT|O_RDWR
		|(as_text?_O_TEXT:_O_BINARY)
		|(do_append?O_APPEND:PA_O_TRUNC), 0664))>=0) {
		if((do_block?lock_exclusive_blocking(f):lock_exclusive_nonblocking(f))!=0) {
			Exception e("file.lock", 
				&file_spec, 
				"shared lock failed: %s (%d), actual filename '%s'", 
				strerror(errno), errno, fname); 
			close(f); 
			if(fail_on_lock_problem)
				throw e;
			return false;
		}

		try {
			action(f, context); 
		} catch(...) {
#ifdef HAVE_FTRUNCATE
			if(!do_append)
				ftruncate(f, lseek(f, 0, SEEK_CUR)); // one can not use O_TRUNC, read lower
#endif
			unlock(f);close(f); 
			/*re*/throw;
		}
		
#ifdef HAVE_FTRUNCATE
		if(!do_append)
			ftruncate(f, lseek(f, 0, SEEK_CUR)); // O_TRUNC truncates even exclusevely write-locked file [thanks to Igor Milyakov <virtan@rotabanner.com> for discovering]
#endif
		unlock(f);close(f); 
		return true;
	} else
		throw Exception(errno==EACCES?"file.access":0, 
			&file_spec, 
			"%s failed: %s (%d), actual filename '%s'", 
				action_name, strerror(errno), errno, fname); 
	// here should be nothing, see rethrow above
}

#ifndef DOXYGEN
struct File_write_action_info {
	const void *data; size_t size;
}; 
#endif
static void file_write_action(int f, void *context) {
	File_write_action_info& info=*static_cast<File_write_action_info *>(context); 
	if(info.size) {
		int written=write(f, info.data, info.size); 
		if(written<0)
			throw Exception(0, 
				0, 
				"write failed: %s (%d)",  strerror(errno), errno); 
	}
}
void file_write(
				const String& file_spec, 
				const void *data, size_t size, 
				bool as_text, 
				bool do_append) {
	File_write_action_info info={data, size}; 
	file_write_action_under_lock(
				file_spec, 
				"write", file_write_action, &info, 
				as_text, 
				do_append); 
}

// throws nothing! [this is required in file_move & file_delete]
static void rmdir(const String& file_spec, size_t pos_after) {
	int pos_before;
	if((pos_before=file_spec.pos("/", 1, pos_after))>=0)
		rmdir(file_spec, pos_before+1); 
	
	rmdir(file_spec.mid(0, pos_after-1/* / */).cstr(String::UL_FILE_SPEC)); 
}
bool file_delete(const String& file_spec, bool fail_on_read_problem) {
	const char* fname=file_spec.cstr(String::UL_FILE_SPEC); 
	if(unlink(fname)!=0)
		if(fail_on_read_problem)
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
				&file_spec, 
				"unlink failed: %s (%d), actual filename '%s'", 
					strerror(errno), errno, fname); 
		else
			return false;

	rmdir(file_spec, 1); 
	return true;
}
void file_move(const String& old_spec, const String& new_spec) {
	const char* old_spec_cstr=old_spec.cstr(String::UL_FILE_SPEC); 
	const char* new_spec_cstr=new_spec.cstr(String::UL_FILE_SPEC); 
	
	create_dir_for_file(new_spec); 

	if(rename(old_spec_cstr, new_spec_cstr)!=0)
		throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
			&old_spec, 
			"rename failed: %s (%d), actual filename '%s' to '%s'", 
				strerror(errno), errno, old_spec_cstr, new_spec_cstr); 

	rmdir(old_spec, 1); 
}


bool entry_exists(const char* fname, struct stat *afinfo) {
	struct stat lfinfo;
	bool result=stat(fname, &lfinfo)==0;
	if(afinfo)
		*afinfo=lfinfo;
	return result;
}

bool entry_exists(const String& file_spec) {
	const char* fname=file_spec.cstr(String::UL_FILE_SPEC); 
	return entry_exists(fname, 0); 
}

static bool entry_readable(const String& file_spec, bool need_dir) {
    char* fname=file_spec.cstr(String::UL_FILE_SPEC); 
	if(need_dir) {
		size_t size=strlen(fname); 
		while(size) {
			char c=fname[size-1]; 
			if(c=='/' || c=='\\')
				fname[--size]=0;
			else
				break;
		}
	}
	struct stat finfo;
	if(access(fname, R_OK)==0 && entry_exists(fname, &finfo)) {
		bool is_dir=(finfo.st_mode&S_IFDIR) != 0;
		return is_dir==need_dir;
	}
	return false;
}
bool file_readable(const String& file_spec) {
	return entry_readable(file_spec, false); 
}
bool dir_readable(const String& file_spec) {
	return entry_readable(file_spec, true); 
}
String *file_readable(const String& path, const String& name) {
	String *result=new(path.pool()) String(path); 
	*result << "/"; 
	*result << name;
	return file_readable(*result)?result:0;
}
bool file_executable(const String& file_spec) {
    return access(file_spec.cstr(String::UL_FILE_SPEC), X_OK)==0;
}

bool file_stat(const String& file_spec, 
			   size_t& rsize, 
			   time_t& ratime, 
			   time_t& rmtime, 
			   time_t& rctime, 
			   bool fail_on_read_problem) {
	Pool& pool=file_spec.pool(); 
	const char* fname=file_spec.cstr(String::UL_FILE_SPEC); 
    struct stat finfo;
	if(stat(fname, &finfo)!=0)
		if(fail_on_read_problem)
			throw Exception("file.missing", 
				&file_spec, 
				"getting file size failed: %s (%d), real filename '%s'", 
					strerror(errno), errno, fname); 
		else
			return false;
	rsize=finfo.st_size;
	ratime=finfo.st_atime;
	rmtime=finfo.st_mtime;
	rctime=finfo.st_ctime;
	return true;
}

char* getrow(char* *row_ref, char delim) {
    char* result=*row_ref;
    if(result) {
		*row_ref=strchr(result, delim); 
		if(*row_ref) 
			*((*row_ref)++)=0; 
		else if(!*result) 
			return 0;
    }
    return result;
}

char* lsplit(char* string, char delim) {
    if(string) {
		char* v=strchr(string, delim); 
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return 0;
}

char* lsplit(char* *string_ref, char delim) {
    char* result=*string_ref;
	char* next=lsplit(*string_ref, delim); 
    *string_ref=next;
    return result;
}

char* rsplit(char* string, char delim) {
    if(string) {
		char* v=strrchr(string, delim); 
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return NULL;	
}

/// @todo less stupid type detection
char* format(Pool& pool, double value, char* fmt) {
	char local_buf[MAX_NUMBER]; 
	size_t size;
	
	if(fmt)
		if(strpbrk(fmt, "diouxX"))
			if(strpbrk(fmt, "ouxX"))
				size=snprintf(local_buf, sizeof(local_buf), fmt, (uint)value); 
			else
				size=snprintf(local_buf, sizeof(local_buf), fmt, (int)value); 
		else
			size=snprintf(local_buf, sizeof(local_buf), fmt, value); 
	else
		size=snprintf(local_buf, sizeof(local_buf), "%d", (int)value); 
	
	char* pool_buf=(char *)pool.malloc(size+1, 4); 
	memcpy(pool_buf, local_buf, size+1); 
	return pool_buf;
}

size_t stdout_write(const void *buf, size_t size) {
#ifdef WIN32
	do{
		int chunk_written=fwrite(buf, 1, min(8*0x400, size), stdout); 
		if(chunk_written<=0)
			break;
		size-=chunk_written;
		buf=((const char*)buf)+chunk_written;
	} while(size>0); 

	return size;
#else
	return fwrite(buf, 1, size, stdout); 
#endif
}

char* unescape_chars(Pool& pool, const char* cp, int len) {
	char* s=(char *)pool.malloc(len + 1, 5); 
	enum EscapeState {
		EscapeRest, 
		EscapeFirst, 
		EscapeSecond
	} escapeState=EscapeRest;
	int escapedValue=0;
	int srcPos=0;
	int dstPos=0;
	while(srcPos < len) {
		int ch=cp[srcPos]; 
		switch(escapeState) {
			case EscapeRest:
			if(ch=='%') {
				escapeState=EscapeFirst;
			} else if(ch=='+') {
				s[dstPos++]=' '; 
			} else {
				s[dstPos++]=ch;	
			}
			break;
			case EscapeFirst:
			escapedValue=hex_value[ch] << 4;	
			escapeState=EscapeSecond;
			break;
			case EscapeSecond:
			escapedValue +=hex_value[ch]; 
			s[dstPos++]=escapedValue;
			escapeState=EscapeRest;
			break;
		}
		srcPos++; 
	}
	s[dstPos]=0;
	return s;
}

#ifdef WIN32
void back_slashes_to_slashes(char* s) {
	if(s)
		for(; *s; s++)
			if(*s=='\\')
				*s='/'; 
}
/*
void slashes_to_back_slashes(char* s) {
	if(s)
		for(; *s; s++)
			if(*s=='/')
				*s='\\'; 
}
*/
#endif

bool StrEqNc(const char* s1, const char* s2, bool strict) {
	while(true) {
		if(!(*s1)) {
			if(!(*s2))
				return true;
			else
				return !strict;
		} else if(!(*s2))
			return !strict;
		if(isalpha(*s1)) {
			if(tolower(*s1) !=tolower(*s2))
				return false;
		} else if((*s1) !=(*s2))
			return false;
		s1++; 
		s2++; 
	}
}

static bool isLeap(int year) {
    return !(
             (year % 4) || ((year % 400) && !(year % 100))
            ); 
}

int getMonthDays(int year, int month) {
    int monthDays[]={
        31, 
        isLeap(year) ? 29 : 28, 
        31, 
        30, 
        31, 
        30, 
        31, 
        31, 
        30, 
        31, 
        30, 
        31
    }; 
    return monthDays[month]; 
}

void remove_crlf(char* start, char* end) {
	for(char* p=start; p<end; p++)
		switch(*p) {
			case '\n': *p='|';  break;
			case '\r': *p=' ';  break;
		}
}


/// must be last in this file
#undef vsnprintf
int __vsnprintf(char* b, size_t s, const char* f, va_list l) {
	if(!s)
		return 0;

	int r;
	// note: on win32& maybe somewhere else
	// vsnprintf do not writes terminating 0 in 'buffer full' case, reducing
	--s;
#if _MSC_VER
	/*
	win32: 
	mk:@MSITStore:C:\Program%20Files\Microsoft%20Visual%20Studio\MSDN\2001APR\1033\vccore.chm::/html/_crt__vsnprintf.2c_._vsnwprintf.htm

	  if the number of bytes to write exceeds buffer, then count bytes are written and –1 is returned
	*/
	r=_vsnprintf(b, s, f, l); 
	if(r<0) 
		r=s;
#else
	r=vsnprintf(b, s, f, l); 
	/*
	solaris: 
	man vsnprintf

	  The snprintf() function returns  the  number  of  characters
	formatted, that is, the number of characters that would have
	been written to the buffer if it were large enough.  If  the
	value  of  n  is  0  on a call to snprintf(), an unspecified
	value less than 1 is returned.
	*/

	if(r<0)
		r=0;
	else if(r>s)
		r=s;
#endif
	b[r]=0;
	return r;
}

int __snprintf(char* b, size_t s, const char* f, ...) {
	va_list l;
    va_start(l, f); 
    int r=__vsnprintf(b, s, f, l); 
    va_end(l); 
	return r;
}

int pa_sleep(unsigned long secs, unsigned long usecs) {
	for (;  usecs >= 1000000; ++secs, usecs -= 1000000); 

#ifdef WIN32
	Sleep(secs * 1000 + usecs / 1000); 
	return 0;
#else
	struct timeval t;
	t.tv_sec = secs;
	t.tv_usec = usecs;
	return (select(0, NULL, NULL, NULL, &t) == -1 ? errno : 0); 
#endif
}


// attributed meaning

/// http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.3
static size_t date_attribute(const VDate& vdate, char *buf, size_t buf_size) {
    const char month_names[12][4]={
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    const char days[7][4]={
		"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	time_t when=vdate.get_time();
	struct tm *tms=gmtime(&when);
	if(!tms)
		throw Exception(0,
			0,
			"bad time in attribute value (seconds from epoch=%ld)", when);
	return snprintf(buf, MAX_STRING, "%s, %.2d %s %.4d %.2d:%.2d:%.2d GMT", 
		days[tms->tm_wday],
		tms->tm_mday,month_names[tms->tm_mon],tms->tm_year+1900,
		tms->tm_hour,tms->tm_min,tms->tm_sec);
}
static void append_attribute_meaning(String& result,
									 Value& value, String::Untaint_lang lang, bool forced) {
	if(const String *string=value.get_string())
		result.append(string->join_chains(result.pool(), 0), lang, forced);
	else
		if(Value *vdate=value.as(VDATE_TYPE, false)) {
			char *buf=(char *)result.malloc(MAX_STRING);
			size_t size=date_attribute(*static_cast<VDate *>(vdate), 
				buf, MAX_STRING);

			result.APPEND_CLEAN(buf, size, "converted from date", 0);
		} else
			throw Exception("parser.runtime",
				&result,
				"trying to append here neither string nor date (%s)",
					value.type());
}
#ifndef DOXYGEN
struct Attributed_meaning_info {
	String *header; // header line being constructed
	String::Untaint_lang lang; // language in which to append to that line
	bool forced; // do they force that lang?
};
#endif
static void append_attribute_subattribute(const Hash::Key& akey, Hash::Val *avalue, 
										  void *info) {
	if(akey==VALUE_NAME)
		return;

	Attributed_meaning_info& ami=*static_cast<Attributed_meaning_info *>(info);

	// ...; charset=windows1251
	*ami.header << "; ";
	ami.header->append(akey, ami.lang, ami.forced);
	*ami.header << "=";
	append_attribute_meaning(*ami.header, *static_cast<Value *>(avalue), ami.lang, ami.forced);
}
const String& attributed_meaning_to_string(Value& meaning, 
										   String::Untaint_lang lang, bool forced) {
	String &result=*new(meaning.pool()) String(meaning.pool());
	if(Hash *hash=meaning.get_hash(0)) {
		// $value(value) $subattribute(subattribute value)
		if(Value *value=static_cast<Value *>(hash->get(*value_name)))
			append_attribute_meaning(result, *value, lang, forced);

		Attributed_meaning_info attributed_meaning_info={&result, lang};
		hash->for_each(append_attribute_subattribute, &attributed_meaning_info);
	} else // result value
		append_attribute_meaning(result, meaning, lang, forced);

	return result;
}
