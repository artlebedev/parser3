/** @file
	Parser: @b hashfile parser type decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vhashfile.h,v 1.15 2002/02/08 08:30:19 paf Exp $
*/

#ifndef PA_VHASHFILE_H
#define PA_VHASHFILE_H

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"
#include "pa_db_table.h"
#include "pa_db_manager.h"

// externs

extern Methoded *hashfile_base_class;

// forwards

class Autosave_marked_to_cancel_cache ;

/// value of type 'hashfile', implemented with GDBM library
class VHashfile : public VStateless_class {
	friend void VHashfile_cleanup(void *);
	friend class Autosave_marked_to_cancel_cache;
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
		fdb_home(0), ffile_name(0), 
		fmarked_to_cancel_cache(false) {
	}

	void assign(const String& adb_home, const String& afile_name) { 
		fdb_home=&adb_home;
		ffile_name=&afile_name;
	}
	DB_Table_ptr get_table_ptr(const String *source) {
		if(!fdb_home)
			throw Exception(0, 0,
				source,
				"can not be applied to uninitialized instance");

		return 
			DB_manager->
			get_connection_ptr(*fdb_home, source)->
			get_table_ptr(*ffile_name, source);
	}

	void mark_to_cancel_cache() { 
		fmarked_to_cancel_cache=true; 
	}
	bool marked_to_cancel_cache() { return fmarked_to_cancel_cache; }

private:

	Value *get_field(const String& aname);
	void put_field(const String& aname, Value *avalue);

private:

	const String *fdb_home;
	const String *ffile_name;
	bool fmarked_to_cancel_cache;

};

///	Auto-object used for temporary changing DB_Table::tid.
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
