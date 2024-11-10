/** @file
	Parser: web server api interface object decl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_SAPI_H
#define PA_SAPI_H

#define IDENT_PA_SAPI_H "$Id: pa_sapi.h,v 1.41 2024/11/10 00:28:42 moko Exp $"

// includes

#include "pa_common.h"
#include "pa_array.h"
#include "pa_exception.h"

// forwards
class SAPI_Info;

/// target web-Server API
struct SAPI {
	/// log error message
	static void log(SAPI_Info& info, const char* fmt, ...);
	/// log error message & exit
	static void die(const char* fmt, ...);
	/// read POST request bytes
	static size_t read_post(SAPI_Info& info, char *buf, size_t max_bytes);
	/// add response header attribute [but do not send it to client]
	static void add_header_attribute(SAPI_Info& info, const char* dont_store_key, const char* dont_store_value);
	/// send collected header attributes to client
	static void send_headers(SAPI_Info& info);
	/// clear collected header attributes
	static void clear_headers(SAPI_Info& info);
	/// output body bytes
	static size_t send_body(SAPI_Info& info, const void *buf, size_t size);
	// send error to client
	static void send_error(SAPI_Info& info, const char *exception_cstr, const char *status = "500");

	class Env {
	public:
		/// entire environment
		static const char* const* get(SAPI_Info& ainfo);
		/// single environment string
		static char* get(SAPI_Info& ainfo, const char* name);
		static bool set(SAPI_Info& ainfo, const char* name, const char* value);

		class Iterator {
		private:
			const char* const* pairs;
			const char* pair;
			const char* eq_at;
		public:
			Iterator(SAPI_Info& asapi_info) : pair(NULL), eq_at(NULL){
				if(pairs=SAPI::Env::get(asapi_info))
					next();
			}
			operator bool () {
				return pair!=0;
			}
			void next() {
				while(pair=*pairs++)
					if(eq_at=strchr(pair, '=')) // valid pair (key=value)
						if(eq_at[1]) // value is not empty
							break;
			}
			char* key(){
				return pa_strdup(pair, eq_at-pair);
			}
			char* value(){
				return pa_strdup(eq_at+1);
			}
		};
	};
};

#endif
