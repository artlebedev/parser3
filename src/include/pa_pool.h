/** @file
	Parser: pool class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_pool.h,v 1.78 2002/02/08 08:30:13 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include "pa_config_includes.h"

#ifdef XML
#	include "gdome.h"
// for xmlChar
#	include "libxml/tree.h"
#endif

// forwards

class Exception;
class String;
class Charset;
class GdomeDOMString_auto_ptr;

/** 
	Pool mechanizm allows users not to free up allocated memory,
	leaving that problem to 'pools'.

	@see Pooled
*/

class Pool {
public:

	Pool(void *astorage);

	///{@ statistics
	size_t total_allocated() { return ftotal_allocated; }
	unsigned int total_times() { return ftotal_times; }
	///}@

	void set_context(void *acontext) { fcontext=acontext; }
	void *get_context() { return fcontext; }

	/// allocates some bytes on pool
	void *malloc(size_t size, int place=0) {
		return check(real_malloc(size, place), size);
	}
	/// allocates some bytes clearing them with zeros
	void *calloc(size_t size) {
		return check(real_calloc(size), size);
	}

	/// registers a routine to clean up non-pooled allocations
	void register_cleanup(void (*cleanup) (void *), void *data) {
		if(!real_register_cleanup(cleanup, data))
			fail_register_cleanup();
	}

	///{@ source charset
	void set_source_charset(Charset& acharset);
	Charset& get_source_charset();
	///}@

	///{@ client charset
	void set_client_charset(Charset& charset);
	Charset& get_client_charset();
	///}@

#ifdef XML

	/// @see Charset::transcode_cstr(xmlChar *s);
	const char *transcode_cstr(xmlChar *s);
	/// @see Charset::transcode(xmlChar *s);
	String& transcode(xmlChar *s);
	/// @see Charset::transcode_cstr(GdomeDOMString *s);
	const char *transcode_cstr(GdomeDOMString *s);
	/// @see Charset::transcode(GdomeDOMString *s);
	String& transcode(GdomeDOMString *s);
	/// @see Charset::transcode(const String& s)
	GdomeDOMString_auto_ptr transcode(const String& s);

#endif

private:

	void *fstorage;
	void *fcontext;
	Charset *source_charset;
	Charset *client_charset;

private: 
	
	//{
	/// @name implementation defined
    void *real_malloc(size_t size, int place);
    void *real_calloc(size_t size);
	bool real_register_cleanup(void (*cleanup) (void *), void *data);
	//}

private: 

	/// checks whether mem allocated OK. throws exception otherwise
	void *check(void *ptr, size_t size) {
		if(ptr) {
			ftotal_allocated+=size;
			ftotal_times++;
			return ptr;
		}

		fail_alloc(size);

		// never reached
		return 0;
	}
	/// throws allocation exception
	void fail_alloc(size_t size) const;

	/// throws register cleanup exception
	void fail_register_cleanup() const;

private: // statistics
	
	size_t ftotal_allocated;
	unsigned int ftotal_times;

private: //disabled

	Pool(const Pool&);
	Pool& operator= (const Pool&);
};

/** 
	Base for all classes that are allocated in 'pools'.

	Holds Pool object. Contains useful wrappers to it's methods.

	@see NEW
*/
class Pooled {
	// the pool i'm allocated on
	Pool *fpool;
public:

	/// the Pooled-sole: Pooled instances can be allocated in Pool rather then on heap
	static void *operator new(size_t size, Pool& apool) { 
		return apool.malloc(size, 1);
	}

	Pooled(Pool& apool) : fpool(&apool) {}

	/// my pool
	Pool& pool() const { return *fpool; }

	/** used for moving objects from one pool to another. 
		in between object can have no pool and can not be used
		@see SQL_Driver_manager
	*/
	void set_pool(Pool *apool) { fpool=apool; }

	//{
	/// @name useful wrapper around pool
	void *malloc(size_t size, int place=0) const { return fpool->malloc(size, place); }
	void *calloc(size_t size) const { return fpool->calloc(size); }
	void register_cleanup(void (*cleanup) (void *), void *data) { fpool->register_cleanup(cleanup, data); }
#ifdef XML

	const char *transcode_cstr(GdomeDOMString *s) { return fpool->transcode_cstr(s); }
	String& transcode(GdomeDOMString *s) { return fpool->transcode(s); }

#endif
	//}
};
/// useful macro for creating objects on current Pooled object Pooled::pool()
#define NEW new(pool())

#endif
