/** @file
	Parser: Charset connection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_charset_connection.h,v 1.2 2001/10/22 16:44:42 parser Exp $
*/

#ifndef PA_CHARSET_CONNECTION_H
#define PA_CHARSET_CONNECTION_H

#include "pcre.h"
#	include "internal.h"

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_common.h"

// defines

/**	Connection with charset file: 
	remembers time and can figure out that it needs recompilation
*/
class Charset_connection : public Pooled {
public:

	Charset_connection(Pool& pool, const String& aname, const String& afile_spec) : Pooled(pool),
		fname(aname),
		ffile_spec(afile_spec),
		prev_disk_time(0) {
	}
	~Charset_connection() {}
	
	void up_to_date(Pool& pool) { 
		time_t new_disk_time=get_new_disk_time();
		if(new_disk_time)
			load(pool, new_disk_time);
	}

	unsigned char *pcre_tables() { return fpcre_tables; }

private:

	time_t get_new_disk_time() {
		time_t now_disk_time=get_disk_time();
		return now_disk_time>prev_disk_time?now_disk_time:0;
	}

	void load(Pool& pool, time_t new_disk_time);
	
	time_t get_disk_time() {
		size_t size;
		time_t atime, mtime, ctime;
		// {file_spec} modification time
		file_stat(ffile_spec, 
			size,
			atime, mtime, ctime,
			true/*exception on error*/);
		return mtime;
	}

private:

	const String& fname;
	const String& ffile_spec;
	time_t prev_disk_time;

	unsigned char fpcre_tables[tables_length];
};

#endif
