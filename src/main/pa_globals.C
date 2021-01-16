/** @file
	Parser: globals.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML
#include "libxml/xmlversion.h"
#include "libxslt/extensions.h"
#include "libxslt/xsltutils.h"
extern "C" {
#include "libexslt/exslt.h"
}
#endif

#include "pa_globals.h"
#include "pa_socks.h"
#include "pa_sapi.h"
#include "pa_xml_io.h"
#include "pa_common.h"
#include "pa_symbols.h"

#include "pa_cache_managers.h"

#include "ltdl.h"
#include "pcre.h"

volatile const char * IDENT_PA_GLOBALS_C="$Id: pa_globals.C,v 1.212 2021/01/16 15:47:05 moko Exp $" IDENT_PA_GLOBALS_H IDENT_PA_SAPI_H;

// defines

//#define PA_DEBUG_XML_GC_MEMORY

// globals

short hex_value[0x100] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,
	  0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

THREAD_LOCAL Request* thread_request=NULL; // every TLS should be referenced elsewhere, or GC will collect it

void pa_register_thread_request(Request& r) {
	thread_request=&r;
}

/// retrives request set by pa_set_request function, useful in contextless places [slow]
Request& pa_thread_request() {
	if(!thread_request)
		throw Exception(0, 0, "request is not initialized");
	return *thread_request;
}


#ifdef XML

class XML_Generic_error_info {
public:/*internal, actually*/
	char buf[MAX_STRING*5];
	size_t used;
public:
	XML_Generic_error_info() {
		buf[used=0]=0;
	}
	const char* get() {
		return used? buf: 0;
	}
};

THREAD_LOCAL XML_Generic_error_info* xml_generic_error_info = NULL;

static void xmlParserGenericErrorFunc(void *  /*ctx*/, const char* msg, ...) { 
	XML_Generic_error_info* p;
	
	if(!(p=xml_generic_error_info)) // occupy empty one
		p=xml_generic_error_info=new(PointerFreeGC) XML_Generic_error_info;
		
	va_list args;
	va_start(args, msg);
	p->used+=vsnprintf(p->buf+p->used, sizeof(p->buf)-p->used, msg, args);
	va_end(args);
}

bool xmlHaveGenericErrors() {
	return xml_generic_error_info!=0;
}

const char* xmlGenericErrors() {
	if(XML_Generic_error_info *p=xml_generic_error_info) {
		xml_generic_error_info=0;
		return p->get();
	}

	return 0; // no errors for our thread_id registered
}

#endif // XML

#ifdef XML

static char *pa_GC_strdup(const char *s) {
	if(!s)
		return 0;

	size_t size=strlen(s)+1;
	char *result=(char *)GC_MALLOC_ATOMIC(size);

	if(!result)
		pa_fail_alloc("duplicate XML string",size);

	memcpy(result, s, size);
#ifdef PA_DEBUG_XML_GC_MEMORY
	fprintf(stderr, "pa_GC_strdup(%p=%s, length=%d)=0x%p\n", s, s, size, result);
#endif
	return result;
}

#ifdef PA_DEBUG_XML_GC_MEMORY

static void* pa_gc_malloc_log(size_t size){
	void *p=GC_MALLOC(size);
        fprintf(stderr, "pa_gc_malloc_log(%d)=0x%p\n", size, p);
	return p;
        
}

static void* pa_gc_malloc_atomic_log(size_t size){
	void *p=GC_MALLOC_ATOMIC(size);
        fprintf(stderr, "pa_gc_malloc_atomic_log(%d)=0x%p\n", size, p);
	return p;
}

static void* pa_gc_realloc_log(void *ptr, size_t size){
	void *p=GC_REALLOC(ptr, size);
        fprintf(stderr, "pa_gc_realloc_log(0x%p, %d)=0x%p\n", ptr, size, p);
	return p;
}

static void pa_gc_free_log(void *p){
        fprintf(stderr, "pa_gc_free_log(0x%p)\n", p);
        GC_FREE(p);
}

#else

inline void *check(void *result, const char *where, size_t size) {
	if(!result)
		pa_fail_alloc(where, size);
	return result;
}

static void* pa_gc_malloc_nonull(size_t size) { 
	return check(GC_MALLOC(size), "allocate XML compsite memory", size);
}

static void* pa_gc_malloc_atomic_nonull(size_t size) { 
	return check(GC_MALLOC_ATOMIC(size), "allocate XML atomic memory", size);
}

static void* pa_gc_realloc_nonull(void* ptr, size_t size) { 
	return check(GC_REALLOC(ptr, size), "reallocate XML memory", size);
}

static void pa_gc_free_maybeignore(void* ptr) {
	GC_FREE(ptr);
}

#endif

#endif // XML

void pa_CORD_oom_fn(void) {
	pa_fail_alloc("expand string", 0);
}

/**
	@todo gc: libltdl: substitute lt_dlmalloc & co
*/
static void gc_substitute_memory_management_functions() {
	// in libxml & libxslt
#ifdef XML
	// asking to use GC memory
#ifdef PA_DEBUG_XML_GC_MEMORY
	xmlGcMemSetup(
		/*xmlFreeFunc */pa_gc_free_log,
		/*xmlMallocFunc */pa_gc_malloc_log,
		/*xmlMallocFunc */pa_gc_malloc_atomic_log,
		/*xmlReallocFunc */pa_gc_realloc_log,
		/*xmlStrdupFunc */pa_GC_strdup);
#else
	xmlGcMemSetup(
		/*xmlFreeFunc */pa_gc_free_maybeignore,
		/*xmlMallocFunc */pa_gc_malloc_nonull,
		/*xmlMallocFunc */pa_gc_malloc_atomic_nonull,
		/*xmlReallocFunc */pa_gc_realloc_nonull,
		/*xmlStrdupFunc */pa_GC_strdup);
#endif

#endif

	// pcre
	pcre_malloc=pa_malloc;
	pcre_free=pa_free;

	// cord
	CORD_oom_fn=pa_CORD_oom_fn;
}

/**
	@test hint on one should call this for each thread xmlSubstituteEntitiesDefault(1);
*/
void pa_globals_init() {

#ifndef PA_DEBUG_DISABLE_GC
	// Dont collect unless explicitly requested. This is quicker (~30% ),
	// but less memory-efficient(~8%), so deciding for speed.
	GC_disable();
	// as we log allocation errors, we don't need default gc warnings (without timestamp and URI)
	GC_set_warn_proc(GC_ignore_warn_proc);
#endif

	// init socks
	pa_socks_init();

	// global variables
	cache_managers=new Cache_managers;

	// in various libraries
	gc_substitute_memory_management_functions();

#ifdef SYMBOLS_CACHING
	// symbols cache
	Symbols::init();
#endif

#ifdef XML
	// initializing xml libs

	// Register the EXSLT extensions and the test module
	exsltRegisterAll();
	xsltRegisterTestModule();
	xmlDefaultSAXHandlerInit();

	// disable CDATA from being built in the document tree
	// never added yet  xmlDefaultSAXHandler.cdataBlock = NULL;
	
	// Initialization function for the XML parser. This is not reentrant. 
	// Call once before processing in case of use in multithreaded programs.
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
	// xmlDoValidityCheckingDefaultValue = 1;

	// regretfully this not only replaces entities on parse, but also on generate	xmlSubstituteEntitiesDefault(1);
	// never switched this on xmlIndentTreeOutput=1;

	xmlSetGenericErrorFunc(0, xmlParserGenericErrorFunc);
	xsltSetGenericErrorFunc(0, xmlParserGenericErrorFunc);

//	FILE *f=fopen("xslt.log", "wt");
//	xsltSetGenericDebugFunc(f/*stderr*/, 0);

	pa_xml_io_init();
#endif
}

static bool is_dlinited=false;

void pa_globals_done() {
	delete cache_managers;
	cache_managers=0;

	if(is_dlinited)
		lt_dlexit();

	pa_socks_done();
}

void pa_dlinit() {
	if(!is_dlinited){
		if(lt_dlinit())
			throw Exception(0,0,"preparation for dynamic library loading failed, %s", lt_dlerror());
		is_dlinited=true;
	}
}

#ifdef _MSC_VER

#define PREFIX "../../../../win32/"

#ifdef _DEBUG
#define CONFIGURATION "Debug"
#else
#define CONFIGURATION "Release"
#endif

#ifdef _WIN64
#define PLATFORM_64 "x64/"
#define PLATFORM_32 ""
#else
#define PLATFORM_64 ""
#define PLATFORM_32 "win32/"
#endif

#pragma comment(lib, PREFIX "pcre/" PLATFORM_64 CONFIGURATION "/pcre.lib")

#ifndef PA_DEBUG_DISABLE_GC

#pragma comment(lib, PREFIX "gc/" PLATFORM_64 CONFIGURATION "/gc.lib")

#endif // PA_DEBUG_DISABLE_GC


#ifdef XML

#define GNOME_LIBS PREFIX "gnome"

#define LIB_XML PREFIX "gnome/libxml2-x.x.x/" PLATFORM_64 PLATFORM_32 CONFIGURATION "/lib/"
#define LIB_XSLT PREFIX "gnome/libxslt-x.x.x/" PLATFORM_64 PLATFORM_32 CONFIGURATION "/lib/"

#ifdef XML_STATIC
#pragma comment(lib, LIB_XML "libxml2_a.lib")
#pragma comment(lib, LIB_XSLT "libxslt_a.lib")
#pragma comment(lib, LIB_XSLT "libexslt_a.lib")
#else
#pragma comment(lib, LIB_XML "libxml2.lib")
#pragma comment(lib, LIB_XSLT "libxslt.lib")
#pragma comment(lib, LIB_XSLT "libexslt.lib")
#endif

#endif // XML

// defines for VS2015 to link with gc/xml libs compiled in the previous VS versions
#if _MSC_VER >= 1900
#pragma comment(lib,"legacy_stdio_definitions.lib")
#endif

#endif // _MSC_VER
