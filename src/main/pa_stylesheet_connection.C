/** @file
	Parser: Stylesheet connection implementation.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"
#ifdef XML

#include "pa_stylesheet_connection.h"
#include "pa_xml_exception.h"

volatile const char * IDENT_PA_STYLESHEET_CONNECTION_C="$Id: pa_stylesheet_connection.C,v 1.13 2020/12/15 17:10:37 moko Exp $" IDENT_PA_STYLESHEET_CONNECTION_H;

void Stylesheet_connection::load(time_t new_disk_time) {
	xsltStylesheet *nstylesheet;
	{
		dependencies=pa_xmlStartMonitoringDependencies(); // just to reference TLS variable
		{
			int saved=xmlDoValidityCheckingDefaultValue;
			xmlDoValidityCheckingDefaultValue=0;
			nstylesheet=xsltParseStylesheetFile(BAD_CAST ffile_spec.cstr());
			xmlDoValidityCheckingDefaultValue = saved;
		}
		dependencies=pa_xmlGetDependencies();
	}
	if(xmlHaveGenericErrors())
		throw XmlException(new String(ffile_spec, String::L_TAINTED), pa_thread_request());
	if(!nstylesheet)
		throw Exception("file.missing", new String(ffile_spec, String::L_TAINTED), "stylesheet failed to load");

	xsltFreeStylesheet(fstylesheet);
	fstylesheet=nstylesheet;
	prev_disk_time=new_disk_time; // note: 0 for the first fime, cache works only from second hit
}

static void update_max_mtime(HashStringValue::key_type stat_file_spec_cstr, bool /*value*/, time_t* max) {
	uint64_t size;
	time_t atime, mtime, ctime;
	file_stat(*new String(stat_file_spec_cstr, String::L_AS_IS), size, atime, mtime, ctime, true/*exception on error*/);

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
