/** @file
	Parser: Stylesheet connection decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_STYLESHEET_CONNECTION_H
#define PA_STYLESHEET_CONNECTION_H

static const char* IDENT_STYLESHEET_CONNECTION_H="$Date: 2003/11/06 09:11:35 $";

#include "libxslt/xslt.h"
#include "libxslt/xsltInternals.h"


#include "pa_xml_exception.h"
#include "pa_common.h"
#include "pa_globals.h"

// defines

#define STYLESHEET_FILENAME_STAMP_SUFFIX ".stamp"

/**	Connection with stylesheet: 
	remembers time and can figure out that it needs recompilation
*/
class Stylesheet_connection: public PA_Object {

	friend class Stylesheet_connection_ptr;

private:

	const String& ffile_spec;
	xsltStylesheet *fstylesheet;
	time_t time_used;
	time_t prev_disk_time;

public:

	Stylesheet_connection(const String& afile_spec):
		ffile_spec(afile_spec),
		prev_disk_time(0),
		fstylesheet(0),
		time_used(0), used(0)  {}
	
	const String& file_spec() { return ffile_spec; }

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
	void close();

private:

	time_t get_new_disk_time() {
		time_t now_disk_time=get_disk_time();
		return now_disk_time>prev_disk_time?now_disk_time:0;
	}

	void load(time_t new_disk_time) {
		int saved=xmlDoValidityCheckingDefaultValue;//
		xmlDoValidityCheckingDefaultValue=0;//
		xsltStylesheet *nstylesheet=
			xsltParseStylesheetFile(BAD_CAST ffile_spec.cstr(String::L_FILE_SPEC));
		xmlDoValidityCheckingDefaultValue = saved;//
		if(xmlHaveGenericErrors()) {
			GdomeException exc=0;
			throw XmlException(&ffile_spec, exc);
		}
		if(!nstylesheet)
			throw Exception("file.missing",
				&ffile_spec,
				"stylesheet failed to load");

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

	int used;
	void use() {
		time_used=time(0); // they started to use at this time
		used++;
	}
	void unuse() {
		used--;
		if(!used)
			close();
	}

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
/*	Stylesheet_connection& operator*() {
		return *fconnection;
	}*/

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
