/** @file
	Parser: @b file parser class.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_FILE_C="$Date: 2003/01/21 15:51:06 $";

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

// defines

#define TEXT_MODE_NAME "text"
#define STDIN_EXEC_PARAM_NAME "stdin"

// consts

/// from apache-1.3|src|support|suexec.c 
static const char *suexec_safe_env_lst[]={
    "AUTH_TYPE",
    "CONTENT_LENGTH",
    "CONTENT_TYPE",
    "DATE_GMT",
    "DATE_LOCAL",
    "DOCUMENT_NAME",
    "DOCUMENT_PATH_INFO",
    "DOCUMENT_ROOT",
    "DOCUMENT_URI",
    "FILEPATH_INFO",
    "GATEWAY_INTERFACE",
    "LAST_MODIFIED",
    "PATH_INFO",
    "PATH_TRANSLATED",
    "QUERY_STRING",
    "QUERY_STRING_UNESCAPED",
    "REMOTE_ADDR",
    "REMOTE_HOST",
    "REMOTE_IDENT",
    "REMOTE_PORT",
    "REMOTE_USER",
    "REDIRECT_QUERY_STRING",
    "REDIRECT_STATUS",
    "REDIRECT_URL",
    "REQUEST_METHOD",
    "REQUEST_URI",
    "SCRIPT_FILENAME",
    "SCRIPT_NAME",
    "SCRIPT_URI",
    "SCRIPT_URL",
    "SERVER_ADMIN",
    "SERVER_NAME",
    "SERVER_ADDR",
    "SERVER_PORT",
    "SERVER_PROTOCOL",
    "SERVER_SOFTWARE",
    "UNIQUE_ID",
    "USER_NAME",
    "TZ",
    NULL
};

// class

class MFile : public Methoded {
public: // VStateless_class
	
	Value *create_new_value(Pool& pool) { return new(pool) VFile(pool); }

public: // Methoded
	bool used_directly() { return true; }

public:
	MFile(Pool& pool);

};

// methods

static void _save(Request& r, const String&, MethodParams *params) {
	Value& vmode_name=params-> as_no_junction(0, "mode must not be code");
	Value& vfile_name=params->as_no_junction(1, "file name must not be code");

	// save
	static_cast<VFile *>(r.get_self())->save(r.absolute(vfile_name.as_string()),
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

static void _load_pass_param(const Hash::Key& key, Hash::Val *value, void *info) {
	Hash& dest=*static_cast<Hash *>(info);
	dest.put(key, value);
}
static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vmode_name=params-> as_no_junction(0, "mode must not be code");
	const String& lfile_name=r.absolute(params->as_no_junction(1, "file name must not be code").as_string());
	Value *third_param=params->size()>2?&params->as_no_junction(2, "filename or options must not be code"):0;
	Hash *third_param_hash=third_param?third_param->get_hash(&method_name):0;
	int alt_filename_param_index=2;
	if(third_param_hash)
		alt_filename_param_index++;

	void *data;  size_t size;
	Hash *fields=0;
	file_read(pool, lfile_name, data, size, 
		vmode_name.as_string()==TEXT_MODE_NAME,
		third_param_hash,
		&fields
	);

	char *user_file_name=params->size()>alt_filename_param_index?
		params->as_string(alt_filename_param_index, "filename must be string").cstr(String::UL_FILE_SPEC)
		:lfile_name.cstr(String::UL_FILE_SPEC);

	Value *vcontent_type=0;
	if(fields)
		vcontent_type=static_cast<Value *>(fields->get(*content_type_name));
	if(!vcontent_type)
		vcontent_type=new(pool) VString(r.mime_type_of(user_file_name));
	
	VFile& self=*static_cast<VFile *>(r.get_self());
	self.set(true/*tainted*/, data, size, user_file_name, vcontent_type);
	if(fields)
		fields->for_each(_load_pass_param, &self.fields());
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
	
	VFile& vfile=*static_cast<VFile *>(r.get_self());
	vfile.set(true/*tainted*/, 0/*no bytes*/, size);
	Hash& ff=vfile.fields();
	ff.put(*new(pool) String(pool, "adate"), new(pool) VDate(pool, atime));
	ff.put(*new(pool) String(pool, "mdate"), new(pool) VDate(pool, mtime));
	ff.put(*new(pool) String(pool, "cdate"), new(pool) VDate(pool, ctime));
	ff.put(*content_type_name, new(pool) VString(r.mime_type_of(lfile_name.cstr(String::UL_FILE_SPEC))));
}

static bool is_safe_env_key(const char *key) {
	if(strncasecmp(key, "HTTP_", 5)==0)
		return true;
	if(strncasecmp(key, "CGI_", 4)==0)
		return true;
	for(int i=0; suexec_safe_env_lst[i]; i++) {
		if(strcasecmp(key, suexec_safe_env_lst[i])==0)
			return true;
	}
	return false;
}
#ifndef DOXYGEN
struct Append_env_pair_info {
	Hash* hash;
	Value* vstdin;
};
#endif
static void append_env_pair(const Hash::Key& key, Hash::Val *avalue, void *info) {
	Append_env_pair_info& pi=*static_cast<Append_env_pair_info *>(info);
	Value& value=*static_cast<Value *>(avalue);

	if(key==STDIN_EXEC_PARAM_NAME) {
		pi.vstdin=&value;
	} else {
		if(!is_safe_env_key(key.cstr()))
			throw Exception("parser.runtime",
				&key,
				"not safe environment variable");
		pi.hash->put(key, &value.as_string());
	}
}
#ifndef DOXYGEN
struct Pass_cgi_header_attribute_info {
	Hash *hash;
	Value *content_type;
};
#endif
static void pass_cgi_header_attribute(Array::Item *value, void *ainfo) {
	String& string=*static_cast<String *>(value);
	Pool& pool=string.pool();
	Pass_cgi_header_attribute_info& info=*static_cast<Pass_cgi_header_attribute_info *>(ainfo);
	int colon_pos=string.pos(":", 1);
	if(colon_pos>0) {
		const String& key=string.mid(0, colon_pos).change_case(pool, String::CC_UPPER);
		Value *value=new(pool) VString(string.mid(colon_pos+1, string.size()));
		info.hash->put(key, value);
		if(key=="CONTENT-TYPE")
			info.content_type=value;
	}
}
/// @todo fix `` in perl - they produced flipping consoles and no output to perl
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
	// passing SAPI::environment
	if(const char *const *pairs=SAPI::environment(pool)) {
		while(const char *pair=*pairs++)
			if(const char *eq_at=strchr(pair, '=')) {
				String& key=*new(pool) String(pool, pair, eq_at-pair);
				String& value=*new(pool) String(pool, eq_at+1);
				env.put(key, &value);
			}
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
	// SCRIPT_*
	env.put(*new(pool) String(pool, "SCRIPT_NAME"), &script_name);
	//env.put(*new(pool) String(pool, "SCRIPT_FILENAME"), ??&script_name);

	bool stdin_specified=false;
	// environment & stdin from param
	String in(pool);
	if(params->size()>1) {
		Value& venv=params->as_no_junction(1, "env must not be code");
		if(Hash *user_env=venv.get_hash(&method_name)) {
			Append_env_pair_info info={&env};
			user_env->for_each(append_env_pair, &info);
			if(info.vstdin) {
				stdin_specified=true;
				if(const String *sstdin=info.vstdin->get_string()) {
					in.append(*sstdin, String::UL_CLEAN, true);
				} else
					if(VFile *vfile=static_cast<VFile *>(info.vstdin->as("file", false)))
						in.APPEND_TAINTED((const char *)vfile->value_ptr(), vfile->value_size(),
							"$.stdin[assigned]", 0);
					else
						throw Exception("parser.runtime",
							&method_name,
							STDIN_EXEC_PARAM_NAME " parameter must be string or file");
			}
		}
	}

	// argv from params
	Array *argv=0;
	if(params->size()>2) {
		argv=new(pool) Array(pool, params->size()-2);
		for(int i=2; i<params->size(); i++)
			*argv+=&params->as_string(i, "parameter must be string");
	}

	// passing POST data
	if(!stdin_specified) // if $.stdin[...] not specified 
		in.APPEND(r.post_data, r.post_size, String::UL_CLEAN, "POST data (passed)", 0);

	// exec!
	String out(pool);
	String& err=*new(pool) String(pool);
	int status=pa_exec(false/*forced_allow*/, script_name, &env, argv, in, out, err);

	VFile& self=*static_cast<VFile *>(r.get_self());

	const String *body=&out; // ^file:exec
	Value *content_type=0;
	const char *eol_marker=0; size_t eol_marker_size;
	const String *header=0;
	if(cgi) { // ^file:cgi
		// construct with 'out' body and header
		int dos_pos=out.pos("\r\n\r\n", 4);
		int unix_pos=out.pos("\n\n", 2);

		bool unix_header_break;
		switch((dos_pos >= 0?10:00) + (unix_pos >= 0?01:00)) {
		case 10: // dos
			unix_header_break=false;
			break;
		case 01: // unix
			unix_header_break=true;
			break;
		case 11: // dos & unix
			unix_header_break=unix_pos<dos_pos;
			break;
		default: // 00
			unix_header_break=false; // calm down, compiler
			throw Exception(0,
				&method_name,
				"output does not contain CGI header; "
				"exit status=%d; stdoutsize=%u; stdout: \"%s\"; stderrsize=%u; stderr: \"%s\"", 
					status, 
					(uint)out.size(), out.cstr(),
					(uint)err.size(), err.cstr());
			break; //never reached
		}

		int header_break_pos;
		if(unix_header_break) {
			header_break_pos=unix_pos;
			eol_marker="\n"; eol_marker_size=1;
		} else {
			header_break_pos=dos_pos;
			eol_marker="\r\n"; eol_marker_size=2;
		}

		header=&out.mid(0, header_break_pos);
		body=&out.mid(header_break_pos+eol_marker_size*2, out.size());
	}
	// body
	self.set(false/*not tainted*/, body->cstr(), body->size());

	// $fields << header
	if(header && eol_marker) {
		Array rows(pool);
		header->split(rows, 0, eol_marker, eol_marker_size);
		Pass_cgi_header_attribute_info info={&self.fields()};
		rows.for_each(pass_cgi_header_attribute, &info);
		if(info.content_type)
			self.fields().put(*content_type_name, info.content_type);
	}

	// $status
	self.fields().put(
		*file_status_name,
		new(pool) VInt(pool, status));
	
	// $stderr
	if(err.size()) {
		self.fields().put(
			*new(pool) String(pool, "stderr"),
			new(pool) VString(err));
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
	const int ovecsize=(1/*match*/)*3;
	int ovector[ovecsize];
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
			file_name.APPEND_TAINTED(file_name_cstr, file_name_size, 
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

static int lastposafter(const String& s, int after, const char *substr, size_t substr_size, bool beforelast=false) {
	size_t size;
	if(beforelast)
		size=s.size();
	int at;
	while((at=s.pos(substr, substr_size, after))>=0) {
		size_t newafter=at+substr_size/*skip substr*/;
		if(beforelast && newafter==size)
			break;
		after=newafter;
	}

	return after;
}

static void _find(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String &file_name=params->as_no_junction(0, "file name must not be code").as_string();
	const String *file_spec;
	if(file_name.first_char()=='/')
		file_spec=&file_name;
	else 
		file_spec=&r.relative(r.info.uri, file_name);

	// easy way
	if(file_readable(r.absolute(*file_spec))) {
		r.write_assign_lang(*file_spec);
		return;
	}

	// monkey way
	int after_base_slash=lastposafter(*file_spec, 0, "/", 1);
	const String *dirname=&file_spec->mid(0, after_base_slash);
	const String& basename=file_spec->mid(after_base_slash, file_spec->size());

	int after_monkey_slash;
	while((after_monkey_slash=lastposafter(*dirname, 0, "/", 1, true))>0) {
		String local_test_name(pool);
		local_test_name<<*(dirname=&dirname->mid(0, after_monkey_slash));
		local_test_name<<basename;
		if(file_readable(r.absolute(local_test_name))) {
			r.write_assign_lang(*new(pool) String(local_test_name));
			return;
		}
	}

	// no way, not found
	if(params->size()==2) {
		Value& not_found_code=params->as_junction(1, "not-found param must be code");
		r.write_pass_lang(r.process(not_found_code));
	}
}

static void _dirname(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& file_spec=params->as_string(0, "file name must be string");
    // /a/some.tar.gz > /a
	// /a/b/ > /a
	int afterslash=lastposafter(file_spec, 0, "/", 1, true);
	if(afterslash>0)
		r.write_assign_lang(file_spec.mid(0, afterslash==1?1:afterslash-1));
	else
		r.write_assign_lang(*new(pool) String(pool, ".", 1));
}

static void _basename(Request& r, const String& method_name, MethodParams *params) {
	const String& file_spec=params->as_string(0, "file name must be string");
    // /a/some.tar.gz > some.tar.gz
	int afterslash=lastposafter(file_spec, 0, "/", 1);
	r.write_assign_lang(file_spec.mid(afterslash, file_spec.size()));
}

static void _justname(Request& r, const String& method_name, MethodParams *params) {
	const String& file_spec=params->as_string(0, "file name must be string");
    // /a/some.tar.gz > some.tar
	int afterslash=lastposafter(file_spec, 0, "/", 1);
	int afterdot=lastposafter(file_spec, afterslash, ".", 1);
	r.write_assign_lang(file_spec.mid(afterslash, afterdot!=afterslash?afterdot-1:file_spec.size()));
}
static void _justext(Request& r, const String& method_name, MethodParams *params) {
	const String& file_spec=params->as_string(0, "file name must be string");
    // /a/some.tar.gz > gz
	int afterdot=lastposafter(file_spec, 0, ".", 1);
	if(afterdot>0)
		r.write_assign_lang(file_spec.mid(afterdot, file_spec.size()));
}

static void _fullpath(Request& r, const String& method_name, MethodParams *params) {
	const String& file_spec=params->as_string(0, "file name must be string");
	const String *result;
	if(file_spec.first_char()=='/')
		result=&file_spec;
	else {
		// /some/page.html: ^file:fullpath[a.gif] => /some/a.gif
		const String& full_disk_path=r.absolute(file_spec);
		size_t document_root_length=strlen(r.info.document_root);

		if(document_root_length>0) {
			char last_char=r.info.document_root[document_root_length-1];
			if(last_char == '/' || last_char == '\\')
				--document_root_length;
		}
		result=&full_disk_path.mid(document_root_length,  full_disk_path.size());
	}
	r.write_assign_lang(*result);
}


// constructor

MFile::MFile(Pool& apool) : Methoded(apool, "file") {
	// ^save[mode;file-name]
	add_native_method("save", Method::CT_DYNAMIC, _save, 2, 2);

	// ^delete[file-name]
	add_native_method("delete", Method::CT_STATIC, _delete, 1, 1);

	// ^move[from-file-name;to-file-name]
	add_native_method("move", Method::CT_STATIC, _move, 2, 2);

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

	// ^find[file-name]
	// ^find[file-name]{when-not-found}
	add_native_method("find", Method::CT_STATIC, _find, 1, 2);

    // ^file:dirname[/a/some.tar.gz]=/a
	// ^file:dirname[/a/b/]=/a
	add_native_method("dirname", Method::CT_STATIC, _dirname, 1, 1);
    // ^file:basename[/a/some.tar.gz]=some.tar.gz
    add_native_method("basename", Method::CT_STATIC, _basename, 1, 1);
    // ^file:justname[/a/some.tar.gz]=some.tar
	add_native_method("justname", Method::CT_STATIC, _justname, 1, 1);
    // ^file:justext[/a/some.tar.gz]=gz
	add_native_method("justext", Method::CT_STATIC, _justext, 1, 1);
    // /some/page.html: ^file:fullpath[a.gif] => /some/a.gif
	add_native_method("fullpath", Method::CT_STATIC, _fullpath, 1, 1);
}

// global variable

Methoded *file_class;

// creator

Methoded *MFile_create(Pool& pool) {
	return file_class=new(pool) MFile(pool);
}
