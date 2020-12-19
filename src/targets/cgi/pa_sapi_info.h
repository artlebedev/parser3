#ifndef PA_SAPI_INFO_H
#define PA_SAPI_INFO_H

#define IDENT_PA_SAPI_INFO_H "$Id: pa_sapi_info.h,v 1.15 2020/12/19 22:34:21 moko Exp $"

#include "pa_sapi.h"
#include "pa_http.h"

/// IIS refuses to read bigger chunks
const size_t READ_POST_CHUNK_SIZE=0x400*0x400; // 1M

// for signal handlers and cgi console detection
static Request *request=0;


class SAPI_Info : public PA_Allocated {
public:
	int http_response_code;

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

	virtual void add_header_attribute(const char* dont_store_key, const char* dont_store_value) {
		if(strcasecmp(dont_store_key, HTTP_STATUS)==0)
			http_response_code=atoi(dont_store_value);
	}

	virtual void send_header() {}

	virtual size_t send_body(const void *buf, size_t size) {
		return stdout_write(buf, size);
	}

} *sapiInfo = NULL;

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

	virtual void add_header_attribute(const char* dont_store_key, const char* dont_store_value) {
		SAPI_Info::add_header_attribute(dont_store_key, dont_store_value);
		if(!request || !request->console.was_used())
			printf("%s: %s\n", capitalize(dont_store_key), dont_store_value);
	}

	virtual void send_header() {
		puts("");
	}


};

char* replace_char(char* str, char from, char to){
    for(char *pos = strchr(str,from); pos; pos=strchr(pos,from)) {
        *pos = to;
    }
    return str;
}

class SAPI_Info_HTTPD : public SAPI_Info {
public:

	HTTPD_Connection &connection;
	String::Body output;
	HashStringString env;

	SAPI_Info_HTTPD(HTTPD_Connection &aconnection) : connection(aconnection) {}

	void populate_env() {
		String::Body host("localhost");
		for(Array_iterator<HTTP_Headers::Header> i(connection.headers()); i.has_next(); ){
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

		env.put("REQUEST_METHOD", connection.method());
		env.put("REQUEST_URI", connection.uri());
		env.put("QUERY_STRING", connection.query());

		env.put("SERVER_NAME", host);
		env.put("SERVER_PORT", HTTPD_Server::port);
		env.put("REMOTE_ADDR", connection.remote_addr);

	}

	virtual char* get_env(const char* name) {
		String::Body value = env.get(name);
		return !value ? NULL : value.cstrm();
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

	virtual void add_header_attribute(const char* dont_store_key, const char* dont_store_value) {
		if(strcasecmp(dont_store_key, "location")==0)
			http_response_code=302;
		if(strcasecmp(dont_store_key, HTTP_STATUS)==0)
			http_response_code=atoi(dont_store_value);
		else
			output << capitalize(dont_store_key) << ": " << pa_strdup(dont_store_value) << "\r\n";
	}

	void clear_response_headers() {
		http_response_code=200;
		output.clear();
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

	virtual void send_header() {
		String result("HTTP/1.0 ");
		result << String::Body::Format(http_response_code) << " " << status_message(http_response_code) << "\r\n" << output << "\r\n";
		send_body(result.cstr(), result.length());
	}

	virtual size_t send_body(const void *buf, size_t size) {
		return connection.send_body(buf, size);
	}

};

#endif
