/** @file
	Parser: Stylesheet connection implementation.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_STYLESHEET_CONNECTION_C="$Date: 2004/02/13 14:06:02 $";

#include "pa_config_includes.h"
#include "pa_stylesheet_connection.h"
#include "pa_xml_exception.h"

void Stylesheet_connection::load(time_t new_disk_time) {
	xsltStylesheet *nstylesheet;
	{
		pa_xmlStartMonitoringDependencies();
		{
			int saved=xmlDoValidityCheckingDefaultValue;//
			xmlDoValidityCheckingDefaultValue=0;//
			nstylesheet=xsltParseStylesheetFile(BAD_CAST ffile_spec.cstr());
			xmlDoValidityCheckingDefaultValue = saved;//
		}
		dependencies=pa_xmlGetDependencies();
	}
	if(xmlHaveGenericErrors()) {
		GdomeException exc=0;
		throw XmlException(new String(ffile_spec, String::L_TAINTED), exc);
	}
	if(!nstylesheet)
		throw Exception("file.missing",
			new String(ffile_spec, String::L_TAINTED),
			"stylesheet failed to load");

	xsltFreeStylesheet(fstylesheet);  
	fstylesheet=nstylesheet;
	prev_disk_time=new_disk_time;
}

static void update_max_mtime(const String::Body stat_file_spec_cstr, bool /*value*/, time_t* max) {
	size_t size;
	time_t atime, mtime, ctime;
	file_stat(*new String(stat_file_spec_cstr, String::L_AS_IS), 
		size,
		atime, mtime, ctime,
		true/*exception on error*/);

	if(mtime>*max)
		*max=mtime;
}

time_t Stylesheet_connection::get_disk_time() {
	assert(dependencies);

	time_t result=0;
	dependencies->for_each(update_max_mtime, &result);
	return result;
}
