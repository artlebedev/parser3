/** @file
	Parser: @b curl parser class.

	Copyright(c) 2001-2009 ArtLebedev Group(http://www.artlebedev.com)
*/

static const char * const IDENT_INET_C="$Date: 2009/12/04 04:20:34 $";

#include "pa_vmethod_frame.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_charsets.h"
#include "ltdl.h"

class MCurl: public Methoded {
public:
	MCurl();

public: // Methoded
	bool used_directly() { return true; }
};

// global variables

DECLARE_CLASS_VAR(curl, new MCurl, 0);

// from file.C
extern bool is_text_mode(const String& mode);

#ifdef HAVE_CURL
#include "curl.h"

typedef CURL *(*t_curl_easy_init)(); t_curl_easy_init f_curl_easy_init;
typedef CURLcode (*t_curl_easy_setopt)(CURL *, CURLoption option, ...); t_curl_easy_setopt f_curl_easy_setopt;
typedef CURLcode (*t_curl_easy_perform)(CURL *); t_curl_easy_perform f_curl_easy_perform;
typedef void (*t_curl_easy_cleanup)(CURL *); t_curl_easy_cleanup f_curl_easy_cleanup;
typedef const char *(*t_curl_easy_strerror)(CURLcode); t_curl_easy_strerror f_curl_easy_strerror;
typedef struct curl_slist *(*t_curl_slist_append)(struct curl_slist *,const char *); t_curl_slist_append f_curl_slist_append;

#define GLINK(name) f_##name=(t_##name)lt_dlsym(handle, #name);
#define DLINK(name) GLINK(name) if(!f_##name) return "function " #name " was not found";
		
const char *dlink(const char *dlopen_file_spec) {
	if(lt_dlinit())
		return lt_dlerror();

	lt_dlhandle handle=lt_dlopen(dlopen_file_spec);

	if(!handle){
		if(const char* result=lt_dlerror())
			return result;
		return "can not open the dynamic link module";
	}

	DLINK(curl_easy_init);
	DLINK(curl_easy_cleanup);
	DLINK(curl_easy_setopt);
	DLINK(curl_easy_perform);
	DLINK(curl_easy_strerror);
	DLINK(curl_slist_append);
	return 0;
}


class ParserOptions {
public:
	const char *filename;
	const String *content_type;
	bool is_text;
	Charset *charset;

	ParserOptions() : filename(0), content_type(0), is_text(true), charset(0) {}
};

// using thread local variables instead of keeping them in request
// not necessary for cgi version
#ifdef WIN32
#define __thread __declspec(thread)
#endif
__thread CURL *fcurl=0;
__thread ParserOptions *foptions;

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
	CURL* saved_curl;
	ParserOptions* saved_options;
public:
	Temp_curl() : saved_curl(fcurl) {
		fcurl = f_curl_easy_init();
		foptions = new ParserOptions();
	}
	~Temp_curl() {
		f_curl_easy_cleanup(fcurl);
		fcurl = saved_curl;
		delete foptions;
		foptions = saved_options;
	}
};

bool curl_linked = false;
#ifdef WIN32
const char *curl_library="libcurl.dll";
#else
const char *curl_library="libcurl.so";
#endif

const char *curl_status = 0;

static void temp_curl(void (*action)(Request&, MethodParams&), Request& r, MethodParams& params){
	if(!curl_linked){
		curl_linked=true;
		curl_status=dlink(curl_library);
	}

	if(curl_status == 0){
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


static char *str_lower(const char *str){
	char *result=pa_strdup(str);
	for(char* c=result; *c; c++)
		*c=(char)tolower((unsigned char)*c);
	return result;
}

static char *str_upper(const char *str){
	char *result=pa_strdup(str);
	for(char* c=result; *c; c++)
		*c=(char)toupper((unsigned char)*c);
	return result;
}

class CurlOption {
public:

	enum OptionType {
		CURL_STRING,
		CURL_URLENCODE, // url-encoded string
		CURL_INT,
		CURL_POST,
		CURL_HEADERS,
		CURL_FILE,
		PARSER_LIBRARY,
		PARSER_NAME,
		PARSER_CONTENT_TYPE,
		PARSER_MODE,
		PARSER_CHARSET
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
		CURL_OPT(CURL_URLENCODE, URL);
		CURL_OPT(CURL_STRING, INTERFACE);
		CURL_OPT(CURL_INT, LOCALPORT);
		CURL_OPT(CURL_INT, PORT);

		CURL_OPT(CURL_INT, HTTPAUTH);
		CURL_OPT(CURL_STRING, USERPWD);

		CURL_OPT(CURL_STRING, USERNAME);
		CURL_OPT(CURL_STRING, PASSWORD);

		CURL_OPT(CURL_INT, AUTOREFERER);
		CURL_OPT(CURL_STRING, ENCODING); // gzip or deflate
		CURL_OPT(CURL_INT, FOLLOWLOCATION);
		CURL_OPT(CURL_INT, UNRESTRICTED_AUTH);

		CURL_OPT(CURL_INT, POST);
		CURL_OPT(CURL_INT, HTTPGET);

		CURL_OPT(CURL_POST, POSTFIELDS); // hopefully is safe too
		CURL_OPT(CURL_POST, COPYPOSTFIELDS);

		CURL_OPT(CURL_HEADERS, HTTPHEADER);
		CURL_OPT(CURL_URLENCODE, COOKIE);
		CURL_OPT(CURL_URLENCODE, COOKIELIST);
		CURL_OPT(CURL_INT, COOKIESESSION);

		CURL_OPT(CURL_INT, IGNORE_CONTENT_LENGTH);
		CURL_OPT(CURL_INT, HTTP_CONTENT_DECODING);
		CURL_OPT(CURL_INT, HTTP_TRANSFER_DECODING);

		CURL_OPT(CURL_INT, TIMEOUT);
		CURL_OPT(CURL_INT, TIMEOUT_MS);
		CURL_OPT(CURL_INT, LOW_SPEED_LIMIT);
		CURL_OPT(CURL_INT, LOW_SPEED_TIME);
		CURL_OPT(CURL_INT, MAXCONNECTS);

		CURL_OPT(CURL_INT, FRESH_CONNECT);
		CURL_OPT(CURL_INT, FORBID_REUSE);
		CURL_OPT(CURL_INT, CONNECTTIMEOUT);
		CURL_OPT(CURL_INT, CONNECTTIMEOUT_MS);

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
		CURL_OPT(CURL_STRING, CAPATH);
		CURL_OPT(CURL_INT, SSL_VERIFYPEER);
		CURL_OPT(CURL_INT, SSL_VERIFYHOST);
		CURL_OPT(CURL_STRING, SSL_CIPHER_LIST);
		CURL_OPT(CURL_INT, SSL_SESSIONID_CACHE);

		PARSER_OPT(PARSER_LIBRARY, "library");
		PARSER_OPT(PARSER_NAME, "name");
		PARSER_OPT(PARSER_CONTENT_TYPE, "content-type");
		PARSER_OPT(PARSER_MODE, "mode");
		PARSER_OPT(PARSER_CHARSET, "charset");
	}

} *curl_options=0;

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
			String(capitalize(i.key().cstr()), String::L_URI)
			<< ": " 
			<< String(i.value()->as_string(), String::L_URI);

		slist=f_curl_slist_append(slist, curl_urlencode(header, r));
	}
	return slist;
}

static void curl_setopt(HashStringValue::key_type key, HashStringValue::value_type value, Request& r) {
	CurlOption *opt=curl_options->get(key);

	if(opt==0)
		throw Exception("curl", 0, "called with invalid option '%s'", key.cstr());

	CURLcode res = CURLE_OK;
	Value &v=r.process_to_value(*value);

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
		case CurlOption::CURL_INT:{
			// int curl option
			int value_int=(int)v.as_double();
			res=f_curl_easy_setopt(curl(), opt->id, value_int);
			break;
		}
		case CurlOption::CURL_POST:{
			// http post curl option
			if(v.get_string()){
				if( (res=f_curl_easy_setopt(curl(), CURLOPT_POSTFIELDSIZE, -1)) == CURLE_OK )
					res=f_curl_easy_setopt(curl(), opt->id, curl_urlencode(v.as_string(), r));
			} else {
				VFile *file=v.as_vfile(String::L_AS_IS);
				if( (res=f_curl_easy_setopt(curl(), CURLOPT_POSTFIELDSIZE, file->value_size())) == CURLE_OK )
					res=f_curl_easy_setopt(curl(), opt->id, file->value_ptr());
			}
			break;
		}
		case CurlOption::CURL_HEADERS:{
			// http headers curl option
			HashStringValue *value_hash=v.get_hash();
			res=f_curl_easy_setopt(curl(), opt->id, value_hash ? curl_headers(value_hash, r) : 0);
			break;
		}
		case CurlOption::CURL_FILE:{
			// file-spec curl option
			const char *value_str=r.absolute(v.as_string()).taint_cstr(String::L_FILE_SPEC);
			res=f_curl_easy_setopt(curl(), opt->id, value_str);
			break;
		}
		case CurlOption::PARSER_LIBRARY:{
			// 'library' parser option
			if(fcurl==0){
				curl_library=v.as_string().taint_cstr(String::L_FILE_SPEC);
			} else 
				throw Exception("curl", 0, "failed to set option '%s': %s", key.cstr(), "already loaded");
			break;
		}
		case CurlOption::PARSER_NAME:{
			// 'name' parser option
			options().filename=v.as_string().taint_cstr(String::L_FILE_SPEC);
			break;
		}
		case CurlOption::PARSER_CONTENT_TYPE:{
			// 'content-type' parser option
			options().content_type=&v.as_string();
			break;
		}
		case CurlOption::PARSER_MODE:{
			// 'mode' parser option
			options().is_text=is_text_mode(v.as_string());
			break;
		}
		case CurlOption::PARSER_CHARSET:{
			// 'charset' parser option
			options().charset=&::charsets.get(v.as_string().change_case(r.charsets.source(), String::CC_UPPER));
			break;
		}
	}

	if(res != CURLE_OK)
		throw Exception("curl", 0, "failed to set option '%s': %s", key.cstr(), f_curl_easy_strerror(res));
}

static void _curl_option(Request& r, MethodParams& params){
	if(curl_options==0)
		curl_options=new CurlOptionHash();

	if(HashStringValue* options=params.as_no_junction(0, OPTIONS_MUST_NOT_BE_CODE).get_hash()){
		options->for_each<Request&>(curl_setopt, r);
	} else
		throw Exception("curl", 0, "options must be hash");
}


class Curl_buffer{
public:
	char *buf;
	size_t length;
	size_t buf_size;

	Curl_buffer() : buf((char *)pa_malloc(MAX_STRING+1)), length(0), buf_size(MAX_STRING){}
};

static int curl_writer(char *data, size_t size, size_t nmemb, Curl_buffer *result){
	if(result == 0)
		return 0;

	size=size*nmemb;
	if(size>0){
		if(result->length + size >= result->buf_size){
			result->buf_size = result->buf_size*2 + size;
			result->buf = (char *)pa_realloc(result->buf, result->buf_size+1);
		}
		memcpy(result->buf+result->length, data, size);
		result->length += size;
	}
	return size;
}

static int curl_header(char *data, size_t size, size_t nmemb, HASH_STRING<char *> *result){
	if(result == 0)
		return 0;

	size=size*nmemb;
	if(size>0){
		char *line=pa_strdup(data, size);
		char *value=lsplit(line,':');
		if(value && *line){
			// we need only headers, not the response code
			result->put(str_upper(line), value);
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
		_curl_option(r, params);

	CURLcode res;

	Curl_buffer body;
	CURL_SETOPT(CURLOPT_WRITEFUNCTION, curl_writer, "curl writer function");
	CURL_SETOPT(CURLOPT_WRITEDATA, &body, "curl write buffer");

	// we need a container for headers as VFile fields can be put only after VFile.set
	HASH_STRING<char *> headers; 
	CURL_SETOPT(CURLOPT_HEADERFUNCTION, curl_header, "curl header function");
	CURL_SETOPT(CURLOPT_WRITEHEADER, &headers, "curl header buffer");

	if((res=f_curl_easy_perform(curl())) != CURLE_OK){
		throw Exception("curl", 0, "failed to exec curl session: %s", f_curl_easy_strerror(res));
	}

	// assure trailing zero
	body.buf[body.length]=0;

	Charset *asked_charset=options().charset;

	if(options().is_text && asked_charset != 0){
		String::C c=Charset::transcode(String::C(body.buf, body.length), *asked_charset, r.charsets.source());
		body.buf=(char *)c.str;
		body.length=c.length;
	}

	Value* vcontent_type=
		options().content_type ? new VString(*options().content_type) :
		options().filename ? new VString(r.mime_type_of(options().filename)) : 0;

	VFile& result=*new VFile;
	result.set(true /*tainted*/, body.buf, body.length, options().filename, vcontent_type);
	result.set_mode(options().is_text);

	for(HASH_STRING<char *>::Iterator i(headers); i; i.next() ){
		String::Body key=i.key();	
		String::Body value=i.value();
		if(asked_charset){
			key=Charset::transcode(key, *asked_charset, r.charsets.source());
			value=Charset::transcode(value, *asked_charset, r.charsets.source());
		}
		result.fields().put(key, new VString(*new String(value.trim(String::TRIM_BOTH, " \t\n\r"), String::L_TAINTED)));
	}

	r.write_no_lang(result);
}

static void _curl_load(Request& r, MethodParams& params){
	fcurl ? _curl_load_action(r, params) : temp_curl(_curl_load_action, r, params);
}

#endif // HAVE_CURL

// constructor
MCurl::MCurl(): Methoded("curl") {
#ifdef HAVE_CURL
	add_native_method("session", Method::CT_STATIC, _curl_session, 1, 1);
	add_native_method("option", Method::CT_STATIC, _curl_option, 1, 1);
	add_native_method("load", Method::CT_STATIC, _curl_load, 0, 1);
#endif // HAVE_CURL
}
