/** @file
	Parser: @b file parser class.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_FILE_C="$Date: 2003/11/20 17:09:35 $";

#include "pa_config_includes.h"

#include "pcre.h"

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_table.h"
#include "pa_vint.h"
#include "pa_exec.h"
#include "pa_vdate.h"
#include "pa_dir.h"
#include "pa_vtable.h"
#include "pa_charset.h"
#include "pa_charsets.h"

// defines

#define TEXT_MODE_NAME "text"
#define STDIN_EXEC_PARAM_NAME "stdin"
#define CHARSET_EXEC_PARAM_NAME "charset"

// class

class MFile: public Methoded {
public: // VStateless_class
	
	Value* create_new_value(Pool&) { return new VFile(); }

public: // Methoded
	bool used_directly() { return true; }

public:
	MFile();

};

// global variable

DECLARE_CLASS_VAR(file, new MFile, 0);

// consts

/// from apache-1.3|src|support|suexec.c 
static const char* suexec_safe_env_lst[]={
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

// statics

static const String::Body adate_name("adate");
static const String::Body mdate_name("mdate");
static const String::Body cdate_name("cdate");

// methods

static void _save(Request& r, MethodParams& params) {
	Value& vmode_name=params. as_no_junction(0, "mode must not be code");
	Value& vfile_name=params.as_no_junction(1, "file name must not be code");

	// save
	GET_SELF(r, VFile).save(r.absolute(vfile_name.as_string()),
		vmode_name.as_string()==TEXT_MODE_NAME);
}

static void _delete(Request& r, MethodParams& params) {
	Value& vfile_name=params.as_no_junction(0, "file name must not be code");

	// unlink
	file_delete(r.absolute(vfile_name.as_string()));
}

static void _move(Request& r, MethodParams& params) {
	Value& vfrom_file_name=params.as_no_junction(0, "from file name must not be code");
	Value& vto_file_name=params.as_no_junction(1, "to file name must not be code");

	// move
	file_move(
		r.absolute(vfrom_file_name.as_string()),
		r.absolute(vto_file_name.as_string()));
}

static void _load_pass_param(
			     HashStringValue::key_type key, 
			     HashStringValue::value_type value, 
			     HashStringValue *dest) {
	dest->put(key, value);
}
static void _load(Request& r, MethodParams& params) {
	Value& vmode_name=params. as_no_junction(0, "mode must not be code");
	const String& lfile_name=r.absolute(params.as_no_junction(1, "file name must not be code").as_string());
	Value* third_param=params.count()>2?&params.as_no_junction(2, "filename or options must not be code")
		:0;
	HashStringValue* third_param_hash=third_param?third_param->get_hash():0;
	size_t alt_filename_param_index=2;
	if(third_param_hash)
		alt_filename_param_index++;

	File_read_result file=file_read(r.charsets, lfile_name,
		vmode_name.as_string()==TEXT_MODE_NAME,
		third_param_hash
	);

	const char *user_file_name=params.count()>alt_filename_param_index?
		params.as_string(alt_filename_param_index, "filename must be string").cstr()
		:lfile_name.cstr(String::L_FILE_SPEC);

	Value* vcontent_type=0;
	if(file.headers)
		vcontent_type=file.headers->get(content_type_name);
	if(!vcontent_type)
		vcontent_type=new VString(r.mime_type_of(user_file_name));
	
	VFile& self=GET_SELF(r, VFile);
	self.set(true/*tainted*/, file.str, file.length, user_file_name, vcontent_type);
	if(file.headers)
		file.headers->for_each(_load_pass_param, &self.fields());
}

static void _stat(Request& r, MethodParams& params) {
	Value& vfile_name=params.as_no_junction(0, "file name must not be code");

	const String& lfile_name=vfile_name.as_string();

	size_t size;
	time_t atime, mtime, ctime;
	file_stat(r.absolute(lfile_name),
		size,
		atime, mtime, ctime);
	
	VFile& self=GET_SELF(r, VFile);
	self.set(true/*tainted*/, 0/*no bytes*/, size);
	HashStringValue& ff=self.fields();
	ff.put(adate_name, new VDate(atime));
	ff.put(mdate_name, new VDate(mtime));
	ff.put(cdate_name, new VDate(ctime));
	ff.put(content_type_name, new VString(r.mime_type_of(lfile_name.cstr(String::L_FILE_SPEC))));
}

static bool is_safe_env_key(const char* key) {
	for(const char* validator=key; *validator; validator++) {
		char c=*validator;
		if(!(c>='A' && c<='Z' || c>='0' && c<='9' || c=='_' || c=='-'))
			return false;
	}
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
	HashStringString* env;
	Value* vstdin;
	Value* vcharset;
};
#endif
static void append_env_pair(
			    HashStringValue::key_type akey, 
			    HashStringValue::value_type avalue, 
			    Append_env_pair_info *info) {
	if(akey==STDIN_EXEC_PARAM_NAME) {
		info->vstdin=avalue;
	} else if(akey==CHARSET_EXEC_PARAM_NAME) {
		info->vcharset=avalue;
	} else {
		if(!is_safe_env_key(akey.cstr()))
			throw Exception("parser.runtime",
				new String(akey, String::L_TAINTED),
				"not safe environment variable");
		info->env->put(akey, avalue->as_string());
	}
}
#ifndef DOXYGEN
struct Pass_cgi_header_attribute_info {
	Charset* charset;
	HashStringValue* fields;
	Value* content_type;
};
#endif
static void pass_cgi_header_attribute(
				      ArrayString::element_type astring, 
				      Pass_cgi_header_attribute_info* info) {
	size_t colon_pos=astring->pos(':');
	if(colon_pos==STRING_NOT_FOUND) {
		const String& key=astring->mid(0, colon_pos).change_case(
			*info->charset, String::CC_UPPER);
		Value* value=new VString(astring->mid(colon_pos+1, astring->length()));
		info->fields->put(key, value);
		if(key=="CONTENT-TYPE")
			info->content_type=value;
	}
}
/// @todo fix `` in perl - they produced flipping consoles and no output to perl
static void _exec_cgi(Request& r, MethodParams& params,
					  bool cgi) {

	Value& vfile_name=params.as_no_junction(0, "file name must not be code");

	const String& script_name=r.absolute(vfile_name.as_string());

	HashStringString env;
	#define ECSTR(name, value_cstr) \
		if(value_cstr) \
			env.put( \
				String::Body(#name), \
				String::Body(value_cstr, 0)); \
	// passing SAPI::environment
	if(const char *const *pairs=SAPI::environment(r.sapi_info)) {
		while(const char* pair=*pairs++)
			if(const char* eq_at=strchr(pair, '='))
				if(eq_at[1]) // has value
					env.put(
						pa_strdup(pair, eq_at-pair),
						pa_strdup(eq_at+1, 0));
	}

	// const
	ECSTR(GATEWAY_INTERFACE, "CGI/1.1");
	// from Request.info
	ECSTR(DOCUMENT_ROOT, r.request_info.document_root);
	ECSTR(PATH_TRANSLATED, r.request_info.path_translated);
	ECSTR(REQUEST_METHOD, r.request_info.method);
	ECSTR(QUERY_STRING, r.request_info.query_string);
	ECSTR(REQUEST_URI, r.request_info.uri);
	ECSTR(CONTENT_TYPE, r.request_info.content_type);
	char content_length_cstr[MAX_NUMBER];  
	snprintf(content_length_cstr, MAX_NUMBER, "%u", r.request_info.content_length);
	//String content_length(content_length_cstr);
	ECSTR(CONTENT_LENGTH, content_length_cstr);
	// SCRIPT_*
	env.put(String::Body("SCRIPT_NAME"), script_name);
	//env.put(String::Body("SCRIPT_FILENAME"), ??&script_name);

	bool stdin_specified=false;
	// environment & stdin from param
	String *in=new String();
	Charset *charset=0; // default script works raw_in 'source' charset = no transcoding needed
	if(params.count()>1) {
		Value& venv=params.as_no_junction(1, "env must not be code");
		if(HashStringValue* user_env=venv.get_hash()) {
			Append_env_pair_info info={&env, 0, 0};
			user_env->for_each(append_env_pair, &info);
			// $.stdin
			if(info.vstdin) {
				stdin_specified=true;
				if(const String* sstdin=info.vstdin->get_string()) {
					in->append(*sstdin, String::L_CLEAN, true);
				} else
					if(VFile* vfile=static_cast<VFile *>(info.vstdin->as("file", false)))
						in->append_know_length((const char* )vfile->value_ptr(), vfile->value_size(), String::L_TAINTED);
					else
						throw Exception("parser.runtime",
							0,
							STDIN_EXEC_PARAM_NAME " parameter must be string or file");
			}
			// $.charset
			if(info.vcharset)
				charset=&charsets.get(info.vcharset->as_string()
					.change_case(r.charsets.source(), String::CC_UPPER));
		}
	}

	// argv from params
	ArrayString argv;
	if(params.count()>2) {
		for(size_t i=2; i<params.count(); i++)
			argv+=&params.as_string(i, "parameter must be string");
	}

	// transcode if necessary
	if(charset) {
		Charset::transcode(env, r.charsets.source(), *charset);
		Charset::transcode(argv, r.charsets.source(), *charset);
		in=&Charset::transcode(*in, r.charsets.source(), *charset);
	}
	// @todo 
	// ifdef WIN32 do  OEM->ANSI transcode on some(.cmd?) programs to 
	// match silent conversion in OS

	// exec!
	PA_exec_result execution=
		pa_exec(false/*forced_allow*/, script_name, &env, argv, *in);

	String *real_out=&execution.out;
	String *real_err=&execution.err;
	// transcode if necessary
	if(charset) {
		real_out=&Charset::transcode(*real_out, *charset, r.charsets.source());
		real_err=&Charset::transcode(*real_err, *charset, r.charsets.source());
	}

	VFile& self=GET_SELF(r, VFile);

	const String* body=real_out; // ^file:exec
	const char* eol_marker=0; size_t eol_marker_size;
	const String* header=0;
	if(cgi) { // ^file:cgi
		// construct with 'out' body and header
		size_t dos_pos=real_out->pos("\r\n\r\n", 4);
		size_t unix_pos=real_out->pos("\n\n", 2);

		bool unix_header_break;
		switch((dos_pos!=STRING_NOT_FOUND?10:00) + (unix_pos!=STRING_NOT_FOUND?01:00)) {
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
				0,
				"output does not contain CGI header; "
				"exit status=%d; stdoutsize=%u; stdout: \"%s\"; stderrsize=%u; stderr: \"%s\"", 
					execution.status, 
					(uint)real_out->length(), real_out->cstr(),
					(uint)real_err->length(), real_err->cstr());
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

		header=&real_out->mid(0, header_break_pos);
		body=&real_out->mid(header_break_pos+eol_marker_size*2, real_out->length());
	}
	// body
	self.set(false/*not tainted*/, body->cstr(), body->length());

	// $fields << header
	if(header && eol_marker) {
		ArrayString rows;
		size_t pos_after=0;
		header->split(rows, pos_after, eol_marker);
		Pass_cgi_header_attribute_info info={0, 0, 0};
		info.charset=&r.charsets.source();
		info.fields=&self.fields();
		rows.for_each(pass_cgi_header_attribute, &info);
		if(info.content_type)
			self.fields().put(content_type_name, info.content_type);
	}

	// $status
	self.fields().put(file_status_name, new VInt(execution.status));
	
	// $stderr
	if(real_err->length())
		self.fields().put(
			String::Body("stderr"),
			new VString(*real_err));
}
static void _exec(Request& r, MethodParams& params) {
	_exec_cgi(r, params, false);
}
static void _cgi(Request& r, MethodParams& params) {
	_exec_cgi(r, params, true);
}

static void _list(Request& r, MethodParams& params) {
	Value& relative_path=params.as_no_junction(0, "path must not be code");

	const String* regexp;
	pcre *regexp_code;
	const int ovecsize=(1/*match*/)*3;
	int ovector[ovecsize];
	if(params.count()>1) {
		regexp=&params.as_no_junction(1, "regexp must not be code").as_string();

		const char* pattern=regexp->cstr();
		const char* errptr;
		int erroffset;
		regexp_code=pcre_compile(pattern, PCRE_EXTRA | PCRE_DOTALL, 
			&errptr, &erroffset, 
			r.charsets.source().pcre_tables);

		if(!regexp_code)
			throw Exception(0, 
				&regexp->mid(erroffset, regexp->length()), 
				"regular expression syntax error - %s", errptr);
	} else {
		regexp=0; // not used, just to calm down compiler
		regexp_code=0;
	}


	const char* absolute_path_cstr=r.absolute(relative_path.as_string()).cstr(String::L_FILE_SPEC);

	Table::columns_type columns(new ArrayString);
	*columns+=new String("name");
	Table& table=*new Table(columns);

	LOAD_DIR(absolute_path_cstr, 
		const char* file_name_cstr=ffblk.ff_name;
		size_t file_name_size=strlen(file_name_cstr);
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
			Table::element_type row(new ArrayString);
			*row+=new String(pa_strdup(file_name_cstr, file_name_size), file_name_size, true);
			table+=row;
		}
	);

	if(regexp_code)
		pcre_free(regexp_code);

	// write out result
	r.write_no_lang(*new VTable(&table));
}

#ifndef DOXYGEN
struct Lock_execute_body_info {
	Request* r;
	Value* body_code;
};
#endif
static void lock_execute_body(int , void *ainfo) {
	Lock_execute_body_info& info=*static_cast<Lock_execute_body_info *>(ainfo);
	// execute body
	info.r->write_assign_lang(info.r->process(*info.body_code));
};
static void _lock(Request& r, MethodParams& params) {
	const String& file_spec=r.absolute(params.as_string(0, "file name must be string"));
	Lock_execute_body_info info={
		&r, 
		&params.as_junction(1, "body must be code")
	};

	file_write_action_under_lock(file_spec, "lock", lock_execute_body, &info);
}

static int lastposafter(const String& s, size_t after, const char* substr, size_t substr_size, bool beforelast=false) {
	size_t size=0; // just to calm down compiler
	if(beforelast)
		size=s.length();
	size_t at;
	while((at=s.pos(String::Body(substr, substr_size), after))!=STRING_NOT_FOUND) {
		size_t newafter=at+substr_size/*skip substr*/;
		if(beforelast && newafter==size)
			break;
		after=newafter;
	}

	return after;
}

static void _find(Request& r, MethodParams& params) {
	const String& file_name=params.as_no_junction(0, "file name must not be code").as_string();
	const String* file_spec;
	if(file_name.first_char()=='/')
		file_spec=&file_name;
	else 
		file_spec=&r.relative(r.request_info.uri, file_name);

	// easy way
	if(file_readable(r.absolute(*file_spec))) {
		r.write_assign_lang(*file_spec);
		return;
	}

	// monkey way
	int after_base_slash=lastposafter(*file_spec, 0, "/", 1);
	const String* dirname=&file_spec->mid(0, after_base_slash);
	const String& basename=file_spec->mid(after_base_slash, file_spec->length());

	int after_monkey_slash;
	while((after_monkey_slash=lastposafter(*dirname, 0, "/", 1, true))>0) {
		String test_name;
		test_name<<*(dirname=&dirname->mid(0, after_monkey_slash));
		test_name<<basename;
		if(file_readable(r.absolute(test_name))) {
			r.write_assign_lang(test_name);
			return;
		}
	}

	// no way, not found
	if(params.count()==2) {
		Value& not_found_code=params.as_junction(1, "not-found param must be code");
		r.write_pass_lang(r.process(not_found_code));
	}
}

static void _dirname(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, "file name must be string");
    // /a/some.tar.gz > /a
	// /a/b/ > /a
	int afterslash=lastposafter(file_spec, 0, "/", 1, true);
	if(afterslash>0)
		r.write_assign_lang(file_spec.mid(0, afterslash==1?1:afterslash-1));
	else
		r.write_assign_lang(String(".", 1));
}

static void _basename(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, "file name must be string");
    // /a/some.tar.gz > some.tar.gz
	int afterslash=lastposafter(file_spec, 0, "/", 1);
	r.write_assign_lang(file_spec.mid(afterslash, file_spec.length()));
}

static void _justname(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, "file name must be string");
    // /a/some.tar.gz > some.tar
	int afterslash=lastposafter(file_spec, 0, "/", 1);
	int afterdot=lastposafter(file_spec, afterslash, ".", 1);
	r.write_assign_lang(file_spec.mid(afterslash, afterdot!=afterslash?afterdot-1:file_spec.length()));
}
static void _justext(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, "file name must be string");
    // /a/some.tar.gz > gz
	int afterdot=lastposafter(file_spec, 0, ".", 1);
	if(afterdot>0)
		r.write_assign_lang(file_spec.mid(afterdot, file_spec.length()));
}

static void _fullpath(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, "file name must be string");
	const String* result;
	if(file_spec.first_char()=='/')
		result=&file_spec;
	else {
		// /some/page.html: ^file:fullpath[a.gif] => /some/a.gif
		const String& full_disk_path=r.absolute(file_spec);
		size_t document_root_length=strlen(r.request_info.document_root);

		if(document_root_length>0) {
			char last_char=r.request_info.document_root[document_root_length-1];
			if(last_char == '/' || last_char == '\\')
				--document_root_length;
		}
		result=&full_disk_path.mid(document_root_length,  full_disk_path.length());
	}
	r.write_assign_lang(*result);
}


// constructor

MFile::MFile(): Methoded("file") {
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
