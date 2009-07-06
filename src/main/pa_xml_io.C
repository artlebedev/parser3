/** @file
	Parser: plugins to xml library, controlling i/o; implementation

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_xml_io.h"

#ifdef XML

static const char * const IDENT="$Date: 2009/07/06 12:07:04 $";

#include "libxslt/extensions.h"

#include "pa_threads.h"
#include "pa_globals.h"
#include "pa_request.h"

static Hash<pa_thread_t, HashStringBool*> xml_dependencies;

static void add_dependency(const String::Body url) { 
	pa_thread_t thread_id=pa_get_thread_id();
	HashStringBool* urls;
	{
		SYNCHRONIZED;

		// try to get existing for this thread_id
		urls=xml_dependencies.get(thread_id);
	}

	if(urls) // do we need to monitor now?
		urls->put(url, true);
}

void pa_xmlStartMonitoringDependencies() { 
	pa_thread_t thread_id=pa_get_thread_id();
	HashStringBool* urls=new HashStringBool;
	{
		SYNCHRONIZED;  // find+fill blocked

		xml_dependencies.put(thread_id, urls);
	}
}

void pa_xmlStopMonitoringDependencies() { 
	pa_thread_t thread_id=pa_get_thread_id();
	{
		SYNCHRONIZED;  // find+fill blocked

		xml_dependencies.put(thread_id, 0);
	}
}

HashStringBool* pa_xmlGetDependencies() {
	pa_thread_t thread_id=pa_get_thread_id();
	{
		SYNCHRONIZED;  // find+remove blocked

		HashStringBool* result=xml_dependencies.get(thread_id);
		xml_dependencies.remove(thread_id);
		return result;
	}
}

#ifndef DOXYGEN
struct MemoryStream {
	const char* m_buf;
	size_t m_size;
	size_t m_position;

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

static void *
xmlFileOpen_ReadIntoStream (const char* do_not_store_filename, bool adjust_path_to_root_from_document_root=false) {
#ifdef PA_SAFE_MODE
//copied from libxml/catalog.c
#	define XML_XML_DEFAULT_CATALOG "file:///etc/xml/catalog"
	// disable attempts to consult default catalog [usually, that file belongs to other user/group]
	if(strcmp(do_not_store_filename, XML_XML_DEFAULT_CATALOG)==0)
		return 0;
#endif

	Request& r=pa_thread_request();
	char adjust_buf[MAX_STRING];	
	if(adjust_path_to_root_from_document_root) {
		const char* document_root=r.request_info.document_root;
		if(!document_root)
			document_root=".";

		adjust_buf[0]=0;
		strcat(adjust_buf, document_root);
		strcat(adjust_buf, &do_not_store_filename[16]);
		do_not_store_filename=adjust_buf;
	} else
		if(!strstr(do_not_store_filename, "http://"))
			if(strstr(do_not_store_filename, "file://"))
				do_not_store_filename+=7/*strlen("file://")*/; 
			else if(*do_not_store_filename && do_not_store_filename[1]!=':' && strstr(do_not_store_filename, "://"))  {
				pa_xmlStopMonitoringDependencies();
				return 0; // plug out [do not handle other prefixes]
			}

	const char* can_store_filename=pa_strdup(do_not_store_filename);
	add_dependency(can_store_filename);

	const char *buf;
	try {
		buf=file_load_text(r, *new String(can_store_filename), 
							true/*fail_on_read_problem*/,
							0/*params*/,
							false/*don't transcode result because it must be fit with @encoding value!*/);
	} catch(const Exception& e) {
		if(strcmp(e.type(), "file.missing")==0)
			return 0; // let the library try that and report an error properly

		buf=e.comment();
	} catch(...) {
		buf="xmlFileOpen_ReadIntoStream: unknown error";
	}
	MemoryStream* stream=new(UseGC) MemoryStream;
	stream->m_buf=buf;
	stream->m_size=strlen(buf);
	return (void *)stream;
}

static int
xmlFileMatchMonitor(const char* /*file_spec_cstr*/) {
	return 1; // always intercept, causing xmlFileOpenMonitor to be called
}
static void *
xmlFileOpenMonitor(const char* filename) {
	return xmlFileOpen_ReadIntoStream(filename); // handles localfile case, else returns 0
}


/**
 * xmlFileMatchWithLocalhostEqDocumentRoot:
 * filename:  the URI for matching
 *
 * check if the URI matches an HTTP one
 *
 * Returns 1 if matches, 0 otherwise
 */
static int
xmlFileMatchLocalhost(const char* filename) {
	if (!strncmp(filename, "http://localhost", 16))
		return(1);
	return(0);
}

/**
 * xmlFileOpenLocalhost:
 * filename:  the URI for matching
 *
 * http://localhost/abc -> $ENV{DOCUMENT_ROOT}/abc | ./abc
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlFileOpenLocalhost (const char* filename) {
	return xmlFileOpen_ReadIntoStream(filename, true/*adjust path to root from document_root*/);
}

static int
xmlFileMatchMethod(const char* filename) {
	if (!strncmp(filename, "parser://", 9 /*strlen("parser://"), and check xmlFileOpenMethod*/))
		return(1);
	return(0);
}

/// parser://method/param/here -> ^MAIN:method[/params/here]
static void *
xmlFileOpenMethod (const char* afilename) {
	const char* buf;
	try {
		Request& r=pa_thread_request();

		char* s=pa_strdup(afilename+9 /*strlen("parser://")*/);
		const char* method_cstr=lsplit(&s, '/');
		const String* method=new String(method_cstr);
		String::Body param_body("/");  
		if(s)
			param_body.append_know_length(s, strlen(s));

		VString* vparam=new VString(*new String(param_body, String::L_TAINTED));
		{
			Temp_lang temp_lang(r, String::L_XML); // default language: XML
			Request::Execute_nonvirtual_method_result body=
				r.execute_nonvirtual_method(r.main_class, *method, vparam, true);
			if(body.string) {
				buf=body.string->untaint_cstr(String::L_AS_IS);
			} else
				throw Exception(0,
					new String(afilename),
					"'%s' method not found in %s class", 
							method_cstr, MAIN_CLASS_NAME);
		}
	} catch(const Exception& e) {
		buf=e.comment();
	} catch(...) {
		buf="xmlFileOpenLocalhost: unknown error";
	}
	MemoryStream* stream=new(UseGC) MemoryStream;
	stream->m_buf=buf;
	stream->m_size=strlen(buf);
	return (void *)stream;
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
static int
pa_xmlFileReadMethod (void * context, //< MemoryStream actually
					  char * buffer, int len) 
{
	MemoryStream& stream=*static_cast<MemoryStream*>(context);

	return stream.read(buffer, len);
}

static int
pa_xmlFileCloseMethod (void * /*context*/) {
	return 0;
}



void pa_xml_io_init() {
	// file open monitorer [for xslt cacher]
	// safe mode checker, always fail match, but checks non-"://" there
	xmlRegisterInputCallbacks(
		xmlFileMatchMonitor, xmlFileOpenMonitor,
		pa_xmlFileReadMethod, pa_xmlFileCloseMethod);

	// http://localhost/abc -> $ENV{DOCUMENT_ROOT}/abc | ./abc
	xmlRegisterInputCallbacks(
		xmlFileMatchLocalhost, xmlFileOpenLocalhost,
		pa_xmlFileReadMethod, pa_xmlFileCloseMethod);

	// parser://method/param/here -> ^MAIN:method[/params/here]
	xmlRegisterInputCallbacks(
		xmlFileMatchMethod, xmlFileOpenMethod,
		pa_xmlFileReadMethod, pa_xmlFileCloseMethod);
}

#endif
