/** @file
	Parser: Stylesheet connection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_stylesheet_connection.h,v 1.18 2002/01/14 17:48:56 paf Exp $
*/

#ifndef PA_STYLESHEET_CONNECTION_H
#define PA_STYLESHEET_CONNECTION_H

#include "libxslt/libxslt.h"
#include "libxslt/xsltInternals.h"

#include "pa_pool.h"
#include "pa_stylesheet_manager.h"
#include "pa_exception.h"
#include "pa_common.h"

// defines

#define STYLESHEET_FILENAME_STAMP_SUFFIX ".stamp"

/**	Connection with stylesheet: 
	remembers time and can figure out that it needs recompilation
*/
class Stylesheet_connection : public Pooled {

public:

	Stylesheet_connection(Pool& pool, const String& afile_spec) : Pooled(pool),
		ffile_spec(afile_spec),
		time_used(0),
		prev_disk_time(0),
		fservices_pool(0),
		fstylesheet(0) {
	}
	
	const String& file_spec() { return ffile_spec; }

	void set_services(Pool *aservices_pool) {
		time_used=time(0); // they started to use at this time
		fservices_pool=aservices_pool;
	}
	bool expired(time_t older_dies) {
		return time_used<older_dies;
	}
	time_t get_time_used() { return time_used; }

	void close() {
		stylesheet_manager->close_connection(ffile_spec, *this);
	}

	void disconnect() { 
		xsltFreeStylesheet(fstylesheet);  fstylesheet=0; 
	}

	bool connected() { return fstylesheet!=0; }

	xsltStylesheet *stylesheet(bool nocache) { 
		time_t new_disk_time=get_new_disk_time();
		if(nocache || new_disk_time)
			load(new_disk_time);
		return fstylesheet; 
	}

private:

	time_t get_new_disk_time() {
		time_t now_disk_time=get_disk_time();
		return now_disk_time>prev_disk_time?now_disk_time:0;
	}

	/// @test grab errors
	void load(time_t new_disk_time) {
		Pool& pool=*fservices_pool;

		xsltStylesheet *nstylesheet;
		nstylesheet=xsltParseStylesheetFile(BAD_CAST ffile_spec.cstr(String::UL_FILE_SPEC));
		if(!nstylesheet)
			throw Exception(0, 0,
				&ffile_spec,
				"error compiling. TODO: grab errors");

		xsltFreeStylesheet(fstylesheet);  
		fstylesheet=nstylesheet;
		prev_disk_time=new_disk_time;
	}

	time_t get_disk_time() {
		size_t size;
		time_t atime, mtime, ctime;
		String stamp_file_spec(ffile_spec); 
		stamp_file_spec << STYLESHEET_FILENAME_STAMP_SUFFIX;
		// {file_spec}.stamp modification time OR {file_spec}
		const String& stat_file_spec=file_readable(stamp_file_spec)?stamp_file_spec:ffile_spec;
		file_stat(stat_file_spec, 
			size,
			atime, mtime, ctime,
			true/*exception on error*/);
		return mtime;
	}

private:

	const String& ffile_spec;
	xsltStylesheet *fstylesheet;
	time_t time_used;
	time_t prev_disk_time;

private:

	Pool *fservices_pool;

private:
	void *malloc(size_t size) { return fservices_pool->malloc(size); }
	void *calloc(size_t size) { return fservices_pool->calloc(size); }
};

#endif
