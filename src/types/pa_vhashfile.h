/** @file
	Parser: @b hashfile parser type decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhashfile.h,v 1.7 2001/10/24 11:06:26 parser Exp $
*/

#ifndef PA_VHASHFILE_H
#define PA_VHASHFILE_H

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"
#include "pa_db_connection.h"

// externs

extern Methoded *hashfile_base_class;

// forwards

class Autosave_marked_to_cancel_cache ;

/// value of type 'hashfile', implemented with GDBM library
class VHashfile : public VStateless_class {
	friend void VHashfile_cleanup(void *);
	friend Autosave_marked_to_cancel_cache ;
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
	void put_element(const String& name, Value *value) {
		put_field(name, value);
	}

public: // usage

	VHashfile(Pool& apool) : VStateless_class(apool, hashfile_base_class),
		fconnection(0), fmarked_to_cancel_cache(false) {
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

	void mark_to_cancel_cache() { fmarked_to_cancel_cache=true; }
	bool marked_to_cancel_cache() { return fmarked_to_cancel_cache; }

private:

	Value *get_field(const String& name);
	void put_field(const String& name, Value *value);

private:

	DB_Connection *fconnection;
	bool fmarked_to_cancel_cache;

};

///	Auto-object used for temporary changing DB_Connection::tid.
class Autosave_marked_to_cancel_cache {
	VHashfile& fhashfile;
	bool saved;
public:
	Autosave_marked_to_cancel_cache(VHashfile& ahashfile) : 
		fhashfile(ahashfile), saved(ahashfile.fmarked_to_cancel_cache) {
	}
	~Autosave_marked_to_cancel_cache() {
		fhashfile.fmarked_to_cancel_cache=saved;
	}
};

#endif
