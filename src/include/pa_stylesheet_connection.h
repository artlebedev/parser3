/** @file
	Parser: Stylesheet connection decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_stylesheet_connection.h,v 1.25 2002/02/08 07:27:45 paf Exp $
*/

#ifndef PA_STYLESHEET_CONNECTION_H
#define PA_STYLESHEET_CONNECTION_H

#include "libxslt/xslt.h"
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

	friend class Stylesheet_connection_ptr;

public:

	Stylesheet_connection(Pool& pool, const String& afile_spec) : Pooled(pool),
		ffile_spec(afile_spec),
		prev_disk_time(0),
		fservices_pool(0),
		fstylesheet(0),
		time_used(0), used(0) {
	}
	
	const String& file_spec() { return ffile_spec; }

	void set_services(Pool *aservices_pool) {
		fservices_pool=aservices_pool;
	}
	bool expired(time_t older_dies) {
		return !used && time_used<older_dies;
	}
	time_t get_time_used() { return time_used; }

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

	/// return to cache
	void close() {
		stylesheet_manager->close_connection(ffile_spec, *this);
	}

private:

	time_t get_new_disk_time() {
		time_t now_disk_time=get_disk_time();
		return now_disk_time>prev_disk_time?now_disk_time:0;
	}

	void load(time_t new_disk_time) {
		Pool& pool=*fservices_pool;

		xsltStylesheet *nstylesheet=
			xsltParseStylesheetFile(BAD_CAST ffile_spec.cstr(String::UL_FILE_SPEC));
		if(!nstylesheet || xmlHaveGenericErrors()) {
			GdomeException exc=0;
			throw Exception(0, 0,
				&ffile_spec,
				exc);
		}

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

private: // connection usage methods

	void use() {
		time_used=time(0); // they started to use at this time
		used++;
	}
	void unuse() {
		used--;
		if(!used)
			close();
	}

private: // connection usage data

	int used;

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

/// Auto-object used to track Stylesheet_connection usage
class Stylesheet_connection_ptr {
	Stylesheet_connection *fconnection;
public:
	explicit Stylesheet_connection_ptr(Stylesheet_connection *aconnection) : 
		fconnection(aconnection) {
		fconnection->use();
	}
	~Stylesheet_connection_ptr() {
		fconnection->unuse();
	}
	Stylesheet_connection* operator->() {
		return fconnection;
	}

	// copying
	Stylesheet_connection_ptr(const Stylesheet_connection_ptr& src) : fconnection(src.fconnection) {
		fconnection->use();
	}
	Stylesheet_connection_ptr& operator =(const Stylesheet_connection_ptr& src) {
		// may do without this=src check
		fconnection->unuse();
		fconnection=src.fconnection;
		fconnection->use();

		return *this;
	}
};

#endif
