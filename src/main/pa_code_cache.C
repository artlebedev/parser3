/** @file
	Parser: compiled code cache.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#include "pa_code_cache.h"

volatile const char * IDENT_PA_CODE_CACHE_C="$Id: pa_code_cache.C,v 1.1 2026/10/05 17:50:18 moko Exp $" IDENT_PA_CODE_CACHE_H;

#ifdef CODE_CACHE

Code_cache::Code_cache(const String& afile_spec): file_spec(afile_spec), file_no(0), fsize(0), fmtime(0) {
	time_t atime, ctime;
	file_stat(file_spec, fsize, atime, fmtime, ctime, false);
}

void Code_cache::add(Kind kind, const String* name, Method* method) {
	Action action={kind, name, method, Operation::Origin::create(0, 0, 0)};
	actions+=action;
}

void Code_cache::add_use(const String* name, Operation::Origin origin) {
	Action action={USE, name, 0, origin};
	actions+=action;
}

bool Code_cache::unchanged() {
	uint64_t size;
	time_t atime, mtime, ctime;
	return file_stat(file_spec, size, atime, mtime, ctime, false) && size==fsize && mtime==fmtime;
}

// filled while the httpd config runs, only read in the subsequent requests
static HashString<Code_cache*>* code_cache=0;
// file_list of the config run
static Array<String::Body>* code_cache_files=0;

static bool code_cache_recording=false;
static bool code_cache_enabled=true;

void Code_cache_manager::start() {
	code_cache=new HashString<Code_cache*>;
	code_cache_recording=true;
}

void Code_cache_manager::finish(Request& r) {
	code_cache_recording=false;
	if(code_cache_enabled) {
		code_cache_files=new Array<String::Body>;
		code_cache_files->append(r.file_list);
	} else
		code_cache=0;
}

void Code_cache_manager::set_enabled(bool aenabled) {
	if(code_cache_recording)
		code_cache_enabled=aenabled;
}

void Code_cache_manager::add_files(Array<String::Body>& file_list) {
	if(code_cache_files) // the request has only the pseudo files yet, the same as in the config run
		file_list.append(*code_cache_files, file_list.count());
}

Code_cache* Code_cache_manager::get(const String& file_spec) {
	if(code_cache && !code_cache_recording)
		if(Code_cache* code=code_cache->get(file_spec))
			if(code->unchanged())
				return code;
	return 0;
}

Code_cache* Code_cache_manager::create(const String& file_spec) {
	// size and time are taken before reading
	return code_cache_recording ? new Code_cache(file_spec) : 0;
}

void Code_cache_manager::put(Code_cache& code) {
	code_cache->put(code.file_spec, &code);
}

#endif
