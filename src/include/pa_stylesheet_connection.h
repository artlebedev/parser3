/** @file
	Parser: Stylesheet connection decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_STYLESHEET_CONNECTION_H
#define PA_STYLESHEET_CONNECTION_H

#define IDENT_PA_STYLESHEET_CONNECTION_H "$Id: pa_stylesheet_connection.h,v 1.48 2020/12/24 07:18:42 moko Exp $"

#include "libxslt/xslt.h"
#include "libxslt/xsltInternals.h"


#include "pa_xml_exception.h"
#include "pa_common.h"
#include "pa_globals.h"
#include "pa_xml_io.h"

// defines

/**	Connection with stylesheet:
	remembers time and can figure out that it needs recompilation
*/
class Stylesheet_connection: public PA_Object {

	friend class Stylesheet_connection_ptr;

private:

	String::Body ffile_spec;
	xsltStylesheet *fstylesheet;
	HashStringBool* dependencies;
	time_t time_used;
	time_t prev_disk_time;

public:

	Stylesheet_connection(String::Body afile_spec):
		ffile_spec(afile_spec),
		fstylesheet(0),
		dependencies(0),
		time_used(0),
		prev_disk_time(0),
		used(0)
	{}

	String::Body file_spec() { return ffile_spec; }

	bool uncachable() {
		return !dependencies /*means they were external*/;
	}

	bool expired(time_t older_dies) {
		return uncachable() || (!used && time_used<older_dies);
	}
	time_t get_time_used() { return time_used; }

	void disconnect() {
		fstylesheet=0;
	}

	bool connected() { return fstylesheet!=0; }

	xsltStylesheet *stylesheet() {
		time_t new_disk_time=0;
		if(uncachable() || (new_disk_time=get_new_disk_time()))
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

	void load(time_t new_disk_time);
	time_t get_disk_time();

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
	Stylesheet_connection_ptr(Stylesheet_connection *aconnection) : fconnection(aconnection) {
		fconnection->use();
	}
	~Stylesheet_connection_ptr() {
		fconnection->unuse();
	}
	Stylesheet_connection* operator->() {
		return fconnection;
	}
};

#endif
