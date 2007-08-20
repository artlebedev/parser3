/** @file
	Parser: @b file parser class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_FILE_C="$Date: 2007/08/20 10:02:51 $";

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
#include "pa_sql_connection.h"
#include "pa_md5.h"

// defines

#define TEXT_MODE_NAME "text"
#define BINARY_MODE_NAME "binary"
#define STDIN_EXEC_PARAM_NAME "stdin"
#define CHARSET_EXEC_PARAM_NAME "charset"

#define NAME_NAME "name"

// externs

extern String sql_limit_name;
extern String sql_offset_name;

// class

class MFile: public Methoded {
public: // VStateless_class
	
	Value* create_new_value(Pool&, HashStringValue&) { return new VFile(); }

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

static bool is_text_mode(const String& mode) {
	if(mode==TEXT_MODE_NAME)
		return true;
	if(mode==BINARY_MODE_NAME)
		return false;
	throw Exception(PARSER_RUNTIME,
		&mode,
		"is invalid mode, must be either '"TEXT_MODE_NAME"' or '"BINARY_MODE_NAME"'");
}

static void _save(Request& r, MethodParams& params) {
	Value& vmode_name=params. as_no_junction(0, "mode must not be code");
	Value& vfile_name=params.as_no_junction(1, FILE_NAME_MUST_NOT_BE_CODE);

	// save
	GET_SELF(r, VFile).save(r.absolute(vfile_name.as_string()),
		is_text_mode(vmode_name.as_string()));
}

static void _delete(Request& r, MethodParams& params) {
	Value& vfile_name=params.as_no_junction(0, FILE_NAME_MUST_NOT_BE_CODE);

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

static void copy_process_source(
			     struct stat& , 
			     int from_file, 
			     const String& , const char* /*fname*/, bool, 
				 void *context) {
	int& to_file=*static_cast<int *>(context);

	int nCount=0;
	do {
		unsigned char buffer[FILE_BUFFER_SIZE];
		nCount = file_block_read(from_file, buffer, sizeof(buffer));
		int written=write(to_file, buffer, nCount); 
		if( written < 0 )
			throw Exception(0, 
				0, 
				"write failed: %s (%d)",  strerror(errno), errno); 
		
	} while(nCount > 0);
}

static void copy_open_target(int f, void *from_spec) {
	String& file_spec=*static_cast<String *>(from_spec);
	file_read_action_under_lock(file_spec, "copy", copy_process_source, &f);
};

static void _copy(Request& r, MethodParams& params) {
	Value& vfrom_file_name=params.as_no_junction(0, "from file name must not be code");
	Value& vto_file_name=params.as_no_junction(1, "to file name must not be code");

	String from_spec = r.absolute(vfrom_file_name.as_string());
	const String& to_spec = r.absolute(vto_file_name.as_string());
	
	file_write_action_under_lock(
			to_spec,
			"copy",
			copy_open_target,
			&from_spec);
}

static void _load_pass_param(
			     HashStringValue::key_type key, 
			     HashStringValue::value_type value, 
			     HashStringValue *dest) {
	dest->put(key, value);
}
static void _load(Request& r, MethodParams& params) {
	Value& vmode_name=params. as_no_junction(0, "mode must not be code");
	const String& lfile_name=r.absolute(params.as_no_junction(1, FILE_NAME_MUST_NOT_BE_CODE).as_string());
	Value* third_param=params.count()>2?&params.as_no_junction(2, "filename or options must not be code")
		:0;
	HashStringValue* third_param_hash=third_param?third_param->get_hash():0;
	size_t alt_filename_param_index=2;
	if(third_param_hash)
		alt_filename_param_index++;

	HashStringValue* options=third_param_hash;
	size_t offset=0;
	size_t limit=0;
	if(options) {
		options=new HashStringValue(*options);
		if(Value *voffset=(Value *)options->get(sql_offset_name)) {
			offset=r.process_to_value(*voffset).as_int();
		}
		if(Value *vlimit=(Value *)options->get(sql_limit_name)) {
			limit=r.process_to_value(*vlimit).as_int();
		}
		// no check on options count here, see file_read
	}
	File_read_result file=file_read(r.charsets, lfile_name,
		is_text_mode(vmode_name.as_string()),
		options, true, 0, offset, limit
	);

	const char *user_file_name=params.count()>alt_filename_param_index?
		params.as_string(alt_filename_param_index, FILE_NAME_MUST_BE_STRING).cstr()
		:lfile_name.cstr(String::L_FILE_SPEC);

	Value* vcontent_type=0;
	if(file.headers)
	{
		if(Value* remote_content_type=file.headers->get("CONTENT-TYPE"))
			vcontent_type=new VString(*new String(remote_content_type->as_string().cstr()));
	} 
	if(!vcontent_type)
		vcontent_type=new VString(r.mime_type_of(user_file_name));
	
	VFile& self=GET_SELF(r, VFile);
	self.set(true/*tainted*/, file.str, file.length, user_file_name, vcontent_type);
	if(file.headers)
		file.headers->for_each<HashStringValue*>(_load_pass_param, &self.fields());
}

static void _create(Request& r, MethodParams& params) {
	Value& vmode_name=params. as_no_junction(0, "mode must not be code");
	if(!is_text_mode(vmode_name.as_string()))
		throw Exception(PARSER_RUNTIME,
			0,
			"only text mode is currently supported");

	const char* user_file_name_cstr=r.absolute(
		params.as_no_junction(1, FILE_NAME_MUST_NOT_BE_CODE).as_string()).cstr(String::L_FILE_SPEC);

	const String& content=params.as_string(2, "content must be string");
	const char* content_cstr=content.cstr(String::L_UNSPECIFIED); // explode content, honor tainting changes

	VString* vcontent_type=new VString(r.mime_type_of(user_file_name_cstr));
	
	VFile& self=GET_SELF(r, VFile);
	self.set(true/*tainted*/, content_cstr, strlen(content_cstr), user_file_name_cstr, vcontent_type);
}

static void _stat(Request& r, MethodParams& params) {
	Value& vfile_name=params.as_no_junction(0, FILE_NAME_MUST_NOT_BE_CODE);

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
	Request_charsets* charsets;
	HashStringString* env;
	Value* vstdin;
};
#endif
static void append_env_pair(
			    HashStringValue::key_type akey, 
			    HashStringValue::value_type avalue, 
			    Append_env_pair_info *info) {
	if(akey==STDIN_EXEC_PARAM_NAME) {
		info->vstdin=avalue;
	} else if(akey==CHARSET_EXEC_PARAM_NAME) {
		// ignore, already processed
	} else {
		if(!is_safe_env_key(akey.cstr()))
			throw Exception(PARSER_RUNTIME,
				new String(akey, String::L_TAINTED),
				"not safe environment variable");
		info->env->put(akey, avalue->as_string().cstr_to_string_body(String::L_UNSPECIFIED, 0, info->charsets));
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
	if(colon_pos!=STRING_NOT_FOUND) {
		const String& key=astring->mid(0, colon_pos).change_case(
			*info->charset, String::CC_UPPER);
		Value* value=new VString(astring->mid(colon_pos+1, astring->length()).trim());
		info->fields->put(key, value);
		if(key=="CONTENT-TYPE")
			info->content_type=value;
	}
}

static void append_to_argv(Request& r, ArrayString& argv, const String* str){
	if( str->length() ){
		argv+=new String(str->cstr_to_string_body(String::L_UNSPECIFIED, 0, &r.charsets), String::L_AS_IS);
	}
}

/// @todo fix `` in perl - they produced flipping consoles and no output to perl
static void _exec_cgi(Request& r, MethodParams& params,
					  bool cgi) {

	Value& vfile_name=params.as_no_junction(0, FILE_NAME_MUST_NOT_BE_CODE);

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
			// $.charset  [previewing to handle URI pieces]
			if(Value* vcharset=user_env->get(CHARSET_EXEC_PARAM_NAME))
				charset=&charsets.get(vcharset->as_string()
					.change_case(r.charsets.source(), String::CC_UPPER));

			// $.others
			Append_env_pair_info info={&r.charsets, &env, 0};
			{
				// influence tainting
				// main target -- $.QUERY_STRING -- URLencoding of tainted pieces to String::L_URI lang
				Temp_client_charset temp(r.charsets, charset? *charset: r.charsets.source());
				user_env->for_each<Append_env_pair_info*>(append_env_pair, &info);
			}
			// $.stdin
			if(info.vstdin) {
				stdin_specified=true;
				if(const String* sstdin=info.vstdin->get_string()) {
					in->append(*sstdin, String::L_CLEAN, true);
				} else
					if(VFile* vfile=static_cast<VFile *>(info.vstdin->as("file", false)))
						in->append_know_length((const char* )vfile->value_ptr(), vfile->value_size(), String::L_TAINTED);
					else
						throw Exception(PARSER_RUNTIME,
							0,
							STDIN_EXEC_PARAM_NAME " parameter must be string or file");
			}
		}
	}

	// argv from params
	ArrayString argv;
	if(params.count()>2) {
   		// influence tainting 
   		// main target -- URLencoding of tainted pieces to String::L_URI lang
   		Temp_client_charset temp(r.charsets, charset? *charset: r.charsets.source());

		for(size_t i=2; i<params.count(); i++) {
			Value& param=params.as_no_junction(i, "parameter must not be code");
			if(param.is_defined()){
				if(param.is_string()){
					append_to_argv(r, argv, param.get_string());
				} else {
					Table* table=param.get_table();
					if(table){
						for(size_t i=0; i<table->count(); i++) {
							append_to_argv(r, argv, table->get(i)->get(0));
						}
					} else {
						throw Exception(PARSER_RUNTIME,
							0,
							"parameter must be string or table");
					}
				}
			}
		}
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
	const String& file_spec=r.absolute(params.as_string(0, FILE_NAME_MUST_BE_STRING));
	Lock_execute_body_info info={
		&r, 
		&params.as_junction(1, "body must be code")
	};

	file_write_action_under_lock(
			file_spec,
			"lock",
			lock_execute_body,
			&info);
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
	const String& file_name=params.as_no_junction(0, FILE_NAME_MUST_NOT_BE_CODE).as_string();
	const String* file_spec;
	if(file_name.first_char()=='/')
		file_spec=&file_name;
	else 
		file_spec=&r.relative(r.request_info.uri, file_name);

	// easy way
	if(file_exist(r.absolute(*file_spec))) {
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
		if(file_exist(r.absolute(test_name))) {
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
	const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
    // /a/some.tar.gz > /a
	// /a/b/ > /a
	int afterslash=lastposafter(file_spec, 0, "/", 1, true);
	if(afterslash>0)
		r.write_assign_lang(file_spec.mid(0, afterslash==1?1:afterslash-1));
	else
		r.write_assign_lang(String(".", 1));
}

static void _basename(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
    // /a/some.tar.gz > some.tar.gz
	int afterslash=lastposafter(file_spec, 0, "/", 1);
	r.write_assign_lang(file_spec.mid(afterslash, file_spec.length()));
}

static void _justname(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
    // /a/some.tar.gz > some.tar
	int afterslash=lastposafter(file_spec, 0, "/", 1);
	int afterdot=lastposafter(file_spec, afterslash, ".", 1);
	r.write_assign_lang(file_spec.mid(afterslash, afterdot!=afterslash?afterdot-1:file_spec.length()));
}
static void _justext(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
    // /a/some.tar.gz > gz
	int afterdot=lastposafter(file_spec, 0, ".", 1);
	if(afterdot>0)
		r.write_assign_lang(file_spec.mid(afterdot, file_spec.length()));
}

static void _fullpath(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
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

static void _sql_string(Request& r, MethodParams&) {
	VFile& self=GET_SELF(r, VFile);

	const char *quoted=r.connection()->quote(self.value_ptr(), self.value_size());
	r.write_assign_lang(*new String(quoted));
}

#ifndef DOXYGEN
class File_sql_event_handlers: public SQL_Driver_query_event_handlers {
	const String& statement_string; const char* statement_cstr;
	int got_columns;
	int got_cells;
public:
	String::C value;
	const String* user_file_name;
	const String* user_content_type;
public:
	File_sql_event_handlers(
		const String& astatement_string, const char* astatement_cstr):
		statement_string(astatement_string), statement_cstr(astatement_cstr),
		got_columns(0),
		got_cells(0),
		user_file_name(0),
		user_content_type(0) {}

	bool add_column(SQL_Error& error, const char* /*str*/, size_t /*length*/) {
		if(got_columns++==3) {
			error=SQL_Error(PARSER_RUNTIME, "result must contain not more then 3 columns");
			return true;
		}
		return false;
	}
	bool before_rows(SQL_Error& /*error*/ ) { /* ignore */ return false; }
	bool add_row(SQL_Error& /*error*/) { /* ignore */ return false; }
	bool add_row_cell(SQL_Error& error, const char* str, size_t length) {
		try {
			switch(got_cells++) {
				case 0:
					value=String::C(str, length); 
					break;
				case 1:
					if(!user_file_name) // user not specified?
						user_file_name=new String(str, length, true);
					break;
				case 2:
					if(!user_content_type) // user not specified?
						user_content_type=new String(str, length, true);
					break;
				default:
					error=SQL_Error(PARSER_RUNTIME, "result must not contain more then one row, three rows");
					return true;
			}
			return false;
		} catch(...) {
			error=SQL_Error("exception occured in File_sql_event_handlers::add_row_cell");
			return true;
		}
	}
};
#endif
static void _sql(Request& r, MethodParams& params) {
	Value& statement=params.as_junction(0, "statement must be code");

	Temp_lang temp_lang(r, String::L_SQL);
	const String& statement_string=r.process_to_string(statement);
	const char* statement_cstr=
		statement_string.cstr(String::L_UNSPECIFIED, r.connection());
	File_sql_event_handlers handlers(statement_string, statement_cstr);

	if(params.count()>1)
		if(HashStringValue* options=
			params.as_no_junction(1, "param must not be code").get_hash()) {
			int valid_options=0;
			if(Value* vfilename=options->get(NAME_NAME)) {
				valid_options++;
				handlers.user_file_name=&vfilename->as_string();
			}
			if(Value* vcontent_type=options->get(CONTENT_TYPE_NAME)) {
				valid_options++;
				handlers.user_content_type=&vcontent_type->as_string();
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME,
					0,
					"called with invalid option");
		}


	r.connection()->query(
		statement_cstr, 
		0, 0,
		0, 0, 
		handlers,
		statement_string);

	if(!handlers.value)
		throw Exception(PARSER_RUNTIME,
			0,
			"produced no result");

	const char* user_file_name_cstr=handlers.user_file_name? handlers.user_file_name->cstr(): 0;

	VString* vcontent_type=handlers.user_content_type? 
		new VString(*handlers.user_content_type)
		: user_file_name_cstr?
			new VString(r.mime_type_of(user_file_name_cstr))
			: 0;
	VFile& self=GET_SELF(r, VFile);
	self.set(true/*tainted*/, handlers.value.str, handlers.value.length, user_file_name_cstr, vcontent_type);
}

static void _base64(Request& r, MethodParams& params) {
	bool dynamic = !(&r.get_self() == file_class);
	if ( dynamic ){
	VFile& self=GET_SELF(r, VFile);
	if(params.count()) {
		// decode
		const char* cstr=params.as_string(0, "parameter must be string").cstr();
		char* decoded_cstr=0;
		size_t decoded_size=0;
		pa_base64_decode(cstr, strlen(cstr), decoded_cstr, decoded_size);
		if(decoded_cstr && decoded_size)
			self.set(true/*tainted*/, decoded_cstr, decoded_size);
	} else {
		// encode 
		const char* encoded=pa_base64_encode(self.value_ptr(), self.value_size());
		r.write_assign_lang(*new String(encoded, 0, true/*once ?param=base64(something) was needed*/));
	}
	} else {
		// encode
		const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
		const char* encoded=pa_base64_encode(r.absolute(file_spec));
		r.write_assign_lang(*new String(encoded, 0, true/*once ?param=base64(something) was needed*/));
	}
}

static void _crc32(Request& r, MethodParams& params) {
	unsigned long crc32 = 0;
	if(&r.get_self() == file_class) {
		// ^file:crc32[file-name]
		if(params.count()) {
			const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
			crc32=pa_crc32(r.absolute(file_spec));
		} else {
			throw Exception(PARSER_RUNTIME,
				0,
				"file name must be defined");
		}
	} else {
		// ^file.crc32[]
		VFile& self=GET_SELF(r, VFile);
		crc32=pa_crc32(self.value_ptr(), self.value_size());
	}
	r.write_no_lang(*new VInt(crc32));
}


static void file_md5_file_action(
			     struct stat& finfo, 
			     int f, 
			     const String& , const char* /*fname*/, bool, 
			     void *context)
{
	PA_MD5_CTX& md5context=*static_cast<PA_MD5_CTX *>(context);
	if(finfo.st_size) {
		int nCount=0;
		do {
			unsigned char buffer[FILE_BUFFER_SIZE];
			nCount = file_block_read(f, buffer, sizeof(buffer));
			if ( nCount ){
				pa_MD5Update(&md5context, (const unsigned char*)buffer, nCount);
			}
		} while(nCount > 0);
	}
}

const char* pa_md5(const String& file_spec)
{
	PA_MD5_CTX context;
	unsigned char digest[16];
	pa_MD5Init(&context);
	file_read_action_under_lock(file_spec, "md5", file_md5_file_action, &context);
	pa_MD5Final(digest, &context);
	
	return hex_string(digest, sizeof(digest), false);
}

const char* pa_md5(const char *in, size_t in_size)
{
	PA_MD5_CTX context;
	unsigned char digest[16];
	pa_MD5Init(&context);
	pa_MD5Update(&context, (const unsigned char*)in, in_size);
	pa_MD5Final(digest, &context);
	
	return hex_string(digest, sizeof(digest), false);
}

static void _md5(Request& r, MethodParams& params) {
	const char* md5;
	if(&r.get_self() == file_class) {
		// ^file:md5[file-name]
		if(params.count()) {
			const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
			md5=pa_md5(r.absolute(file_spec));
		} else {
			throw Exception(PARSER_RUNTIME,
				0,
				"file name must be defined");
		}
	} else {
		// ^file.md5[]
		VFile& self=GET_SELF(r, VFile);
		md5=pa_md5(self.value_ptr(), self.value_size());

	}
	r.write_no_lang(*new String(md5));
}

// constructor

MFile::MFile(): Methoded("file") {
	// ^file::create[text;user-name;string]
	// ^file::create[binary;user-name;SOMEDAY SOMETHING]
	add_native_method("create", Method::CT_DYNAMIC, _create, 3, 3);

	// ^file.save[mode;file-name]
	add_native_method("save", Method::CT_DYNAMIC, _save, 2, 2);

	// ^file:delete[file-name]
	add_native_method("delete", Method::CT_STATIC, _delete, 1, 1);

	// ^file:move[from-file-name;to-file-name]
	add_native_method("move", Method::CT_STATIC, _move, 2, 2);

	// ^file::load[mode;disk-name]
	// ^file::load[mode;disk-name;user-name]
	add_native_method("load", Method::CT_DYNAMIC, _load, 2, 3);

	// ^file::stat[disk-name]
	add_native_method("stat", Method::CT_DYNAMIC, _stat, 1, 1);

	// ^file::cgi[file-name]
	// ^file::cgi[file-name;env hash]
	// ^file::cgi[file-name;env hash;1cmd;2line;3ar;4g;5s]
	add_native_method("cgi", Method::CT_DYNAMIC, _cgi, 1, 2+50);

	// ^file::exec[file-name]
	// ^file::exec[file-name;env hash]
	// ^file::exec[file-name;env hash;1cmd;2line;3ar;4g;5s]
	add_native_method("exec", Method::CT_DYNAMIC, _exec, 1, 2+50);

	// ^file:list[path]
	// ^file:list[path][regexp]
	add_native_method("list", Method::CT_STATIC, _list, 1, 2);

	// ^file:lock[path]{code}
	add_native_method("lock", Method::CT_STATIC, _lock, 2, 2);

	// ^file:find[file-name]
	// ^file:find[file-name]{when-not-found}
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

    // ^file.sql-string[]
	add_native_method("sql-string", Method::CT_DYNAMIC, _sql_string, 0, 0);

    // ^file::sql[[alt_name]]{}
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// ^file::base64[string] << decode
	// ^file.base64[] << encode
	// ^file:base64[file-name] << encode
	add_native_method("base64", Method::CT_ANY, _base64, 0, 1);

	// ^file.crc32[]
	// ^file:crc32[file-name]
	add_native_method("crc32", Method::CT_ANY, _crc32, 0, 1);

	// ^file.md5[]
	// ^file:md5[file-name]
	add_native_method("md5", Method::CT_ANY, _md5, 0, 1);

	// ^file:copy[from-file-name;to-file-name]
	add_native_method("copy", Method::CT_STATIC, _copy, 2, 2);
}
