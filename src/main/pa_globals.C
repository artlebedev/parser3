/** @file
	Parser: globals.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_GLOBALS_C="$Date: 2008/07/23 14:07:33 $";

#include "pa_config_includes.h"

#ifdef XML
#include "libxml/xmlversion.h"
#include "libxslt/extensions.h"
#include "libxslt/xsltutils.h"
extern "C" {
#include "libexslt/exslt.h"
};
#endif

#include "pa_globals.h"
#include "pa_string.h"
#include "pa_sapi.h"
#include "pa_threads.h"
#include "pa_xml_io.h"
#include "pa_common.h"

#include "pa_cache_managers.h"

namespace PCRE {
#include "pcre.h"
};

// defines

//#define PA_DEBUG_XML_GC_MEMORY

//20051130 trying to remove this, author claims that fixed a lot there // 20040920 for now both workarounds needed. wait for new libxml/xsl versions
// // there is a problem with testcase, it's unstable. 
// // see paf@six/bug20040920/cgi-bin/t for it-showed-bug-on-20040920-day
// #define PA_WORKAROUND_BUGGY_FREE_IN_LIBXML_GC_MEMORY
// #define PA_WORKAROUND_BUGGY_MALLOCATOMIC_IN_LIBXML_GC_MEMORY

// globals

short hex_value[0x100];

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


Hash<pa_thread_t, Request*> thread_request;
void pa_register_thread_request(Request& r) {
	thread_request.put(pa_get_thread_id(), &r);
}
/// retrives request set by pa_set_request function, useful in contextless places [slow]
Request& pa_thread_request() {
	return *thread_request.get(pa_get_thread_id());
}

#ifdef PA_RELEASE_ASSERTS
void pa_release_assert(const char* str, const char* file, int line) {
	SAPI::die("%s at %s:%d", str, file, line); 
}
#endif


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

static Hash<pa_thread_t, XML_Generic_error_info*> xml_generic_error_infos;

static void xmlParserGenericErrorFunc(void *  /*ctx*/, const char* msg, ...) { 
//_asm int 3;
	pa_thread_t thread_id=pa_get_thread_id();

	XML_Generic_error_info* p;
	{
		SYNCHRONIZED;  // find+fill blocked

		// first try to get existing for this thread_id
		p=xml_generic_error_infos.get(thread_id);
		if(!p) // occupy empty one
			xml_generic_error_infos.put(thread_id, (p=new(PointerFreeGC) XML_Generic_error_info));
	}
		
	va_list args;
	va_start(args, msg);
	p->used+=vsnprintf(p->buf+p->used, sizeof(p->buf)-p->used, msg, args);
	va_end(args);
}

bool xmlHaveGenericErrors() {
	pa_thread_t thread_id=pa_get_thread_id();

	SYNCHRONIZED;  // find blocked

	return xml_generic_error_infos.get(thread_id)!=0;
}

const char* xmlGenericErrors() {
	pa_thread_t thread_id=pa_get_thread_id();

	SYNCHRONIZED;  // find+free blocked

	if(XML_Generic_error_info *p=xml_generic_error_infos.get(thread_id)) {
		xml_generic_error_infos.remove(thread_id);
		return p->get();
	}

	return 0; // no errors for our thread_id registered
}

#endif

void pa_globals_destroy(void *) {
/*
	try {
#ifdef XML
#endif
	} catch(.../*const Exception& e* /) {
//		SAPI::abort("pa_globals_destroy failed: %s", e.comment());
	}
*/
}


#ifdef XML

static char *pa_GC_strdup(const char *s) {
	if(!s)
		return 0;

	size_t size=strlen(s)+1;
	char *result=(char *)GC_MALLOC_ATOMIC(size);
	if(!result)
		SAPI::abort("out of memory (while duplicating XML string [size=%d])", size);

	memcpy(result, s, size);
#ifdef PA_DEBUG_XML_GC_MEMORY
	fprintf(stderr, "pa_GC_strdup(%p=%s, length=%d)=0x%p\n", s, s, size, result);
#endif
	return result;
}

#ifdef PA_DEBUG_XML_GC_MEMORY
void *pa_look_for[]={(void*)0x84ba980,(void*)0x8969460,(void*)0x0,(void*)0x0,
			(void*)0x0,(void*)0x0,(void*)0x0,(void*)0x0};
bool pa_looked(void*p) {
	for(int i=0; i<8; i++)
		if(pa_look_for[i]==p) {
			__asm__("int $3");
			return true;
		}
	if((((int)p)&~0xFF)==0x89a7700) {
		__asm__("int $3");
		return true;
	}
	return false;
}
static void* pa_gc_malloc_log(size_t size){
	void *p=pa_gc_malloc(size);
        fprintf(stderr, "pa_gc_malloc_log(%d)=0x%p\n", size, p);
	if(pa_looked(p))
		fprintf(stderr,"catched debug malloc(%d)=0x%p\n", size, p);
	return p;
        
}
static void* pa_gc_malloc_atomic_log(size_t size){
#ifdef PA_WORKAROUND_BUGGY_MALLOCATOMIC_IN_LIBXML_GC_MEMORY
	void *p=pa_gc_malloc(size);
        fprintf(stderr, "pa_gc_malloc_atomicFAKE_log(%d)=0x%p\n", size, p);
#else
	void *p=pa_gc_malloc_atomic(size);
        fprintf(stderr, "pa_gc_malloc_atomic_log(%d)=0x%p\n", size, p);
#endif
	if(pa_looked(p))
		fprintf(stderr,"catched debug malloc atomic(%d)=0x%p\n", size, p);
	return p;
}
static void* pa_gc_realloc_log(void *ptr, size_t size){
	void *p=pa_gc_realloc(ptr, size);
        fprintf(stderr, "pa_gc_realloc_log(0x%p, %d)=0x%p\n", ptr, size, p);
	if(pa_looked(p))
		fprintf(stderr,"catched debug realloc(%d)=0x%p\n", size, p);
	return p;
}
static void pa_gc_free_log(void *p){
#ifdef PA_WORKAROUND_BUGGY_FREE_IN_LIBXML_GC_MEMORY
        fprintf(stderr, "pa_gc_freeIGNORE_log(0x%p)\n", p);
#else
        fprintf(stderr, "pa_gc_free_log(0x%p)\n", p);
#endif
	if(pa_looked(p))
		fprintf(stderr,"catched debug free(0x%p)\n", p);
#ifndef PA_WORKAROUND_BUGGY_FREE_IN_LIBXML_GC_MEMORY
        pa_gc_free(p);
#endif
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
#ifdef PA_WORKAROUND_BUGGY_MALLOCATOMIC_IN_LIBXML_GC_MEMORY
	return check(pa_gc_malloc(size), "allocating XML composite memory (asked atomic)", size);
#else
	return check(pa_gc_malloc_atomic(size), "allocating XML atomic memory", size);
#endif
}
static void* pa_gc_realloc_nonull(void* ptr, size_t size) { 
	return check(pa_gc_realloc(ptr, size), "reallocating XML memory", size);
}

static void pa_gc_free_maybeignore(
	void* 
#ifndef PA_WORKAROUND_BUGGY_FREE_IN_LIBXML_GC_MEMORY
		ptr
#endif
	) {
#ifndef PA_WORKAROUND_BUGGY_FREE_IN_LIBXML_GC_MEMORY
	pa_gc_free(ptr);
#endif
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
		/*xmlFreeFunc */pa_gc_free_maybeignore,
		/*xmlMallocFunc */pa_gc_malloc_nonull,
		/*xmlMallocFunc */pa_gc_malloc_atomic_nonull,
		/*xmlReallocFunc */pa_gc_realloc_nonull,
		/*xmlStrdupFunc */pa_GC_strdup);
#endif

#else
	xmlMemSetup(
		/*xmlFreeFunc */pa_gc_free_maybeignore,
		/*xmlMallocFunc */pa_gc_malloc,
		/*xmlReallocFunc */pa_gc_realloc,
		/*xmlStrdupFunc */pa_GC_strdup);
#endif

#endif

	// pcre
	PCRE::pcre_malloc=pa_gc_malloc;
	PCRE::pcre_free=pa_gc_free;


	// cord
	CORD_oom_fn=pa_CORD_oom_fn;
}

/**
	@test hint on one should call this for each thread xmlSubstituteEntitiesDefault(1);
*/
void pa_globals_init() {
	// global variables 
	cache_managers=new Cache_managers;


	// in various libraries
	gc_substitute_memory_management_functions();

	// hex value
	setup_hex_value();

#ifdef XML
	// initializing xml libs

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

	pa_xml_io_init();
#endif
}

void pa_globals_done() {
	delete cache_managers;  cache_managers=0;
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

#define GNOME_LIBS "../../../../win32/gnome"

#ifdef WITH_MAILRECEIVE
#	pragma comment(lib, GNOME_LIBS "/glib/lib/libglib-1.3-11.lib")
#endif

#ifdef XML
#	ifdef _DEBUG

#		ifdef LIBXML_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxml2-x.x.x/win32/debug/lib/libxml2_a.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxml2-x.x.x/win32/debug/lib/libxml2.lib")
#		endif

#		ifdef LIBXSLT_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/debug/lib/libxslt_a.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/debug/lib/libxslt.lib")
#		endif
#		ifdef LIBEXSLT_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/debug/lib/libexslt_a.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/debug/lib/libexslt.lib")
#		endif

#else

#		ifdef LIBXML_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxml2-x.x.x/win32/release/lib/libxml2_a.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxml2-x.x.x/win32/release/lib/libxml2.lib")
#		endif

#		ifdef LIBXSLT_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/release/lib/libxslt_a.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/release/lib/libxslt.lib")
#		endif
#		ifdef LIBEXSLT_STATIC
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/release/lib/libexslt_a.lib")
#		else
#			pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/release/lib/libexslt.lib")
#		endif

#	endif
#endif

#endif
