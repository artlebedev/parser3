/** @file
	Parser: @b file parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_FILE_C="$Date: 2011/11/23 12:17:22 $";

#include "pa_config_includes.h"

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
#include "pa_vregex.h"
#include "pa_version.h"

// defines

#define STDIN_EXEC_PARAM_NAME "stdin"
#define CHARSET_EXEC_PARAM_NAME "charset"

#define NAME_NAME "name"

// externs

extern String sql_limit_name;
extern String sql_offset_name;

// class

class MFile: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VFile(); }
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
	bool is_text=VFile::is_text_mode(params.as_string(0, MODE_MUST_NOT_BE_CODE));
	Value& vfile_name=params.as_no_junction(1, FILE_NAME_MUST_NOT_BE_CODE);

	Charset* asked_charset=0;
	if(params.count()>2)
		if(HashStringValue* options=params.as_hash(2)){
			int valid_options=0;
			if(Value* vcharset_name=options->get(PA_CHARSET_NAME)){
				asked_charset=&::charsets.get(vcharset_name->as_string().change_case(r.charsets.source(), String::CC_UPPER));
				valid_options++;
			}
			if(valid_options != options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	// save
	GET_SELF(r, VFile).save(r.charsets, r.absolute(vfile_name.as_string()), is_text, asked_charset);
}

static void _delete(Request& r, MethodParams& params) {
	const String& file_name=params.as_string(0, FILE_NAME_MUST_NOT_BE_CODE);

	// unlink
	file_delete(r.absolute(file_name));
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
			throw Exception("file.access", 
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
	bool as_text=VFile::is_text_mode(params.as_string(0, MODE_MUST_NOT_BE_CODE));
	const String& lfile_name=r.absolute(params.as_string(1, FILE_NAME_MUST_NOT_BE_CODE));

	size_t param_index=params.count()-1;
	Value* param_value=param_index>1?&params.as_no_junction(param_index, "file name or options must not be code"):0;

	HashStringValue* options=0;
	const String* user_file_name=0;

	if(param_value){
		options=param_value->get_hash();
		if(options || param_index>2)
			param_index--;
		if(param_index>1){
			const String& luser_file_name=params.as_string(param_index, FILE_NAME_MUST_BE_STRING);
			if(!luser_file_name.is_empty())
				user_file_name=&luser_file_name;
		}
	}
	if(!user_file_name)
		user_file_name=&lfile_name;

	size_t offset=0;
	size_t limit=0;

	if(options){
		options=new HashStringValue(*options);
		if(Value *voffset=(Value *)options->get(sql_offset_name)){
			offset=r.process_to_value(*voffset).as_int();
		}
		if(Value *vlimit=(Value *)options->get(sql_limit_name)){
			limit=r.process_to_value(*vlimit).as_int();
		}
		// no check on options count here, see file_read
	}
	File_read_result file=file_load(r, lfile_name,
		as_text, options, true, 0, offset, limit
	);

	Value* vcontent_type=0;
	if(file.headers){
		if(Value* remote_content_type=file.headers->get(HTTP_CONTENT_TYPE_UPPER))
			vcontent_type=new VString(*new String(remote_content_type->as_string().cstr()));
	} 
	
	VFile& self=GET_SELF(r, VFile);
	self.set(true/*tainted*/, file.str, file.length, user_file_name, vcontent_type, &r);

	self.set_mode(as_text);

	if(file.headers){
		file.headers->for_each<HashStringValue*>(_load_pass_param, &self.fields());
	} else {
		size_t size;
		time_t atime, mtime, ctime;

		file_stat(lfile_name, size, atime, mtime, ctime);
	
		HashStringValue& ff=self.fields();
		ff.put(adate_name, new VDate(atime));
		ff.put(mdate_name, new VDate(mtime));
		ff.put(cdate_name, new VDate(ctime));
	}
}

static void _create(Request& r, MethodParams& params) {
	const String* mode=0;
	const String* file_name=0;
	bool is_text=true;

	// new format: ^file::create[string-or-file-content[;$.mode[text|binary] $.name[...] $.content-type[...] $.charset[...] ]]
	size_t content_index=0;
	size_t options_index=1;
	bool extended_options=true;

	if(params.count()>=3){
		// old format: ^file::create[text|binary;file-name;string-or-file-content[;options]] 
		mode=&params.as_string(0, MODE_MUST_NOT_BE_CODE);
		is_text=VFile::is_text_mode(*mode);
		file_name=&params.as_string(1, FILE_NAME_MUST_NOT_BE_CODE);
		content_index=2;
		options_index=3;
		extended_options=false;
	}

	VString* vcontent_type=0;
	Charset* asked_charset=0;
	if(params.count()>options_index)
		if(HashStringValue* options=params.as_hash(options_index)) {
			int valid_options=0;
			if(extended_options) {
				if(Value* vmode=options->get(MODE_NAME)) {
					mode=&vmode->as_string();
					is_text=VFile::is_text_mode(*mode);
					valid_options++;
				}
				if(Value* vfile_name=options->get(NAME_NAME)) {
					file_name=&vfile_name->as_string();
					valid_options++;
				}
			}
			if(Value* vcharset_name=options->get(PA_CHARSET_NAME)) {
				asked_charset=&::charsets.get(vcharset_name->as_string().change_case(r.charsets.source(), String::CC_UPPER));
				valid_options++;
			}
			if(Value* value=options->get(CONTENT_TYPE_NAME)) {
				vcontent_type=new VString(value->as_string());
				valid_options++;
			}
			if(valid_options != options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	Value& vcontent=params.as_no_junction(content_index, "content must be string or file");

	VFile& self=GET_SELF(r, VFile);

	if(const String* content_str=vcontent.get_string()){
		String::Body body=content_str->cstr_to_string_body_untaint(String::L_AS_IS); // explode content, honor tainting changes
		if(asked_charset && is_text)
			body=Charset::transcode(body, r.charsets.source(), *asked_charset);
		self.set(true/*tainted*/, body.cstr(), body.length());
		self.set_mode(is_text);
	} else {
		if(asked_charset)
			throw Exception(PARSER_RUNTIME, 0, "charset option can not be used with file-content");
		self.set(*vcontent.as_vfile(String::L_AS_IS));
		if(mode)
			self.set_mode(is_text);
	}

	self.set_name(file_name);

	self.set_content_type(vcontent_type, file_name, &r);
}

static void _stat(Request& r, MethodParams& params) {
	const String& lfile_name=params.as_string(0, FILE_NAME_MUST_NOT_BE_CODE);

	size_t size;
	time_t atime, mtime, ctime;
	file_stat(r.absolute(lfile_name),
		size,
		atime, mtime, ctime);
	
	VFile& self=GET_SELF(r, VFile);

	self.set(true/*tainted*/, 0/*no bytes*/, size, &lfile_name, 0, &r);
	HashStringValue& ff=self.fields();
	ff.put(adate_name, new VDate(atime));
	ff.put(mdate_name, new VDate(mtime));
	ff.put(cdate_name, new VDate(ctime));
}

static bool is_safe_env_key(const char* key) {
	for(const char* validator=key; *validator; validator++) {
		char c=*validator;
		if(!(c>='A' && c<='Z' || c>='0' && c<='9' || c=='_' || c=='-'))
			return false;
	}
#ifdef PA_SAFE_MODE
	if(strncasecmp(key, "HTTP_", 5)==0)
		return true;
	if(strncasecmp(key, "CGI_", 4)==0)
		return true;
	for(int i=0; suexec_safe_env_lst[i]; i++) {
		if(strcasecmp(key, suexec_safe_env_lst[i])==0)
			return true;
	}
	return false;
#else
	return true;
#endif
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
		info->env->put(akey, avalue->as_string().cstr_to_string_body_untaint(String::L_AS_IS, 0, info->charsets));
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
		if(key==HTTP_CONTENT_TYPE_UPPER)
			info->content_type=value;
	}
}

static void append_to_argv(Request& r, ArrayString& argv, const String* str){
	if(!str->is_empty())
		argv+=new String(str->cstr_to_string_body_untaint(String::L_AS_IS, 0, &r.charsets), String::L_AS_IS);
}

/// @todo fix `` in perl - they produced flipping consoles and no output to perl
static void _exec_cgi(Request& r, MethodParams& params, bool cgi) {
	bool is_text=true;
	size_t param_index=0;
	const String& mode=params.as_string(0, FIRST_ARG_MUST_NOT_BE_CODE);
	if(VFile::is_valid_mode(mode)) {
		is_text=VFile::is_text_mode(mode);
		param_index++;
	}

	if(param_index>=params.count())
		throw Exception(PARSER_RUNTIME, 0, FILE_NAME_MUST_BE_SPECIFIED);

	const String& script_name=r.absolute(params.as_string(param_index++, FILE_NAME_MUST_NOT_BE_CODE));

	HashStringString env;
	#define ECSTR(name, value_cstr) \
		if(value_cstr) \
			env.put( \
				String::Body(#name), \
				String::Body(*value_cstr?value_cstr:0)); \
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
	ECSTR(PARSER_VARSION, PARSER_VERSION);
	// from Request.info
	ECSTR(DOCUMENT_ROOT, r.request_info.document_root);
	ECSTR(PATH_TRANSLATED, r.request_info.path_translated);
	ECSTR(REQUEST_METHOD, r.request_info.method);
	ECSTR(QUERY_STRING, r.request_info.query_string);
	ECSTR(REQUEST_URI, r.request_info.uri);
	ECSTR(CONTENT_TYPE, r.request_info.content_type);
	ECSTR(CONTENT_LENGTH, format(r.request_info.content_length, "%u"));
	// SCRIPT_*
	env.put(String::Body("SCRIPT_NAME"), script_name);
	//env.put(String::Body("SCRIPT_FILENAME"), ??&script_name);

	// environment & stdin from param
	String *in=new String();
	Charset *charset=0; // default script works raw_in 'source' charset = no transcoding needed
	if(param_index < params.count()) {
		Value& venv=params.as_no_junction(param_index++, "env must not be code");
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
				if(const String* sstdin=info.vstdin->get_string()) {
					// untaint stdin
					in = new String(sstdin->cstr_to_string_body_untaint(String::L_AS_IS), String::L_AS_IS);
				} else
					if(VFile* vfile=static_cast<VFile *>(info.vstdin->as("file")))
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
	if(param_index < params.count()) {
		// influence tainting 
		Temp_client_charset temp(r.charsets, charset? *charset: r.charsets.source());

		for(size_t i=param_index; i<params.count(); i++) {
			Value& param=params.as_no_junction(i, PARAM_MUST_NOT_BE_CODE);
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
							"param must be string or table");
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
	PA_exec_result execution=pa_exec(false/*forced_allow*/, script_name, &env, argv, *in);

	File_read_result *file_out=&execution.out;
	String *real_err=&execution.err;

	// transcode err if necessary (@todo: need fix line breaks in err as well )
	if(charset)
		real_err=&Charset::transcode(*real_err, *charset, r.charsets.source());

	if(file_out->length && is_text){
		fix_line_breaks(file_out->str, file_out->length);
		// treat output as string
		String *real_out = new String(file_out->str);

		// transcode out if necessary
		if(charset)
			real_out=&Charset::transcode(*real_out, *charset, r.charsets.source());

		// FIXME: unsafe cast
		file_out->str=const_cast<char *>(real_out->cstr()); // hacking a little
		file_out->length = real_out->length();
	}

	VFile& self=GET_SELF(r, VFile);

	if(cgi) { // ^file::cgi
		const char* eol_marker=0;
		size_t eol_marker_size;

		// construct with 'out' body and header
		size_t dos_pos=(file_out->length)?strpos(file_out->str, "\r\n\r\n"):STRING_NOT_FOUND;
		size_t unix_pos=(file_out->length)?strpos(file_out->str, "\n\n"):STRING_NOT_FOUND;

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
				throw Exception("file.execute",
					0,
					"output does not contain CGI header; "
					"exit status=%d; stdoutsize=%u; stdout: \"%s\"; stderrsize=%u; stderr: \"%s\"", 
						execution.status, 
						file_out->length, (file_out->length) ? (file_out->str) : "",
						real_err->length(), real_err->cstr());
				break; //never reached
		}

		size_t header_break_pos;
		if(unix_header_break) {
			header_break_pos=unix_pos;
			eol_marker="\n";
			eol_marker_size=1;
		} else {
			header_break_pos=dos_pos;
			eol_marker="\r\n";
			eol_marker_size=2;
		}

		file_out->str[header_break_pos] = 0;
		String *header=new String(file_out->str);
		unsigned long headersize = header_break_pos+eol_marker_size*2;
		file_out->str += headersize;
		file_out->length -= headersize;

		// $body
		self.set(false/*not tainted*/, file_out->str, file_out->length);

		// $fields << header
		if(header) {
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
	} else { // ^file::exec
		// $body
		self.set(false/*not tainted*/, file_out->str, file_out->length);
	}

	self.set_mode(is_text);

	// $status
	self.fields().put(file_status_name, new VInt(execution.status));
	
	// $stderr
	if(!real_err->is_empty())
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

	VRegex* vregex=0;
	VRegexCleaner vrcleaner;
	if(params.count()>1){
		Value& regexp=params.as_no_junction(1, "regexp must not be code");
		if(regexp.is_defined()){
			if(Value* value=regexp.as(VREGEX_TYPE)){
				vregex=static_cast<VRegex*>(value);
			} else {
				vregex=new VRegex(r.charsets.source(), &regexp.as_string(), 0/*options*/);
				vregex->study();
				vrcleaner.vregex=vregex;
			}
		}
	}

	const char* absolute_path_cstr=r.absolute(relative_path.as_string()).taint_cstr(String::L_FILE_SPEC);

	Table::columns_type columns(new ArrayString);
	*columns+=new String("name");
	Table& table=*new Table(columns);

	const int ovector_size=(1/*match*/)*3;
	int ovector[ovector_size];

	LOAD_DIR(absolute_path_cstr, 
		const char* file_name_cstr=ffblk.ff_name;
		size_t file_name_size=strlen(file_name_cstr);

		if(!vregex || vregex->exec(ffblk.ff_name, file_name_size, ovector, ovector_size)>=0) {
			Table::element_type row(new ArrayString);
			*row+=new String(pa_strdup(file_name_cstr, file_name_size), String::L_TAINTED);
			table+=row;
		}
	);

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

static void _find(Request& r, MethodParams& params) {
	const String& file_name=params.as_string(0, FILE_NAME_MUST_NOT_BE_CODE);
	Value* not_found_code=(params.count()==2)?&params.as_junction(1, "not-found param must be code"):0;

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
	if(not_found_code)
		r.write_pass_lang(r.process(*not_found_code));
}

static void _dirname(Request& r, MethodParams& params) {
	const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
	// /a/some.tar.gz > /a
	// /a/b/ > /a
	int afterslash=lastposafter(file_spec, 0, "/", 1, true);
	if(afterslash>0)
		r.write_assign_lang(file_spec.mid(0, afterslash==1?1:afterslash-1));
	else
		r.write_assign_lang(String("."));
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
						user_file_name=new String(str, String::L_TAINTED);
					break;
				case 2:
					if(!user_content_type) // user not specified?
						user_content_type=new String(str, String::L_TAINTED);
					break;
				default:
					error=SQL_Error(PARSER_RUNTIME, "result must not contain more then one row, three columns");
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
	const char* statement_cstr=statement_string.untaint_cstr(r.flang, r.connection());

	File_sql_event_handlers handlers(statement_string, statement_cstr);

	ulong limit=SQL_NO_LIMIT;
	ulong offset=0;

	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1)){
			int valid_options=0;
			if(Value* vfilename=options->get(NAME_NAME)) {
				valid_options++;
				handlers.user_file_name=&vfilename->as_string();
			}
			if(Value* vcontent_type=options->get(CONTENT_TYPE_NAME)) {
				valid_options++;
				handlers.user_content_type=&vcontent_type->as_string();
			}
			if(Value* vlimit=options->get(sql_limit_name)) {
				valid_options++;
				limit=(ulong)r.process_to_value(*vlimit).as_double();
			}
			if(Value* voffset=options->get(sql_offset_name)) {
				valid_options++;
				offset=(ulong)r.process_to_value(*voffset).as_double();
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}


	r.connection()->query(
		statement_cstr, 
		0, 0,
		offset, limit,
		handlers,
		statement_string);

	if(!handlers.value)
		throw Exception(PARSER_RUNTIME,
			0,
			"produced no result");

	VFile& self=GET_SELF(r, VFile);

	self.set(true/*tainted*/, handlers.value.str, handlers.value.length, handlers.user_file_name
				, handlers.user_content_type ? new VString(*handlers.user_content_type) : 0
				, &r);
	self.set_mode(false/*binary*/);
}

static void _base64(Request& r, MethodParams& params) {
	bool dynamic=!(&r.get_self() == file_class);
	if(dynamic) {
		VFile& self=GET_SELF(r, VFile);
		if(params.count()) {
			// decode: 
			//	^file::base64[encoded] // backward
			//	^file::base64[mode;user-file-name;encoded[;$.content-type[...]]]
			bool is_text=false;
			VString* vcontent_type=0;
			const String* user_file_name=0;
			size_t param_index=0;

			if(params.count() > 1) {
				if(params.count() < 3)
					throw Exception(PARSER_RUNTIME,
						0,
						"constructor can not have less then 3 parameters (has %d parameters)",
						params.count()); // actually it accepts 1 parameter (backward)

				is_text=VFile::is_text_mode(params.as_string(0, MODE_MUST_NOT_BE_CODE));
				user_file_name=&params.as_string(1, FILE_NAME_MUST_BE_STRING);

				if(params.count() == 4)
					if(HashStringValue* options=params.as_hash(3)) {
						int valid_options=0;
						if(Value* value=options->get(CONTENT_TYPE_NAME)) {
							vcontent_type=new VString(value->as_string());
							valid_options++;
						}
						if(valid_options!=options->count())
							throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
					}

				param_index=2;
			}

			const char* encoded=params.as_string(param_index, PARAMETER_MUST_BE_STRING).cstr();

			char* decoded=0;
			size_t length=0;
			pa_base64_decode(encoded, strlen(encoded), decoded, length);

			if(length && is_text)
				fix_line_breaks(decoded, length);
		
			self.set(true/*tainted*/, decoded, length, user_file_name, vcontent_type, &r);

			if(params.count() > 1)
				self.set_mode(is_text);
		} else {
			// encode: ^f.base64[]
			const char* encoded=pa_base64_encode(self.value_ptr(), self.value_size());
			r.write_assign_lang(*new String(encoded, String::L_TAINTED/*once ?param=base64(something) was needed**/));
		}
	} else {
		// encode: ^file:base64[filespec]
		const String& file_spec=params.as_string(0, FILE_NAME_MUST_BE_STRING);
		const char* encoded=pa_base64_encode(r.absolute(file_spec));
		r.write_assign_lang(*new String(encoded, String::L_TAINTED/*once ?param=base64(something) was needed*/));
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
			throw Exception(PARSER_RUNTIME, 0, FILE_NAME_MUST_BE_SPECIFIED);
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
			throw Exception(PARSER_RUNTIME, 0, FILE_NAME_MUST_BE_SPECIFIED);
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
	// ^file::create[text|binary;file-name;string-or-file[;options hash]]
	// ^file::create[string-or-file[;options hash]]
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 4);

	// ^file.save[mode;file-name]
	// ^file.save[mode;file-name;$.charset[...]]
	add_native_method("save", Method::CT_DYNAMIC, _save, 2, 3);

	// ^file:delete[file-name]
	add_native_method("delete", Method::CT_STATIC, _delete, 1, 1);

	// ^file:move[from-file-name;to-file-name]
	add_native_method("move", Method::CT_STATIC, _move, 2, 2);

	// ^file::load[mode;disk-name]
	// ^file::load[mode;disk-name;user-name]
	// ^file::load[mode;disk-name;user-name;options hash]
	// ^file::load[mode;disk-name;options hash]
	add_native_method("load", Method::CT_DYNAMIC, _load, 2, 4);

	// ^file::stat[disk-name]
	add_native_method("stat", Method::CT_DYNAMIC, _stat, 1, 1);

	// ^file::cgi[mode;file-name]
	// ^file::cgi[mode;file-name;env hash]
	// ^file::cgi[mode;file-name;env hash;1cmd;2line;3ar;4g;5s]
	add_native_method("cgi", Method::CT_DYNAMIC, _cgi, 1, 3+50);

	// ^file::exec[mode;file-name]
	// ^file::exec[mode;file-name;env hash]
	// ^file::exec[mode;file-name;env hash;1cmd;2line;3ar;4g;5s]
	add_native_method("exec", Method::CT_DYNAMIC, _exec, 1, 3+50);

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

	// ^file::sql{}
	// ^file::sql{}[options hash]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// encode:
	//   ^file.base64[]
	//   ^file:base64[file-name]
	// decode:
	//   ^file::base64[encoded] // backward
	//   ^file::base64[mode;user-file-name;encoded]
	//   ^file::base64[mode;user-file-name;encoded;$.content-type[...]]
	add_native_method("base64", Method::CT_ANY, _base64, 0, 4);

	// ^file.crc32[]
	// ^file:crc32[file-name]
	add_native_method("crc32", Method::CT_ANY, _crc32, 0, 1);

	// ^file.md5[]
	// ^file:md5[file-name]
	add_native_method("md5", Method::CT_ANY, _md5, 0, 1);

	// ^file:copy[from-file-name;to-file-name]
	add_native_method("copy", Method::CT_STATIC, _copy, 2, 2);
}
