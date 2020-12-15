/** @file
	Parser: @b curl parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_config_includes.h"

#include "pa_vmethod_frame.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_charsets.h"
#include "pa_vstring.h"
#include "pa_vdate.h"
#include "pa_vtable.h"
#include "pa_common.h"
#include "pa_http.h" 
#include "ltdl.h"

volatile const char * IDENT_CURL_C="$Id: curl.C,v 1.67 2020/12/15 17:10:27 moko Exp $";

class MCurl: public Methoded {
public:
	MCurl();
};

// global variables

DECLARE_CLASS_VAR(curl, new MCurl);

#include "curl.h"

typedef CURL *(*t_curl_easy_init)(); t_curl_easy_init f_curl_easy_init;
typedef CURLcode (*t_curl_easy_setopt)(CURL *, CURLoption option, ...); t_curl_easy_setopt f_curl_easy_setopt;
typedef CURLcode (*t_curl_easy_perform)(CURL *); t_curl_easy_perform f_curl_easy_perform;
typedef void (*t_curl_easy_cleanup)(CURL *); t_curl_easy_cleanup f_curl_easy_cleanup;
typedef const char *(*t_curl_easy_strerror)(CURLcode); t_curl_easy_strerror f_curl_easy_strerror;
typedef CURLcode (*t_curl_easy_getinfo)(CURL *curl, CURLINFO info, ...); t_curl_easy_getinfo f_curl_easy_getinfo;
typedef struct curl_slist *(*t_curl_slist_append)(struct curl_slist *,const char *); t_curl_slist_append f_curl_slist_append;
typedef const char *(*t_curl_version)(); t_curl_version f_curl_version;
typedef CURLFORMcode (*t_curl_formadd)(struct curl_httppost **httppost, struct curl_httppost **last_post, ...); t_curl_formadd f_curl_formadd;
typedef void (*t_curl_formfree)(struct curl_httppost *form); t_curl_formfree f_curl_formfree;

#define GLINK(name) f_##name=(t_##name)lt_dlsym(handle, #name);
#define DLINK(name) GLINK(name) if(!f_##name) return "function " #name " was not found";
		
static const char *dlink(const char *dlopen_file_spec) {
	pa_dlinit();

	lt_dlhandle handle=lt_dlopen(dlopen_file_spec);

	if(!handle){
		if(const char* result=lt_dlerror())
			return result;
		return "can not open the dynamic link module";
	}

	DLINK(curl_easy_init);
	DLINK(curl_easy_cleanup);
	DLINK(curl_version);
	DLINK(curl_easy_setopt);
	DLINK(curl_easy_perform);
	DLINK(curl_easy_strerror);
	DLINK(curl_easy_getinfo);
	DLINK(curl_slist_append);
	DLINK(curl_formadd);
	DLINK(curl_formfree);
	return 0;
}


struct ParserOptions : public PA_Allocated {
	// real options
	const String *filename;
	const String *content_type;
	bool is_text;
	Charset *charset, *response_charset;

	// stuff for internal use
	const char *url;
	struct curl_httppost *f_post;
	FILE *f_stderr;

	// if response content-length check required
	bool no_body;
	// stuff to walkaround curl request content-length bugs
	bool is_post;
	bool has_content_length;

	ParserOptions() : filename(0), content_type(0), is_text(true), charset(0), response_charset(0), url(0), f_post(0), f_stderr(0), no_body(false), is_post(false), has_content_length(false){}
	~ParserOptions() {
		f_curl_formfree(f_post);
		if(f_stderr)
			fclose(f_stderr);
	}

};

// using TLS instead of keeping variables in request
THREAD_LOCAL CURL *fcurl = 0;
THREAD_LOCAL ParserOptions *foptions = 0;

static CURL *curl(){
	if(!fcurl)
		throw Exception("curl", 0, "outside of 'session' operator");
	return fcurl;
}

static ParserOptions &options(){
	if(!foptions)
		throw Exception("curl", 0, "outside of 'session' operator");
	return *foptions;
}

// using temporal object scheme to garanty cleanup call
class Temp_curl {
	CURL *saved_curl;
	ParserOptions *saved_options;

	// every TLS should be referenced elsewhere, or GC will collect it
	CURL *thread_curl;
	ParserOptions *thread_options;
public:
	Temp_curl() : saved_curl(fcurl), saved_options(foptions){
		thread_curl = fcurl = f_curl_easy_init();
		thread_options = foptions = new ParserOptions();
		f_curl_easy_setopt(fcurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); // avoid ipv6 by default
	}

	~Temp_curl() {
		f_curl_easy_cleanup(fcurl);
		fcurl = saved_curl;
		delete foptions;
		foptions = saved_options;
	}
};

bool curl_linked = false;
const char *curl_status = 0;
const char *curl_library="libcurl" LT_MODULE_EXT;

static void temp_curl(void (*action)(Request&, MethodParams&), Request& r, MethodParams& params){
	if(!curl_linked)
		curl_status=dlink(curl_library);

	if(curl_status == 0){
		curl_linked=true;
		Temp_curl temp_curl;
		action(r,params);
	} else {
		throw Exception("curl", 0, "failed to load curl library %s: %s", curl_library, curl_status);
	}
}

static void _curl_session_action(Request& r, MethodParams& params){
	Value& body_code=params.as_junction(0, "body must be code");
	r.process_write(body_code);
}

static void _curl_session(Request& r, MethodParams& params){
	temp_curl(_curl_session_action, r, params);
}

static void _curl_version_action(Request& r, MethodParams& ){
	r.write(*new VString(*new String(f_curl_version(), String::L_TAINTED)));
}

static void _curl_version(Request& r, MethodParams& params){
	fcurl ? _curl_version_action(r, params) : temp_curl(_curl_version_action, r, params);
}

struct CurlOption : public PA_Allocated{

	enum OptionType {
		CURL_STRING,
		CURL_URLENCODE, // url-encoded string
		CURL_URL,
		CURL_INT,
		CURL_NO_BODY,
		CURL_POST,
		CURL_POSTFIELDS,
		CURL_FORM,
		CURL_HEADERS,
		CURL_FILE,
		CURL_STDERR,
		CURL_HTTP_VERSION,
		PARSER_LIBRARY,
		PARSER_NAME,
		PARSER_CONTENT_TYPE,
		PARSER_MODE,
		PARSER_CHARSET,
		PARSER_RESPONSE_CHARSET
	};

	CURLoption id;
	OptionType type;
	CurlOption(CURLoption aid, OptionType atype): id(aid), type(atype) {}
};

class CurlOptionHash: public HashString<CurlOption*> {
public:
	CurlOptionHash() {
#define	CURL_OPT(type, name) put(str_lower(#name),new CurlOption(CURLOPT_##name, CurlOption::type));
#define	PARSER_OPT(type, name) put(name,new CurlOption((CURLoption)0, CurlOption::type));
		CURL_OPT(CURL_URL, URL);
		CURL_OPT(CURL_STRING, INTERFACE);
		CURL_OPT(CURL_INT, LOCALPORT);
		CURL_OPT(CURL_INT, PORT);
		
		CURL_OPT(CURL_INT, VERBOSE);
		CURL_OPT(CURL_STDERR, STDERR);
		CURL_OPT(CURL_INT, MAXFILESIZE);
		
		CURL_OPT(CURL_INT, HTTPAUTH);
		CURL_OPT(CURL_STRING, USERPWD);

		CURL_OPT(CURL_STRING, USERNAME);
		CURL_OPT(CURL_STRING, PASSWORD);

		CURL_OPT(CURL_URLENCODE, USERAGENT);
		CURL_OPT(CURL_URLENCODE, REFERER);
		CURL_OPT(CURL_INT, AUTOREFERER);

		CURL_OPT(CURL_STRING, ENCODING); // gzip or deflate
		CURL_OPT(CURL_STRING, ACCEPT_ENCODING); // gzip or deflate

		CURL_OPT(CURL_INT, FOLLOWLOCATION);
		CURL_OPT(CURL_INT, UNRESTRICTED_AUTH);
		CURL_OPT(CURL_INT, IPRESOLVE);

		CURL_OPT(CURL_POST, POST);
		CURL_OPT(CURL_INT, HTTPGET);
		CURL_OPT(CURL_NO_BODY, NOBODY);
		CURL_OPT(CURL_STRING, CUSTOMREQUEST);

		CURL_OPT(CURL_POSTFIELDS, POSTFIELDS); // hopefully is safe too
		CURL_OPT(CURL_POSTFIELDS, COPYPOSTFIELDS);
		CURL_OPT(CURL_FORM, HTTPPOST);

		CURL_OPT(CURL_HEADERS, HTTPHEADER);
		CURL_OPT(CURL_URLENCODE, COOKIE);
		CURL_OPT(CURL_URLENCODE, COOKIELIST);
		CURL_OPT(CURL_INT, COOKIESESSION);

		CURL_OPT(CURL_INT, IGNORE_CONTENT_LENGTH);
		CURL_OPT(CURL_INT, HTTP_CONTENT_DECODING);
		CURL_OPT(CURL_INT, HTTP_TRANSFER_DECODING);

		CURL_OPT(CURL_INT, MAXREDIRS);
		CURL_OPT(CURL_INT, POSTREDIR);

		CURL_OPT(CURL_STRING, RANGE);

		CURL_OPT(CURL_INT, TIMEOUT);
		CURL_OPT(CURL_INT, TIMEOUT_MS);
		CURL_OPT(CURL_INT, LOW_SPEED_LIMIT);
		CURL_OPT(CURL_INT, LOW_SPEED_TIME);
		CURL_OPT(CURL_INT, MAXCONNECTS);

		CURL_OPT(CURL_STRING, PROXY);
		CURL_OPT(CURL_INT, PROXYPORT);
		CURL_OPT(CURL_INT, PROXYTYPE);
		CURL_OPT(CURL_INT, HTTPPROXYTUNNEL); 
		CURL_OPT(CURL_STRING, PROXYUSERPWD);
		CURL_OPT(CURL_INT, PROXYAUTH);

		CURL_OPT(CURL_INT, FRESH_CONNECT);
		CURL_OPT(CURL_INT, FORBID_REUSE);
		CURL_OPT(CURL_INT, CONNECTTIMEOUT);
		CURL_OPT(CURL_INT, CONNECTTIMEOUT_MS);
		CURL_OPT(CURL_INT, FAILONERROR);

		CURL_OPT(CURL_FILE, SSLCERT);
		CURL_OPT(CURL_STRING, SSLCERTTYPE);
		CURL_OPT(CURL_FILE, SSLKEY);
		CURL_OPT(CURL_STRING, SSLKEYTYPE);
		CURL_OPT(CURL_STRING, KEYPASSWD);
		CURL_OPT(CURL_STRING, SSLENGINE);
		CURL_OPT(CURL_STRING, SSLENGINE_DEFAULT);

		CURL_OPT(CURL_FILE, ISSUERCERT);
		CURL_OPT(CURL_FILE, CRLFILE);

		CURL_OPT(CURL_STRING, CAINFO);
		CURL_OPT(CURL_FILE, CAPATH);
		CURL_OPT(CURL_INT, SSL_VERIFYPEER);
		CURL_OPT(CURL_INT, SSL_VERIFYHOST);
		CURL_OPT(CURL_STRING, SSL_CIPHER_LIST);
		CURL_OPT(CURL_INT, SSL_SESSIONID_CACHE);
		CURL_OPT(CURL_INT, SSLVERSION);
		CURL_OPT(CURL_HTTP_VERSION, HTTP_VERSION);

		PARSER_OPT(PARSER_LIBRARY, "library");
		PARSER_OPT(PARSER_NAME, "name");
		PARSER_OPT(PARSER_CONTENT_TYPE, "content-type");
		PARSER_OPT(PARSER_MODE, "mode");
		PARSER_OPT(PARSER_CHARSET, "charset");
		PARSER_OPT(PARSER_RESPONSE_CHARSET, "response-charset");
	}

} *curl_options=0;

struct CurlInfo : public PA_Allocated{

	enum OptionType {
		CURL_STRING,
		CURL_INT,
		CURL_DOUBLE,
		CURL_HTTP_VERSION
	};

	CURLINFO id;
	OptionType type;
	CurlInfo(CURLINFO aid, OptionType atype): id(aid), type(atype) {}
};

class CurlInfoHash: public OrderedHashString<CurlInfo*> {
public:
	CurlInfoHash() {
#define	CURL_INF(type, name) put(str_lower(#name),new CurlInfo(CURLINFO_##name, CurlInfo::type));
		CURL_INF(CURL_STRING, SCHEME);
		CURL_INF(CURL_HTTP_VERSION, HTTP_VERSION);
		CURL_INF(CURL_STRING, EFFECTIVE_URL);
		CURL_INF(CURL_STRING, CONTENT_TYPE);
		CURL_INF(CURL_INT, RESPONSE_CODE);
		CURL_INF(CURL_INT, OS_ERRNO);

		CURL_INF(CURL_DOUBLE, NAMELOOKUP_TIME);
		CURL_INF(CURL_DOUBLE, APPCONNECT_TIME);
		CURL_INF(CURL_DOUBLE, PRETRANSFER_TIME);
		CURL_INF(CURL_DOUBLE, STARTTRANSFER_TIME);
		CURL_INF(CURL_DOUBLE, CONNECT_TIME);
		CURL_INF(CURL_DOUBLE, TOTAL_TIME);

		CURL_INF(CURL_DOUBLE, CONTENT_LENGTH_DOWNLOAD);
		CURL_INF(CURL_DOUBLE, CONTENT_LENGTH_UPLOAD);
		CURL_INF(CURL_INT, HEADER_SIZE);
		CURL_INF(CURL_INT, REQUEST_SIZE);
		CURL_INF(CURL_DOUBLE, SIZE_DOWNLOAD);
		CURL_INF(CURL_DOUBLE, SIZE_UPLOAD);
		CURL_INF(CURL_DOUBLE, SPEED_DOWNLOAD);
		CURL_INF(CURL_DOUBLE, SPEED_UPLOAD);

		CURL_INF(CURL_INT, NUM_CONNECTS);
		CURL_INF(CURL_STRING, PRIMARY_IP);
		CURL_INF(CURL_INT, HTTPAUTH_AVAIL);
		CURL_INF(CURL_INT, PROXYAUTH_AVAIL);
		CURL_INF(CURL_INT, REDIRECT_COUNT);
		CURL_INF(CURL_DOUBLE, REDIRECT_TIME);
		CURL_INF(CURL_STRING, REDIRECT_URL);
		CURL_INF(CURL_INT, SSL_VERIFYRESULT);
	}

} *curl_infos=0;

static const char *curl_urlencode(const String &s, Request& r){
	if(options().charset){
		Temp_client_charset temp(r.charsets, *options().charset);
		return s.untaint_and_transcode_cstr(String::L_URI, &r.charsets);
	} else
		return s.untaint_cstr(String::L_URI);
}

static struct curl_slist *curl_headers(HashStringValue *value_hash, Request& r) {
	struct curl_slist *slist=NULL;

	for(HashStringValue::Iterator i(*value_hash); i; i.next() ){
		String header = 
			String(pa_http_safe_header_name(capitalize(i.key().cstr())), String::L_AS_IS)
			<< ": " 
			<< String(i.value()->as_string(), String::L_HTTP_HEADER);

		slist=f_curl_slist_append(slist, curl_urlencode(header, r));
	}
	return slist;
}

static const char* curl_transcode(const String &s, Request& r){
	return options().charset ? Charset::transcode(s.cstr(), r.charsets.source(), *options().charset).cstr() : s.cstr();
}

static void curl_form(HashStringValue *value_hash, Request& r){ 
	struct curl_httppost *f_last=0;
	for(HashStringValue::Iterator i(*value_hash); i; i.next() ){
		const char *key = curl_transcode(String(i.key().cstr()), r);
		if(const String* svalue = i.value()->get_string()){ 
			// string
			f_curl_formadd(&options().f_post, &f_last, 
				CURLFORM_PTRNAME, key,
				CURLFORM_PTRCONTENTS, curl_transcode(String(svalue->cstr()), r), 
				CURLFORM_END);
		} else if(Table* tvalue = i.value()->get_table()){
			// table
			for(size_t t = 0; t < tvalue->count(); t++) {
				f_curl_formadd(&options().f_post, &f_last, 
					CURLFORM_PTRNAME, key,
					CURLFORM_PTRCONTENTS, curl_transcode(String(tvalue->get(t)->get(0)->cstr()), r), 
					CURLFORM_END);
			}
		} else if(VFile* fvalue=static_cast<VFile *>(i.value()->as("file"))){
			// file
			f_curl_formadd(&options().f_post, &f_last, 
				CURLFORM_PTRNAME, key,
				CURLFORM_BUFFER, curl_transcode(String(fvalue->fields().get("name")->as_string(), String::L_FILE_SPEC), r),
				CURLFORM_BUFFERLENGTH, (long)fvalue->value_size(), 
				CURLFORM_BUFFERPTR, fvalue->value_ptr(), 
				CURLFORM_CONTENTTYPE, fvalue->fields().get("content-type")->as_string().taint_cstr(String::L_URI), 
				CURLFORM_END);
		} else {
			throw Exception("curl", new String(i.key(), String::L_TAINTED), "is %s, form option value can be string, table or file only", i.value()->type());
		}
	}
}

static const char *curl_check_file(const String &file_spec){
	const char *file_spec_cstr=file_spec.taint_cstr(String::L_FILE_SPEC);
	struct stat finfo;
	if(pa_stat(file_spec_cstr, &finfo)==0)
		check_safe_mode(finfo, file_spec, file_spec_cstr);
	return file_spec_cstr;
}

static long curl_http_version(const String &name){
	if(name.is_empty()) return CURL_HTTP_VERSION_NONE;

	if(name == "1.0") return CURL_HTTP_VERSION_1_0;
	if(name == "1.1") return CURL_HTTP_VERSION_1_1;
	if(name == "2") return CURL_HTTP_VERSION_2;
	if(name == "2.0") return CURL_HTTP_VERSION_2_0;

	const char *sname = str_upper(name.cstr());
	if(!strcmp(sname,"2TLS")) return CURL_HTTP_VERSION_2TLS;
	if(!strcmp(sname,"2ONLY")) return CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE;
	throw Exception("curl", &name, "invalid http_version option value");
}

static const char *curl_http_version_name(long value){
	if(value == CURL_HTTP_VERSION_NONE) return "none";
	if(value == CURL_HTTP_VERSION_1_0) return "1.0";
	if(value == CURL_HTTP_VERSION_1_1) return "1.1";
	if(value == CURL_HTTP_VERSION_2) return "2";
	throw Exception("curl", 0, "invalid http version '%d' in info", value);
}

static void curl_setopt(HashStringValue::key_type key, HashStringValue::value_type value, Request& r) {
	CurlOption *opt=curl_options->get(key);

	if(opt==0)
		throw Exception("curl", 0, "called with invalid option '%s'", key.cstr());

	CURLcode res = CURLE_OK;
	Value &v=r.process(*value);

	switch (opt->type){
		case CurlOption::CURL_STRING:{
			// string curl option
			const char *value_str=v.as_string().cstr();
			res=f_curl_easy_setopt(curl(), opt->id, value_str);
			break;
		}
		case CurlOption::CURL_URLENCODE:{
			// url-encoded string curl option
			const char *value_str=curl_urlencode(v.as_string(), r);
			res=f_curl_easy_setopt(curl(), opt->id, value_str);
			break;
		}
		case CurlOption::CURL_URL:{
			// url-encoded string curl_url option
			const String url = v.as_string();
			if(!url.starts_with("http://") && !url.starts_with("https://"))
				throw Exception("curl", 0, "failed to set option '%s': invalid url scheme '%s'", key.cstr(), url.cstr());
			options().url=curl_urlencode(url, r);
			res=f_curl_easy_setopt(curl(), opt->id, options().url);
			break;
		}
		case CurlOption::CURL_INT:{
			// integer curl option
			long value_int=(long)v.as_double();
			res=f_curl_easy_setopt(curl(), opt->id, value_int);
			break;
		}
		case CurlOption::CURL_NO_BODY:{
			// integer curl option
			long value_int=(long)v.as_double();
			res=f_curl_easy_setopt(curl(), opt->id, value_int);
			options().no_body=value_int != 0;
			break;
		}
		case CurlOption::CURL_POST:{
			// integer curl option
			long value_int=(long)v.as_double();
			res=f_curl_easy_setopt(curl(), opt->id, value_int);
			options().is_post=value_int != 0;
			break;
		}
		case CurlOption::CURL_POSTFIELDS:{
			// http post curl option
			if(v.get_string()){
				if( (res=f_curl_easy_setopt(curl(), CURLOPT_POSTFIELDSIZE, -1L)) == CURLE_OK )
					res=f_curl_easy_setopt(curl(), opt->id, curl_urlencode(v.as_string(), r));
			} else {
				VFile *file=v.as_vfile(String::L_AS_IS);
				if( (res=f_curl_easy_setopt(curl(), CURLOPT_POSTFIELDSIZE, (long)file->value_size())) == CURLE_OK )
					res=f_curl_easy_setopt(curl(), opt->id, file->value_ptr());
			}
			options().has_content_length=true;
			break;
		}
		case CurlOption::CURL_FORM:{
			HashStringValue *value_hash = v.as_hash("failed to set option 'httppost': value");
			if(value_hash){
				curl_form(value_hash, r);
			} else {
				if(options().f_post)
					f_curl_formfree(options().f_post);
				options().f_post = 0;
			}
			res=f_curl_easy_setopt(curl(), CURLOPT_HTTPPOST, foptions->f_post);
			options().has_content_length=true;
			break;
		}
		case CurlOption::CURL_HEADERS:{
			// http headers curl option
			HashStringValue *value_hash=v.as_hash("failed to set option 'httpheader': value");
			res=f_curl_easy_setopt(curl(), opt->id, value_hash ? curl_headers(value_hash, r) : 0);
			break;
		}
		case CurlOption::CURL_FILE:{
			// file-spec curl option
			const char *file_spec_cstr=curl_check_file(r.full_disk_path(v.as_string()));
			res=f_curl_easy_setopt(curl(), opt->id, file_spec_cstr);
			break;
		}
		case CurlOption::CURL_STDERR:{
			// verbose output redirection from stderr to file curl option
			const char *file_spec_cstr=curl_check_file(r.full_disk_path(v.as_string()));
			FILE *f_stderr=options().f_stderr=pa_fopen(file_spec_cstr, "wt");
			if (f_stderr){
				res=f_curl_easy_setopt(curl(), opt->id, f_stderr);
			} else {
				throw Exception("curl", 0, "failed to set option '%s': unable to open file '%s'", key.cstr(), file_spec_cstr);
			}
			break;
		}
		case CurlOption::CURL_HTTP_VERSION:{
			// http protocol version name curl option
			long value_int=curl_http_version(v.as_string());
			res=f_curl_easy_setopt(curl(), opt->id, value_int);
			break;
		}
		case CurlOption::PARSER_LIBRARY:{
			// 'library' parser option
			if(!curl_linked){
				curl_library=v.as_string().taint_cstr(String::L_FILE_SPEC);
			} else 
				throw Exception("curl", 0, "failed to set option '%s': already loaded", key.cstr());
			break;
		}
		case CurlOption::PARSER_NAME:{
			// 'name' parser option
			options().filename=&v.as_string();
			break;
		}
		case CurlOption::PARSER_CONTENT_TYPE:{
			// 'content-type' parser option
			options().content_type=&v.as_string();
			break;
		}
		case CurlOption::PARSER_MODE:{
			// 'mode' parser option
			options().is_text=VFile::is_text_mode(v.as_string());
			break;
		}
		case CurlOption::PARSER_CHARSET:{
			// 'charset' parser option should be processed before other options
			break;
		}
		case CurlOption::PARSER_RESPONSE_CHARSET:{
			// 'response-charset' parser option
			options().response_charset=&pa_charsets.get(v.as_string());
			break;
		}
	}

	if(res != CURLE_OK)
		throw Exception("curl", 0, "failed to set option '%s': %s", key.cstr(), f_curl_easy_strerror(res));
}

static void _curl_options(Request& r, MethodParams& params){
	if(curl_options==0)
		curl_options=new CurlOptionHash();

	if(HashStringValue* options_hash=params.as_hash(0)){
		if(Value* value=options_hash->get("charset")){
			// charset should be handled first as params may require transcode
			Value &v=r.process(*value);
			options().charset=&pa_charsets.get(v.as_string());
		}
		options_hash->for_each<Request&>(curl_setopt, r);
	}
}

#define CURL_GETINFO(arg) \
	if((res=f_curl_easy_getinfo(curl(), info->id, &arg)) != CURLE_OK){ \
		if (fail_on_error) \
			throw Exception("curl", 0, "failed to get %s info: %s", key.cstr(), f_curl_easy_strerror(res)); \
		return 0; \
	}

static Value *curl_getinfo(const String::Body &key, CurlInfo *info, bool fail_on_error=false) {
	CURLcode res;
	switch (info->type){
		case CurlInfo::CURL_STRING:{
			char *str=0;
			CURL_GETINFO(str);
			return new VString(str ? *new String(pa_strdup(str), String::L_TAINTED) : String::Empty);
		}
		case CurlInfo::CURL_INT:{
			long l=0;
			CURL_GETINFO(l);
			return new VInt(l);
		}
		case CurlInfo::CURL_DOUBLE:{
			double d=0;
			CURL_GETINFO(d);
			return new VDouble(d);
		}
		case CurlInfo::CURL_HTTP_VERSION:{
			long l=0;
			CURL_GETINFO(l);
			return new VString(*new String(curl_http_version_name(l), String::L_TAINTED));
		}
	}
	return VVoid::get();
}

static void _curl_info(Request& r, MethodParams& params){
	if(curl_infos==0)
		curl_infos=new CurlInfoHash();
	if(params.count()==1){
		const String &name=params.as_string(0, "name must be string");
		CurlInfo *info=curl_infos->get(name);
		if(info==0)
			throw Exception("curl", 0, "called with invalid parameter '%s'", name.cstr());
		r.write(*curl_getinfo(name, info, true));
	} else {
		VHash& result=*new VHash;
		for(CurlInfoHash::Iterator i(*curl_infos); i; i.next() ){
			Value *value=curl_getinfo(i.key(), i.value());
			if(value)
				result.get_hash()->put(i.key(), value);
		}
		r.write(result);
	}
}

class Curl_buffer{
public:
	char *buf;
	size_t length;
	size_t buf_size;
	HTTP_Headers& headers;

	Curl_buffer(HTTP_Headers& aheaders) : buf((char *)pa_malloc_atomic(MAX_STRING)), length(0), buf_size(MAX_STRING-1), headers(aheaders){}

	void resize(size_t size){
		buf_size=size;
		buf=(char *)pa_realloc(buf, size+1);
	}
};

static int curl_writer(char *data, size_t size, size_t nmemb, Curl_buffer *result){
	if(result == 0)
		return 0;

	size=size*nmemb;
	if(size>0){
		size_t buf_required = result->length + size;
		if(buf_required > result->buf_size)
			result->resize(buf_required <= result->headers.content_length ? (size_t)result->headers.content_length : result->buf_size*2 + size);
		memcpy(result->buf+result->length, data, size);
		result->length += size;
	}
	return size;
}

static int curl_header(char *data, size_t size, size_t nmemb, HTTP_Headers *result){
	if(result == 0)
		return 0;

	size=size*nmemb;
	if(size>0){
		char *header=pa_strdup(data, size);
		if(!pa_strncasecmp(header, "HTTP/") && !strchr(header, ':')){
			// response code, clearing possible headers from previous requests
			result->clear();
		} else {
			result->add_header(header);
			if(result->content_length>pa_file_size_limit && !options().no_body)
				return 0;
		}
	}
	return size;
}

#define CURL_SETOPT(option, arg, message) \
	if( (res=f_curl_easy_setopt(curl(), option, arg)) != CURLE_OK){ \
		throw Exception("curl", 0, "failed to set " message ": %s", f_curl_easy_strerror(res)); \
	}

static void _curl_load_action(Request& r, MethodParams& params){
	if(params.count()==1)
		_curl_options(r, params);

	CURLcode res;

	// we need a container for headers as VFile fields can be put only after VFile.set
	HTTP_Headers response;
	CURL_SETOPT(CURLOPT_HEADERFUNCTION, curl_header, "curl header function");
	CURL_SETOPT(CURLOPT_WRITEHEADER, &response, "curl header buffer");

	Curl_buffer body(response);
	CURL_SETOPT(CURLOPT_WRITEFUNCTION, curl_writer, "curl writer function");
	CURL_SETOPT(CURLOPT_WRITEDATA, &body, "curl write buffer");

	if(options().is_post && !options().has_content_length){
		// libcurl bug walkaround. Prior to 7.38 (Debian Jessie) curl passed Content-length: -1
		// after that no Content-length header is passed, that hangs request to nginx.
		CURL_SETOPT(CURLOPT_POSTFIELDSIZE, 0, "post content-length");
	}

	ALTER_EXCEPTION_SOURCE(res=f_curl_easy_perform(curl()), new String(options().url));
	if(res != CURLE_OK){
		const char *ex_type = 0; 
		switch(res){
			case CURLE_OPERATION_TIMEDOUT:
				ex_type = "curl.timeout"; break;
			case CURLE_COULDNT_RESOLVE_HOST:
				ex_type = "curl.host"; break;
			case CURLE_COULDNT_CONNECT:
				ex_type = "curl.connect"; break;
			case CURLE_HTTP_RETURNED_ERROR:
				ex_type = "curl.status"; break;
			case CURLE_SSL_CONNECT_ERROR:
			case CURLE_SSL_CERTPROBLEM:
			case CURLE_SSL_CIPHER:
			case CURLE_SSL_CACERT:
			case CURLE_SSL_ENGINE_INITFAILED:
				ex_type = "curl.ssl"; break;
			case CURLE_WRITE_ERROR:
				check_file_size(response.content_length, new String(options().url)); break;
			default: break;
		}
		throw Exception( PA_DEFAULT(ex_type, "curl.fail"), new String(options().url), "%s", f_curl_easy_strerror(res));
	}

	// assure trailing zero
	body.buf[body.length]=0;

	VFile& result=*new VFile;

	Charset *asked_charset = options().response_charset;
	if (!asked_charset && !response.content_type.is_empty())
		asked_charset=detect_charset(response.content_type.cstr());

	if(options().is_text)
		asked_charset=pa_charsets.checkBOM(body.buf, body.length, asked_charset);

	if (!asked_charset)
		asked_charset = options().charset;

	if(options().is_text && asked_charset != 0){
		String::C c=Charset::transcode(String::C(body.buf, body.length), *asked_charset, r.charsets.source());
		body.buf=(char *)c.str;
		body.length=c.length;
	}

	const String *content_type = PA_DEFAULT(options().content_type, response.content_type.is_empty() ? 0 : new String(response.content_type, String::L_TAINTED));
	const String *filename = PA_DEFAULT(options().filename, new String(options().url));

	result.set(true/*tainted*/, options().is_text, body.buf, body.length, filename, content_type ? new VString(*content_type) : 0, &r);

	long http_status = 0;
	if(f_curl_easy_getinfo(curl(), CURLINFO_RESPONSE_CODE, &http_status) == CURLE_OK){
		result.fields().put("status", new VInt(http_status));
	}

	VHash* vtables=new VHash;
	result.fields().put("tables", vtables);

	for(Array_iterator<HTTP_Headers::Header> i(response.headers); i.has_next(); ){
		HTTP_Headers::Header header=i.next();

		if(asked_charset)
			header.transcode(*asked_charset, r.charsets.source());

		String &header_value=*new String(header.value, String::L_TAINTED);

		tables_update(vtables->hash(), header.name, header_value);
		result.fields().put(header.name, new VString(header_value));
	}

	// filling $.cookies
	if(Value *vcookies=vtables->hash().get("SET-COOKIE"))
		result.fields().put(HTTP_COOKIES_NAME, new VTable(parse_cookies(r, vcookies->get_table())));

	r.write(result);
}

static void _curl_load(Request& r, MethodParams& params){
	fcurl ? _curl_load_action(r, params) : temp_curl(_curl_load_action, r, params);
}

// constructor
MCurl::MCurl(): Methoded("curl") {
	add_native_method("session", Method::CT_STATIC, _curl_session, 1, 1);
	add_native_method("version", Method::CT_STATIC, _curl_version, 0, 0);
	add_native_method("options", Method::CT_STATIC, _curl_options, 1, 1);
	add_native_method("info", Method::CT_STATIC, _curl_info, 0, 1);
	add_native_method("load", Method::CT_STATIC, _curl_load, 0, 1);
}
