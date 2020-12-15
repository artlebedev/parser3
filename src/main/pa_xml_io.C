/** @file
	Parser: plugins to xml library, controlling i/o; implementation

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_xml_io.h"

#ifdef XML

volatile const char * IDENT_PA_XML_IO_C="$Id: pa_xml_io.C,v 1.42 2020/12/15 17:10:37 moko Exp $" IDENT_PA_XML_IO_H;

#include "libxslt/extensions.h"

#include "pa_threads.h"
#include "pa_globals.h"
#include "pa_request.h"

THREAD_LOCAL HashStringBool* xml_dependencies = NULL; // every TLS should be referenced elsewhere, or GC will collect it

static void add_dependency(const String::Body url) { 
	if(xml_dependencies) // do we need to monitor now?
		xml_dependencies->put(url, true);
}

HashStringBool* pa_xmlStartMonitoringDependencies() {
	return xml_dependencies=new HashStringBool; // to keep another reference on TLS
}

void pa_xmlStopMonitoringDependencies() { 
	xml_dependencies=NULL;
}

HashStringBool* pa_xmlGetDependencies() {
	HashStringBool* result=xml_dependencies;
	xml_dependencies=NULL;
	return result;
}

#ifndef DOXYGEN
struct MemoryStream : public PA_Allocated {
	const char* m_buf;
	size_t m_size;
	size_t m_position;

	MemoryStream(const char *a_buf) : m_buf(a_buf), m_size(strlen(m_buf)), m_position(0) {}

	int read(char* a_buffer, size_t a_size) {
		size_t left=m_size-m_position;
		if(!left)
			return 0;

		size_t to_read=min(a_size, left);
		memcpy(a_buffer, m_buf+m_position, to_read);
		m_position+=to_read;
		return to_read;
	}

};
#endif

static int xmlFileMatchMonitor(const char* /*file_spec_cstr*/) {
	return 1; // always intercept, causing xmlFileOpenMonitor to be called
}

/**
 * xmlFileOpenMonitor:
 * afilename:  the URI for matching
 *
 * http://localhost/abc -> $ENV{DOCUMENT_ROOT}/abc | ./abc
 *
 * Returns an I/O context or NULL in case of error
 */
static void *xmlFileOpenMonitor(const char* afilename) {
#ifdef PA_SAFE_MODE
//copied from libxml/catalog.c
#	define XML_XML_DEFAULT_CATALOG "file:///etc/xml/catalog"
	// disable attempts to consult default catalog [usually, that file belongs to other user/group]
	if(strcmp(afilename, XML_XML_DEFAULT_CATALOG)==0)
		return 0;
#endif

	Request& r=pa_thread_request();
	char adjust_buf[MAX_STRING];
	if(!strncmp(afilename, "http://localhost", 16)) {
		const char* document_root=r.request_info.document_root;
		if(!document_root)
			document_root=".";

		adjust_buf[0]=0;
		strcat(adjust_buf, document_root);
		strcat(adjust_buf, &afilename[16]);
		afilename=adjust_buf;
	} else
		if(!strstr(afilename, "http://")) {
			if(strstr(afilename, "file://")) {
				afilename+=7 /*strlen("file://")*/;
#ifdef WIN32
				if(afilename[0]=='/' && afilename[1] && afilename[2]==':' && afilename[3]=='/') {
					// skip leading slash for absolute path file:///C:/path/to/file
					afilename++;
				}
#endif
			} else if(afilename[0] && afilename[1]!=':' && strstr(afilename, "://")) {
				pa_xmlStopMonitoringDependencies();
				return 0; // plug out [do not handle other prefixes]
			}
		}

	const char* can_store_filename=pa_strdup(afilename);
	add_dependency(can_store_filename);

	const char *buf;
	try {
		buf=file_load_text(r, *new String(can_store_filename), 
			true /*fail_on_read_problem*/,
			0 /*params*/,
			false /*don't transcode result because it must be fit with @encoding value!*/);
	} catch(const Exception& e) {
		if(strcmp(e.type(), "file.missing")==0)
			return 0; // let the library try that and report an error properly

		buf=e.comment();
	} catch(...) {
		buf="xmlFileOpen_ReadIntoStream: unknown error";
	}
	return (void *)new MemoryStream(buf);
}

static int xmlFileMatchMethod(const char* filename) {
	return !strncmp(filename, "parser://", 9 /*strlen("parser://"), and check xmlFileOpenMethod*/);
}

/// parser://method/param/here -> ^MAIN:method[/params/here]
static void *xmlFileOpenMethod (const char* afilename) {
	const char* buf;
	try {
		Request& r=pa_thread_request();

		char* s=pa_strdup(afilename+9 /*strlen("parser://")*/);
		const char* method_cstr=lsplit(&s, '/');
		const String* method=new String(method_cstr);
		String::Body param_body("/");
		if(s)
			param_body.append_know_length(s, strlen(s));

		const String *body=r.execute_method(r.main_class, *method, new VString(*new String(param_body, String::L_TAINTED)));
		if(!body)
			throw Exception(0, new String(afilename), "'%s' method not found in %s class", method_cstr, MAIN_CLASS_NAME);
		buf=body->untaint_cstr(String::L_XML);
	} catch(const Exception& e) {
		buf=e.comment();
	} catch(...) {
		buf="xmlFileOpenLocalhost: unknown error";
	}
	return (void *)new MemoryStream(buf);
}

/**
 * pa_xmlFileReadMethod:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
static int pa_xmlFileReadMethod (void* context, char* buffer, int len){
	MemoryStream& stream=*static_cast<MemoryStream*>(context);
	return stream.read(buffer, len);
}

static int pa_xmlFileCloseMethod (void* /*context*/) {
	return 0;
}

void pa_xml_io_init() {
	// file open monitorer [for xslt cacher]
	// safe mode checker, always fail match, but checks non-"://" there
	xmlRegisterInputCallbacks(xmlFileMatchMonitor, xmlFileOpenMonitor, pa_xmlFileReadMethod, pa_xmlFileCloseMethod);

	// parser://method/param/here -> ^MAIN:method[/params/here] - should be last to be called first
	xmlRegisterInputCallbacks(xmlFileMatchMethod, xmlFileOpenMethod, pa_xmlFileReadMethod, pa_xmlFileCloseMethod);
}

#endif
