/** @file
	Parser: compiled code cache.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_CODE_CACHE_H
#define PA_CODE_CACHE_H

#define IDENT_PA_CODE_CACHE_H "$Id: pa_code_cache.h,v 1.1 2026/10/05 17:50:18 moko Exp $"

#include "pa_request.h"

/// httpd mode: file compilations while the config runs are recorded and replayed in the subsequent requests
#define CODE_CACHE

#ifdef CODE_CACHE

/// compilation recording, includes cached methods and all class modifications
class Code_cache: public PA_Object {
public:
	enum Kind { CLASS, USE, BASE, OPTION, METHOD, MAIN };

	struct Action {
		Kind kind;
		const String* name; // class, base class, used file, option or method name
		Method* method;
		Operation::Origin origin; // of @USE
	};

	Array<Action> actions;
	const String& file_spec;
	uint file_no; // index in request.file_list

	Code_cache(const String& afile_spec);
	void add(Kind kind, const String* name=0, Method* method=0);
	void add_use(const String* name, Operation::Origin origin);
	bool unchanged();

	/// replays the compilation, see compile.C
	ArrayClass& replay(Request& r, VStateless_class& aclass);

private:
	uint64_t fsize;
	time_t fmtime;
};

class Code_cache_manager {
public:
	/// the httpd config runs between these
	static void start();
	static void finish(Request& r);

	/// $MAIN:HTTPD.code-cache, taken while the config runs
	static void set_enabled(bool aenabled);

	/// so that the file numbers in the cached code are valid in the request
	static void add_files(Array<String::Body>& file_list);

	/// if code is cached and unchanged
	static Code_cache* get(const String& file_spec);

	/// while the config runs: where to record the file compilation
	static Code_cache* create(const String& file_spec);
	/// after the file is compiled
	static void put(Code_cache& code);
};

#endif

#endif
