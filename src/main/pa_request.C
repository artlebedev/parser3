/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.120 2001/04/23 10:44:31 paf Exp $
*/

#include "pa_config_includes.h"

#include "pcre.h"
#include "internal.h"

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_vclass.h"
#include "_op.h"
#include "_table.h"
#include "_file.h"
#include "pa_globals.h"
#include "pa_vint.h"
#include "pa_vmethod_frame.h"
#include "pa_types.h"
#include "pa_vtable.h"
#include "_random.h"
#include "pa_vfile.h"
#include "_mail.h"
#include "_exec.h"
#include "_image.h"

/// $LIMITS.post_max_size default 10M
const size_t MAX_POST_SIZE_DEFAULT=10*0x400*400;

/// content type of exception response, when no @MAIN:exception handler defined
const char *UNHANDLED_EXCEPTION_CONTENT_TYPE="text/plain";

/// content type of response when no $MAIN:defaults.content-type defined
const char *DEFAULT_CONTENT_TYPE="text/html";

//
Request::Request(Pool& apool,
				 Info& ainfo,
				 String::Untaint_lang adefault_lang) : Pooled(apool),
	stack(apool),
	OP(apool),
	env(apool),
	form(apool),
	request(apool, *this),
	response(apool),
	cookie(apool),
	fclasses(apool),
	fdefault_lang(adefault_lang), flang(adefault_lang),
	info(ainfo),
	post_data(0), post_size(0),
	used_files(apool),
	default_content_type(0),
	mime_types(0),
	connection(0), protocol2library(0),
	mail(0), 
	pcre_tables(0)
{
	// root superclass, 
	//   parent of all classes, 
	//   operators holder
	initialize_op_class(pool(), OP);
	classes().put(*op_class_name, &OP);
	// table class
	classes().put(*table_class_name, table_class);	
	// file class
	classes().put(*file_class_name, file_class);	
	// random class
	classes().put(*random_class_name, random_class);	
	// env class
	classes().put(*env_class_name, &env);
	// form class
	classes().put(*form_class_name, &form);	
	// request class
	classes().put(*request_class_name, &request);	
	// response class
	classes().put(*response_class_name, &response);	
	// cookie class
	classes().put(*cookie_class_name, &cookie);
	// mail class
	classes().put(*mail_class_name, mail_class);
	// image class
	classes().put(*image_class_name, image_class);
}

static void element2ctypes(unsigned char *tables, 
	Value& ctype, const String& name, 
	unsigned char bit,
	int group_offset=-1,
	bool skip_ws=true) {
	Value *value=ctype.get_element(name);
	if(!value)
		return;

	unsigned char *ctypes_table=tables+ctypes_offset;
	const unsigned char *cstr=
		(const unsigned char *)value->as_string().cstr(String::UL_AS_IS);
	for(; *cstr; cstr++) {
		unsigned char c=*cstr;
		if(skip_ws && (c=='\n' || c=='\t' || c==' '))
			continue;
		ctypes_table[c]|=bit;

		if(group_offset>=0)
			tables[cbits_offset+group_offset+c/8] |= 1 << (c%8);
	}				
}
static void cstr2ctypes(unsigned char *tables, const unsigned char *cstr, 
						unsigned char bit) {
	unsigned char *ctypes_table=tables+ctypes_offset;
	ctypes_table[0]=bit;
	for(; *cstr; cstr++) {
		unsigned char c=*cstr;
		ctypes_table[c]|=bit;
	}
}
static void prepare_case_tables(unsigned char *tables) {
	unsigned char *lcc_table=tables+lcc_offset;
	unsigned char *fcc_table=tables+fcc_offset;
	for(int i=0; i<0x100; i++)
		lcc_table[i]=fcc_table[i]=i;
}
static void element2case(unsigned char *tables, Value& ctype, const String& name) {
	Value *value=ctype.get_element(name);
	if(!value)
		return;

	unsigned char *lcc_table=tables+lcc_offset;
	unsigned char *fcc_table=tables+fcc_offset;
	const unsigned char *cstr=
		(const unsigned char *)value->as_string().cstr(String::UL_AS_IS);
	unsigned char from=0;
	for(; *cstr; cstr++) {
		unsigned char c=*cstr;
		if(c=='\n' || c=='\t' || c==' ')
			continue;
		if(from) {
			lcc_table[from]=c;
			fcc_table[from]=c; fcc_table[c]=from;
			from=0;
		} else
			from=c;
	}
}
/**
	load MAIN class, execute @main.
	MAIN class consists of all the auto.p files we'd manage to find
	plus
	the file user requested us to process
	all located classes become children of one another,
	composing class we name 'MAIN'
*/
void Request::core(const char *root_auto_path, bool root_auto_fail,
				   const char *site_auto_path, bool site_auto_fail,
				   bool header_only) {
	VStateless_class *main_class=0;
	bool need_rethrow=false;  Exception rethrow_me;
	TRY {
		char *auto_filespec=(char *)malloc(MAX_STRING);
		
		// loading root auto.p 
		if(root_auto_path) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(root_auto_path, 0, "root_auto", 0);
			filespec.APPEND_CONST("/" AUTO_FILE_NAME);
			main_class=use_file(
				filespec, root_auto_fail,
				main_class_name, main_class);
		}

		// $MAIN:LIMITS hash used here,
		//	until someone with less privileges have overriden them
		{
			Value *limits=main_class?main_class->get_element(*limits_name):0;
			if(info.method && StrEqNc(info.method, "post", true)) {
				// $limits.post_max_size default 10M
				Value *element=limits?limits->get_element(*post_max_size_name):0;
				size_t value=element?(size_t)element->as_double():0;
				size_t post_max_size=value?value:MAX_POST_SIZE_DEFAULT;
				
				if(size_t limit=max(0, min(info.content_length, post_max_size))) {
					// read POST data
					post_data=(char *)malloc(limit);
					post_size=SAPI::read_post(pool(), post_data, limit);
					if(post_size!=limit)
						THROW(0, 0, 
							0, 
							"post_size(%d)!=limit(%d)", 
								post_size, limit);
				}
			}

			form.fill_fields(*this);
		}

		// filling cookies
		cookie.fill_fields(*this);

		// loading site auto.p
		if(site_auto_path) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(site_auto_path, 0, "site_auto", 0);
			filespec.APPEND_CONST("/" AUTO_FILE_NAME);
			main_class=use_file(
				filespec, site_auto_fail,
				main_class_name, main_class);
		}

		// loading auto.p files from document_root/.. 
		// to the one beside requested file.
		// all assigned bases from upper dir
		{
			Array ladder(pool());
			const char *after=info.path_translated;
			size_t drlen=strlen(info.document_root);
			if(memcmp(after, info.document_root, drlen)==0) {
				after+=drlen;
				if(after[-1]=='/') 
					--after;
			}
			
			while(const char *before=strchr(after, '/')) {
				String& step=*NEW String(pool());
				if(after!=info.path_translated) {
					step.APPEND_CLEAN(
						info.path_translated, before+1/* / */-info.path_translated,
						"path-translated-scanned", ladder.size());
					step << AUTO_FILE_NAME;
					ladder+=&step;
				}
				after=before+1;
			}
			for(int i=ladder.size(); --i>=0; ) {
				const String& sfile_spec=*ladder.get_string(i);
				main_class=use_file(sfile_spec, false/*ignore read problem*/,
					main_class_name, main_class);
			}
		}

		// compiling requested file
		String& spath_translated=*NEW String(pool());
		spath_translated.APPEND_CLEAN(info.path_translated, 0, "user-request", 0);
		main_class=use_file(spath_translated, true/*don't ignore read problem*/,
			main_class_name, main_class);

		// $MAIN:DEFAULTS
		Value *defaults=main_class->get_element(*defaults_name);
		// value must be allocated on request's pool for that pool used on
		// meaning constructing @see attributed_meaning_to_string
		default_content_type=defaults?defaults->get_element(*content_type_name):0;
		if(Value *element=main_class->get_element(*user_html_name))
			if(Table *table=element->get_table())
				pool().set_tag(table);

		// $MAIN:SQL.drivers
		if(Value *sql=main_class->get_element(*main_sql_name))
			if(Value *element=sql->get_element(*main_sql_drivers_name))
				protocol2library=element->get_table();		

		// $MAIN:MIME-TYPES
		if(Value *element=main_class->get_element(*mime_types_name))
			if(Table *table=element->get_table())
				mime_types=table;			

		/*
			$MAIN:CTYPE[
				$white-space[
					^#09^#0A^#0B^#0C^#0D^#20]
				$digit[
					0123456789]
				$hex-digit[
					0123456789ABCDEFabcdef]
				$letter[
					ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]
				$word[
					0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz]
				$meta[
					^#00$()*+.?[^{|]

				$lowercase[
					AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz]
			]
		*/
		if(Value *ctype=main_class->get_element(*ctype_name)) {
			// lowcase, flipcase, bits digit+word+whitespace, masks
			pcre_tables=(unsigned char *)calloc(tables_length);
			prepare_case_tables(pcre_tables);

			element2ctypes(pcre_tables, *ctype, *ctype_white_space_name, ctype_space, cbit_space, false);
			element2ctypes(pcre_tables, *ctype, *ctype_digit_name, ctype_digit, cbit_digit);
			element2ctypes(pcre_tables, *ctype, *ctype_hex_digit_name, ctype_xdigit);
			element2ctypes(pcre_tables, *ctype, *ctype_letter_name, ctype_letter);
			element2ctypes(pcre_tables, *ctype, *ctype_word_name, ctype_word, cbit_word);
			cstr2ctypes(pcre_tables, (const unsigned char *)"*+?{^.$|()[", ctype_meta);

			element2case(pcre_tables, *ctype, *ctype_lowercase_name);
		}

		// $MAIN:MAIL[$SMTP[mail.design.ru]]
		if(Value *mail_element=main_class->get_element(*mail_name))
			if(!(mail=mail_element->get_hash()))
				THROW(0, 0,
					0,
					"$MAIN:MAIL is not hash");


		// execute @main[]
		const String *body_string=execute_method(*main_class, *main_method_name);
		if(!body_string)
			THROW(0,0,
			0, 
			"'"MAIN_METHOD_NAME"' method not found");

		VString body_vstring_before_post_process(*body_string);
		VString *body_vstring_after_post_process=&body_vstring_before_post_process;
		
		// @postprocess
		if(Value *value=main_class->get_element(*post_process_method_name))
			if(Junction *junction=value->get_junction())
				if(const Method *method=junction->method) {
					// preparing to pass parameters to 
					//	@postprocess[data]
					VMethodFrame frame(pool(), *junction, false);
					frame.set_self(*main_class);

					frame.store_param(method->name, 
						&body_vstring_before_post_process);
					body_vstring_after_post_process=
						NEW VString(*execute_method(frame, *method));
				}

		const VFile *body_file=body_vstring_after_post_process->as_vfile();

		// extract response body
		Value *body_value=static_cast<Value *>(
			response.fields().get(*body_name));
		if(body_value) // there is some $response.body
			body_file=body_value->as_vfile();

		// OK. write out the result
		output_result(*body_file, header_only);
	} 
	CATCH(e) { // request handling problem
		// we're returning not result, but error explanation
		TRY {
			// log the beast
			const String *problem_source=e.problem_source();
			if(problem_source && problem_source->size())
				SAPI::log(pool(),
#ifndef NO_STRING_ORIGIN
					"%s(%d): "
#endif
					"'%s' %s [%s %s]",
#ifndef NO_STRING_ORIGIN
					problem_source->origin().file?problem_source->origin().file:"global",
					problem_source->origin().line,
#endif
					problem_source->cstr(String::UL_AS_IS),
					e.comment(),
					e.type()?e.type()->cstr(String::UL_AS_IS):"-",
					e.code()?e.code()->cstr(String::UL_AS_IS):"-"
				);
			else
				SAPI::log(pool(),
					"%s [%s %s]",
					e.comment(),
					e.type()?e.type()->cstr(String::UL_AS_IS):"-",
					e.code()?e.code()->cstr(String::UL_AS_IS):"-"
					);

			// reset language to default
			flang=fdefault_lang;
			
			// reset response
			response.fields().clear();

			// this is what we'd return in $response:body
			const String *body_string=0;

			if(main_class) { // we've managed to end up with some main_class
				// maybe we'd be lucky enough as to report an error
				// in a gracefull way...
				if(Value *value=main_class->get_element(*exception_method_name))
					if(Junction *junction=value->get_junction())
						if(const Method *method=junction->method) {
							// preparing to pass parameters to 
							//	@exception[origin;source;comment;type;code]
							VMethodFrame frame(pool(), *junction, false);
							frame.set_self(*main_class);

							const String *problem_source=e.problem_source();
							// origin
							Value *origin_value=0;
#ifndef NO_STRING_ORIGIN
							if(problem_source && problem_source->size()) {
								const Origin& origin=problem_source->origin();
								if(origin.file) {
									char *buf=(char *)malloc(MAX_STRING);
									size_t buf_size=snprintf(buf, MAX_STRING, "%s(%d):", 
										origin.file, 1+origin.line);
									String *origin_file_line=NEW String(pool(),
										buf, buf_size, true);
									origin_value=NEW VString(*origin_file_line);
								}
							}
#endif
							frame.store_param(method->name, 
								origin_value?origin_value:NEW VUnknown(pool()));

							// source
							Value *source_value=0;
							if(problem_source && problem_source->size()) {
								String& problem_source_copy=*NEW String(pool());
								problem_source_copy.append(*problem_source, 
									flang, true);
								source_value=NEW VString(problem_source_copy);
							}
							frame.store_param(method->name, 
								source_value?source_value:NEW VUnknown(pool()));

							// comment
							String *comment_value=NEW String(pool(),
								e.comment(), 0, true);
							frame.store_param(method->name, 
								NEW VString(*comment_value));

							// type
							Value *type_value;
							if(e.type()) {
								String& type_copy=*NEW String(pool());
								type_value=NEW VString(type_copy.append(*e.type(), 
									flang, true));
							} else
								type_value=NEW VUnknown(pool());
							frame.store_param(method->name, type_value);

							// code
							Value *code_value;
							if(e.code()) {
								String& code_copy=*NEW String(pool());
								code_value=NEW VString(code_copy.append(*e.code(), 
									flang, true));
							} else
								code_value=NEW VUnknown(pool());
							frame.store_param(method->name, code_value);

							// future $response:body=
							//   execute ^exception[origin;source;comment;type;code]
							body_string=execute_method(frame, *method);
						}
			}
			
			if(!body_string) {  // couldn't report an error beautifully?
				// doing that ugly

				// make up result: $origin $source $comment $type $code
				char *buf=(char *)malloc(MAX_STRING);
				size_t printed=0;
				const String *problem_source=e.problem_source();
				if(problem_source) {
#ifndef NO_STRING_ORIGIN
					const Origin& origin=problem_source->origin();
					if(origin.file)
						printed+=snprintf(buf+printed, MAX_STRING-printed, "%s(%d): ", 
						origin.file, 1+origin.line);
#endif
					printed+=snprintf(buf+printed, MAX_STRING-printed, "'%s' ", 
						problem_source->cstr(String::UL_AS_IS));
				}
				printed+=snprintf(buf+printed, MAX_STRING-printed, "%s", 
					e.comment());
				const String *type=e.type();
				if(type) {
					printed+=snprintf(buf+printed, MAX_STRING-printed, "  type: %s", 
						type->cstr(String::UL_AS_IS));
					const String *code=e.code();
					if(code)
						printed+=snprintf(buf+printed, MAX_STRING-printed, ", code: %s", 
						code->cstr(String::UL_AS_IS));
				}

				// future $response:content-type
				response.fields().put(*content_type_name, 
					NEW VString(*NEW String(pool(), UNHANDLED_EXCEPTION_CONTENT_TYPE)));
				// future $response:body
				body_string=NEW String(pool(), buf);
			}

			const VString body_vstring(*body_string);
			const VFile *body_file=body_vstring.as_vfile();

			// ERROR. write it out
			output_result(*body_file, header_only);
		}
		CATCH(e) {
			// exception in request exception handler
			// remember to rethrow it
			rethrow_me=e;  need_rethrow=true; 
		}
		END_CATCH
	}
	END_CATCH // do not use pool() after this point - no exception handler set
	          // any throw() would try to use zero exception() pointer 

	if(need_rethrow) // were there an exception for us to rethrow?
		THROW(rethrow_me.type(), rethrow_me.code(),
			rethrow_me.problem_source(),
			rethrow_me.comment());
}

VStateless_class *Request::use_file(const String& file_spec, bool fail_on_read_problem,
									const String *name, 
									VStateless_class *base_class) {
	// cyclic dependence check
	if(used_files.get(file_spec))
		return base_class;
	used_files.put(file_spec, (Hash::Val *)true);


	char *source=file_read_text(pool(), file_spec, fail_on_read_problem);
	if(!source)
		return base_class;

	return use_buf(source, file_spec.cstr(), 0/*new class*/, name, base_class);
}

VStateless_class *Request::use_buf(const char *source, const char *file,
								   VStateless_class *aclass, const String *name, 
								   VStateless_class *base_class) {
	// compile loaded class
	VStateless_class& cclass=COMPILE(source, aclass, name, base_class, file);

	// locate and execute possible @auto[] static method
	execute_method(cclass, *auto_method_name, false /*no result needed*/);
	return &cclass;
}

const String& Request::relative(const char *apath, const String& relative_name) {
	int lpath_buf_size=strlen(apath)+1;
    char *lpath=(char *)malloc(lpath_buf_size);
	memcpy(lpath, apath, lpath_buf_size);
    if(!rsplit(lpath, '/'))
		strcpy(lpath, ".");
	String& result=*NEW String(pool(), lpath);
    result << "/" << relative_name;
    return result;
}

const String& Request::absolute(const String& relative_name) {
	char *relative_name_cstr=relative_name.cstr();
	if(relative_name_cstr[0]=='/') {
		String& result=*NEW String(pool(), info.document_root);
		result << relative_name;
		return result;
	} else 
		return relative(info.path_translated, relative_name);
}

static void add_header_attribute(const Hash::Key& aattribute, Hash::Val *ameaning, 
								 void *info) {
	String *attribute_to_exclude=static_cast<String *>(info);
	if(aattribute==*attribute_to_exclude)
		return;

	Value& lmeaning=*static_cast<Value *>(ameaning);
	Pool& pool=lmeaning.pool();

	SAPI::add_header_attribute(pool,
		aattribute.cstr(String::UL_AS_IS), 
		attributed_meaning_to_string(lmeaning, String::UL_HTTP_HEADER).cstr());
}
void Request::output_result(const VFile& body_file, bool header_only) {
	// header: cookies
	cookie.output_result();
	
	// set content-type
	if(String *body_file_content_type=static_cast<String *>(
		body_file.fields().get(*vfile_mime_type_name))) {
		// body file content type
		response.fields().put(*content_type_name, body_file_content_type);
	} else {
		// default content type
		response.fields().put_dont_replace(*content_type_name, 
			default_content_type?default_content_type
			:NEW VString(*NEW String(pool(), DEFAULT_CONTENT_TYPE)));
	}

	// content-disposition
	if(VString *vfile_name=static_cast<VString *>(body_file.fields().get(*name_name)))
		if(vfile_name->string()!=NONAME_DAT) {
			VHash& vhash=*NEW VHash(pool());
			vhash.hash().put(*content_disposition_filename_name, vfile_name);
			response.fields().put(*content_disposition_name, &vhash);
		}

	// prepare header: $response:fields without :body
	response.fields().for_each(add_header_attribute, /*excluding*/ body_name);

	// prepare...
	const void *body=body_file.value_ptr();
	size_t content_length=body_file.value_size();

	// prepare header: content-length
	if(content_length) { // useful for redirecting [header "location: http://..."]
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%u", content_length);
		SAPI::add_header_attribute(pool(), "content-length", content_length_cstr);
	}

	// send header
	SAPI::send_header(pool());
	
	// send body
	if(!header_only)
		SAPI::send_body(pool(), body, content_length);
}

const String& Request::mime_type_of(const char *user_file_name_cstr) {
	if(mime_types)
		if(const char *cext=strrchr(user_file_name_cstr, '.')) {
			String sext(pool(), ++cext);
			if(mime_types->locate(0, sext))
				if(const String *result=mime_types->item(1))
					return *result;
				else
					THROW(0, 0,
						mime_types->origin_string(),
						"MIME-TYPE table column elements must not be empty");
		}
	return *NEW String(pool(), "application/octet-stream");
}