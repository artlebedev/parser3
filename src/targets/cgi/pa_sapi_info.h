#ifndef PA_SAPI_INFO_H
#define PA_SAPI_INFO_H

#define IDENT_PA_SAPI_INFO_H "$Id: pa_sapi_info.h,v 1.22 2025/05/26 00:52:15 moko Exp $"

#include "pa_sapi.h"
#include "pa_http.h"

/// IIS refuses to read bigger chunks
const size_t READ_POST_CHUNK_SIZE=0x400*0x400; // 1M
const int HEADERS_SENT = 999; // can't send error message after headers have been sent

// for signal handlers and cgi console detection
static Request *request=0;


class SAPI_Info : public PA_Allocated {
public:
	int http_response_code;
	String::Body headers;

	SAPI_Info() : http_response_code(200) {}

	virtual char* get_env(const char* name) {
		if(char *local=getenv(name))
			return pa_strdup(local);
		else
			return 0;
	}

	virtual bool set_env(const char*, const char*) {
		return false;
	}

	virtual const char* const *get_env() {
#ifdef _MSC_VER
		extern char **_environ;
		return _environ;
#else
		extern char **environ;
		return environ;
#endif
	}

	virtual size_t read_post(char *, size_t) {
		return 0;
	}

	virtual void add_header(const char* dont_store_key, const char* dont_store_value) {
		if(strcasecmp(dont_store_key, "location")==0)
			http_response_code=302;
		if(strcasecmp(dont_store_key, HTTP_STATUS)==0)
			http_response_code=atoi(dont_store_value);
	}

	virtual void send_headers() {}

	void clear_headers() {
		http_response_code=200;
		headers.clear();
	}

	virtual size_t send_body(const void *buf, size_t size) {
		return stdout_write(buf, size);
	}

	virtual void send_error(const char *exception_cstr, const char *status){
		http_response_code=atoi(status);
		send_body(exception_cstr, strlen(exception_cstr));
	}
};

class SAPI_Info_CGI : public SAPI_Info {
public:

	virtual size_t read_post(char *buf, size_t max_bytes) {
		size_t read_size=0;
		do {
			ssize_t chunk_size=read(fileno(stdin), buf+read_size, min(READ_POST_CHUNK_SIZE, max_bytes-read_size));
			if(chunk_size<=0)
			break;
			read_size+=chunk_size;
		} while(read_size<max_bytes);
		return read_size;
	}

	virtual void add_header(const char* dont_store_key, const char* dont_store_value) {
		headers << capitalize(dont_store_key) << ": " << pa_strdup(dont_store_value) << "\r\n";
	}

	virtual void send_headers() {
		if(!request || !request->console.was_used()){
			headers << "\r\n";
			send_body(headers.cstr(), headers.length());
			http_response_code=HEADERS_SENT;
		}
	}

	virtual void send_error(const char *exception_cstr, const char *status){
		if (http_response_code==HEADERS_SENT)
			return;
		// memory allocation is not allowed
		char buf[MAX_STRING];
		snprintf(buf, MAX_STRING, HTTP_STATUS_CAPITALIZED ": %s\r\n"
				HTTP_CONTENT_TYPE_CAPITALIZED ": text/plain\r\n\r\n", status);
		send_body(buf, strlen(buf));
		send_body(exception_cstr, strlen(exception_cstr));
	}

};

static char* replace_char(char* str, char from, char to){
    for(char *pos = strchr(str,from); pos; pos=strchr(pos,from)) {
        *pos = to;
    }
    return str;
}

class SAPI_Info_HTTPD : public SAPI_Info {
public:

	HTTPD_Connection &connection;
	HashStringString env;

	SAPI_Info_HTTPD(HTTPD_Connection &aconnection) : connection(aconnection) {}

	void populate_env() {
		String::Body host("localhost");
		for(Array_iterator<HTTP_Headers::Header> i(connection.headers()); i; ){
			HTTP_Headers::Header header=i.next();
			String name("HTTP_");
			name << replace_char(header.name.cstrm(), '-', '_');
			String::Body value=header.value;

			if(header.name == "HOST"){
				size_t port=value.pos(':');
				if(port != STRING_NOT_FOUND)
					value=value.mid(0, port);
				host=value;
			}
			env.put(name, value);
		}

		HASH_PUT_CSTR(env, "REQUEST_METHOD", String::Body(connection.method()));
		HASH_PUT_CSTR(env, "REQUEST_URI", String::Body(connection.uri()));
		HASH_PUT_CSTR(env, "QUERY_STRING", String::Body(connection.query()));

		HASH_PUT_CSTR(env, "SERVER_NAME", host);
		HASH_PUT_CSTR(env, "SERVER_PORT", String::Body(HTTPD_Server::port));
		HASH_PUT_CSTR(env, "REMOTE_ADDR", String::Body(connection.remote_addr));

	}

	virtual char* get_env(const char* name) {
		String::Body request_value = env.get(name);
		if(!request_value.is_empty())
			return request_value.cstrm();
		if(char *server_value=getenv(name))
			return pa_strdup(server_value);
		return NULL;
	}

	virtual bool set_env(const char* name, const char* value) {
		env.put(name, *new String(value));
		return true;
	}

	virtual const char* const *get_env() {
		const char** result=new(PointerGC) const char*[env.count()+1/*0*/];
		const char** cur=result;
		for(HashStringString::Iterator i(env); i; i.next()){
			String pair;
			pair << i.key() << "=" << i.value();
			*cur++=pair.cstr();
		}
		*cur=NULL;
		return result;
	}

	virtual size_t read_post(char *buf, size_t max_bytes) {
		return connection.read_post(buf, max_bytes);
	}

	static const char *exception_http_status(const char *type) {
		struct Lookup {
			const char *code;
			const char *type;
		} static lookup[] = {
			{  "",  "httpd.write"},
			{"400", "httpd.request"},
			{"400", "http.response"},
			{"404", "file.missing"},
			{"408", "httpd.timeout"},
			{"408", "httpd.read"},
			{"501", "httpd.method"},
			{ NULL, ""}
		};
		Lookup *cur = lookup;
		for(; cur->code; cur++)
			if(!strcmp(type, cur->type))
				return cur->code;
		return "500";
	}

	static const char *status_message(int code) {
		struct Lookup {
			int code;
			const char *message;
		} static lookup[] = {
			{200, "OK"},
			{204, "No Content"},
			{206, "Partial Content"},
			{301, "Moved Permanently"},
			{302, "Found"},
			{304, "Not Modified"},
			{400, "Bad Request"},
			{401, "Unauthorized"},
			{403, "Forbidden"},
			{404, "Not Found"},
			{408, "Request Timeout"},
			{416, "Range Not Satisfiable"},
			{500, "Internal Server Error"},
			{501, "Not Implemented"},
			{502, "Bad Gateway"},
			{504, "Gateway Timeout"},
			{  0, "Undescribed"}
		};
		Lookup *cur = lookup;
		for(; cur->code; cur++)
			if(code == cur->code)
				return cur->message;
		return cur->message;
	}

	virtual void add_header(const char* dont_store_key, const char* dont_store_value) {
		if(strcasecmp(dont_store_key, "location")==0)
			http_response_code=302;
		if(strcasecmp(dont_store_key, HTTP_STATUS)==0)
			http_response_code=atoi(dont_store_value);
		else
			headers << capitalize(dont_store_key) << ": " << pa_strdup(dont_store_value) << "\r\n";
	}

	virtual void send_headers() {
		String result("HTTP/1.0 ");
		result << pa_uitoa(http_response_code) << " " << status_message(http_response_code) << "\r\n" << headers << "\r\n";
		send_body(result.cstr(), result.length());
		http_response_code=HEADERS_SENT;
	}

	virtual size_t send_body(const void *buf, size_t size) {
		return connection.send_body(buf, size);
	}

	virtual void send_error(const char *exception_cstr, const char *status){
		if (http_response_code==HEADERS_SENT)
			return;
		// memory allocation is not allowed
		char buf[MAX_STRING];
		snprintf(buf, MAX_STRING, "HTTP/1.0 %s %s\r\n"
				HTTP_CONTENT_TYPE_CAPITALIZED ": text/plain\r\n\r\n", status, status_message(atoi(status)));
		send_body(buf, strlen(buf));
		send_body(exception_cstr, strlen(exception_cstr));
	}

};

#endif
