/** @file
	Parser: pool class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_pool.h,v 1.70 2001/11/16 11:03:34 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include "pa_config_includes.h"

#ifdef XML
#include <memory>
#include <XalanDOM/XalanDOMString.hpp>
#include <util/TransService.hpp>
#endif

// forwards

class Exception;
class String;

/** 
	Pool mechanizm allows users not to free up allocated memory,
	leaving that problem to 'pools'.

	@see Pooled
*/

class Pool {
public:

	Pool(void *astorage);
	~Pool();

	///{@ statistics
	size_t total_allocated() { return ftotal_allocated; }
	unsigned int total_times() { return ftotal_times; }
	///}@

	void set_context(void *acontext) { fcontext=acontext; }
	void *get_context() { return fcontext; }

	void set_tag(void *atag) { ftag=atag; }
	void *tag() { return ftag; }

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

	/// resets transcoder if they change charset 
	void set_charset(const String &charset);
	/// returns current charset
	const String& get_charset() { return *charset; }

private:

	void *fstorage;
	void *fcontext;
	void *ftag;
	const String *charset;

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

#ifdef XML

public:
	/// converts Xalan string to char *
	const char *transcode_cstr(const XalanDOMString& s);
	/// converts Xalan string to parser String
	String& transcode(const XalanDOMString& s);
	/// converts char * to Xalan string
	std::auto_ptr<XalanDOMString> transcode_buf(const char *buf, size_t buf_size);
	/// converts parser String to Xalan string
	std::auto_ptr<XalanDOMString> transcode(const String& s);

private:

	void set_charset(const char *new_scharset);
	void update_transcoder();

private:

	XMLTranscoder *transcoder;

#endif

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
	const char *transcode_cstr(const XalanDOMString& s) { return fpool->transcode_cstr(s); }
	String& transcode(const XalanDOMString& s) { return fpool->transcode(s); }
#endif
	//}
};
/// useful macro for creating objects on current Pooled object Pooled::pool()
#define NEW new(pool())

#endif
