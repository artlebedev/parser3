/** @file
	Parser: Stylesheet connection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_stylesheet_connection.h,v 1.14 2001/10/22 16:44:42 parser Exp $
*/

#ifndef PA_STYLESHEET_CONNECTION_H
#define PA_STYLESHEET_CONNECTION_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_stylesheet_manager.h"
#include "pa_exception.h"
#include "pa_common.h"

#include "XalanTransformer2.hpp"

// defines

#define STYLESHEET_FILENAME_STAMP_SUFFIX ".stamp"

/**	Connection with stylesheet: 
	remembers time and can figure out that it needs recompilation
*/
class Stylesheet_connection : public Pooled {

public:

	Stylesheet_connection(Pool& pool, const String& afile_spec) : Pooled(pool),
		ftransformer(new XalanTransformer2),

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

	void close() {
		stylesheet_manager->close_connection(ffile_spec, *this);
	}

	void disconnect() { 
		/*ignore error*/ftransformer->destroyStylesheet(fstylesheet);  fstylesheet=0; 

		// connection effectively useless now, free up some memory
		delete ftransformer; 
	}

	bool connected() { return fstylesheet!=0; }

	const XalanCompiledStylesheet& stylesheet(bool nocache) { 
		time_t new_disk_time=get_new_disk_time();
		if(nocache || new_disk_time)
			load(new_disk_time);
		return *fstylesheet; 
	}

private:

	time_t get_new_disk_time() {
		time_t now_disk_time=get_disk_time();
		return now_disk_time>prev_disk_time?now_disk_time:0;
	}

	void load(time_t new_disk_time) {
		Pool& pool=*fservices_pool;

		try{
			const XalanCompiledStylesheet *nstylesheet;
			ftransformer->compileStylesheet2(ffile_spec.cstr(String::UL_FILE_SPEC), nstylesheet);
			ftransformer->destroyStylesheet(fstylesheet);  
			fstylesheet=nstylesheet;
		}
		catch (XSLException& e)	{
			Exception::provide_source(pool, &ffile_spec, e);
		}
		catch (SAXParseException& e)	{
			Exception::provide_source(pool, &ffile_spec, e);
		}
		catch (SAXException& e)	{
			Exception::provide_source(pool, &ffile_spec, e);
		}
		catch (XMLException& e) {
			Exception::provide_source(pool, &ffile_spec, e);
		}
		catch(const XalanDOMException&	e)	{
			Exception::provide_source(pool, &ffile_spec, e);
		}

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
	const XalanCompiledStylesheet *fstylesheet;
	time_t time_used;
	time_t prev_disk_time;

private:

	XalanTransformer2 *ftransformer;

	Pool *fservices_pool;

private:
	void *malloc(size_t size) { return fservices_pool->malloc(size); }
	void *calloc(size_t size) { return fservices_pool->calloc(size); }
};

#endif
