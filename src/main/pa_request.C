/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.162 2001/09/30 14:36:26 parser Exp $
*/

#include "pa_config_includes.h"

#include "pcre.h"
#include "internal.h"
extern "C" unsigned char pcre_default_tables[]; // pcre/chartables.c

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_vclass.h"
#include "pa_globals.h"
#include "pa_vint.h"
#include "pa_vmethod_frame.h"
#include "pa_types.h"
#include "pa_vtable.h"
#include "pa_vfile.h"
#include "pa_dictionary.h"

#ifdef XML
#	include <util/XercesDefs.hpp>
#	include <util/TransENameMap.hpp>
#	include <util/XML256TableTranscoder.hpp>
#	include <util/PlatformUtils.hpp>
#	include <PlatformSupport/XalanTranscodingServices.hpp>
#endif

/// content type of exception response, when no @MAIN:exception handler defined
const char *UNHANDLED_EXCEPTION_CONTENT_TYPE="text/plain";

/// content type of response when no $MAIN:defaults.content-type defined
const char *DEFAULT_CONTENT_TYPE="text/html";
const char *ORIGINS_CONTENT_TYPE="text/plain";

Methoded *MOP_create(Pool&);


inline void prepare_case_tables(unsigned char *tables) {
	unsigned char *lcc_table=tables+lcc_offset;
	unsigned char *fcc_table=tables+fcc_offset;
	for(int i=0; i<0x100; i++)
		lcc_table[i]=fcc_table[i]=i;
}
inline void cstr2ctypes(unsigned char *tables, const unsigned char *cstr, 
						unsigned char bit) {
	unsigned char *ctypes_table=tables+ctypes_offset;
	ctypes_table[0]=bit;
	for(; *cstr; cstr++) {
		unsigned char c=*cstr;
		ctypes_table[c]|=bit;
	}
}
inline unsigned int to_wchar_code(const String *s) {
	if(!s || s->size()==0)
		return 0;
	if(s->size()==1)
		return (unsigned int)s->first_char();
	return (unsigned int)s->as_int();
}
inline bool to_bool(const String *s) {
	return s && s->size()!=0;
}
static void element2ctypes(unsigned char c, bool belongs,
						   unsigned char *tables,  unsigned char bit, int group_offset=-1) {
	if(!belongs)
		return;

	unsigned char *ctypes_table=tables+ctypes_offset;

	ctypes_table[c]|=bit;
	if(group_offset>=0)
		tables[cbits_offset+group_offset+c/8] |= 1 << (c%8);
}
static void element2case(unsigned char from, unsigned char to,
						 unsigned char *tables) {
	if(!to) 
		return;

	unsigned char *lcc_table=tables+lcc_offset;
	unsigned char *fcc_table=tables+fcc_offset;
	lcc_table[from]=to;
	fcc_table[from]=to; fcc_table[to]=from;
}

#ifndef DOXYGEN
struct Charset_tables {
	// pcre_tables
	unsigned char *pcre_tables;
#ifdef XML
	// transcoder
	XMLCh *fromTable;
	XMLTransService::TransRec *toTable;
	unsigned int toTableSz;
#endif
};
#endif

static void ctype_table_row_to_charset_tables(Array::Item *value, void *info) {
	Array& row=*static_cast<Array *>(value);
	Charset_tables& tables=*static_cast<Charset_tables *>(info);
	
// char	white-space	digit	hex-digit	letter	word	lowercase	unicode1	unicode2	
	unsigned int c=to_wchar_code(row.get_string(0));
	
	// pcre_tables
	element2ctypes(c, to_bool(row.get_string(1)), tables.pcre_tables, ctype_space, cbit_space);
	element2ctypes(c, to_bool(row.get_string(2)), tables.pcre_tables, ctype_digit, cbit_digit);
	element2ctypes(c, to_bool(row.get_string(3)), tables.pcre_tables, ctype_xdigit);
	element2ctypes(c, to_bool(row.get_string(4)), tables.pcre_tables, ctype_letter);
	element2ctypes(c, to_bool(row.get_string(5)), tables.pcre_tables, ctype_word, cbit_word);
	element2case(c, to_wchar_code(row.get_string(6)), tables.pcre_tables);

#ifdef XML
	XMLCh unicode1=row.size()>7?(XMLCh)to_wchar_code(row.get_string(7)):0;
	XMLCh unicode2=row.size()>8?(XMLCh)to_wchar_code(row.get_string(8)):0;
	if(!tables.fromTable[c])
		tables.fromTable[c]=unicode1?unicode1:(XMLCh)c;
	tables.toTable[tables.toTableSz].intCh=unicode1?unicode1:(XMLCh)c;
	tables.toTable[tables.toTableSz].extCh=(XMLByte)c;
	tables.toTableSz++;
	if(unicode2) {
		tables.toTable[tables.toTableSz].intCh=unicode2;
		tables.toTable[tables.toTableSz].extCh=(XMLByte)c;
		tables.toTableSz++;
	}
#endif
}
static int sort_cmp_Trans_rec_intCh(const void *a, const void *b) {
	const XMLCh ca=static_cast<const XMLTransService::TransRec *>(a)->intCh;
	const XMLCh cb=static_cast<const XMLTransService::TransRec *>(b)->intCh;
	// move zeros to end of table
	if(ca==0)
		return +1;
	if(cb==0)
		return -1;

	return ca-cb;
}


#ifdef XML
template <class TType> class ENameMapFor2 : public ENameMap
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    ENameMapFor2(
		const XMLCh* const encodingName
        , const XMLCh* const                        fromTable
        , const XMLTransService::TransRec* const    toTable
        , const unsigned int                        toTableSize
		) : ENameMap(encodingName),
		ffromTable(fromTable),
		ftoTable(toTable),
		ftoTableSize(toTableSize) {}
    ~ENameMapFor2() {}

    // -----------------------------------------------------------------------
    //  Implementation of virtual factory method
    // -----------------------------------------------------------------------
    virtual XMLTranscoder* makeNew(const unsigned int blockSize) const {
		return new TType(
			getKey(), 
			blockSize,
			ffromTable,
			ftoTable, ftoTableSize);
	}
private:
	const XMLCh* const                        ffromTable;
	const XMLTransService::TransRec* const    ftoTable;
	const unsigned int                        ftoTableSize;

private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    ENameMapFor2();
    ENameMapFor2(const ENameMapFor2<TType>&);
    void operator=(const ENameMapFor2<TType>&);
};

class XML256TableTranscoder2 : public XML256TableTranscoder
{
public :
    XML256TableTranscoder2(
        const   XMLCh* const                        encodingName
        , const unsigned int                        blockSize
        , const XMLCh* const                        fromTable
        , const XMLTransService::TransRec* const    toTable
        , const unsigned int                        toTableSize
		) : XML256TableTranscoder(encodingName, blockSize, fromTable, toTable, toTableSize) {}

private :
    XML256TableTranscoder2();
    XML256TableTranscoder2(const XML256TableTranscoder2&);
    void operator=(const XML256TableTranscoder2&);
};
#endif

#ifdef XALAN_HACK_DIGITAL_ENTITIES
static void hack_s_maximumCharacterValues(const XalanDOMString& encoding) {
/*
	open:
		xml-xalan/c/src/PlatformSupport/XalanTranscodingServices.hpp 
	find: 
		static const MaximumCharacterValueMapType&	s_maximumCharacterValues;
	paste to next line:
		friend static void hack_s_maximumCharacterValues(const XalanDOMString& encoding); // hack by paf
*/

	/*
	const_cast<XalanTranscodingServices::MaximumCharacterValueMapType &>(
		XalanTranscodingServices::s_maximumCharacterValues).insert(
		XalanTranscodingServices::MaximumCharacterValueMapType::value_type(encoding, 0xFFFF));
		*/
}
#endif

static void load_charset(const Hash::Key& akey, Hash::Val *avalue, 
										  void *info) {
	Hash& CTYPE=*static_cast<Hash *>(info);
	Pool& pool=CTYPE.pool();

	Charset_tables tables={
		// pcre_tables
		// lowcase, flipcase, bits digit+word+whitespace, masks
		(unsigned char *)pool.calloc(tables_length) // pcre_tables
#ifdef XML
		// transcoder
		, (XMLCh *)pool.calloc(sizeof(XMLCh)*0x100) // fromTable
		, 0 // toTable
		, 0 // toTableSz
#endif
	};
	prepare_case_tables(tables.pcre_tables);
	cstr2ctypes(tables.pcre_tables, (const unsigned char *)"*+?{^.$|()[", ctype_meta);

	// fill tables
	Value& value=*static_cast<Value *>(avalue);
	if(Table *table=value.get_table()) {
#ifdef XML
		tables.toTable=(XMLTransService::TransRec *)pool.calloc(
			sizeof(XMLTransService::TransRec)*table->size()*2);
#endif
		table->for_each(ctype_table_row_to_charset_tables, &tables);
#ifdef XML
		// sort by the Unicode code point
		_qsort(tables.toTable, tables.toTableSz, sizeof(*tables.toTable), 
			sort_cmp_Trans_rec_intCh);
#endif
	} else
		PTHROW(0, 0,
			&value.name(),
			"must be hash");

	// charset->pcre_tables 
	CTYPE.put(akey, tables.pcre_tables);

#ifdef XML
	// charset->transcoder
	XalanDOMString sencoding(akey.cstr());
#ifdef XALAN_HACK_DIGITAL_ENTITIES
	hack_s_maximumCharacterValues(sencoding);
#endif
	const XMLCh* const auto_encoding_cstr=sencoding.c_str();
	int size=sizeof(XMLCh)*(sencoding.size()+1);
	XMLCh* pool_encoding_cstr=(XMLCh*)malloc(size);
	memcpy(pool_encoding_cstr, auto_encoding_cstr, size);
    XMLString::upperCase(pool_encoding_cstr);

    XMLPlatformUtils::fgTransService->addEncoding(
		pool_encoding_cstr, 
		new ENameMapFor2<XML256TableTranscoder2>(
			pool_encoding_cstr
			, tables.fromTable
			, tables.toTable
			, tables.toTableSz
		));
#endif
}

//
Request::Request(Pool& apool,
				 Info& ainfo,
				 String::Untaint_lang adefault_lang) : Pooled(apool),
	stack(apool),
	OP(*MOP_create(apool)),
	env(apool),
	form(apool),
	math(apool),
	request(apool, *this),
	response(apool),
	cookie(apool),
	fclasses(apool),
	CTYPE(apool),
	fdefault_lang(adefault_lang), flang(adefault_lang),
	info(ainfo),
	post_data(0), post_size(0),
	used_files(apool),
	default_content_type(0),
	mime_types(0),
	main_class(0),
	connection(0),
	classes_conf(apool),
	anti_endless_execute_recoursion(0)	
{
	/// directly used
	// operators
	OP.register_directly_used(*this);
	// classes:
	// table, file, random, mail, image, ...
	methoded_array->register_directly_used(*this);

	/// methodless
	// env class
	classes().put(*NEW String(pool(), ENV_CLASS_NAME), &env);
	// request class
	classes().put(*NEW String(pool(), REQUEST_CLASS_NAME), &request);	
	// cookie class
	classes().put(*NEW String(pool(), COOKIE_CLASS_NAME), &cookie);

	/// methoded
	// response class
	classes().put(response.get_class()->name(), &response);	

	/// bases used
	// form class
	classes().put(form.get_class()->base()->name(), &form);	
	// math class
	classes().put(math.get_class()->base()->name(), &math);	
}

/**
	load MAIN class, execute @main.
	MAIN class consists of all the auto.p files we'd manage to find
	plus
	the file user requested us to process
	all located classes become children of one another,
	composing class we name 'MAIN'
*/
void Request::core(
				   const char *root_config_filespec, bool root_config_fail_on_read_problem,
				   const char *site_config_filespec, bool site_config_fail_on_read_problem,
				   bool header_only) {
	//_asm { int 3 }
	bool need_rethrow=false;  Exception rethrow_me;
	TRY {
		char *auto_filespec=(char *)malloc(MAX_STRING);
		
		// loading root config
		if(root_config_filespec) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(root_config_filespec, 0, "root_config", 0);
			main_class=use_file(
				filespec, 
				true/*ignore class_path*/, root_config_fail_on_read_problem,
				main_class_name, main_class);
		}

		// configure root options
		//	until someone with less privileges have overriden them
		OP.configure_admin(*this);
		methoded_array->configure_admin(*this);

		// loading site config
		if(site_config_filespec) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(site_config_filespec, 0, "site_config", 0);
			main_class=use_file(
				filespec, 
				true/*ignore class_path*/, site_config_fail_on_read_problem,
				main_class_name, main_class);
		}

		// loading auto.p files from document_root/.. 
		// to the one beside requested file.
		// all assigned bases from upper dir
		{
			const char *after=info.path_translated;
			size_t drlen=strlen(info.document_root);
			if(memcmp(after, info.document_root, drlen)==0) {
				after+=drlen;
				if(after[-1]=='/') 
					--after;
			}
			
			int step=0;
			while(const char *before=strchr(after, '/')) {
				String& sfile_spec=*NEW String(pool());
				if(after!=info.path_translated) {
					sfile_spec.APPEND_CLEAN(
						info.path_translated, before-info.path_translated,
						"path-translated-scanned", step++);
					sfile_spec << "/" AUTO_FILE_NAME;

					main_class=use_file(sfile_spec, 
						true/*ignore class_path*/, false/*ignore read problem*/,
						main_class_name, main_class);
				}
				after=before+1;
			}
		}

		// compile requested file
		String& spath_translated=*NEW String(pool());
		spath_translated.APPEND_TAINTED(info.path_translated, 0, "user-request", 0);
		main_class=use_file(spath_translated, 
			true/*ignore class_path*/, true/*don't ignore read problem*/,
			main_class_name, main_class);

		// configure not-root=user options
		OP.configure_user(*this);
		methoded_array->configure_user(*this);

		// $MAIN:DEFAULTS
		Value *defaults=main_class->get_element(*defaults_name);
		// value must be allocated on request's pool for that pool used on
		// meaning constructing @see attributed_meaning_to_string
		default_content_type=defaults?defaults->get_element(*content_type_name):0;
#ifdef XML
		// record default charset
		if(default_content_type)
			if(Hash *hash=default_content_type->get_hash())
				if(Value *vcharset=(Value *)hash->get(*charset_name))
					pool().set_charset(vcharset->as_string());		
#endif

		if(Value *element=main_class->get_element(*user_html_name))
			if(Table *table=element->get_table())
				pool().set_tag(NEW Dictionary(*table));

		// $MAIN:MIME-TYPES
		if(Value *element=main_class->get_element(*mime_types_name))
			if(Table *table=element->get_table())
				mime_types=table;			

		if(Value *vcharsets=main_class->get_element(*charsets_name)) {
			if(Hash *charsets=vcharsets->get_hash())
				charsets->for_each(load_charset, &CTYPE);
			else
				THROW(0, 0,
					&vcharsets->name(),
					"must be hash");
		}

		// filling form fields
		form.fill_fields_and_tables(*this);

		// filling cookies
		cookie.fill_fields(*this);

		// execute @main[]
		const String *body_string=execute_virtual_method(
			*main_class, *main_method_name);
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
					VMethodFrame frame(pool(), value->name(), *junction);
					frame.set_self(*main_class);

					frame.store_param(method->name, 
						&body_vstring_before_post_process);
					body_vstring_after_post_process=
						NEW VString(*execute_method(frame, *method));
				}

		bool origins_mode=main_class->get_element(*origins_mode_name)!=0;

		const VFile *body_file=body_vstring_after_post_process->as_vfile(
			String::UL_UNSPECIFIED, origins_mode);

		// extract response body
		Value *body_value=static_cast<Value *>(
			response.fields().get(*body_name));
		if(body_value) // there is some $response.body
			body_file=body_value->as_vfile();
		else if(origins_mode)
			response.fields().put(*content_type_name, 
				NEW VString(*NEW String(pool(), ORIGINS_CONTENT_TYPE)));

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
			if(flang==String::UL_USER_HTML)
				flang=String::UL_HTML; // no _ & Co conversions in @exception[params]
			
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
							VMethodFrame frame(pool(), value->name(), *junction);
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
								origin_value?origin_value:NEW VVoid(pool()));

							// source
							Value *source_value=0;
							if(problem_source && problem_source->size()) {
								String& problem_source_copy=*NEW String(pool());
								problem_source_copy.append(*problem_source, 
									flang, true);
								source_value=NEW VString(problem_source_copy);
							}
							frame.store_param(method->name, 
								source_value?source_value:NEW VVoid(pool()));

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
								type_value=NEW VVoid(pool());
							frame.store_param(method->name, type_value);

							// code
							Value *code_value;
							if(e.code()) {
								String& code_copy=*NEW String(pool());
								code_value=NEW VString(code_copy.append(*e.code(), 
									flang, true));
							} else
								code_value=NEW VVoid(pool());
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

			VString body_vstring(*body_string);
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

VStateless_class *Request::use_file(const String& file_name, 
									bool ignore_class_path, bool fail_on_read_problem,
									const String *name, 
									VStateless_class *base_class) {
	// cyclic dependence check
	if(used_files.get(file_name))
		return base_class;
	used_files.put(file_name, (Hash::Val *)true);

	const String *file_spec;
	if(ignore_class_path) // ignore_class_path?
		file_spec=&file_name;
	else if(file_name.first_char()=='/') //absolute path, no need to scan MAIN:CLASS_PATH?
		file_spec=&absolute(file_name);
	else {
		file_spec=0;
		if(main_class)
			if(Value *element=main_class->get_element(*class_path_name)) {
				if(element->is_string()) {
					file_spec=file_readable(element->as_string(), file_name); // found at class_path?
				} else if(Table *table=element->get_table()) {
					int size=table->size();
					for(int i=size; i--; ) {
						const String& path=*static_cast<Array *>(table->get(i))->get_string(0);
						if(file_spec=file_readable(path, file_name))
							break; // found along class_path
					}
				} else
					THROW(0, 0,
						&element->name(),
						"must be string or table");
				if(!file_spec)
					THROW(0, 0,
						&file_name,
						"not found along " MAIN_CLASS_NAME ":" CLASS_PATH_NAME);
			}
		if(!file_spec)
			THROW(0, 0,
				&file_name,
				"usage failed - no " MAIN_CLASS_NAME  ":" CLASS_PATH_NAME " were specified");
	}

	char *source=file_read_text(pool(), *file_spec, fail_on_read_problem);
	if(!source)
		return base_class;

	return use_buf(source, file_spec->cstr(), 0/*new class*/, name, base_class);
}

VStateless_class *Request::use_buf(const char *source, const char *file,
								   VStateless_class *aclass, const String *name, 
								   VStateless_class *base_class) {
	// compile loaded class
	VStateless_class& cclass=COMPILE(source, aclass, name, base_class, file);

	// locate and execute possible @auto[] static method
	execute_nonvirtual_method(cclass, *auto_method_name, false /*no result needed*/);
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

unsigned char *Request::pcre_tables() {
	if(unsigned char *result=(unsigned char *)CTYPE.get(pool().get_charset()))
		return result;

	return pcre_default_tables;
}
