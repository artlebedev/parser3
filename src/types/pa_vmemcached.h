/** @file
	Parser: memcached class decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors:
		Ivan Poluyanov <ivan-poluyanov@yandex.ru>
		Artem Stepanov <timofei1394@thesecure.in>
*/

#ifndef PA_VMEMCACHED_H
#define PA_VMEMCACHED_H

#define IDENT_PA_VMEMCACHED_H "$Id: pa_vmemcached.h,v 1.10 2020/12/15 17:10:41 moko Exp $"

#include "classes.h"
#include "pa_vstateless_object.h"

#include "pa_memcached.h"

// defines
#define VMEMCACHED_TYPE "memcached"

// externs
extern Methoded *memcached_class;

class VMemcached: public VStateless_object {
public:
	override const char* type() const { return VMEMCACHED_TYPE; }
	override VStateless_class *get_class() { return memcached_class; }
	override bool as_bool() const { return true; }

	override Value* get_element(const String& aname);
	override const VJunction* put_element(const String& aname, Value* avalue);

public: // usage
	VMemcached(): fm(0), fttl(0) {}

	~VMemcached(){
		if(fm)
			f_memcached_free(fm);
	}

	void open(const String& options_string, time_t attl, bool connect);
	void open_parse(const String& connect_string, time_t attl);
	
	bool add(const String& aname, Value* avalue);
	void remove(const String& aname);
	void flush(time_t attl=0);
	void quit();
	Value &mget(ArrayString &akeys);

private:
	memcached_st* fm;
	time_t fttl;

};

#endif

