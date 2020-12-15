/** @file
	Parser: pool class decl.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#define IDENT_PA_POOL_H "$Id: pa_pool.h,v 1.95 2020/12/15 17:10:31 moko Exp $"

#include "pa_config_includes.h"
#include "pa_array.h"

/** 
	Pool mechanizm allows users not to free up allocated objects,
	leaving that problem to 'pools'.

	@see Pooled
*/

class Pool {
public:

	struct Cleanup : public PA_Allocated {
		void (*cleanup) (void *);
		void *data;

		Cleanup(void (*acleanup) (void *), void *adata): cleanup(acleanup), data(adata) {}
	};

	Pool();
	~Pool();

	/// registers a routine to clean up non-pooled allocations
	void register_cleanup(void (*cleanup) (void *), void *data);
	/// unregister it, looking it up by it's data
	void unregister_cleanup(void *cleanup_data);

private:

	Array<Cleanup> cleanups;

private: 
	
	//{
	/// @name implementation defined
	bool real_register_cleanup(void (*cleanup) (void *), void *data);
	//}

private: 

	/// throws register cleanup exception
	void fail_register_cleanup() const;

private: //disabled

	Pool(const Pool&);
	Pool& operator= (const Pool&);
};

/** 
	Base for all classes that are allocated in 'pools'.
	Holds Pool object.
*/
class Pooled {
	// the pool i'm allocated on
	Pool& fpool;
public:

	Pooled(Pool& apool);

	/// my pool
	//Pool& pool() const { return *fpool; }

	/// Sole: this got called automatically from Pool::~Pool()
	virtual ~Pooled();

};

#endif
