/** @file
	Parser: globals.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_GLOBALS_C="$Date: 2003/11/20 15:35:31 $";

#include "pa_config_includes.h"

#ifdef XML
#include "libxml/xmlversion.h"
#include "libxslt/extensions.h"
#include "libxslt/xsltutils.h"
extern "C" {
#include "libexslt/exslt.h"
};
#endif

#include "pcre.h"

#include "pa_globals.h"
#include "pa_string.h"
#include "pa_sapi.h"
#include "pa_threads.h"


// defines

//#define PA_DEBUG_XML_GC_MEMORY

// globals

short hex_value[0x100];

#ifdef XML
GdomeDOMImplementation *domimpl;
#endif

static void setup_hex_value() {
	memset(hex_value, 0, sizeof(hex_value));
	hex_value['0'] = 0;	
	hex_value['1'] = 1;	
	hex_value['2'] = 2;	
	hex_value['3'] = 3;	
	hex_value['4'] = 4;	
	hex_value['5'] = 5;	
	hex_value['6'] = 6;	
	hex_value['7'] = 7;	
	hex_value['8'] = 8;	
	hex_value['9'] = 9;
	hex_value['A'] = 10;
	hex_value['B'] = 11;
	hex_value['C'] = 12;
	hex_value['D'] = 13;
	hex_value['E'] = 14;
	hex_value['F'] = 15;
	hex_value['a'] = 10;
	hex_value['b'] = 11;
	hex_value['c'] = 12;
	hex_value['d'] = 13;
	hex_value['e'] = 14;
	hex_value['f'] = 15;
}

#ifdef XML

const int MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS=10;

class XML_Generic_error_info {
public:
	pa_thread_t thread_id;
	char buf[MAX_STRING];
	size_t used;
public:
	XML_Generic_error_info() {
		reset();
	}
	void reset() { 
		thread_id=0; 
		buf[used=0]=0;
	}
	const char* get_and_reset() {
		char* result=new(PointerFreeGC) char[used+1];
		memcpy(result, buf, used+1);
		reset();
		return result;
	}
} xml_generic_error_infos[MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS];

XML_Generic_error_info *xml_generic_error_info(pa_thread_t thread_id) {
	for(int i=0; i<MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS; i++) {
		XML_Generic_error_info *p=xml_generic_error_infos+i;
		if(p->thread_id==thread_id)
			return p;
	}
	return 0;
}

static void
xmlParserGenericErrorFunc(void *  /*ctx*/, const char* msg, ...) { 
	//_asm int 3;
	pa_thread_t thread_id=pa_get_thread_id();

	// infinitely looking for free slot to fill it
	while(true) {
		SYNCHRONIZED;  // find+fill blocked

		// first try to get existing for this thread_id
		XML_Generic_error_info *p=xml_generic_error_info(thread_id);
		if(!p) { // occupy empty one
			p=xml_generic_error_info(0);
			if(!p) // wait for empty one to appear
				continue;
		}

		p->thread_id=thread_id;
		
		va_list args;
		va_start(args, msg);
		p->used+=vsnprintf(p->buf+p->used, sizeof(p->buf)-p->used, msg, args);
		va_end(args);

		break;
	}
}

bool xmlHaveGenericErrors() {
	pa_thread_t thread_id=pa_get_thread_id();

	SYNCHRONIZED;  // find blocked

	return xml_generic_error_info(thread_id)!=0;
}

const char* xmlGenericErrors() {
	pa_thread_t thread_id=pa_get_thread_id();

	SYNCHRONIZED;  // find+free blocked

	XML_Generic_error_info *p=xml_generic_error_info(thread_id);
	if(!p) // no errors for our thread_id registered
		return 0;

	return p->get_and_reset();
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
	//_asm int 3;
	FILE *fd;
	const char* documentRoot;
	char path[1000];
	
	path[0]=0;
	strcat(path, (documentRoot=getenv("DOCUMENT_ROOT"))?documentRoot:".");
	strcat(path, &filename[16]);
	
#ifdef WIN32
	fd = fopen(path, "rb");
#else
	fd = fopen(path, "r");
#endif /* WIN32 */
	return((void *) fd);
}

/**
 * xmlFileRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
pa_xmlFileRead (void * context, char * buffer, int len) {
	return(fread(&buffer[0], 1,  len, (FILE *) context));
}

/**
 * xmlFileClose:
 * @context:  the I/O context
 *
 * Close an I/O channel
 */
static int
pa_xmlFileClose (void * context) {
	return ( ( fclose((FILE *) context) == EOF ) ? -1 : 0 );
}

#endif

void pa_globals_destroy(void *) {
	try {
#ifdef XML
		GdomeException exc;
		gdome_di_unref (domimpl, &exc);
		// uncomment SAPI::abort below if adding potential-throw code here
#endif
	} catch(const Exception& e) {
//		SAPI::abort("pa_globals_destroy failed: %s", e.comment());
	}
}


#ifdef XML

static char *pa_GC_strdup(const char *s) {
	if(!s)
		return 0;

	size_t size=strlen(s)+1;
	char *result=(char *)GC_malloc_atomic(size);
	if(!result)
		SAPI::abort("out of memory (while duplicating XML string [size=%d])", size);

	memcpy(result, s, size);
	return result;
}

#ifdef PA_DEBUG_XML_GC_MEMORY
void *pa_look_for[]={(void*)0x8abe000,(void*)0x0,(void*)0x0,(void*)0x0,
			(void*)0x0,(void*)0x0,(void*)0x0,(void*)0x0};
bool pa_looked(void*p) {
	for(int i=0; i<8; i++)
		if(pa_look_for[i]==p)
			return true;
	return false;
}
static void* pa_gc_malloc_log(size_t size){
	void *p=pa_gc_malloc(size);
        fprintf(stderr, "pa_gc_malloc_log(%d)=0x%p\n", size, p);
//	if(pa_looked(p))
//		fprintf(stderr,"catched debug malloc(%d)=0x%p\n", size, p);
	return p;
        
}
static void* pa_gc_malloc_atomic_log(size_t size){
	void *p=pa_gc_malloc_atomic(size);
        fprintf(stderr, "pa_gc_malloc_atomic_log(%d)=0x%p\n", size, p);
//	if(pa_looked(p))
//		fprintf(stderr,"catched debug malloc atomic(%d)=0x%p\n", size, p);
	return p;
}
static void* pa_gc_realloc_log(void *ptr, size_t size){
	void *p=pa_gc_realloc(ptr, size);
        fprintf(stderr, "pa_gc_realloc_log(0x%p, %d)=0x%p\n", ptr, size, p);
//	if(pa_looked(p))
//		fprintf(stderr,"catched debug realloc(%d)=0x%p\n", size, p);
	return p;
}
//static void pa_gc_free_ignore(void *){}
static void pa_gc_free_log(void *p){
        fprintf(stderr, "pa_gc_free_log(0x%p)\n", p);
//	if(pa_looked(p))
//		fprintf(stderr,"catched debug free(0x%p)\n", p);
        pa_gc_free(p);
}
#else

inline void *check(void *result, const char *where, size_t size) {
	if(!result)
		SAPI::abort("out of memory (while %s [size=%d])", where, size);

	return result;
}
static void* pa_gc_malloc_nonull(size_t size) { 
	return check(pa_gc_malloc(size), "allocating XML compsite memory", size);
}
static void* pa_gc_malloc_atomic_nonull(size_t size) { 
	return check(pa_gc_malloc_atomic(size), "allocating XML atomic memory", size);
}
static void* pa_gc_realloc_nonull(void* ptr, size_t size) { 
	return check(pa_gc_realloc(ptr, size), "reallocating XML memory", size);
}

#endif
#endif

void pa_CORD_oom_fn(void) {
	SAPI::abort("out of memory (while expanding string)");
}

/**
	@todo gc: libltdl: substitute lt_dlmalloc & co
*/
static void gc_substitute_memory_management_functions() {
	// in libxml & libxslt
#ifdef XML
	// asking to use GC memory
#if LIBXML_VERSION >= 20507
#ifdef PA_DEBUG_XML_GC_MEMORY
	xmlGcMemSetup(
		/*xmlFreeFunc */pa_gc_free_log,
		/*xmlMallocFunc */pa_gc_malloc_log,
		/*xmlMallocFunc */pa_gc_malloc_atomic_log,
		/*xmlReallocFunc */pa_gc_realloc_log,
		/*xmlStrdupFunc */pa_GC_strdup);
#else
	xmlGcMemSetup(
		/*xmlFreeFunc */pa_gc_free,
		/*xmlMallocFunc */pa_gc_malloc_nonull,
		/*xmlMallocFunc */pa_gc_malloc_atomic_nonull,
		/*xmlReallocFunc */pa_gc_realloc_nonull,
		/*xmlStrdupFunc */pa_GC_strdup);
#endif

#else
	xmlMemSetup(
		/*xmlFreeFunc */pa_gc_free,
		/*xmlMallocFunc */pa_gc_malloc,
		/*xmlReallocFunc */pa_gc_realloc,
		/*xmlStrdupFunc */pa_GC_strdup);
#endif

#endif

	// pcre
	pcre_malloc=pa_gc_malloc;
	pcre_free=pa_gc_free;


	// cord
	CORD_oom_fn=pa_CORD_oom_fn;
}

/**
	@test hint on one should call this for each thread xmlSubstituteEntitiesDefault(1);
*/
void pa_globals_init() {
	// in various libraries
	gc_substitute_memory_management_functions();

	// hex value
	setup_hex_value();

#ifdef XML
	// initializing xml libs

	/* First get a DOMImplementation reference */
	domimpl = gdome_di_mkref ();
	/*
	* Register the EXSLT extensions and the test module
	*/
	exsltRegisterAll();
	xsltRegisterTestModule();
	xmlDefaultSAXHandlerInit();
	/*
	* disable CDATA from being built in the document tree
	*/
	// never added yet  xmlDefaultSAXHandler.cdataBlock = NULL;
	
	/*
	 * Initialization function for the XML parser.
	 * This is not reentrant. Call once before processing in case of
	 * use in multithreaded programs.
	*/
	xmlInitParser();

	// 1. this is needed for proper parsing of stylesheets
	// there were a situation where honest entity ruined innocent xpath compilation
	// doc says "you sould turn it on on stylesheet load" without deepening into details
	// 2. when dom tree with entites goes under transform text nodes 
	// got [erroreosly] cut on first entity occurance
	// --
	// that is why this is:
	xmlSubstituteEntitiesDefault(1);
	
	// Bit in the loadsubset context field to tell to do ID/REFs lookups 
	xmlLoadExtDtdDefaultValue |= XML_DETECT_IDS;
	// Bit in the loadsubset context field to tell to do complete the elements attributes lists 
	// with the ones defaulted from the DTDs 
	xmlLoadExtDtdDefaultValue |= XML_COMPLETE_ATTRS;

	// validate each document after load/create (?)
	//xmlDoValidityCheckingDefaultValue = 1;

//regretfully this not only replaces entities on parse, but also on generate	xmlSubstituteEntitiesDefault(1);
	// never switched this on xmlIndentTreeOutput=1;

	xmlSetGenericErrorFunc(0, xmlParserGenericErrorFunc);
	xsltSetGenericErrorFunc(0, xmlParserGenericErrorFunc);
//	FILE *f=fopen("y:\\xslt.log", "wt");
//	xsltSetGenericDebugFunc(f/*stderr*/, 0);

	// http://localhost/abc -> $ENV{DOCUMENT_ROOT}/abc | ./abc
	xmlRegisterInputCallbacks(
		xmlFileMatchLocalhost, xmlFileOpenLocalhost,
		pa_xmlFileRead, pa_xmlFileClose);
#endif
}

#ifdef _MSC_VER

#ifndef PA_DEBUG_DISABLE_GC
#	define GC_LIB "../../../../win32/gc"
#	ifdef _DEBUG
#		pragma comment(lib, GC_LIB "/Debug/gc.lib")
#	else
#		pragma comment(lib, GC_LIB "/Release/gc.lib")
#	endif

#endif

#ifdef XML
#	define GNOME_LIBS "../../../../win32/gnome"
#	pragma comment(lib, GNOME_LIBS "/glib/lib/libglib-1.3-11.lib")
#	ifdef _DEBUG

#		ifdef LIBXML_STATIC
#			pragma comment(lib, GNOME_LIBS "/gnome-xml/win32/binaries-debug/libxml2_a.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/gnome-xml/win32/binaries-debug/libxml2.lib")
#		endif

#		ifdef LIBXSLT_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libxslt_DebugStatic/libxslt.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libxslt_DebugDynamic/libxslt.lib")
#		endif
#		ifdef LIBEXSLT_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libexslt_DebugStatic/libexslt.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libexslt_DebugDynamic/libexslt.lib")
#		endif

#		ifdef LIBGDOME_STATIC
#			pragma comment(lib, GNOME_LIBS "/gdome2-x.x.x/win32/dsp/libgdome_DebugStatic/libgdome.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/gdome2-x.x.x/win32/dsp/libgdome_DebugDynamic/libgdome.lib")
#		endif

#else

#		ifdef LIBXML_STATIC
#			pragma comment(lib, GNOME_LIBS "/gnome-xml/win32/binaries-release/libxml2_a.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/gnome-xml/win32/binaries-release/libxml2.lib")
#		endif

#		ifdef LIBXSLT_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libxslt_ReleaseStatic/libxslt.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libxslt_ReleaseDynamic/libxslt.lib")
#		endif
#		ifdef LIBEXSLT_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libexslt_ReleaseStatic/libexslt.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libexslt_ReleaseDynamic/libexslt.lib")
#		endif

#		ifdef LIBGDOME_STATIC
#			pragma comment(lib, GNOME_LIBS "/gdome2-x.x.x/win32/dsp/libgdome_ReleaseStatic/libgdome.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/gdome2-x.x.x/win32/dsp/libgdome_ReleaseDynamic/libgdome.lib")
#		endif

#	endif
#endif

#endif
