/** @file
	Parser: plugins to xml library, controlling i/o; implementation

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_xml_io.h"

#ifdef XML

static const char * const IDENT="$Date: 2003/11/28 10:41:11 $";

#include "libxslt/extensions.h"

#include "pa_globals.h"
#include "pa_request.h"

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
		memcpy(a_buffer, m_buf, to_read);
		m_position+=to_read;
		return to_read;
	}

};
#endif


#ifdef PA_SAFE_MODE
static int
xmlFileMatchSafeMode(const char* file_spec_cstr) {
	if(strstr(filename, "://")) {
		String* file_spec=new String(file_spec_cstr, true);
		struct stat finfo;
		if(stat(file_spec_cstr, &finfo)!=0)
			throw Exception("file.missing", 
				file_spec, 
				"stat failed: %s (%d)", 
					strerror(errno), errno);

		check_safe_mode(finfo, file_spec, file_spec_cstr);
	}
	return 0;
}
#endif


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
 * xmlFileOpenHttpLocalhost :
 * filename:  the URI for matching
 *
 * http://localhost/abc -> $ENV{DOCUMENT_ROOT}/abc | ./abc
 *
 * input from FILE *, supports compressed input
 * if filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlFileOpenLocalhost (const char* filename) {
	Request& r=pa_thread_request();
	const char* document_root=r.request_info.document_root;
	if(!document_root)
		document_root=".";

	char path[MAX_STRING];	
	path[0]=0;
	strcat(path, document_root);
	strcat(path, &filename[16]);

	MemoryStream *stream=new(UseGC) MemoryStream;
	stream->m_buf=file_read_text(r.charsets, *new String(filename), true);
	stream->m_size=strlen(stream->m_buf);

	return (void *)stream;
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
			MemoryStream *stream=new(UseGC) MemoryStream;
			stream->m_buf=body.string->cstr(String::L_UNSPECIFIED);
			stream->m_size=strlen(stream->m_buf);
			return (void*)stream;
		}
	}

	throw Exception(0,
		new String(afilename),
		"'%s' method not found in %s class", 
			method_cstr, MAIN_CLASS_NAME);
}

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
#ifdef PA_SAFE_MODE
	// safe mode checker, always fail match, but checks non-"://" there
	xmlRegisterInputCallbacks(
		xmlFileMatchSafeMode, 0,
		0, 0);
#endif
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
