/** @file
	Parser: @b file parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: file.C,v 1.80 2002/04/18 10:50:59 paf Exp $
*/

#include "pa_config_includes.h"

#include "pcre.h"

#include "classes.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_table.h"
#include "pa_vint.h"
#include "pa_exec.h"
#include "pa_vdate.h"
#include "pa_dir.h"
#include "pa_vtable.h"
#include "pa_charset.h"

// consts

// defines

#define TEXT_MODE_NAME "text"

// class

class MFile : public Methoded {
public: // VStateless_class
	
	Value *create_new_value(Pool& pool) { return new(pool) VFile(pool); }

public: // Methoded
	bool used_directly() { return true; }

public:
	MFile(Pool& pool);

};

// consts

const int FIND_MONKEY_MAX_HOPS=10;

// methods

static void _save(Request& r, const String&, MethodParams *params) {
	Value& vmode_name=params-> as_no_junction(0, "mode must not be code");
	Value& vfile_name=params->as_no_junction(1, "file name must not be code");

	// save
	static_cast<VFile *>(r.self)->save(r.absolute(vfile_name.as_string()),
		vmode_name.as_string()==TEXT_MODE_NAME);
}

static void _delete(Request& r, const String&, MethodParams *params) {
	Value& vfile_name=params->as_no_junction(0, "file name must not be code");

	// unlink
	file_delete(r.absolute(vfile_name.as_string()));
}

static void _move(Request& r, const String&, MethodParams *params) {
	Value& vfrom_file_name=params->as_no_junction(0, "from file name must not be code");
	Value& vto_file_name=params->as_no_junction(1, "to file name must not be code");

	// move
	file_move(
		r.absolute(vfrom_file_name.as_string()),
		r.absolute(vto_file_name.as_string()));
}

static void _find(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfile_name=params->as_no_junction(0, "file name must not be code");

	const String &lfile_name=vfile_name.as_string();

	// passed file name simply exists in current dir
	if(file_readable(r.absolute(lfile_name))) {
		r.write_no_lang(lfile_name);
		return;
	}

	// scan .. dirs for result
	for(int i=0; i<FIND_MONKEY_MAX_HOPS; i++) {
		String local_test_name(pool);
		for(int j=0; j<i; j++)
			local_test_name.APPEND_CONST("../");
		local_test_name.append(lfile_name, String::UL_CLEAN);
		if(file_readable(r.absolute(local_test_name))) {
			r.write_no_lang(*new(pool) String(local_test_name));
			return;
		}
	}

	// not found
	if(params->size()==2) {
		Value& not_found_code=params->as_junction(1, "not-found param must be code");
		r.write_pass_lang(r.process(not_found_code));
	}
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vmode_name=params-> as_no_junction(0, "mode must not be code");
	Value& vfile_name=params->as_no_junction(1, "file name must not be code");

	const String& lfile_name=vfile_name.as_string();

	void *data;  size_t size;
	file_read(pool, r.absolute(lfile_name), data, size, 
		vmode_name.as_string()==TEXT_MODE_NAME);

	char *user_file_name=params->size()>2?
		params->as_string(2, "filename must be string").cstr(String::UL_FILE_SPEC)
		:lfile_name.cstr(String::UL_FILE_SPEC);
	
	static_cast<VFile *>(r.self)->set(true/*tainted*/, data, size, 
		user_file_name, new(pool) VString(r.mime_type_of(user_file_name)));
}

static void _stat(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfile_name=params->as_no_junction(0, "file name must not be code");

	const String& lfile_name=vfile_name.as_string();

	size_t size;
	time_t atime, mtime, ctime;
	file_stat(r.absolute(lfile_name),
		size,
		atime, mtime, ctime);
	
	VFile& vfile=*static_cast<VFile *>(r.self);
	vfile.set(true/*tainted*/, 0/*no bytes*/, size);
	Hash& ff=vfile.fields();
	ff.put(*new(pool) String(pool, "adate"), new(pool) VDate(pool, atime));
	ff.put(*new(pool) String(pool, "mdate"), new(pool) VDate(pool, mtime));
	ff.put(*new(pool) String(pool, "cdate"), new(pool) VDate(pool, ctime));
}

static void append_env_pair(const Hash::Key& key, Hash::Val *value, void *info) {
	Hash& hash=*static_cast<Hash *>(info);
	hash.put(key, &static_cast<Value *>(value)->as_string());
}

static void pass_cgi_header_attribute(Array::Item *value, void *info) {
	String& string=*static_cast<String *>(value);
	Hash& hash=*static_cast<Hash *>(info);
	int colon_pos=string.pos(":", 1);
	if(colon_pos>0)
		hash.put(string.mid(0, colon_pos), 
		new(string.pool()) VString(string.mid(colon_pos+1, string.size())));
}
/** @todo fix `` in perl - they produced flipping consoles and no output to perl
	@test EPASS, ECSTR [touched them when optimized hash]
*/
static void _exec_cgi(Request& r, const String& method_name, MethodParams *params,
					  bool cgi) {
	Pool& pool=r.pool();

	Value& vfile_name=params->as_no_junction(0, "file name must not be code");

	const String& script_name=r.absolute(vfile_name.as_string());

	Hash env(pool);
	#define ECSTR(name, value_cstr) \
		String name##key(pool, #name); \
		String name##value(pool); \
		if(value_cstr) { \
			name##value.APPEND_CONST(value_cstr); \
			env.put(name##key, &name##value); \
		}
	#define EPASS(name) \
		String name##key(pool, #name); \
		String name##value(pool); \
		if(const char *value_cstr=SAPI::get_env(pool, #name)) { \
			name##value.APPEND_CONST(value_cstr); \
			env.put(name##key, &name##value); \
		}

	// const
	ECSTR(GATEWAY_INTERFACE, "CGI/1.1");
	// from Request.info
	ECSTR(DOCUMENT_ROOT, r.info.document_root);
	ECSTR(PATH_TRANSLATED, r.info.path_translated);
	ECSTR(REQUEST_METHOD, r.info.method);
	ECSTR(QUERY_STRING, r.info.query_string);
	ECSTR(REQUEST_URI, r.info.uri);
	ECSTR(CONTENT_TYPE, r.info.content_type);
	char content_length_cstr[MAX_NUMBER];  
	snprintf(content_length_cstr, MAX_NUMBER, "%u", r.info.content_length);
	String content_length(pool, content_length_cstr);
	ECSTR(CONTENT_LENGTH, content_length_cstr);
	ECSTR(HTTP_COOKIE, r.info.cookie);
	ECSTR(HTTP_USER_AGENT, r.info.user_agent);
	// passing some SAPI:get_env-s
	EPASS(SERVER_PROTOCOL);
	EPASS(SERVER_NAME);
	EPASS(SERVER_PORT);
	EPASS(HTTP_REFERER);
	EPASS(REMOTE_ADDR);
	EPASS(REMOTE_HOST);
	EPASS(REMOTE_USER);
	// SCRIPT_NAME
	env.put(*new(pool) String(pool, "SCRIPT_NAME"), &script_name);
#ifdef WIN32
	// WIN32 shell
	EPASS(COMSPEC);
#endif

	if(params->size()>1) {
		Value& venv=params->as_no_junction(1, "env must not be code");
		if(Hash *user_env=venv.get_hash(&method_name))
			user_env->for_each(append_env_pair, &env);
	}

	Array *argv=0;
	if(params->size()>2) {
		argv=new(pool) Array(pool, params->size()-2);
		for(int i=2; i<params->size(); i++)
			*argv+=&params->as_string(i, "parameter must be string");
	}

	String in(pool);
	in.APPEND(r.post_data, r.post_size, String::UL_CLEAN, "passing post data", 0);
	String out(pool);
	//out.APPEND_CONST("content-type:text/plain\nheader:test-header\n\ntest-body");
	//out<<in;
	String& err=*new(pool) String(pool);
	int status=pa_exec(false/*forced_allow*/, script_name, &env, argv, in, out, err);

	VFile& self=*static_cast<VFile *>(r.self);

	const String *body=&out; // ^file:exec
	if(cgi) { // ^file:cgi
		// construct with 'out' body and header
		int delim_size;
		const char *eol_marker="\r\n"; size_t eol_marker_size=2;
		int pos=out.pos("\r\n\r\n", delim_size=4);
		if(pos<0) {
			eol_marker="\n"; eol_marker_size=1;
			pos=out.pos("\n\n", delim_size=2);
		}
		if(pos<0) {
			delim_size=0; // calm down, compiler
			throw Exception(0,
				&method_name,
				"output does not contain CGI header; exit status=%d; stdoutsize=%u; stdout: \"%s\"; stderrsize=%u; stderr: \"%s\"", 
					status, 
					(uint)out.size(), out.cstr(),
					(uint)err.size(), err.cstr());
		}

		const String& header=out.mid(0, pos);
		body=&out.mid(pos+delim_size, out.size());

		// header to $fields
		{
			Array rows(pool);
			header.split(rows, 0, eol_marker, eol_marker_size, String::UL_CLEAN);
			rows.for_each(pass_cgi_header_attribute, &self.fields());
		}
	}
	// body
	self.set(false/*not tainted*/, body->cstr(), body->size());

	// $status
	self.fields().put(
		*new(pool) String(pool, "status"),
		new(pool) VInt(pool, status));
	
	// $stderr
	if(err.size()) {
		self.fields().put(
			*new(pool) String(pool, "stderr"),
			new(pool) VString(err));

		SAPI::log(pool, "file:%s: %s", cgi?"cgi":"exec", err.cstr());
	}
}
static void _exec(Request& r, const String& method_name, MethodParams *params) {
	_exec_cgi(r, method_name, params, false);
}
static void _cgi(Request& r, const String& method_name, MethodParams *params) {
	_exec_cgi(r, method_name, params, true);
}

static void _list(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& relative_path=params->as_no_junction(0, "path must not be code");

	const String *regexp;
	pcre *regexp_code;
	int ovecsize;
	int *ovector;
	if(params->size()>1) {
		regexp=&params->as_no_junction(1, "regexp must not be code").as_string();

		const char *pattern=regexp->cstr();
		const char *errptr;
		int erroffset;
		regexp_code=pcre_compile(pattern, PCRE_EXTRA | PCRE_DOTALL, 
			&errptr, &erroffset, 
			pool.get_client_charset().pcre_tables);

		if(!regexp_code)
			throw Exception(0, 
				&regexp->mid(erroffset, regexp->size()), 
				"regular expression syntax error - %s", errptr);

		ovector=(int *)pool.malloc(sizeof(int)*(ovecsize=(1/*match*/)*3));
	} else 
		regexp_code=0;


	const char* absolute_path_cstr=r.absolute(relative_path.as_string())
		.cstr(String::UL_FILE_SPEC);

	Array& columns=*new(pool) Array(pool);
	columns+=new(pool) String(pool, "name");	
	Table& table=*new(pool) Table(pool, &method_name, &columns);

	LOAD_DIR(absolute_path_cstr, 
		size_t file_name_size=strlen(ffblk.ff_name);
		bool suits=true;
		if(regexp_code) {
			int exec_result=pcre_exec(regexp_code, 0, 
				ffblk.ff_name, file_name_size, 0, 
				0, ovector, ovecsize);
			
			if(exec_result==PCRE_ERROR_NOMATCH)
				suits=false;
			else if(exec_result<0) {
				(*pcre_free)(regexp_code);
				throw Exception(0, 
					regexp, 
					"regular expression execute (%d)", 
						exec_result);
			}
		}

		if(suits) {
			char *file_name_cstr=(char *)pool.malloc(file_name_size);
			memcpy(file_name_cstr, ffblk.ff_name, file_name_size);
			String &file_name=*new(pool) String(pool);
			file_name.APPEND(file_name_cstr, file_name_size, String::UL_FILE_SPEC, 
				method_name.origin().file, method_name.origin().line);
		
			Array& row=*new(pool) Array(pool);
			row+=&file_name;
			table+=&row;
		}
	);

	if(regexp_code)
		(*pcre_free)(regexp_code);

	// write out result
	VTable& result=*new(pool) VTable(pool, &table);
	r.write_no_lang(result);
}

#ifndef DOXYGEN
struct Lock_execute_body_info {
	Request *r;
	Value *body_code;
};
#endif
static void lock_execute_body(int , void *context) {
	Lock_execute_body_info& info=*static_cast<Lock_execute_body_info *>(context);

	// execute body
	info.r->write_assign_lang(info.r->process(*info.body_code));
};
static void _lock(Request& r, const String& method_name, MethodParams *params) {
	const String& file_spec=r.absolute(params->as_string(0, "file name must be string"));
	Value& body_code=params->as_junction(1, "body must be code");

	Lock_execute_body_info info={&r, &body_code};
	file_write_action_under_lock(file_spec, "lock", lock_execute_body, &info);
}

// constructor

MFile::MFile(Pool& apool) : Methoded(apool, "file") {
	// ^save[mode;file-name]
	add_native_method("save", Method::CT_DYNAMIC, _save, 2, 2);

	// ^delete[file-name]
	add_native_method("delete", Method::CT_STATIC, _delete, 1, 1);

	// ^move[from-file-name;to-file-name]
	add_native_method("move", Method::CT_STATIC, _move, 2, 2);

	// ^find[file-name]
	// ^find[file-name]{when-not-found}
	add_native_method("find", Method::CT_STATIC, _find, 1, 2);

	// ^load[mode;disk-name]
	// ^load[mode;disk-name;user-name]
	add_native_method("load", Method::CT_DYNAMIC, _load, 2, 3);

	// ^stat[disk-name]
	add_native_method("stat", Method::CT_DYNAMIC, _stat, 1, 1);

	// ^cgi[file-name]
	// ^cgi[file-name;env hash]
	// ^cgi[file-name;env hash;1cmd;2line;3ar;4g;5s]
	add_native_method("cgi", Method::CT_DYNAMIC, _cgi, 1, 2+10);

	// ^exec[file-name]
	// ^exec[file-name;env hash]
	// ^exec[file-name;env hash;1cmd;2line;3ar;4g;5s]
	add_native_method("exec", Method::CT_DYNAMIC, _exec, 1, 2+10);

	// ^file:list[path]
	// ^file:list[path][regexp]
	add_native_method("list", Method::CT_STATIC, _list, 1, 2);

	// ^file:lock[path]{code}
	add_native_method("lock", Method::CT_STATIC, _lock, 2, 2);

}

// global variable

Methoded *file_class;

// creator

Methoded *MFile_create(Pool& pool) {
	return file_class=new(pool) MFile(pool);
}
