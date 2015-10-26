/** @file
	Parser: web server api interface object decl.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SAPI_H
#define PA_SAPI_H

#define IDENT_PA_SAPI_H "$Id: pa_sapi.h,v 1.31 2015/10/26 01:21:56 moko Exp $"

// includes


#include "pa_types.h"
#include "pa_array.h"

// forwards
class SAPI_Info;

/// target web-Server API
struct SAPI {
	/// log error message
	static void log(SAPI_Info& info, const char* fmt, ...);
	/// log error message & exit
	static void die(const char* fmt, ...);
	/// log error message & abort[write core]
	static void abort(const char* fmt, ...);
	/// read POST request bytes
	static size_t read_post(SAPI_Info& info, char *buf, size_t max_bytes);
	/// add response header attribute [but do not send it to client]
	static void add_header_attribute(SAPI_Info& info, const char* dont_store_key, const char* dont_store_value);
	/// send collected header attributes to client
	static void send_header(SAPI_Info& info);
	/// output body bytes
	static size_t send_body(SAPI_Info& info, const void *buf, size_t size);

	class Env {
	public:
		/// entire environment
		static const char* const* get(SAPI_Info& ainfo);
		/// single environment string
		static char* get(SAPI_Info& ainfo, const char* name);

		class Iterator {
		private:
			const char* const* pairs;
			const char* pair;
			const char* eq_at;
		public:
			Iterator(SAPI_Info& asapi_info) {
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
