/** @file
	Parser: @b file parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: file.C,v 1.52 2001/09/13 14:10:54 parser Exp $
*/
static const char *RCSId="$Id: file.C,v 1.52 2001/09/13 14:10:54 parser Exp $"; 

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

// consts

// defines

#define FILE_CLASS_NAME "file"

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
	Pool& pool=r.pool();
	Value& vfile_name=params->as_no_junction(0, "file name must not be code");

	// unlink
	file_delete(pool, r.absolute(vfile_name.as_string()));
}

static void _move(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfrom_file_name=params->as_no_junction(0, "from file name must not be code");
	Value& vto_file_name=params->as_no_junction(1, "to file name must not be code");

	// move
	file_move(pool, 
		r.absolute(vfrom_file_name.as_string()),
		r.absolute(vto_file_name.as_string()));
}

static void _find(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfile_name=params->as_no_junction(0, "file name must not be code");

	const String &lfile_name=vfile_name.as_string();

	// passed file name simply exists in current dir
	if(file_readable(r.absolute(lfile_name))) {
		r.write_no_lang(*new(pool) VString(lfile_name));
		return;
	}

	// scan .. dirs for result
	for(int i=0; i<FIND_MONKEY_MAX_HOPS; i++) {
		String test_name(pool);
		for(int j=0; j<i; j++)
			test_name.APPEND_CONST("../");
		test_name.append(lfile_name, String::UL_CLEAN);
		if(file_readable(r.absolute(test_name))) {
			r.write_no_lang(*new(pool) VString(*new(pool) String(test_name)));
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

	char *user_file_name=params->size()>2?params->get(2).as_string().cstr()
		:lfile_name.cstr(String::UL_FILE_NAME);
	
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
/// @todo fix `` in perl - they produced flipping consoles and no output to perl
static void _exec_cgi(Request& r, const String& method_name, MethodParams *params,
					  bool cgi) {
	Pool& pool=r.pool();

	Value& vfile_name=params->as_no_junction(0, "file name must not be code");

	const String& script_name=r.absolute(vfile_name.as_string());

	Hash env(pool);
	#define PASS(key) \
		String key(pool); \
		if(const char *value=SAPI::get_env(pool, #key)) { \
			key.APPEND_CONST(value); \
			env.put(String(pool, #key), &key); \
		}
	#define INFO(key, value) \
		String value(pool); \
		if(r.info.value) { \
			value.APPEND_CONST(r.info.value); \
			env.put(String(pool, key), &value); \
		}

	// const
	String gateway_interface(pool, "CGI/1.1");
	env.put(String(pool, "GATEWAY_INTERFACE"), &gateway_interface);
	// from Request.info
	INFO("DOCUMENT_ROOT", document_root);
	INFO("PATH_TRANSLATED", path_translated);
	INFO("SERVER_PROTOCOL", method);
	INFO("QUERY_STRING", query_string);
	INFO("REQUEST_URI", uri);
	INFO("CONTENT_TYPE", content_type);
	char content_length_cstr[MAX_NUMBER];  
	snprintf(content_length_cstr, MAX_NUMBER, "%u", r.info.content_length);
	String content_length(pool, content_length_cstr);
	env.put(String(pool, "CONTENT_LENGTH"), &content_length);
	INFO("HTTP_COOKIE", cookie);
	INFO("HTTP_USER_AGENT", user_agent);
	// passing some SAPI:get_env-s
	PASS(SERVER_NAME);
	PASS(SERVER_PORT);
	PASS(HTTP_REFERER);
	PASS(REMOTE_ADDR);
	PASS(REMOTE_HOST);
	PASS(REMOTE_USER);
	// SCRIPT_NAME
	env.put(String(pool, "SCRIPT_NAME"), &script_name);
#ifdef WIN32
	// WIN32 shell
	PASS(COMSPEC);
#endif

	if(params->size()>1) {
		Value& venv=params->as_no_junction(1, "env must not be code");
		if(Hash *user_env=venv.get_hash())
			user_env->for_each(append_env_pair, &env);
	}

	Array *argv=0;
	if(params->size()>2) {
		argv=new(pool) Array(pool, params->size()-2);
		for(int i=2; i<params->size(); i++)
			*argv+=&params->get(i).as_string();
	}

	const String in(pool, r.post_data, r.post_size);
	String out(pool);
	//out.APPEND_CONST("content-type:text/plain\nheader:test-header\n\ntest-body");
	//out<<in;
	String& err=*new(pool) String(pool);
	int status=pa_exec(script_name, &env, argv, in, out, err);

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
			PTHROW(0, 0,
				&method_name,
				"output does not contain CGI header; exit code=%d; outsize=%u; out: \"%s\"; errsize=%u; err: \"%s\"", 
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
	self.set(false/*not tainted*/, body->cstr(String::UL_AS_IS), body->size());

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

		const char *pattern=regexp->cstr(String::UL_AS_IS);
		const char *errptr;
		int erroffset;
		regexp_code=pcre_compile(pattern, PCRE_EXTRA | PCRE_DOTALL, 
			&errptr, &erroffset, 
			r.pcre_tables);

		if(!regexp_code)
			PTHROW(0, 0, 
				&regexp->mid(erroffset, regexp->size()), 
				"regular expression syntax error - %s", errptr);

		ovector=(int *)pool.malloc(sizeof(int)*(ovecsize=(1/*match*/)*3));
	} else 
		regexp_code=0;


	const char* absolute_path_cstr=r.absolute(relative_path.as_string())
		.cstr(String::UL_FILE_NAME);

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
				PTHROW(0, 0, 
					regexp, 
					"regular expression execute (%d)", 
						exec_result);
			}
		}

		if(suits) {
			char *file_name_cstr=(char *)pool.malloc(file_name_size);
			memcpy(file_name_cstr, ffblk.ff_name, file_name_size);
			String &file_name=*new(pool) String(pool);
			file_name.APPEND(file_name_cstr, file_name_size, String::UL_FILE_NAME, 
				method_name.origin().file, method_name.origin().line);
		
			Array& row=*new(pool) Array(pool);
			row+=&file_name;
			table+=&row;
		}
	);

	if(regexp_code)
		(*pcre_free)(regexp_code);

	VTable& result=*new(pool) VTable(pool, &table);
	result.set_name(method_name);
	r.write_no_lang(result);
}

// constructor

MFile::MFile(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), FILE_CLASS_NAME));


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

}

// global variable

Methoded *file_class;

// creator

Methoded *MFile_create(Pool& pool) {
	return file_class=new(pool) MFile(pool);
}
