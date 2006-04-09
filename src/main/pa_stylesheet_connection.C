/** @file
	Parser: Stylesheet connection implementation.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"
#ifdef XML
static const char * const IDENT_STYLESHEET_CONNECTION_C="$Date: 2006/04/09 13:38:47 $";

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
	if(xmlHaveGenericErrors())
		throw XmlException(new String(ffile_spec, String::L_TAINTED));
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
	dependencies->for_each<time_t*>(update_max_mtime, &result);
	return result;
}

#endif
