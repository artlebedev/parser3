/** @file
	Parser: @b hashfile parser type decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhashfile.h,v 1.5 2001/10/24 09:34:26 parser Exp $
*/

#ifndef PA_VHASHFILE_H
#define PA_VHASHFILE_H

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"
#include "pa_db_connection.h"

extern Methoded *hashfile_base_class;

/// value of type 'hashfile', implemented with GDBM library
class VHashfile : public VStateless_class {
	friend void VHashfile_cleanup(void *);
public: // value

	const char *type() const { return "hashfile"; }

	/// VHashfile: true
	bool is_defined() const { return true; }

	/// VHashfile: convert to VHash
	Hash *get_hash(const String *source);

	/// VHashfile: 0
	VStateless_class *get_class() { return 0; }

	/// VHashfile: (key)=value
	Value *get_element(const String& name) { 
		// $CLASS,$method
		if(Value *result=VStateless_class::get_element(name))
			return result;
		
		// $element
		return get_field(name);
	}
	
	/// VHashfile: (key)=value, (key)=(value+expires)
	void put_element(const String& name, Value *value);

public: // usage

	VHashfile(Pool& apool) : VStateless_class(apool, hashfile_base_class),
		fconnection(0) {
		register_cleanup(VHashfile_cleanup, this);
	}
private:
	void cleanup() {
		if(fconnection)
			fconnection->close();  // cache it
	}
public:

	void set_connection(DB_Connection& aconnection) { fconnection=&aconnection; }
	DB_Connection& get_connection(const String *source) const { 
		if(!fconnection)
			throw Exception(0, 0,
				source,
				"can not be applied to uninitialized instance");

		return *fconnection;
	}

private:

	Value *get_field(const String& name);
	void put_field(const String& name, Value *value, time_t time_to_die);

private:

	DB_Connection *fconnection;

};

#endif
